/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dnahon <dnahon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 15:43:53 by tniagolo          #+#    #+#             */
/*   Updated: 2025/12/12 19:43:00 by dnahon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/network/Socket.hpp"

Socket::Socket(int port) : _fd(-1), _port(port)
{
	// Initialisation de la struct qui contient le socket internet a 0
	std::memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET; // AF_INET = IPv4
	_address.sin_port = htons(port); // La fonction htons permet de transformer le port qui est en int en une valeur lisible pour la machine
	_address.sin_addr.s_addr = htonl(INADDR_ANY); // La fonction htonl avec comme parametre INADRR_ANY signifie que ca pourra ecouter sur n'importe quelle adresse
}

Socket::~Socket()
{
	if (_fd >= 0)
		::close(_fd);
}

Socket::Socket(const Socket &other)
{
	(void)other;
}

Socket &Socket::operator=(const Socket &other)
{
	(void)other;
	return (*this);
}

void Socket::_setNonBlocking(void)
{
	int	saved_errno;

	if (_fd < 0)
		throw std::runtime_error("Socket::_setNonBlocking: invalid fd");
	// Ajout du flag O_NONBLOCK qui permet d'eviter un coincement lors d'un syscall
	// (comme accept) comme ca le syscall return immediatement pour eviter de bloquer le thread
	if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1)
	{
		saved_errno = errno;
		std::string msg = "fcntl(F_SETFL, O_NONBLOCK) failed: ";
		throw std::runtime_error(msg + std::strerror(saved_errno));
	}
}

void Socket::_setReUseAddr(void)
{
	int	opt;
	int	saved_errno;

	if (_fd < 0)
		throw std::runtime_error("Socket::_setReuseAddr: invalid fd");
	opt = 1;
	// On utilise la fonction setsockopt (set socket option) pour utiliser l'option SO_REUSEADDR, qui permet de bind sur un port qui etait precedemment utilise
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt,
			(sizeof(opt))) == -1) // (exemple : ca permet de relancer le serveur directement apres avoir fait ctrl + C donc sans attendre la fin du TIME_WAIT)
	{
		saved_errno = errno;
		std::string msg = "setsockopt(SO_REUSEADDR) failed: ";
		throw std::runtime_error(msg + std::strerror(saved_errno));
	}
}

void Socket::create(void)
{
	int	saved_errno;

	_fd = socket(AF_INET, SOCK_STREAM, 0); // On defini le fd avec un appel a socket qui prend en parametre le type d'ip (ipv4), le 2eme parametre etant le SOCKET TCP, et le 3eme c'est le protocole (ici il est defini par defaut donc TCP car SOCK_STREAM) 
	if (_fd == -1)
	{
		saved_errno = errno;
		std::string msg = "socket() failed: ";
		throw std::runtime_error(msg + std::strerror(saved_errno));
	}
	_setReUseAddr(); // On active l'option socket SO_REUSEADDR
	_setNonBlocking(); // On ajoute le flag non bloquant
	bind(); // On appelle bind avec pour lier le port et toutes les adresses qu'on ecoute au fd
	listen();
}

void Socket::bind(void)
{
	int	saved_errno;

	if (_fd < 0)
		throw std::runtime_error("Socket::bind: invalid fd");
	if (::bind(_fd, (struct sockaddr *)&_address, sizeof(_address)) == -1) // Associe le socket sur TOUTES les interfaces de la machine
	{
		saved_errno = errno;
		::close(_fd);
		_fd = -1;
		std::string msg = "bind() failed: ";
		throw std::runtime_error(msg + std::strerror(saved_errno));
	}
	// Check que le port est bien > 0 dans le parsing
}

void Socket::listen(int backlog)
{
	int	saved_errno;

	if (_fd < 0)
		throw std::runtime_error("Socket::listen: invalid fd");
	if (::listen(_fd, backlog) == -1) // ca permettra d'accepter les connexions sur le fd donne, avec comme taille maximale de la file d'attente, le backlog (qui est de 10)
	{
		saved_errno = errno;
		::close(_fd);
		_fd = -1;
		std::string msg = "listen() failed: ";
		throw std::runtime_error(msg + std::strerror(saved_errno));
	}
}

int Socket::accept(void)
{
	struct sockaddr_in	clientAddr;
	socklen_t			clientLen;
	int					clientFd;
	int					saved_errno;

	if (_fd < 0)
		throw std::runtime_error("Socket::accept: invalid fd");
	clientLen = sizeof(clientAddr); 
	std::memset(&clientAddr, 0, sizeof(clientAddr));
	clientFd = ::accept(_fd, reinterpret_cast<struct sockaddr *>(&clientAddr),
			&clientLen);
	// Lorsque le serveur recoit une nouvelle connexion, il va l'intercepter et ouvrir un nouveau socket (FD) pour le client
	if (clientFd == -1)
		return (-1); // Si accept echoue, on return donc -1 qui signifie que le socket est invalide
	// Rendre le socket client non-bloquant
	if (::fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
	{
		saved_errno = errno;
		::close(clientFd);
		errno = saved_errno; // on save le errno pour la fonction qui appellera Socket::accept
		return (-1);
	}
	return (clientFd);
}

void Socket::close(void)
{
	int	saved_errno;

	if (_fd < 0)
		throw std::runtime_error("Socket::close: invalid fd");
	if (::close(_fd) == -1)
	{
		saved_errno = errno;
		std::string msg = "close() failed: ";
		throw std::runtime_error(msg + std::strerror(saved_errno));
	}
	_fd = -1;
}

int Socket::getFd(void) const
{
	return (_fd);
}

int Socket::getPort(void) const
{
	return (_port);
}

const struct sockaddr_in &Socket::getAddress() const
{
	return (_address);
}

struct sockaddr_in &Socket::getAddress()
{
	return (_address);
}

std::string Socket::getIpString() const
{
	char buffer[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &_address.sin_addr, buffer, INET_ADDRSTRLEN);
	return (std::string(buffer));
}