/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dnahon <dnahon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 16:27:51 by dnahon            #+#    #+#             */
/*   Updated: 2025/12/14 17:49:37 by dnahon           ###   ########.fr       */
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

static int	get_color(int i, int len)
{
	return (76 + (180. * (float)i / (float)len));
}

static void	put_color_char(char c, int r, int g, int b)
{
	std::cout << "\e[1;38;2;" << r << ";" << g << ";" << b << "m" << c << "\e[m";
}

static void	put_gradient(const std::string &text)
{
	unsigned long	width = 0;
	int				height = 0;
	unsigned long	cur_w = 0;

	for (size_t k = 0; k < text.size(); k++)
	{
		if (text[k] == '\n')
		{
			height++;
			if (cur_w > width) width = cur_w;
			cur_w = 0;
		}
		else
			cur_w++;
	}
	if (cur_w > 0)
	{
		height++;
		if (cur_w > width) width = cur_w;
	}
	if (height == 0) return;

	int line = 0;
	int col = 0;
	for (size_t k = 0; k < text.size(); k++)
	{
		if (text[k] == '\n')
		{
			std::cout << std::endl;
			line++;
			col = 0;
		}
		else
		{
			put_color_char(text[k], get_color(line, height), 200, get_color(col, width));
			col++;
		}
	}
	if (col > 0)
		std::cout << std::endl;
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
	std::string ft_telegram = 
	"  ______ _______   _______    _                                 \n"
	" |  ____|__   __| |__   __|  | |                                \n"
	" | |__     | |       | | ___ | | ___  __ _ _ __ __ _ _ __ ___   \n"
	" |  __|    | |       | |/ _ \\| |/ _ \\/ _` | '__/ _` | '_ ` _ \\  \n"
	" | |       | |       | |  __/| |  __/ (_| | | | (_| | | | | | | \n"
	" |_|       |_|       |_|\\___||_|\\___|\\__, |_|  \\__,_|_| |_| |_| \n"
	"                                      __/ |                     \n"
	"                                     |___/                      ";
	put_gradient(ft_telegram);
	std::cout << "Server listening on port " << port << std::endl;
	std::cout << "Password: " << password << std::endl;

	server.run(100);

	std::cout << "\nServer shutting down gracefully..." << std::endl;
	g_server = NULL;

	return (0);
}
