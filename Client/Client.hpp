/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiteixei <kiteixei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 23:59:13 by kiteixei          #+#    #+#             */
/*   Updated: 2025/11/21 17:12:01 by kiteixei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <ostream>
#include <string>

class Client {

private:
  std::string _nickName;
  std::string _userName;
  std::string _hostName;
  int _fd;
  time_t _userTime;

public:
  Client(int fd) : _nickName(""), _userName(""), _hostName("") {};
  ~Client() {};
  std::string getNickName() const;
  std::string getUserName() const;
  std::string getHostName() const;
  void setNickName(std::string nickName);
  void setUserName(std::string userName);
  void setHostName(std::string hostName);
  int getFd();
  time_t userTime();
};

std::ostream &operator<<(std::ostream const &o, const Client &client);
#endif
