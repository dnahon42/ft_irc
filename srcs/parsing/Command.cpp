/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tniagolo <tniagolo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 02:13:57 by tniagolo          #+#    #+#             */
/*   Updated: 2025/11/28 03:21:37 by tniagolo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/parsing/Command.hpp"
#include "../../includes/common/Client.hpp"

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

// PASS, NICK, USER, PING, QUIT, JOIN, PRIVMSG, KICK, INVITE, TOPIC, MODE