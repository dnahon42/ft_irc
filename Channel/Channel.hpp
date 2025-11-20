/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiteixei <kiteixei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 10:45:59 by kiteixei          #+#    #+#             */
/*   Updated: 2025/11/20 17:21:15 by kiteixei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANELL_HPP
#define CHANELL_HPP

#include "../Client/Client.hpp"
#include <iostream>
#include <map>
#include <ostream>
#include <set>
#include <string>
class Client;

typedef std::map<std::string, Client *> MembersMap;
typedef std::set<std::string> OperatorMap;
class Channel {
private:
  std::string _name;
  std::string _topic;
  std::string _topicAuthor;
  time_t _topicTimestamp;

  std::string _key;
  int _userLimit;
  bool _isOperator;
  bool _isClient;
  bool _hasKey;
  bool _limitSet;
  bool _inviteOnly;
  bool _topicRestricted;
  bool _hasKeyOptionK;
  MembersMap _members;
  OperatorMap _operator;

public:
  Channel();
  ~Channel();

  std::string getName() const;
  std::string getTopic() const;
  std::string getTopicAuthor() const;

  time_t getTopicTimesStamp() const;
  std::string getKey() const;
  int getUserLimit() const;

  bool IsHasKey();
  bool IsLimitSet();
  bool IsInviteOnly();
  bool IsTopicRestricted();
  bool IsHasKeyOptionK();

  void addMember(Client *client);
  void invitMember(Client *client);
  void removeMember(std::string &nick);
  bool isOperator(const Client &c);
  bool isMember(const Client &c);
  void removeOperator(Client *client);
  void addOperator(Client *client);
};
std::ostream &operator<<(std::ostream &os, const Channel &s);
#endif
