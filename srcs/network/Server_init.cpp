/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_init.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dnahon <dnahon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/20 21:50:31 by tniagolo          #+#    #+#             */
/*   Updated: 2025/12/08 16:44:10 by dnahon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/network/Server.hpp"
#include "../../includes/network/Connection.hpp"
#include "../../includes/common/Client.hpp"
#include "../../includes/common/Channel.hpp"

Server::Server(const std::string &address, unsigned short port, const std::string &password) : _address(address), _port(port), _password(password), _running(false)
{
    _listenSocket = NULL;
    _listenFd = -1; // On ecoute rien par defaut
}

Server::~Server()
{
    if (_listenFd >= 0)
        _pollManager.remove(_listenFd);

    // Free tous les clients
    for (std::map<int, Client*>::iterator it = _clientsData.begin();
         it != _clientsData.end(); ++it)
    {
        delete it->second;
    }
    _clientsData.clear();
    _nickIndex.clear();

    // Free les connections
    for (std::map<int, Connection*>::iterator it = _clients.begin();
         it != _clients.end(); ++it)
    {
        delete it->second;
    }
    _clients.clear();

    // Free les channels
    for (std::map<std::string, Channel*>::iterator it = _channels.begin();
         it != _channels.end(); ++it)
    {
        delete it->second;
    }
    _channels.clear();

    // Free les sockets
    if (_listenSocket != NULL)
    {
        _listenSocket->close();
        delete _listenSocket;
        _listenSocket = NULL;
        _listenFd = -1;
    }
    _pollManager.clear();
}

bool Server::init(void)
{
    if (_port < 0)
        return false;
    return createListenSocket();
}


void Server::run(int poll_timeout_ms)
{
    if (_listenFd < 0)
        return;
    _running = true;
    while (_running)
    {
        for (std::map<int, Connection*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            if (it->second && it->second->hasPendingOutput())
                _pollManager.addEvent(it->first, POLLOUT);
        }

        int ready = _pollManager.pollOnce(poll_timeout_ms);
        if (ready < 0)
        {
            if (errno == EINTR) // interrompu par signal -> relancer
                continue;
            break; // erreur fatale
        }
        if (ready == 0) // timeout expire -> rien à faire (pour que ca soit non bloquant)
            continue;
        std::vector<struct pollfd> fds = _pollManager.getFds();
        for (size_t i = 0; i < fds.size(); ++i)
        {
            short revents = fds[i].revents;
            if (revents == 0) // tant qu'aucun event s'est produit, on continue d'iterer
                continue;
            int fd = fds[i].fd;

            if (revents & (POLLERR | POLLHUP | POLLNVAL)) // si erreur on enleve le client
            {
                removeClient(fd);
                continue;
            }

            if (fd == _listenFd) // si le fd est le fd d'ecoute du serveur et que son event est POLLIN, on accepte la connection sinon on continue d'iterer
            {
                if (revents & POLLIN)
                    acceptNewConnections();
                continue;
            }
            std::map<int, Connection*>::iterator it = _clients.find(fd);
            if (it != _clients.end() && it->second != NULL)
            {
                if (revents & POLLIN) // si la connexion a l'event POLLIN, alors le serveur va gerer la lecture du message du client
                {
                    handleReadable(fd, it->second);
                    continue;
                }
                if (revents & POLLOUT) // si la connexion a l'event POLLOUT, alors le serveur va envoyer la reponse au client
                {
                    handleWritable(fd, it->second);
                    continue;
                }
            }
            else
            {
                removeClient(fd);
                continue;
            }
        }
    }
}

void Server::addClient(int fd)
{
    if (fd < 0 || _pollManager.contains(fd))
        return ;
    try
    {
        Connection *conn = new Connection(fd);
        _clients[fd] = conn;
        _pollManager.add(fd, POLLIN);
        
        // Cree le client pour link la connection au client
        Client *client = createClient(fd);
        if (client != NULL && client->getConnection() == NULL)
            client->setConnection(conn);
    }
    catch (const std::exception &)
    {
        // En cas d'echec d'allocation on ferme proprement le fd et
        // n'enregistre rien dans le poller.
        ::close(fd);
        return;
    }
}

void Server::removeClient(int fd)
{
    if (fd < 0 || !_pollManager.contains(fd))
        return ;

    // Ne pas toucher au socket d'ecoute
    if (fd == _listenFd)
        return ;

    _pollManager.remove(fd);

   // Supprimer le client et ensuite on delete la connection puis on close le fd
    destroyClient(fd);

    std::map<int, Connection*>::iterator it = _clients.find(fd);
    if (it != _clients.end())
    {
        if (it->second != NULL)
        {
            delete it->second;
        }
        else
        {
            ::close(fd);
        }
        _clients.erase(it);
    }
    else
        ::close(fd);
}

bool Server::createListenSocket(void)
{
    if (_port < 0)
        return false;
    
    if (_listenSocket == NULL)
        _listenSocket = new Socket(_port);

    // Si une adresse est fournie, convertir la chaîne en binaire et la placer dans la struct du socket
    if (!_address.empty())
    {
        int ret = inet_pton(AF_INET, _address.c_str(), &(_listenSocket->getAddress().sin_addr));
        if (ret != 1)
        {
        // 1 = conversion OK ; 0 = chaîne invalide ; -1 = af invalide
            delete _listenSocket;
            _listenSocket = NULL;
            _listenFd = -1;
            return false;
        }
    }
    // Creer le socket d'ecoute (bind/listen/non-blocking) et enregistrer le fd
    try
    {
        _listenSocket->create();
        _listenFd = _listenSocket->getFd();
        _pollManager.add(_listenFd, POLLIN);
    }
    catch (std::exception &e)
    {
        delete _listenSocket;
        _listenSocket = NULL;
        _listenFd = -1;
        return false;
    }
    return true;
}

void Server::acceptNewConnections(void)
{
    if (_listenSocket == NULL || _listenFd < 0)
        return;

    // Accepter en boucle toutes les connexions en attente (non-bloquant)
    while (true)
    {
        int clientFd = _listenSocket->accept();
        if (clientFd == -1)
        {
            // accept() retourne -1 si aucune connexion n'est prête ou si erreur
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // Plus de connexions à accepter pour l'instant
                break;
            }
            // erreur fatale imprévue -> on sort
            break;
        }
        // Enregistrer le nouveau client dans les structures du serveur
        addClient(clientFd);
    }
}

int Server::getListenFd(void) const
{
    return (_listenFd);
}

PollManager &Server::getPollManager()
{
    return (_pollManager);
}

std::string Server::getPassword() const
{
    return (_password);
}

void Server::stop()
{
    _running = false;
}