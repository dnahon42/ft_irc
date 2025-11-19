/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tniagolo <tniagolo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 14:56:42 by tniagolo          #+#    #+#             */
/*   Updated: 2025/11/18 15:05:52 by tniagolo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// Standard C++
#include <string>
#include <map>
#include <vector>
#include <iostream>

// Sockets
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>

// Signaux
#include <signal.h>

#include "Client.hpp"
#include "Socket.hpp"

class Server
{
    
};