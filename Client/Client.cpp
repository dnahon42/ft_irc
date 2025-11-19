/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiteixei <kiteixei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/20 00:24:21 by kiteixei          #+#    #+#             */
/*   Updated: 2025/11/20 01:10:09 by kiteixei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

std::string Client::getNickName() const { return (_nickName); }

int Client::getFd() { return (_fd); }

std::ostream &operator<<(std::ostream &s, const Client &client) {
  s << client.getNickName();
  return (s);
}
