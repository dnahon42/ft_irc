/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dnahon <dnahon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 01:35:03 by tniagolo          #+#    #+#             */
/*   Updated: 2025/12/05 19:32:55 by dnahon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../common/Utils.hpp"

#define RPL_WELCOME "001"
#define RPL_AWAY "301"
#define RPL_CHANNELMODEIS "324"
#define RPL_NOTOPIC "331"
#define RPL_TOPIC "332"
#define RPL_INVITING "341"
#define RPL_NAMREPLY "353"
#define RPL_ENDOFNAMES "366"
#define RPL_BANLIST "367"
#define RPL_ENDOFBANLIST "368"

#define ERR_NOSUCHNICK "401"
#define ERR_NOSUCHCHANNEL "403"
#define ERR_CANNOTSENDTOCHAN "404"
#define ERR_TOOMANYCHANNELS "405"
#define ERR_TOOMANYTARGETS "407"
#define ERR_NORECIPIENT "411"
#define ERR_NOTEXTTOSEND "412"
#define ERR_NOTOPLEVEL "413"
#define ERR_WILDTOPLEVEL "414"
#define ERR_NONICKNAMEGIVEN "431"
#define ERR_ERRONEUSNICKNAME "432"
#define ERR_NICKNAMEINUSE "433"
#define ERR_USERNOTINCHANNEL "441"
#define ERR_NOTONCHANNEL "442"
#define ERR_USERONCHANNEL "443"
#define ERR_NEEDMOREPARAMS "461"
#define ERR_ALREADYREGISTRED "462"
#define ERR_PASSWDMISMATCH "464"
#define ERR_KEYSET "467"
#define ERR_CHANNELISFULL "471"
#define ERR_UNKNOWNMODE "472"
#define ERR_INVITEONLYCHAN "473"
#define ERR_BANNEDFROMCHAN "474"
#define ERR_BADCHANNELKEY "475"
#define ERR_BADCHANMASK "476"
#define ERR_CHANOPRIVSNEEDED "482"



class Client;
class Server;

class Command
{
  private:
	std::string _prefix;
	std::string _command;
	std::vector<std::string> _params;

  public:
	Command();
	~Command();

	std::string getPrefix() const;
	std::string getCommand() const;
	const std::vector<std::string> &getParams() const;

	void setPrefix(const std::string &prefix);
	void setCommand(const std::string &command);
	void addParam(const std::string &param);

};

void handlePingCommand(Client *client, Command cmd);
void handleNickCommand(Server *server, Client *client, Command cmd);
void handleUserCommand(Server *server, Client *client, Command cmd);
void handlePassCommand(Client *client, Command cmd);
void handleQuitCommand(Server *server, Client *client, Command cmd);
void handleJoinCommand(Server *server, Client *client, Command cmd);
void handlePartCommand(Server *server, Client *client, Command cmd);
void handlePrivmsgCommand(Server *server, Client *client, Command cmd);
void handleKickCommand(Server *server, Client *client, Command cmd);
void handleInviteCommand(Server *server, Client *client, Command cmd);
void handleTopicCommand(Server *server, Client *client, Command cmd);
void handleModeCommand(Server *server, Client *client, Command cmd);
void handleNoticeCommand(Server *server, Client *client, Command cmd);