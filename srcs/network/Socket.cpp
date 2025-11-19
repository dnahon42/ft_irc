/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tniagolo <tniagolo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 15:43:53 by tniagolo          #+#    #+#             */
/*   Updated: 2025/11/19 16:07:24 by tniagolo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/network/Socket.hpp"

Socket::Socket(int port) : _fd(-1), _port(port)
{
	std::memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	_address.sin_port = htons(port);
	_address.sin_addr.s_addr = htonl(INADDR_ANY);
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
	int	flags;
	int	new_flags;
	int	saved_errno;

	if (_fd < 0)
		throw std::runtime_error("Socket::_setNonBlocking: invalid fd");
	flags = fcntl(_fd, F_GETFL, 0);
	if (flags == -1)
	{
		saved_errno = errno;
		std::string msg = "fcntl(F_GETFL) failed: ";
		throw std::runtime_error(msg + std::strerror(saved_errno));
	}
	if (flags & O_NONBLOCK)
		return ;
	new_flags = flags | O_NONBLOCK;
	if (fcntl(_fd, F_SETFL, new_flags) == -1)
	{
		saved_errno = errno;
		std::string msg = "fcntl(F_SETFL) failed: ";
		throw std::runtime_error(msg + std::strerror(saved_errno));
	}
	// std::cerr << "Socket fd=" << _fd << " set non-blocking\n";
}

void Socket::_setReUseAddr(void)
{
	int	opt;
	int	saved_errno;

	if (_fd < 0)
		throw std::runtime_error("Socket::_setReuseAddr: invalid fd");
	opt = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt,
			(sizeof(opt))) == -1)
	{
		saved_errno = errno;
		std::string msg = "setsockopt(SO_REUSEADDR) failed: ";
		throw std::runtime_error(msg + std::strerror(saved_errno));
	}
}

void Socket::create(void)
{
	int	saved_errno;

	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == -1)
	{
		saved_errno = errno;
		std::string msg = "socket() failed: ";
		throw std::runtime_error(msg + std::strerror(saved_errno));
	}
	_setReUseAddr();
	_setNonBlocking();
	bind();
	listen();
}

void Socket::bind(void)
{
	int	saved_errno;

	if (_fd < 0)
		throw std::runtime_error("Socket::bind: invalid fd");
	if (::bind(_fd, (struct sockaddr *)&_address, sizeof(_address)) == -1)
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
	if (::listen(_fd, backlog) == -1)
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
	int					flags;
	int					saved_errno;
	int					fdflags;

	if (_fd < 0)
		throw std::runtime_error("Socket::accept: invalid fd");
	clientLen = sizeof(clientAddr);
	std::memset(&clientAddr, 0, sizeof(clientAddr));
	clientFd = ::accept(_fd, reinterpret_cast<struct sockaddr *>(&clientAddr),
			&clientLen);
	if (clientFd == -1)
		return (-1);
	flags = ::fcntl(clientFd, F_GETFL, 0);
	if (flags == -1)
	{
		saved_errno = errno;
		::close(clientFd);
		errno = saved_errno;
		return (-1);
	}
	if (!(flags & O_NONBLOCK))
	{
		if (::fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) == -1)
		{
			saved_errno = errno;
			::close(clientFd);
			errno = saved_errno;
			return (-1);
		}
	}
	fdflags = ::fcntl(clientFd, F_GETFD, 0);
	if (fdflags != -1)
	{
		::fcntl(clientFd, F_SETFD, fdflags | FD_CLOEXEC);
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

std::string Socket::getIpString() const
{
	char buffer[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &_address.sin_addr, buffer, INET_ADDRSTRLEN);
	return (std::string(buffer));
}