/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiteixei <kiteixei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 12:45:55 by kiteixei          #+#    #+#             */
/*   Updated: 2025/11/22 01:05:18 by kiteixei         ###   ########.fr       */
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

bool Channel::IsHasKeyOptionK() { return (_hasKeyOptionK); }

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
