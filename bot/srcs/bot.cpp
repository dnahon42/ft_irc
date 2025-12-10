/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dnahon <dnahon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 20:43:56 by dnahon            #+#    #+#             */
/*   Updated: 2025/12/10 18:18:59 by dnahon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/bot.hpp"

static int g_fd = 0;

static std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
        tokens.push_back(token);
    return tokens;
}

void signalHandler(int signum)
{
	(void)signum;
	if (g_fd)
        close(g_fd);
}

int	main(int ac, char **av)
{
	if (ac != 7)
	{
		std::cerr << "Usage: " << av[0] << " <hostname> <port> <password> <nick> <user> <channels>" << std::endl;
		return (1);
	}
	if (std::atoi(av[2]) <= 0)
	{
		std::cerr << "Port must be higher than 0" << std::endl;
		return (1);
	}
	std::string hostname = av[1];
	unsigned short port = static_cast<unsigned short>(std::atoi(av[2]));
	std::string password = av[3];
	std::string nick = av[4];
	std::string user = av[5];
    std::vector<std::string> channels = split(av[6], ',');

	struct hostent *host = gethostbyname(hostname.c_str());
	if (!host)
	{
		std::cerr << "Error: could not resolve hostname: " << hostname << std::endl;
		return (1);
	}

	struct sockaddr_in botaddress;
	std::memset(&botaddress, 0, sizeof(botaddress));
	botaddress.sin_family = AF_INET;
	botaddress.sin_port = htons(port);
	botaddress.sin_addr = *((struct in_addr *)host->h_addr);
	try
	{
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1)
        {
            int saved_errno = errno;
            std::string msg = "socket() failed: ";
            throw std::runtime_error(msg + std::strerror(saved_errno));
        }
        g_fd = fd;
	    signal(SIGINT, signalHandler);
		if (::connect(fd, (struct sockaddr *)&botaddress, sizeof(botaddress)) ==
			-1)
		{
			int saved_errno = errno;
			::close(fd);
			fd = -1;
			std::string msg = "connect() failed: ";
			throw std::runtime_error(msg + std::strerror(saved_errno));
		}
        password = "PASS " + password + "\r\n";
        nick = "NICK " + nick + "\r\n";
        user = "USER " + user + "\r\n";
		::send(fd, password.data(), password.size(), 0);
		::send(fd, nick.data(), nick.size(), 0);
		::send(fd, user.data(), user.size(), 0);
        for (size_t i = 0; i < channels.size(); ++i) 
        {
            std::string channelName = "JOIN " + channels[i] + "\r\n";
            ::send(fd, channelName.data(), channelName.size(), 0);
        }
        while (true)
        {
            char buf[4096];
            std::memset(buf, 0, sizeof(buf));
            ssize_t bytes_received = ::recv(fd, buf, sizeof(buf) - 1, 0);
            if (bytes_received <= 0)
                break;
            std::string message = buf;
	        std::string part;
	        std::stringstream ss(message);
            std::string channelSentTo;
            if (message[0] == ':')
                ss >> part;
            ss >> part;
            if (part == "PRIVMSG")
            {
                if (ss >> part)
                    channelSentTo = part;
                if (ss >> part)
                {
                    if (message.find("quoi", message.size() - 6) != std::string::npos)
                    {
                        std::string response = "PRIVMSG " + channelSentTo + " coubeh\r\n";
		                ::send(fd, response.data(), response.size(), 0);
                    }
                }
            }     
        }
        close(fd);
	}
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
	return (0);
}