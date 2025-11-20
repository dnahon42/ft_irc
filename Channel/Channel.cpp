/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiteixei <kiteixei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 12:45:55 by kiteixei          #+#    #+#             */
/*   Updated: 2025/11/21 02:07:24 by kiteixei         ###   ########.fr       */
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

Channel::MemberAdd Channel::addMember(Client *client) {
  MembersMap::iterator it = _members.find(client->getNickName());

  if (it != _members.end())
    return (ALREADY_MEMBER);

  else if (_members.size() == 0) {
    _operator.insert(client->getNickName());
    _members[client->getNickName()] = client;
    return (OPERATOR_VIA_MEMBER_ADD);
  } else {
    _members[client->getNickName()] = client;
    return (MEMBER_OK);
  }
}

Channel::MembRemove Channel::removeMember(std::string &nick) {
  MembersMap::iterator it = _members.find(nick);

  if (it != _members.end()) {
    _members.erase(it);
    return (MEMB_REMOVE_OK);
  }
  return (MEMB_REMOVE_NOT_FOUND);
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

Channel::OperatorAdd Channel::addOperator(Client *client) {
  if (isOperator(*client) == true)
    return (ALREADY_OPERATOR);
  else {
    _operator.insert(client->getNickName());
    return (OPERATOR_OK);
  }
  return (OPERATOR_NOT_FOUND);
}

Channel::OperatorRemove Channel::removeOperator(Client *client) {
  if (isOperator(*client) == true) {
    _operator.erase(_operator.find(client->getUserName()));
    return (OPERATOR_REMOVE_OK);
  }
  return (OPERATOR_REMOVE_NOT_FOUND);
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

Channel::RemoveInvit Channel::removeInvit(std::string &nick) {

  listInvit::iterator it = _listInvit.find(nick);

  if (it != _listInvit.end()) {
    _listInvit.erase(it);
    return REMOVE_OK;
  }
  return REMOVE_NOT_FOUND;
}

std::ostream &operator<<(std::ostream &os, const Channel &s) {
  os << s.getName();
  return (os);
}
