/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiteixei <kiteixei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 12:45:55 by kiteixei          #+#    #+#             */
/*   Updated: 2025/11/22 23:51:32 by kiteixei         ###   ########.fr       */
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

bool Channel::IsHasKey() { return (_hasKey); }

bool Channel::IsLimitSet() { return (_limitSet); }

bool Channel::IsInviteOnly() { return (_inviteOnly); }

bool Channel::IsTopicRestricted() { return (_topicRestricted); }

bool Channel::OptionK() { return (_hasOptionK); }
bool Channel::isPrivate() const { return (_isPrivate); }

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
void Channel::setAuthor(std::string newAuthor) { _topicAuthor = newAuthor; }

void Channel::setMsgTopic(std::string newTopic) { _topic = newTopic; }

Channel::TopicStatus Channel::setTopic(Client *client,
                                       const std::string &newTopic) {
  if (IsTopicRestricted() == true && isOperator(*client) == false)
    return TOPIC_NEED_OP;

  if (newTopic.empty())
    return TOPIC_EMPTY;

  setMsgTopic(newTopic);
  setAuthor(client->getNickName());
  return TOPIC_OK;
}

bool Channel::setBoolPass() { return (_hasPassword); }

std::string Channel::getPassword() const { return (_passWord); }

Channel::ModeStatus Channel::setPassword(Client *client,
                                         std::string &password) {
  if (isOperator(*client)) {
    if (password.empty())
      return (PASS_EMPTY);
    _passWord = password;
    _hasPassword = true;
    return (PASS_SET_OK);
  }
  return (NOT_OPERATOR);
}
Channel::ModeStatus Channel::clearPassword(Client *client) {

  if (!isOperator(*client))
    return (NOT_OPERATOR);
  if (!OptionK())
    return (PASS_ACTIVED);
  _passWord = "";
  _hasPassword = false;
  return (PASS_CLEAR);
}

Channel::LimitStatus Channel::setLimit(Client *client, int limit) {
  if (!isOperator(*client))
    return (LIMIT_NOT_OP);
  if (limit <= 0)
    return (LIMIT_INVALID);
  _userLimit = limit;
  _limitSet = true;
  return (LIMIT_SET_OK);
}

Channel::LimitStatus Channel::clearLimit(Client *client) {
  if (!isOperator(*client))
    return (LIMIT_NOT_OP);
  if (!IsLimitSet())
    return (LIMIT_NOT_SET);
  _userLimit = 0;
  _limitSet = false;
  return (LIMIT_UNSET_OK);
}

Channel::JoinStatus Channel::canJoin(Client *client,
                                     const std::string &password) {
  if (OptionK() == true) {
    if (password.empty())
      return (PASSWORD_EMPTY);
    if (password != getPassword())
      return (PASSWORD_INCORECT);
    return (PASSWORD_OK);
  }
}

Channel::OperatorStatus Channel::clearOperator(Client *client,
                                               std::string &nick) {
  OperatorMap::iterator i = _operator.find(nick);

  if (!isOperator(*client))
    return (OP_NOT_FOUND);
  if (i == _operator.end()) {
    return (OP_NOT_OP);
  }
  _operator.erase(i);
  return (OP_NOT_MEMBER);
}

Channel::OperatorStatus Channel::setOperator(Client *client,
                                             std::string &nick) {
  MembersMap::iterator it = _members.find(nick);
  OperatorMap::iterator i = _operator.find(nick);
  if (!isOperator(*client))
    return (OP_NOT_FOUND);
  if (it == _members.end())
    return (OP_NOT_MEMBER);
  if (i != _operator.end())
    return (OP_ALREADY);
  _operator.insert(nick);
  return (OP_OK);
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
