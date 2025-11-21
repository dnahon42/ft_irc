/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiteixei <kiteixei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 10:45:59 by kiteixei          #+#    #+#             */
/*   Updated: 2025/11/21 14:44:12 by kiteixei         ###   ########.fr       */
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
typedef std::set<std::string> listInvit;
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
  listInvit _listInvit;

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
  bool isOperator(const Client &c);
  bool isMember(const Client &c);

  enum MemberStatus {
    MEMBER_OP_AUTOPROMOTE,
    MEMBER_OK,
    MEMBER_ALREADY,
    MEMBER_NOT_FOUND
  };
  enum OperatorStatus { OP_OK, OP_ALREADY, OP_NOT_FOUND, OP_NOT_MEMBER };
  enum InvitedStatus {
    REMOVE_INVIT_NOT_FOUND,
    REMOVE_INVIT_OK,
    INVIT_OK,
    INVITE_ALREADY,
    INVITE_NOT_FOUND
  };
  enum MembRemove { MEMB_REMOVE_OK, MEMB_REMOVE_NOT_FOUND };
  InvitedStatus removeInvit(std::string &nick);
  InvitedStatus isInvited(std::string &nick);
  InvitedStatus addInvit(Client *client);
  OperatorStatus addOperator(Client *client);
  OperatorStatus removeOperator(Client *client);
  MemberStatus addMember(Client *client);
  MemberStatus removeMember(std::string &nick);
};
std::ostream &operator<<(std::ostream &os, const Channel &s);
#endif
