#include "../../includes/network/Connection.hpp"

Connection::Connection(int fd) : _fd(fd), _closed(false), _inBuf(), _outBuf(),
	_writeCursor(0), _lastActivity(std::time(nullptr))
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
		if (n > 0) // tant qu'on a quelque chose a lire, on l'ajoute dans notre buffer de lecture
		{
			_inBuf.append(buf, static_cast<size_t>(n));
			_lastActivity = std::time(nullptr);
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
		if (crlf != std::string::npos)
			end = crlf; // find a bien trouve "\r\n" dnas l'input, on initialise la find de la string qu'on veut a crlf (index)
		else if (lf != std::string::npos)
			end = lf;
		else
			break ; 
		std::string line = _inBuf.substr(0, end); // on cree une substring qui n'aura plus le \n
		if (!line.empty() && line.back() == '\r')
			line.pop_back(); // on retire le \r dans le cas ou c'etait crlf
		delim_len = (crlf != std::string::npos) ? 2 : 1; // on defini la taille du delimiteur en fonction de si c'est crlf ou lf
		_inBuf.erase(0, end + delim_len); // on clear le buffer pour la prochaine fois
		handleCommand(line);
	}
}

void Connection::handleCommand(const std::string &line)
{
	// Placeholder behaviour: echo received line back to client prefixed
	// with "Received: ". Replace this with actual IRC parsing/handling.
	std::string reply = "Received: ";
	reply += line;
	reply += "\r\n";
	enqueueResponse(reply);
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
		if (n > 0) // tant qu'on a quelque chose a ecrire, on deplace notre curseur d'ecriture
		{
			_writeCursor += static_cast<size_t>(n);
			_lastActivity = std::time(nullptr);
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
