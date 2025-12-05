/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tniagolo <tniagolo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 15:02:57 by tniagolo          #+#    #+#             */
/*   Updated: 2025/11/28 03:56:06 by tniagolo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../common/Utils.hpp"

class	Connection;
class	Channel;

class Client
{
  private:
	std::string _nickName;
	std::string _userName;
	std::string _realName;
	std::string _hostName;
	std::string _password;
	int _fd;
	time_t _userTime;
	bool _registered;
	
	Connection *_connection;
	std::set<Channel *> _channels;

  public:
	Client(int fd) : _nickName(""), _userName(""), _realName(""), _hostName(""), _fd(fd),
		_userTime(0), _registered(false), _connection(NULL){};
	~Client(){};

	std::string getNickName() const;
	std::string getUserName() const;
	std::string getRealName() const;
	std::string getHostName() const;
	std::string getPassword() const;
	int getFd() const;
	time_t getUserTime() const;

	void setNickName(const std::string &nickName);
	void setUserName(const std::string &userName);
	void setRealName(const std::string &realName);
	void setHostName(const std::string &hostName);
	void setPassword(const std::string &password);

	bool isRegistered() const;
	void setRegistered(bool reg);

	void setConnection(Connection *conn);
	Connection *getConnection() const;

	void joinChannel(Channel *ch);
	void leaveChannel(Channel *ch);
	bool isOnChannel(Channel *ch) const;
	const std::set<Channel *> &getChannels() const;

	void sendMessage(const std::string &msg);
};

std::ostream &operator<<(std::ostream &o, const Client &client);