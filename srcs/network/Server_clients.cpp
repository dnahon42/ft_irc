#include "../../includes/common/Channel.hpp"
#include "../../includes/common/Client.hpp"
#include "../../includes/network/Connection.hpp"
#include "../../includes/network/Server.hpp"
#include "../../includes/parsing/Parser.hpp"
#include "../../includes/parsing/Command.hpp"

Client *Server::createClient(int fd)
{
	Client		*existing;
	Client		*client;
	Connection	*conn;

	if (fd < 0)
		return (NULL);
	// Si un objet Client existe deja pour ce fd, on verifie qu'il est lie a
	// une Connection existante et on le retourne
	existing = getClientByFd(fd);
	if (existing != NULL)
	{
		std::map<int, Connection *>::iterator it2 = _clients.find(fd);
		if (it2 != _clients.end() && it2->second != NULL
			&& existing->getConnection() == NULL)
			existing->setConnection(it2->second);
		return (existing);
	}
	// Creer un nouveau Client et l'attacher a une Connection si elle existe
	client = new Client(fd);
	std::map<int, Connection *>::iterator it = _clients.find(fd);
	if (it != _clients.end() && it->second != NULL)
	{
		conn = it->second;
		client->setConnection(conn);
	}
	// Enregistrer le client dans la map du serveur. Ownership: Server possede Client*
	_clientsData[fd] = client;
	return (client);
}

void Server::destroyClient(int fd)
{
	Client		*client;
	Connection	*conn;

	client = getClientByFd(fd);
	if (client == NULL)
		return ;
	// Retirer du nick index si present
	std::string nick = client->getNickName();
	if (!nick.empty())
	{
		std::map<std::string, Client *>::iterator nit = _nickIndex.find(nick);
		if (nit != _nickIndex.end())
			_nickIndex.erase(nit);
	}
	// Delier la connection
	conn = client->getConnection();
	if (conn != NULL)
		client->setConnection(NULL);
	// Retirer de la map client data et supprimer
	_clientsData.erase(fd);
	delete client;
}

void Server::removeChannelIfEmpty(Channel *ch)
{
	if (ch == NULL)
		return ;
	if (ch->getMemberCount() <= 0)
	{
		_channels.erase(ch->getName());
		delete ch;
	}
}

void Server::dispatchCommand(Client *client, const std::string &line)
{
	if (client == NULL)
		return ;

	Command cmd = Parser::parse(line);
	std::string commandName = cmd.getCommand();

	bool handled = false;
	try
	{
		if (commandName == "PING")
		{
			handlePingCommand(client, cmd);
			handled = true;
		}
		else if (commandName == "NICK")
		{
			handleNickCommand(client, cmd);
			handled = true;
		}
		else if (commandName == "USER")
		{
			handleUserCommand(client, cmd);
			handled = true;
		}
		else if (commandName == "PASS")
		{
			handlePassCommand(client, cmd);
			handled = true;
		}
		else if (commandName == "QUIT")
		{
			handleQuitCommand(client, cmd);
			handled = true;
		}
		else if (commandName == "JOIN")
		{
			handleJoinCommand(client, cmd);
			handled = true;
		}
		else if (commandName == "PRIVMSG")
		{
			handlePrivmsgCommand(client, cmd);
			handled = true;
		}
		else if (commandName == "KICK")
		{
			handleKickCommand(client, cmd);
			handled = true;
		}
		else if (commandName == "INVITE")
		{
			handleInviteCommand(client, cmd);
			handled = true;
		}
		else if (commandName == "TOPIC")
		{
			handleTopicCommand(client, cmd);
			handled = true;
		}
		else if (commandName == "MODE")
		{
			handleModeCommand(client, cmd);
			handled = true;
		}
	}
	catch (const std::exception &e)
	{
		(void)e;
		return;
	}

	if (!handled)
	{
		std::string reply = ":localhost 421 " + commandName + " :Unknown command\r\n";
		client->sendMessage(reply);
	}
}

Client *Server::getClientByFd(int fd)
{
	std::map<int, Client *>::iterator it = _clientsData.find(fd);
	if (it != _clientsData.end())
		return (it->second);
	return (NULL);
}

Client *Server::getClientByNick(const std::string &nick)
{
	std::map<std::string, Client *>::iterator it = _nickIndex.find(nick);
	if (it != _nickIndex.end())
		return (it->second);
	return (NULL);
}

Channel *Server::getOrCreateChannel(const std::string &name)
{
	std::map<std::string, Channel *>::iterator it = _channels.find(name);
	if (it != _channels.end())
		return (it->second);
	
	Channel *newChannel = new Channel(name);
	_channels[name] = newChannel;
	return (newChannel);
}
