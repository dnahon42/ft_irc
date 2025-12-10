/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dnahon <dnahon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 00:17:31 by tniagolo          #+#    #+#             */
/*   Updated: 2025/12/08 16:54:05 by dnahon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/parsing/Parser.hpp"

Command Parser::parse(const std::string &line)
{
	Command cmd;
	std::string part;
	std::stringstream ss(line);

	if (line.empty())
		return (cmd);
	if (line[0] == ':') // on recupere le prefix (l'emetteur du message)
	{
		ss >> part;
		cmd.setPrefix(part.substr(1));
	}

	if (ss >> part)
	{
		cmd.setCommand(part); // on recupere la commande execute
	}

	while (ss >> part) // tant q'uon a des parametres on les recupere
	{
		if (part[0] == ':') // si il y a ':' au debut du parametre, ca signifie que c'est le dernier parametre
		{
			std::string lastparam;
			getline(ss, lastparam);
			cmd.addParam(part.substr(1) + lastparam);
			break ;
		}
		else
		{
			cmd.addParam(part);
		}
	}
	return (cmd);
}
