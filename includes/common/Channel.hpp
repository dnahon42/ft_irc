/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dnahon <dnahon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 10:45:59 by kiteixei          #+#    #+#             */
/*   Updated: 2025/12/05 19:42:36 by dnahon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "../../includes/common/Client.hpp"
#include <iostream>
#include <map>
#include <ostream>
#include <set>
#include <string>

class Client;

typedef std::map<std::string, Client *> MembersMap;
typedef std::set<std::string> OperatorMap;
typedef std::set<std::string> listInvit;

class Channel {
private:
  std::string _name;
  std::string _topic;
  std::string _topicAuthor;
  time_t _topicTimestamp;
  std::string _passWord;

  int _memberCount;
  int _userLimit;
  bool _limitSet;
  bool _inviteOnly;
  bool _topicRestricted;
  bool _hasPassword;
  MembersMap _members;
  OperatorMap _operator;
  listInvit _listInvit;

public:
  Channel(const std::string &name);
  ~Channel();

  std::string getName() const;
  int getMemberCount() const; // on a rajoute ca car lorsque ca atteint 0 on doit supprimer le channel
  std::string getTopic() const;
  std::string getTopicAuthor() const;
  std::string getPassword() const;
  const MembersMap &getMembers() const; // on a rajoute

  time_t getTopicTimesStamp() const;
  
  int getUserLimit() const;
  
  void applyJoin(Client *client);

  bool IsLimitSet();
  bool IsInviteOnly();
  bool IsTopicRestricted();
  bool hasPassword();
  bool isOperator(const Client &c);
  bool isMember(const Client &c);
  void setInviteOnly(bool status);
  void setAuthor(std::string newAuthor);
  void setMsgTopic(std::string newTopic);
  void setTopicBool(bool status);
  bool setBoolPass();

  enum TopicStatus { TOPIC_OK, TOPIC_NEED_OP, TOPIC_EMPTY };
  enum MembRemove { MEMB_REMOVE_OK, MEMB_REMOVE_NOT_FOUND };
  enum JoinStatus {
    ALREADY_MEMB,
    PASSWORD_EMPTY,
    PASSWORD_OK,
    PASSWORD_INCORECT,
    NOT_OP,
    INVIT_REQUIRED,
    FULL_CHANNEL,
    JOIN_OK
  };
  enum ModeStatus {
    PASS_ACTIVED,
    PASS_CLEAR,
    PASS_SET_OK,
    NOT_OPERATOR,
    PASS_EMPTY,
    UNKNOW_MODE,
    PARAM_EMPTY,
    PARAM_NOT_ALLOWED,
    PARAM_REQUIRED,
    INVALID_SIGN,
    MEMBER_NOT,
    MODE_OK,
    ALREADY_OP,
    INVALID_LIMIT,
    CLIENT_NOT_OP,
    MODE_NOT_ACTIVED
  };
  enum MemberStatus {
    MEMBER_OP_AUTOPROMOTE,
    MEMBER_OK,
    MEMBER_ALREADY,
    MEMBER_NOT_FOUND
  };
  enum OperatorStatus {
    NOT_MEMBER,
    OP_OK,
    OP_ALREADY,
    OP_NOT_FOUND, 
    OP_REMOVE_OK,
    OP_NOT_OP,
    OP_NOT_MEMBER
  };
  enum LimitStatus {
    LIMIT_UNSET_OK,
    LIMIT_SET_OK,
    LIMIT_NOT_OP,
    LIMIT_NOT_SET,
    LIMIT_INVALID
  };
  enum InvitedStatus {
    REMOVE_INVIT_NOT_FOUND,
    REMOVE_INVIT_OK,
    INVIT_OK,
    INVITE_ALREADY,
    INVITE_NOT_FOUND
  };
 
  ModeStatus setPassword(Client *client, std::string &password);
  ModeStatus clearPassword(Client *client);
  ModeStatus applyMod(Client *client, char sign, char mode, std::string &param);

  JoinStatus canJoin(Client *client, const std::string &password);

  TopicStatus setTopic(Client *client, const std::string &newTopic);

  LimitStatus clearLimit(Client *client);
  LimitStatus setLimit(Client *client, int limit);

  OperatorStatus clearOperator(Client *client, std::string &nick);
  OperatorStatus setOperator(Client *client, std::string &nick);

  InvitedStatus removeInvit(std::string &nick);
  InvitedStatus isInvited(std::string &nick);
  InvitedStatus addInvit(Client *client);

  OperatorStatus addOperator(Client *client);
  OperatorStatus removeOperator(Client *client);

  MemberStatus addMember(Client *client);
  MemberStatus removeMember(const std::string &nick);
};
std::ostream &operator<<(std::ostream &os, const Channel &s);
#endif