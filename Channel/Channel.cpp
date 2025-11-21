/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiteixei <kiteixei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 12:45:55 by kiteixei          #+#    #+#             */
/*   Updated: 2025/11/21 15:57:10 by kiteixei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel() {}

Channel::~Channel() {}

std::string Channel::getName() const { return (_name); }

std::string Channel::getTopic() const { return (_topic); }

std::string Channel::getTopicAuthor() const { return (_topicAuthor); }

time_t Channel::getTopicTimesStamp() const { return (_topicTimestamp); }

std::string Channel::getKey() const { return (_key); }

int Channel::getUserLimit() const { return (_userLimit); }

bool Channel::IsHasKey() {
  if (_hasKey)
    return (true);
  return (false);
}

bool Channel::IsLimitSet() {
  if (_limitSet)
    return (true);
  return (false);
}

bool Channel::IsInviteOnly() {
  if (_inviteOnly)
    return (true);
  return (false);
}

bool Channel::IsTopicRestricted() {
  if (_topicRestricted)
    return (true);
  return (false);
}

bool Channel::IsHasKeyOptionK() {
  if (_hasKeyOptionK)
    return (true);
  return (false);
}

bool Channel::isOperator(const Client &c) {
  if (_operator.find(c.getUserName()) != _operator.end())
    return (true);
  return (false);
}

bool Channel::isMember(const Client &c) {
  if (_members.find(c.getUserName()) != _members.end())
    return (true);
  return (false);
}

Channel::MemberStatus Channel::addMember(Client *client) {
  MembersMap::iterator it = _members.find(client->getNickName());

  if (it != _members.end())
    return (MEMBER_ALREADY);

  else if (_members.size() == 0) {
    _operator.insert(client->getNickName());
    _members[client->getNickName()] = client;
    return (MEMBER_OP_AUTOPROMOTE);
  } else {
    _members[client->getNickName()] = client;
    return (MEMBER_OK);
  }
  return (MEMBER_NOT_FOUND);
}

Channel::MemberStatus Channel::removeMember(std::string &nick) {
  MembersMap::iterator it = _members.find(nick);

  if (it != _members.end()) {
    _members.erase(it);
    return (MEMBER_OK);
  }
  return (MEMBER_NOT_FOUND);
}

Channel::OperatorStatus Channel::addOperator(Client *client) {
  if (isMember(*client) == true) {
    if (isOperator(*client) == true)
      return (OP_ALREADY);
    else {
      _operator.insert(client->getNickName());
      return (OP_OK);
    }
  }
  return (OP_NOT_FOUND);
}

Channel::OperatorStatus Channel::removeOperator(Client *client) {
  if (isOperator(*client) == true) {
    _operator.erase(_operator.find(client->getUserName()));
    return (OP_OK);
  }
  return (OP_NOT_FOUND);
}

Channel::InvitedStatus Channel::addInvit(Client *client) {

  listInvit::iterator it = _listInvit.find(client->getUserName());

  if (it != _listInvit.end())
    return (INVITE_ALREADY);
  else {
    _listInvit.insert(client->getNickName());
    return (INVIT_OK);
  }
  return (INVITE_NOT_FOUND);
}

Channel::InvitedStatus Channel::removeInvit(std::string &nick) {

  listInvit::iterator it = _listInvit.find(nick);

  if (it != _listInvit.end()) {
    _listInvit.erase(it);
    return REMOVE_INVIT_OK;
  }
  return REMOVE_INVIT_NOT_FOUND;
}

std::ostream &operator<<(std::ostream &os, const Channel &s) {
  os << s.getName();
  os << s.getKey();
  os << s.getTopic();
  os << s.getUserLimit();
  os << s.getTopicTimesStamp();
  return (os);
}
