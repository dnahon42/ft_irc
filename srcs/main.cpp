/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dnahon <dnahon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 16:27:51 by dnahon            #+#    #+#             */
/*   Updated: 2025/12/05 16:27:57 by dnahon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/network/Server.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return (1);
	}

	unsigned short port = static_cast<unsigned short>(std::atoi(argv[1]));
	std::string password = argv[2];

	Server server("127.0.0.1", port, password);

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
