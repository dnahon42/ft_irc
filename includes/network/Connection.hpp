/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tniagolo <tniagolo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/20 11:45:14 by tniagolo          #+#    #+#             */
/*   Updated: 2025/11/22 03:47:04 by tniagolo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../common/Utils.hpp"

class	Server;

class Connection
{
  private:
	int _fd;
	bool _closed;
	std::string _inBuf;  // donnees lues mais non encore traitees
	std::string _outBuf; // donnees à écrire
	size_t _writeCursor; // position d'ecriture dans _outBuf
	time_t _lastActivity;

	void handleCommand(const std::string &line);

	void closeConnection();

  public:
	Connection(int fd);
	~Connection();

	int getFd() const;
	bool isClosed() const;

	void onReadable();
	void onWritable();

	void enqueueResponse(const std::string &msg);
	void processInput();
	bool hasPendingOutput() const;
};
