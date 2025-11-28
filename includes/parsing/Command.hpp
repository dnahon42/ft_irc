/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tniagolo <tniagolo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 01:35:03 by tniagolo          #+#    #+#             */
/*   Updated: 2025/11/28 03:14:38 by tniagolo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../common/Utils.hpp"

class Client;

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