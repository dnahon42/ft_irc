/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dnahon <dnahon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 16:27:51 by dnahon            #+#    #+#             */
/*   Updated: 2025/12/08 16:43:05 by dnahon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/network/Server.hpp"
#include <csignal>

static Server* g_server = NULL;

void signalHandler(int signum)
{
	(void)signum;
	if (g_server)
		g_server->stop();
}

int main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
		return (1);
	}
	if (std::atoi(av[1]) <= 0)
	{
		std::cerr << "Port must be higher than 0" << std::endl;
		return (1);
	}
	unsigned short port = static_cast<unsigned short>(std::atoi(av[1]));
	std::string password = av[2];

	Server server("127.0.0.1", port, password);
	g_server = &server;

	signal(SIGINT, signalHandler);

	if (!server.init())
	{
		std::cerr << "Failed to initialize server" << std::endl;
		g_server = NULL;
		return (1);
	}

	std::cout << "Server listening on port " << port << std::endl;
	std::cout << "Password: " << password << std::endl;

	server.run(100);

	std::cout << "\nServer shutting down gracefully..." << std::endl;
	g_server = NULL;

	return (0);
}
