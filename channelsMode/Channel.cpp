/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiteixei <kiteixei@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 12:45:55 by kiteixei          #+#    #+#             */
/*   Updated: 2025/11/19 13:40:18 by kiteixei         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include <ostream>

Channel::Channel() {}

Channel::~Channel() {}

std::string Channel::getName() const { return (_name); }

std::string Channel::getTopic() const { return (_topic); }

std::string Channel::getTopicAuthor() const { return (_topicAuthor); }

time_t Channel::getTopicTimesStamp() const { return (_topicTimestamp); }

std::string Channel::getKey() const { return (_key); }

int Channel::getUserLimit() const { return (_userLimit); }

bool Channel::IsHasKey() {
  if (hasKey)
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

std::ostream &operator<<(std::ostream &os, const Channel &s) {
  os << s.getName();
  return (os);
}
