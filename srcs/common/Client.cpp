/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tniagolo <tniagolo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/20 00:24:21 by kiteixei          #+#    #+#             */
/*   Updated: 2025/11/23 02:28:04 by tniagolo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/common/Client.hpp"
#include "../../includes/network/Connection.hpp"

std::string Client::getNickName() const
{
	return (_nickName);
}

std::string Client::getUserName() const
{
	return (_userName);
}

std::string Client::getHostName() const
{
	return (_hostName);
}

std::string Client::getPassword() const
{
	return (_password);
}

int Client::getFd() const
{
	return (_fd);
}

void Client::setNickName(const std::string &nickName)
{
	_nickName = nickName;
}

void Client::setUserName(const std::string &userName)
{
	_userName = userName;
}
void Client::setHostName(const std::string &hostName)
{
	_hostName = hostName;
}

void Client::setPassword(const std::string &password)
{
	_password = password;
}

bool Client::isRegistered(void) const
{
	return ((_registered) ? true : false);
}

void Client::setRegistered(bool reg)
{
	_registered = reg;
}

void Client::setConnection(Connection *conn)
{
	_connection = conn;
}
Connection *Client::getConnection(void) const
{
	return (_connection);
}
void Client::joinChannel(Channel *ch)
{
	std::set<Channel *>::iterator it = _channels.find(ch);
	if (it == _channels.end())
		_channels.insert(ch);
}

void Client::leaveChannel(Channel *ch)
{
	std::set<Channel *>::iterator it = _channels.find(ch);
	if (it != _channels.end())
		_channels.erase(it);
}

bool Client::isOnChannel(Channel *ch) const
{
	return (_channels.find(ch) != _channels.end());
}

const std::set<Channel *> &Client::getChannels() const
{
	return (_channels);
}

std::ostream &operator<<(std::ostream &s, const Client &client)
{
	s << client.getNickName();
	return (s);
}

void Client::sendMessage(const std::string &msg)
{
	if (_connection != NULL)
		_connection->enqueueResponse(msg);
}