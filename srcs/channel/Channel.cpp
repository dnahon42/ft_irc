/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dnahon <dnahon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 12:45:55 by kiteixei          #+#    #+#             */
/*   Updated: 2025/12/05 20:22:48 by dnahon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/common/Channel.hpp"

Channel::Channel(const std::string &name)
    : _name(name), _topic(""), _topicAuthor(""), _topicTimestamp(0),
      _passWord(""), _memberCount(0), _userLimit(0),
      _limitSet(false), _inviteOnly(false),
      _topicRestricted(false), _hasPassword(false){} 

Channel::~Channel() {}

std::string Channel::getName() const { return (_name); }

int Channel::getMemberCount() const { return (_memberCount); } // ajout

std::string Channel::getTopic() const { return (_topic); }

std::string Channel::getTopicAuthor() const { return (_topicAuthor); }

time_t Channel::getTopicTimesStamp() const { return (_topicTimestamp); }


std::string Channel::getPassword() const { return (_passWord); } // rajoute

const MembersMap &Channel::getMembers() const { return (_members); } // rajoute

int Channel::getUserLimit() const { return (_userLimit); }

bool Channel::IsLimitSet() { return (_limitSet); }

bool Channel::IsInviteOnly() { return (_inviteOnly); }

bool Channel::IsTopicRestricted() { return (_topicRestricted); }

bool Channel::hasPassword() { return (_hasPassword); }

bool Channel::isOperator(const Client &c) {
  if (_operator.find(c.getNickName()) != _operator.end())
    return (true);
  return (false);
}

bool Channel::isMember(const Client &c) {
  if (_members.find(c.getNickName()) != _members.end())
    return (true);
  return (false);
}

void Channel::setAuthor(std::string newAuthor) { _topicAuthor = newAuthor; }

void Channel::setInviteOnly(bool status) { _inviteOnly = status; }

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

Channel::ModeStatus Channel::setPassword(Client *client,
                                         std::string &password) {
  if (!isOperator(*client))
    return (NOT_OPERATOR);
  if (hasPassword())
    return (PASS_ACTIVED);
  if (password.empty())
    return (PASS_EMPTY);
  _passWord = password;
  _hasPassword = true;
  return (PASS_SET_OK);
}
Channel::ModeStatus Channel::clearPassword(Client *client) {

  if (!isOperator(*client))
    return (NOT_OPERATOR);
  if (!hasPassword())
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

  if (isMember(*client))
    return ALREADY_MEMB;

  if (_inviteOnly) {
    if (_listInvit.find(client->getNickName()) == _listInvit.end())
      return INVIT_REQUIRED;
  }

  if (hasPassword()) {
    if (password.empty())
      return PASSWORD_EMPTY;
    if (password != getPassword())
      return PASSWORD_INCORECT;
  }

  if (_limitSet && _memberCount >= _userLimit)
    return FULL_CHANNEL;

  return JOIN_OK;
}

void Channel::setTopicBool(bool status) { _topicRestricted = status; }

Channel::ModeStatus Channel::applyMod(Client *client, char sign, char mode,
                                      std::string &param) {
  if (sign != '-' && sign != '+')
    return INVALID_SIGN;

  if (mode != 'i' && mode != 't' && mode != 'k' && mode != 'l' && mode != 'o')
    return UNKNOW_MODE;

  if ((mode == 'i' || mode == 't') && !param.empty())
    return PARAM_NOT_ALLOWED;

  if (sign == '+' && (mode == 'k' || mode == 'l' || mode == 'o') &&
      param.empty())
    return PARAM_REQUIRED;

  if (sign == '-' && mode == 'o' && param.empty())
    return PARAM_REQUIRED;

  if (sign == '-' && (mode == 'k' || mode == 'l') && !param.empty())
    return PARAM_NOT_ALLOWED;

  if (sign == '+') {
    switch (mode) {

    case 'i':
      setInviteOnly(true);
      return MODE_OK;

    case 't':
      setTopicBool(true);
      return MODE_OK;

    case 'k':
      return setPassword(client, param);

    case 'o': {
      OperatorStatus st = setOperator(client, param);
      if (st == OP_OK)
        return MODE_OK;
      if (st == OP_NOT_FOUND)
        return NOT_OPERATOR;
      if (st == OP_ALREADY)
        return ALREADY_OP;
      if (st == OP_NOT_MEMBER)
        return MEMBER_NOT;
      return UNKNOW_MODE;
    }

    case 'l': {
      int limit = atoi(param.c_str());
      LimitStatus st = setLimit(client, limit);
      if (st == LIMIT_SET_OK)
        return MODE_OK;
      if (st == LIMIT_INVALID)
        return INVALID_LIMIT;
      if (st == LIMIT_NOT_OP)
        return NOT_OPERATOR;
      return UNKNOW_MODE;
    }
    }
  }

  if (sign == '-') {
    switch (mode) {

    case 'i':
      setInviteOnly(false);
      return MODE_OK;

    case 't':
      setTopicBool(false);
      return MODE_OK;

    case 'k':
      return clearPassword(client);

    case 'o': {
      OperatorStatus st = clearOperator(client, param);
      if (st == OP_OK)
        return MODE_OK;
      if (st == OP_NOT_FOUND)
        return CLIENT_NOT_OP;
      if (st == OP_NOT_OP)
        return NOT_OPERATOR;
      return UNKNOW_MODE;
    }

    case 'l': {
      LimitStatus st = clearLimit(client);
      if (st == LIMIT_UNSET_OK)
        return MODE_OK;
      if (st == LIMIT_NOT_OP)
        return NOT_OPERATOR;
      if (st == LIMIT_NOT_SET)
        return MODE_NOT_ACTIVED;
      return UNKNOW_MODE;
    }
    }
  }

  return MODE_OK;
}

void Channel::applyJoin(Client *client) {

  addMember(client);
  if (_inviteOnly == true) {
    listInvit::iterator i = _listInvit.find(client->getNickName());
    if (i != _listInvit.end())
      _listInvit.erase(client->getNickName());
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
  return (OP_OK);
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

Channel::MemberStatus Channel::addMember(Client *client) {
  MembersMap::iterator it = _members.find(client->getNickName());

  if (it != _members.end())
    return (MEMBER_ALREADY);

  else if (_memberCount == 0) {
    _operator.insert(client->getNickName());
    _members[client->getNickName()] = client;
    _memberCount++;
    return (MEMBER_OP_AUTOPROMOTE);
  } else {
    _members[client->getNickName()] = client;
    _memberCount++;
    return (MEMBER_OK);
  }
}

Channel::MemberStatus Channel:: removeMember(const std::string &nick) {
  MembersMap::iterator it = _members.find(nick);

  if (it != _members.end()) {
    bool wasOperator = (_operator.find(nick) != _operator.end());

    _members.erase(it);
    _operator.erase(nick);
    _memberCount--;
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
    _operator.erase(_operator.find(client->getNickName()));
    return (OP_OK);
  }
  return (OP_NOT_FOUND);
}

Channel::InvitedStatus Channel::addInvit(Client *client) {

  listInvit::iterator it = _listInvit.find(client->getNickName());

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
  os << s.getPassword();
  os << s.getTopic();
  os << s.getUserLimit();
  os << s.getTopicTimesStamp();
  return (os);
}