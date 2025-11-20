/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiteixei <kiteixei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 10:45:59 by kiteixei          #+#    #+#             */
/*   Updated: 2025/11/21 02:21:10 by kiteixei         ###   ########.fr       */
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

  enum MemberAdd {
    OPERATOR_VIA_MEMBER_ADD,
    MEMBER_OK,
    ALREADY_MEMBER,
    MEMBER_NOT_FOUND
  };
  enum OperatorAdd { OPERATOR_OK, ALREADY_OPERATOR, OPERATOR_NOT_FOUND };
  enum OperatorRemove { OPERATOR_REMOVE_OK, OPERATOR_REMOVE_NOT_FOUND };
  enum InvitedStatus { INVIT_OK, INVITE_ALREADY, INVITE_NOT_FOUND };
  enum RemoveInvit { REMOVE_OK, REMOVE_NOT_FOUND };
  enum MembRemove { MEMB_REMOVE_OK, MEMB_REMOVE_NOT_FOUND };
  RemoveInvit removeInvit(std::string &nick);
  InvitedStatus isInvited(std::string &nick);
  OperatorAdd addOperator(Client *client);
  InvitedStatus addInvit(Client *client);
  OperatorRemove removeOperator(Client *client);
  MemberAdd addMember(Client *client);
  MembRemove removeMember(std::string &nick);
};
std::ostream &operator<<(std::ostream &os, const Channel &s);
#endif
