/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_io.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tniagolo <tniagolo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 04:45:45 by tniagolo          #+#    #+#             */
/*   Updated: 2025/11/22 04:48:58 by tniagolo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/network/Server.hpp"
#include "../../includes/network/Connection.hpp"

void Server::handleReadable(int fd, Connection *conn)
{
    if (conn != NULL)
    {
        conn->onReadable(); // le serveur lit le message envoye par le client
        if (conn->isClosed()) // si la connexion a ete interrompu pendant la lecture, on free
        {
            removeClient(fd);
            return;
        }
        if (conn->hasPendingOutput())
            _pollManager.addEvent(fd, POLLOUT); // si en envoyant son message il a recu une reponse, alors le serveur ajoute l'event POLLOUT
    }
    else
    {
        removeClient(fd);
    }
}

void Server::handleWritable(int fd, Connection *conn)
{
    if (conn != NULL)
    {
        conn->onWritable(); // le serveur envoie la reponse au message du client
        if (conn->isClosed()) // si la connexion a ete interrompu pendant l'ecriture, on free
        {
            removeClient(fd);
            return;
        }
        if (!conn->hasPendingOutput())
            _pollManager.removeEvent(fd, POLLOUT); // si le serveur a fini d'envoyer sa reponse, on retire le flag POLLOUT
    }
    else
    {
        removeClient(fd);
    }
}
