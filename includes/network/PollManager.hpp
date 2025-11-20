/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PollManager.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tniagolo <tniagolo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 19:45:20 by tniagolo          #+#    #+#             */
/*   Updated: 2025/11/20 12:28:31 by tniagolo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../common/Utils.hpp"

class PollManager
{
  private:
    std::vector<struct pollfd> _fds;
    std::map<int, size_t>      _fdIndex;

  public:
    PollManager();
    ~PollManager();

    void add(int fd, short events);
    void modify(int fd, short events);
    void addEvent(int fd, short eventFlag);
    void removeEvent(int fd, short eventFlag);
    void remove(int fd);
    int pollOnce(int timeout);
    const std::vector<struct pollfd> &getFds() const;
    std::vector<struct pollfd> &getFds();

    // Utilitaires
    bool contains(int fd) const;
    size_t size() const;
    void clear();
};