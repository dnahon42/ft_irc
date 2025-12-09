/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dnahon <dnahon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 01:26:14 by tniagolo          #+#    #+#             */
/*   Updated: 2025/12/09 21:20:34 by dnahon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/network/Connection.hpp"

Connection::Connection(int fd) : _fd(fd), _closed(false), _inBuf(), _outBuf(),
	_writeCursor(0)
{
}

Connection::~Connection()
{
	closeConnection();
}

int Connection::getFd() const
{
	return (_fd);
}

bool Connection::isClosed() const
{
	return (_closed);
}

void Connection::closeConnection()
{
	if (!_closed && _fd >= 0)
	{
		::close(_fd);
		_fd = -1;
	}
	_closed = true;
}

void Connection::onReadable()
{
	char	buf[4096];
	ssize_t	n;

	if (_closed)
		return ;
	while (true)
	{
		n = ::recv(_fd, buf, sizeof(buf), 0);
		// tant qu'on a quelque chose a lire, on l'ajoute dans notre buffer de lecture
		if (n > 0)
		{
			_inBuf.append(buf, static_cast<size_t>(n));
		}
		else if (n == 0) // si EOF, ca veut dire que le client a arrete le programme (irssi ferme par exemple)
		{
			closeConnection();
			return ;
		}
		else // recv echoue
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK) // plus rien a lire
				break ;
			else if (errno == EINTR) // interrompu par un signal, donc on continue
				continue ;
			else // erreur fatale donc on close le socket
			{
				closeConnection();
				return ;
			}
		}
	}
	processInput(); // on traite les donnees recues
}

void Connection::processInput()
{
	size_t	pos;
	size_t	crlf; // Carriage return + line feed (entree + retour a la ligne)
	size_t	lf; // Line feed
	size_t end;
	size_t	delim_len;

	pos = 0;
	while (true)
	{	
		// on utilise find pour savoir si l'input se termine par CRLF ou LF
		crlf = _inBuf.find("\r\n", pos);
		lf = _inBuf.find('\n', pos);
		// find a bien trouve "\r\n" dans l'input, on initialise la fin de la string qu'on veut a crlf (index)
		if (crlf != std::string::npos)
			end = crlf;
		else if (lf != std::string::npos)
			end = lf;
		else
			break ; 
		// on cree une substring qui n'aura plus le \n
		std::string line = _inBuf.substr(0, end);
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1); // on retire le \r dans le cas ou c'etait crlf
		delim_len = (crlf != std::string::npos) ? 2 : 1; // on defini la taille du delimiteur en fonction de si c'est crlf ou lf
		_inBuf.erase(0, end + delim_len); // on clear le buffer pour la prochaine fois
		_commandQueue.push_back(line); // push notre command dans notre file d'attente
		
		std::cout << "Server received an input : " + line << std::endl;
	}
}

bool Connection::popCommand(std::string &command)
{
	if (_commandQueue.empty())
		return (false);
	command = _commandQueue.front();
	_commandQueue.pop_front();
	return (true);
}

void Connection::enqueueResponse(const std::string &msg)
{
	if (_closed)
		return ;
	_outBuf.append(msg);
}

bool Connection::hasPendingOutput() const
{
	return (_writeCursor < _outBuf.size());
}

void Connection::onWritable()
{
	ssize_t	n;

	if (_closed || !hasPendingOutput())
		return ;
	while (_writeCursor < _outBuf.size()) // tant que notre curseur d'ecriture (writeCursor) est inferieur a la taille de notre buffeur (message a envoyer) on reste dans la boucle
	{
		n = ::send(_fd, _outBuf.data() + _writeCursor, _outBuf.size()
				- _writeCursor, 0);
		// tant qu'on a quelque chose a ecrire, on deplace notre curseur d'ecriture
		if (n > 0)
		{
			_writeCursor += static_cast<size_t>(n);
		}
		else if (n == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK) // plus rien a ecrire
			{
				break ;
			}
			else if (errno == EINTR) // interrompu par un signal, donc on continue
			{
				continue ;
			}
			else  // erreur fatale donc on close le socket
			{
				closeConnection();
				return ;
			}
		}
	}
	if (_writeCursor >= _outBuf.size()) 
	{
		// si notre curseur est arrive a la fin, on clear notre outBuf et on remet notre curseur a 0
		_outBuf.clear();
		_writeCursor = 0;
	}
}
