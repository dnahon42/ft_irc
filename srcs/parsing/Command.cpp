/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dnahon <dnahon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 02:13:57 by tniagolo          #+#    #+#             */
/*   Updated: 2025/12/05 19:39:31 by dnahon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/parsing/Command.hpp"
#include "../../includes/common/Client.hpp"
#include "../../includes/network/Server.hpp"
#include "../../includes/common/Channel.hpp"

Command::Command() : _prefix(""), _command("")
{
}

Command::~Command()
{
}

std::string Command::getPrefix() const
{
	return (_prefix);
}

std::string Command::getCommand() const
{
	return (_command);
}

const std::vector<std::string> &Command::getParams() const
{
	return (_params);
}

void Command::setPrefix(const std::string &prefix)
{
	_prefix = prefix;
}

void Command::setCommand(const std::string &command)
{
	_command = command;
}

void Command::addParam(const std::string &param)
{
	_params.push_back(param);
}

void handlePingCommand(Client *client, Command cmd)
{
    std::string reply = "PONG";
    if (!cmd.getParams().empty())
        reply += " " + cmd.getParams()[0];
    reply += "\r\n";
    client->sendMessage(reply);
}

static bool isValidNickname(const std::string &nick)
{
	if (nick.empty() || nick.length() > 9)
		return (false);
	if (isdigit(nick[0]) || nick[0] == '-')
		return (false);
	std::string special = "[]\\`_^{}|-";
	for (size_t i = 0; i < nick.length(); ++i)
	{
		if (!isalnum(nick[i]) && special.find(nick[i]) == std::string::npos)
			return (false);
	}
	return (true);
}

void handleNickCommand(Server *server, Client *client, Command cmd)
{
	// Verifie si un nickname a ete donne
	if (cmd.getParams().empty())
	{
		client->sendMessage(":localhost " ERR_NONICKNAMEGIVEN " " + client->getNickName() + " :No nickname given\r\n");
		return ;
	}

	std::string nick = cmd.getParams()[0];

	// Verifie si le nickname est valide (caracteres autorises, longueur)
	if (!isValidNickname(nick))
	{
		client->sendMessage(":localhost " ERR_ERRONEUSNICKNAME " " + client->getNickName() + " " + nick + " :Erroneus nickname\r\n");
		return ;
	}

	// Verifie si le nickname est deja utilise par un autre client
	if (server->getClientByNick(nick) != NULL)
	{
		client->sendMessage(":localhost " ERR_NICKNAMEINUSE " " + client->getNickName() + " " + nick + " :Nickname is already in use\r\n");
		return ;
	}

	std::string oldNick = client->getNickName();
	// Met a jour le nickname du client et on update le serveur
	server->updateClientNick(client, nick);
	client->setNickName(nick);

	// Si le client est deja enregistre, on informe tout le monde du changement de nick
	if (client->isRegistered())
	{
		std::string msg = ":" + oldNick + "!" + client->getUserName() + "@" + client->getHostName() + " NICK :" + nick + "\r\n";
		std::set<Client *> recipients;
		recipients.insert(client);
		
		// On recupere tous les salons ou est le client pour prevenir les autres membres
		const std::set<Channel *> &channels = client->getChannels();
		for (std::set<Channel *>::const_iterator it = channels.begin(); it != channels.end(); ++it)
		{
			const MembersMap &members = (*it)->getMembers();
			for (MembersMap::const_iterator mit = members.begin(); mit != members.end(); ++mit)
			{
				recipients.insert(mit->second);
			}
		}
		// On parcour notre set de clients pour envoyer le message a tout les membres qui sont dans les channels du client
		for (std::set<Client *>::iterator it = recipients.begin(); it != recipients.end(); ++it)
		{
			(*it)->sendMessage(msg);
		}
	}
	else
	{
		// Si le client n'est pas encore enregistre mais que le username est set, on finalise l'enregistrement
		if (!client->getUserName().empty())
		{
			if (server->getPassword() != client->getPassword())
        	{
            	client->sendMessage(":localhost " ERR_PASSWDMISMATCH " " + client->getNickName() + " :Password incorrect\r\n");
            	return;
        	}
			client->setRegistered(true);
			client->sendMessage(":localhost " RPL_WELCOME " " + nick + " :Welcome to the FT_IRC Network " + nick + "\r\n");
		}
	}
}

void handleUserCommand(Server *server, Client *client, Command cmd)
{
	if (client->isRegistered())
	{
		client->sendMessage(":localhost " ERR_ALREADYREGISTRED " " + client->getNickName() + " :Unauthorized command (already registered)\r\n");
		return ;
	}

	if (cmd.getParams().size() < 4)
	{
		client->sendMessage(":localhost " ERR_NEEDMOREPARAMS " " + client->getNickName() + " USER :Not enough parameters\r\n");
		return ;
	}

	client->setUserName(cmd.getParams()[0]);
	// Le hostname et servername (params 1 et 2) sont ignores pour les connexions directes client-serveur
	// pour des raisons de securite (RFC 1459). On pourrait stocker le hostname si on voulait.
	client->setRealName(cmd.getParams()[3]);

	if (!client->getNickName().empty())
	{
		if (server->getPassword() != client->getPassword())
        {
            client->sendMessage(":localhost " ERR_PASSWDMISMATCH " " + client->getNickName() + " :Password incorrect\r\n");
            return;
        }
		client->setRegistered(true);
		client->sendMessage(":localhost " RPL_WELCOME " " + client->getNickName() + " :Welcome to the FT_IRC Network " + client->getNickName() + "\r\n");
	}
}

void handlePassCommand(Client *client, Command cmd)
{
	if (client->isRegistered())
	{
		client->sendMessage(":localhost " ERR_ALREADYREGISTRED " " + client->getNickName() + " :Unauthorized command (already registered)\r\n");
		return ;
	}
	if (cmd.getParams().empty())
	{
		client->sendMessage(":localhost " ERR_NEEDMOREPARAMS " " + client->getNickName() + " PASS :Not enough parameters\r\n");
		return ;
	}
	client->setPassword(cmd.getParams()[0]);
}

void handleQuitCommand(Server *server, Client *client, Command cmd)
{
    std::string reason;
    if (!cmd.getParams().empty())
        reason = cmd.getParams()[0];
    else
        reason = "Client Quit";
	
    std::string quitMsg = ":" + client->getNickName() + "!" + client->getUserName() + "@" + client->getHostName() + " QUIT :" + reason + "\r\n";

    // Diffuser aux salons communs (comme pour NICK)
    std::set<Client *> recipients;
    const std::set<Channel *> &channels = client->getChannels();
    for (std::set<Channel *>::const_iterator it = channels.begin(); it != channels.end(); ++it)
    {
        const std::map<std::string, Client *> &members = (*it)->getMembers();
        for (std::map<std::string, Client *>::const_iterator mit = members.begin(); mit != members.end(); ++mit)
        {
            // On n'envoie pas le message au client qui part, car il va être déconnecté
            if (mit->second != client)
                recipients.insert(mit->second);
        }
    }

    for (std::set<Client *>::iterator it = recipients.begin(); it != recipients.end(); ++it)
        (*it)->sendMessage(quitMsg);

    server->removeClient(client->getFd());
}

static std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
        tokens.push_back(token);
    return tokens;
}

void handleJoinCommand(Server *server, Client *client, Command cmd)
{
    if (cmd.getParams().empty())
    {
        client->sendMessage(":localhost " ERR_NEEDMOREPARAMS " " + client->getNickName() + " JOIN :Not enough parameters\r\n");
        return ;
    }

    std::string channelsParam = cmd.getParams()[0];
    std::string keysParam = (cmd.getParams().size() > 1) ? cmd.getParams()[1] : "";//cree une stirng de clee si il y en a.

    std::vector<std::string> channels = split(channelsParam, ',');//split les param si il y a plusieurs channels.
    std::vector<std::string> keys = split(keysParam, ',');

	for (size_t i = 0; i < channels.size(); ++i) 
    {
        std::string channelName = channels[i];//set le channel actuel au vector correspondant.
        std::string key = (i < keys.size()) ? keys[i] : "";

        if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&'))//verifi que le channel commence bien par # || &.
        {
            client->sendMessage(":localhost " ERR_BADCHANMASK " " + client->getNickName() + " " + channelName + " :Bad Channel Mask\r\n");
            continue;
        }

        Channel *channel = server->getOrCreateChannel(channelName);//cree le channel dans le serveur.
        if (!channel)
            continue;

        Channel::JoinStatus status = channel->canJoin(client, key); // En fonction de la valeur return, on va savoir si l'utilisateur est autorise ou pas a join le channel (exemple si il est invite ou non)
        
		if (status == Channel::JOIN_OK)
		{
            channel->addMember(client);
            client->joinChannel(channel);

            if (channel->getMemberCount() == 1)
                channel->addOperator(client);//si cest le seul client il sera op

            std::string joinMsg = ":" + client->getNickName() + "!" + client->getUserName() + "@" + client->getHostName() + " JOIN " + channelName + "\r\n";
            const std::map<std::string, Client *> &members = channel->getMembers();
            for (std::map<std::string, Client *>::const_iterator it = members.begin(); it != members.end(); ++it)
                it->second->sendMessage(joinMsg);

            if (!channel->getTopic().empty())
                client->sendMessage(":localhost " RPL_TOPIC " " + client->getNickName() + " " + channelName + " :" + channel->getTopic() + "\r\n");
            
            std::string namesList;
            for (std::map<std::string, Client *>::const_iterator it = members.begin(); it != members.end(); ++it)
            {
                if (channel->isOperator(*(it->second)))
                    namesList += "@";
                namesList += it->second->getNickName() + " ";
            }
            if (!namesList.empty())
                namesList.resize(namesList.size() - 1);

            client->sendMessage(":localhost " RPL_NAMREPLY " " + client->getNickName() + " = " + channelName + " :" + namesList + "\r\n");
            client->sendMessage(":localhost " RPL_ENDOFNAMES " " + client->getNickName() + " " + channelName + " :End of /NAMES list\r\n");
        }
		else
        {
            if (status == Channel::INVIT_REQUIRED)
                client->sendMessage(":localhost " ERR_INVITEONLYCHAN " " + client->getNickName() + " " + channelName + " :Cannot join channel (+i)\r\n");
            else if (status == Channel::PASSWORD_INCORECT)
                client->sendMessage(":localhost " ERR_BADCHANNELKEY " " + client->getNickName() + " " + channelName + " :Cannot join channel (+k)\r\n");
            else if (status == Channel::FULL_CHANNEL)
                client->sendMessage(":localhost " ERR_CHANNELISFULL " " + client->getNickName() + " " + channelName + " :Cannot join channel (+l)\r\n");
        }
    }
}

void handlePartCommand(Server *server, Client *client, Command cmd)
{
	if (cmd.getParams().empty())
    {
        client->sendMessage(":localhost " ERR_NEEDMOREPARAMS " " + client->getNickName() + " PART :Not enough parameters\r\n");
        return ;
    }
    std::string channelsParam = cmd.getParams()[0];
    std::vector<std::string> channels = split(channelsParam, ',');//split les param si il y a plusieurs channels.
	for (size_t i = 0; i < channels.size(); ++i) 
    {
        std::string channelName = channels[i];
		Channel *channel = server->getChannel(channelName);
		if (!channel)
            client->sendMessage(":localhost " ERR_NOSUCHCHANNEL " " + client->getNickName() + " " + channelName + " :No such channel\r\n");
		else
		{
			if (channel->removeMember(client->getNickName()) == Channel::MEMBER_NOT_FOUND)
            	client->sendMessage(":localhost " ERR_NOTONCHANNEL " " + client->getNickName() + " " + channelName + " :You're not on that channel\r\n");
			else
			{
				std::string reason = (cmd.getParams().size() > 1) ? cmd.getParams()[1] : "";
				std::string partMsg = ":" + client->getNickName() + "!" + client->getUserName() + "@" + client->getHostName() + " PART " + channelName;
				if (!reason.empty())
					partMsg += " :" + reason;
				partMsg += "\r\n";

				client->sendMessage(partMsg);

				const std::map<std::string, Client *> &members = channel->getMembers();
				for (std::map<std::string, Client *>::const_iterator it = members.begin(); it != members.end(); ++it)
					it->second->sendMessage(partMsg);

				client->leaveChannel(channel);
				server->removeChannelIfEmpty(channel);
			}
		}
	}
}

void handlePrivmsgCommand(Server *server, Client *client, Command cmd)
{
    if (cmd.getParams().empty())
    {
        client->sendMessage(":localhost " ERR_NORECIPIENT " " + client->getNickName() + " :No recipient given (PRIVMSG)\r\n");
        return;
    }
    if (cmd.getParams().size() < 2)
    {
        client->sendMessage(":localhost " ERR_NOTEXTTOSEND " " + client->getNickName() + " :No text to send\r\n");
        return;
    }

    std::string targetsParam = cmd.getParams()[0];
    std::string message = cmd.getParams()[1];
    std::vector<std::string> targets = split(targetsParam, ',');

    for (size_t i = 0; i < targets.size(); ++i)
    {
        std::string targetName = targets[i];
        if (targetName[0] == '#' || targetName[0] == '&')
        {
            Channel *channel = server->getChannel(targetName);
            if (!channel)
            {
                client->sendMessage(":localhost " ERR_NOSUCHNICK " " + client->getNickName() + " " + targetName + " :No such nick/channel\r\n");
                continue;
            }
            if (!channel->isMember(*client))
            {
                client->sendMessage(":localhost " ERR_CANNOTSENDTOCHAN " " + client->getNickName() + " " + targetName + " :Cannot send to channel\r\n");
                continue;
            }
            std::string msgToSend = ":" + client->getNickName() + "!" + client->getUserName() + "@" + client->getHostName() + " PRIVMSG " + targetName + " :" + message + "\r\n";
            const std::map<std::string, Client *> &members = channel->getMembers();
            for (std::map<std::string, Client *>::const_iterator it = members.begin(); it != members.end(); ++it)
            {
                if (it->second != client)
                    it->second->sendMessage(msgToSend);
            }
        }
        else // si jamais on veut envoyer un message a un autre client au lieu d'envoyer un message dans un channel donne
        {
            Client *targetClient = server->getClientByNick(targetName);
            if (!targetClient)
            {
                client->sendMessage(":localhost " ERR_NOSUCHNICK " " + client->getNickName() + " " + targetName + " :No such nick/channel\r\n");
                continue;
            }
            std::string msgToSend = ":" + client->getNickName() + "!" + client->getUserName() + "@" + client->getHostName() + " PRIVMSG " + targetName + " :" + message + "\r\n";
            targetClient->sendMessage(msgToSend);
        }
    }
}

void handleKickCommand(Server *server, Client *client, Command cmd)
{
    if (cmd.getParams().size() < 2)
    {
        client->sendMessage(":localhost " ERR_NEEDMOREPARAMS " " + client->getNickName() + " KICK :Not enough parameters\r\n");
        return ;
    }

    std::string channelName = cmd.getParams()[0];
    std::string user = cmd.getParams()[1];

	if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&')) //verifie que le channel commence bien par # || &.
	{
		client->sendMessage(":localhost " ERR_BADCHANMASK " " + client->getNickName() + " " + channelName + " :Bad Channel Mask\r\n");
        return;
	}

	Channel *channel = server->getChannel(channelName);
    if (!channel)
    {
        client->sendMessage(":localhost " ERR_NOSUCHCHANNEL " " + client->getNickName() + " " + channelName + " :No such channel\r\n");
        return;
    }

    if (!channel->isMember(*client))
    {
        client->sendMessage(":localhost " ERR_NOTONCHANNEL " " + client->getNickName() + " " + channelName + " :You're not on that channel\r\n");
        return;
    }

    if (!channel->isOperator(*client))
    {
        client->sendMessage(":localhost " ERR_CHANOPRIVSNEEDED " " + client->getNickName() + " " + channelName + " :You're not channel operator\r\n");
        return;
    }

    Client *targetClient = server->getClientByNick(user);
    if (!targetClient || !channel->isMember(*targetClient))
    {
        client->sendMessage(":localhost " ERR_USERNOTINCHANNEL " " + client->getNickName() + " " + user + " " + channelName + " :They aren't on that channel\r\n");
        return;
    }

    std::string reason = (cmd.getParams().size() > 2) ? cmd.getParams()[2] : user;
    std::string kickMsg = ":" + client->getNickName() + "!" + client->getUserName() + "@" + client->getHostName() + " KICK " + channelName + " " + user + " :" + reason + "\r\n";

    const std::map<std::string, Client *> &members = channel->getMembers();
    for (std::map<std::string, Client *>::const_iterator it = members.begin(); it != members.end(); ++it)
        it->second->sendMessage(kickMsg);

    channel->removeMember(user);
    targetClient->leaveChannel(channel);
    server->removeChannelIfEmpty(channel);
}

void handleInviteCommand(Server *server, Client *client, Command cmd)
{
    if (cmd.getParams().size() < 2)
    {
        client->sendMessage(":localhost " ERR_NEEDMOREPARAMS " " + client->getNickName() + " INVITE :Not enough parameters\r\n");
        return ;
    }

    std::string nickname = cmd.getParams()[0];
    std::string channelName = cmd.getParams()[1];

	if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&'))
	{
		client->sendMessage(":localhost " ERR_BADCHANMASK " " + client->getNickName() + " " + channelName + " :Bad Channel Mask\r\n");
        return;
	}

	Channel *channel = server->getChannel(channelName);
    if (!channel)
    {
        client->sendMessage(":localhost " ERR_NOSUCHCHANNEL " " + client->getNickName() + " " + channelName + " :No such channel\r\n");
        return;
    }
    if (!channel->isMember(*client))
    {
        client->sendMessage(":localhost " ERR_NOTONCHANNEL " " + client->getNickName() + " " + channelName + " :You're not on that channel\r\n");
        return;
    }
    if (!channel->isOperator(*client))
    {
        client->sendMessage(":localhost " ERR_CHANOPRIVSNEEDED " " + client->getNickName() + " " + channelName + " :You're not channel operator\r\n");
        return;
    }
    Client *targetClient = server->getClientByNick(nickname);
	if (!targetClient)
	{
		client->sendMessage(":localhost " ERR_NOSUCHNICK " " + client->getNickName() + " " + nickname + " :No such nick/channel\r\n");
		return ;
	}
	if (channel->isMember(*targetClient))
	{
		client->sendMessage(":localhost " ERR_USERONCHANNEL " " + client->getNickName() + " " + nickname + " " + channelName + " :is already on channel\r\n");
		return ;
	}
    
    channel->addInvit(targetClient);
    client->sendMessage(":localhost " RPL_INVITING " " + client->getNickName() + " " + nickname + " " + channelName + "\r\n");
    targetClient->sendMessage(":" + client->getNickName() + "!" + client->getUserName() + "@" + client->getHostName() + " INVITE " + nickname + " :" + channelName + "\r\n");
}

void handleTopicCommand(Server *server, Client *client, Command cmd)
{
    if (cmd.getParams().empty())
    {
        client->sendMessage(":localhost " ERR_NEEDMOREPARAMS " " + client->getNickName() + " TOPIC :Not enough parameters\r\n");
        return;
    }

    std::string channelName = cmd.getParams()[0];
    Channel *channel = server->getChannel(channelName);

    if (!channel)
    {
        client->sendMessage(":localhost " ERR_NOSUCHCHANNEL " " + client->getNickName() + " " + channelName + " :No such channel\r\n");
        return;
    }

    if (!channel->isMember(*client))
    {
        client->sendMessage(":localhost " ERR_NOTONCHANNEL " " + client->getNickName() + " " + channelName + " :You're not on that channel\r\n");
        return;
    }

    // si on a un seul argument on affiche seulement le topic sinon
    if (cmd.getParams().size() == 1)
    {
        if (channel->getTopic().empty())
            client->sendMessage(":localhost " RPL_NOTOPIC " " + client->getNickName() + " " + channelName + " :No topic is set\r\n");
        else
            client->sendMessage(":localhost " RPL_TOPIC " " + client->getNickName() + " " + channelName + " :" + channel->getTopic() + "\r\n");
        return;
    }

	// si le channel esten mode t seul les op peuvent modifier le topic
    if (channel->IsTopicRestricted() && !channel->isOperator(*client))
    {
        client->sendMessage(":localhost " ERR_CHANOPRIVSNEEDED " " + client->getNickName() + " " + channelName + " :You're not channel operator\r\n");
        return;
    }

    std::string newTopic = cmd.getParams()[1];
    channel->setTopic(client, newTopic);

    // envoyer a tous les membres du channel le nouveau topic
    std::string msg = ":" + client->getNickName() + "!" + client->getUserName() + "@" + client->getHostName() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
    const std::map<std::string, Client *> &members = channel->getMembers();
    for (std::map<std::string, Client *>::const_iterator it = members.begin(); it != members.end(); ++it)
        it->second->sendMessage(msg);
}

void handleNoticeCommand(Server *server, Client *client, Command cmd)//meme chose que privmsg mais sans les erreurs
{
    if (cmd.getParams().size() < 2)
        return;

    std::string targetsParam = cmd.getParams()[0];
    std::string message = cmd.getParams()[1];
    std::vector<std::string> targets = split(targetsParam, ',');

    for (size_t i = 0; i < targets.size(); ++i)
    {
        std::string targetName = targets[i];
        if (targetName[0] == '#' || targetName[0] == '&')
        {
            Channel *channel = server->getChannel(targetName);
            if (!channel)
                continue;
            
            if (!channel->isMember(*client))
                continue;

            std::string msgToSend = ":" + client->getNickName() + "!" + client->getUserName() + "@" + client->getHostName() + " NOTICE " + targetName + " :" + message + "\r\n";
            const std::map<std::string, Client *> &members = channel->getMembers();
            for (std::map<std::string, Client *>::const_iterator it = members.begin(); it != members.end(); ++it)
            {
                if (it->second != client)
                    it->second->sendMessage(msgToSend);
            }
        }
        else
        {
            Client *targetClient = server->getClientByNick(targetName);
            if (!targetClient)
                continue;
            
            std::string msgToSend = ":" + client->getNickName() + "!" + client->getUserName() + "@" + client->getHostName() + " NOTICE " + targetName + " :" + message + "\r\n";
            targetClient->sendMessage(msgToSend);
        }
    }
}

//    RPL_BANLIST                     RPL_ENDOFBANLIST

void handleModeCommand(Server *server, Client *client, Command cmd)
{
    if (cmd.getParams().size() < 2)
    {
        client->sendMessage(":localhost " ERR_NEEDMOREPARAMS " " + client->getNickName() + " MODE :Not enough parameters\r\n");
        return;
    }
    
    std::string channelName = cmd.getParams()[0];
    Channel *channel = server->getChannel(channelName);
    
    if (!channel)
    {
        client->sendMessage(":localhost " ERR_NOSUCHCHANNEL " " + client->getNickName() + " " + channelName + " :No such channel\r\n");
        return;
    }
    if (!channel->isMember(*client))
    {
        client->sendMessage(":localhost " ERR_NOTONCHANNEL " " + client->getNickName() + " " + channelName + " :You're not on that channel\r\n");
        return;
    }
    if (!channel->isOperator(*client))
    {
        client->sendMessage(":localhost " ERR_CHANOPRIVSNEEDED " " + client->getNickName() + " " + channelName + " :You're not channel operator\r\n");
        return;
    }
    
    std::string modeString = cmd.getParams()[1];
    if (modeString.empty() || (modeString[0] != '+' && modeString[0] != '-'))
    {
        client->sendMessage(":localhost " ERR_UNKNOWNMODE " " + client->getNickName() + " :Unknown MODE flag\r\n");
        return;
    }
    
    char sign = modeString[0];
    size_t paramIndex = 2;
    std::string appliedModes = "";
    std::string modeParams = "";
    
    for (size_t i = 1; i < modeString.length(); i++)
    {
        char mode = modeString[i];
        
        if (mode != 'i' && mode != 't' && mode != 'k' && mode != 'l' && mode != 'o') // verifier si le mode est valide
        {
            client->sendMessage(":localhost " ERR_UNKNOWNMODE " " + client->getNickName() + " " + std::string(1, mode) + " :is unknown mode char to me\r\n");
            continue;
        }
        std::string param = "";
        
        // Modes qui necessitent un parametre
        if ((sign == '+' && (mode == 'k' || mode == 'l' || mode == 'o')) ||
            (sign == '-' && mode == 'o'))
        {
            if (paramIndex >= cmd.getParams().size())
            {
                client->sendMessage(":localhost " ERR_NEEDMOREPARAMS " " + client->getNickName() + " MODE :Not enough parameters\r\n");
                continue;
            }
            param = cmd.getParams()[paramIndex++];
        }
        
        // Appliquer le mode
        Channel::ModeStatus status = channel->applyMod(client, sign, mode, param);
        
        // Gerer les erreurs retournees par applyMod
        if (status != Channel::MODE_OK)
        {
            switch (status)
            {
                case Channel::NOT_OPERATOR:
                    client->sendMessage(":localhost " ERR_CHANOPRIVSNEEDED " " + client->getNickName() + " " + channelName + " :You're not channel operator\r\n");
                    break;
                case Channel::MEMBER_NOT:
                    client->sendMessage(":localhost " ERR_USERNOTINCHANNEL " " + client->getNickName() + " " + param + " " + channelName + " :They aren't on that channel\r\n");
                    break;
                case Channel::PASS_ACTIVED:
                    client->sendMessage(":localhost " ERR_KEYSET " " + client->getNickName() + " " + channelName + " :Channel key already set\r\n");
                    break;
                default:
                    break;
            }
            continue;
        }
        
        // Si le mode a ete applique avec succes, l'ajouter a la liste
        appliedModes += mode;
        if (!param.empty() && (mode == 'k' || mode == 'l' || mode == 'o'))
        {
            modeParams += " " + param;
        }
    }
    if (!appliedModes.empty())
    {
        std::string modeChange = ":" + client->getNickName() + " MODE " + channelName + " " + sign + appliedModes + modeParams + "\r\n";
            const std::map<std::string, Client *> &members = channel->getMembers();
            for (std::map<std::string, Client *>::const_iterator it = members.begin(); it != members.end(); ++it)
            {
                if (it->second != client)
                    it->second->sendMessage(modeChange);
            }
    }
}