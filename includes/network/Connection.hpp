/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dnahon <dnahon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/20 11:45:14 by tniagolo          #+#    #+#             */
/*   Updated: 2025/12/06 14:14:13 by dnahon           ###   ########.fr       */
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
	std::deque<std::string> _commandQueue;

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
	bool popCommand(std::string &command);
};
