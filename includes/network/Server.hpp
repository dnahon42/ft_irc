/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dnahon <dnahon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 14:56:42 by tniagolo          #+#    #+#             */
/*   Updated: 2025/11/29 05:42:19 by dnahon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "PollManager.hpp"
#include "Socket.hpp"

class	Connection;
class	Client;
class	Channel;

class Server
{
  private:
	std::string _address;
	unsigned short _port;
	std::string _password;

	Socket *_listenSocket;
	int _listenFd;
	PollManager _pollManager;

	std::map<int, Connection *> _clients;
	std::map<int, Client *> _clientsData;
	std::map<std::string, Client *> _nickIndex;
	std::map<std::string, Channel *> _channels;
	bool createListenSocket();
	void acceptNewConnections();

	void handleReadable(int fd, Connection *conn);
	void handleWritable(int fd, Connection *conn);

	// Server_clients.cpp
	Client *createClient(int fd);
	void destroyClient(int fd);
	void dispatchCommand(Client *client, const std::string &line);

	Server(const Server &other);
	Server &operator=(const Server &other);

  public:
	Server(const std::string &address, unsigned short port, const std::string &password);
	~Server();

	bool init();
	void run(int poll_timeout_ms = -1);

	int getListenFd() const;
	std::string getPassword() const;
	PollManager &getPollManager();

	void addClient(int fd);
	void removeClient(int fd);
	void updateClientNick(Client *client, const std::string &newNick);
	void removeChannelIfEmpty(Channel *ch);
	Client *getClientByFd(int fd);
	Client *getClientByNick(const std::string &nick);
	Channel *getOrCreateChannel(const std::string &name);
	Channel *getChannel(const std::string &name);
};