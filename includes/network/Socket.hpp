/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tniagolo <tniagolo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 15:02:41 by tniagolo          #+#    #+#             */
/*   Updated: 2025/11/19 14:30:06 by tniagolo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <exception>

// Sockets
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

// Errno
#include <cerrno>
#include <cstring>

class Socket
{
  private:
	int _fd;
	int _port;
	struct sockaddr_in _address;

	Socket(const Socket &other);
	Socket &operator=(const Socket &other);
	void _setNonBlocking();
	void _setReUseAddr();

  public:
	Socket(int port);
	~Socket();

	void create();
	void bind();
	void listen(int backlog = 10);
	int accept();
	void close();

	int getFd() const;
	int getPort() const;
	const struct sockaddr_in &getAddress() const;

	std::string getIpString() const;
};