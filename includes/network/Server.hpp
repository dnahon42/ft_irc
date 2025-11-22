/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tniagolo <tniagolo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 14:56:42 by tniagolo          #+#    #+#             */
/*   Updated: 2025/11/22 02:52:09 by tniagolo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "PollManager.hpp"
#include "Socket.hpp"

class	Connection;

class Server
{
  private:
	std::string _address;
	unsigned short _port;

	Socket *_listenSocket;
	int _listenFd;
	PollManager _pollManager;

	std::map<int, Connection *> _clients;

	bool createListenSocket();
	void acceptNewConnections();

	/* I/O helpers (implemented in Server_io.cpp) */
	void handleReadable(int fd, Connection *conn);
	void handleWritable(int fd, Connection *conn);

	Server(const Server &other);
	Server &operator=(const Server &other);

  public:
	Server(const std::string &address, unsigned short port);
	~Server();

	bool init();
	void run(int poll_timeout_ms = -1);

	int getListenFd() const;
	PollManager &getPollManager();

	void addClient(int fd);
	void removeClient(int fd);
};