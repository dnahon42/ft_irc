/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: copilot <copilot@example.com>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 05:00:00 by copilot           #+#    #+#             */
/*   Updated: 2025/11/23 05:00:00 by copilot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/network/Server.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return (1);
	}

	unsigned short port = static_cast<unsigned short>(std::atoi(argv[1]));
	std::string password = argv[2];

	Server server("127.0.0.1", port);

	if (!server.init())
	{
		std::cerr << "Failed to initialize server" << std::endl;
		return (1);
	}

	std::cout << "Server listening on port " << port << std::endl;
	std::cout << "Password: " << password << std::endl;

	server.run();

	return (0);
}
