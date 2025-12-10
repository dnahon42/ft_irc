/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tniagolo <tniagolo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 15:02:41 by tniagolo          #+#    #+#             */
/*   Updated: 2025/11/21 19:59:46 by tniagolo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../common/Utils.hpp"

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
	struct sockaddr_in &getAddress();

	std::string getIpString() const;
};