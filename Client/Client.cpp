/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiteixei <kiteixei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/20 00:24:21 by kiteixei          #+#    #+#             */
/*   Updated: 2025/11/20 17:37:37 by kiteixei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

std::string Client::getNickName() const { return (_nickName); }

std::string Client::getHostName() const { return (_hostName); }

std::string Client::getUserName() const { return (_userName); }

void Client::setNickName(std::string nickName) { _nickName = nickName; }

void Client::setHostName(std::string HostName) { _hostName = HostName; }

int Client::getFd() { return (_fd); }

std::ostream &operator<<(std::ostream &s, const Client &client) {
  s << client.getNickName();
  return (s);
}
