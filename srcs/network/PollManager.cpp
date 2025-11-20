/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PollManager.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tniagolo <tniagolo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 19:52:17 by tniagolo          #+#    #+#             */
/*   Updated: 2025/11/20 21:30:37 by tniagolo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/network/PollManager.hpp"

PollManager::PollManager()
{
}
PollManager::~PollManager()
{
}

void PollManager::add(int fd, short events)
{
	struct pollfd	paul;

	if (fd < 0)
		return ;
	std::map<int, size_t>::iterator it = _fdIndex.find(fd); // On initialise l'iterator au find fd
	if (it != _fdIndex.end())
	{
		// si l'iterator est different de end cest quil existe dans la map
		if (it->second < _fds.size() && _fds[it->second].fd == fd) // on verifie que notre index est bien inferieur a la size (peut arriver si interruption anormale d'une fonction style remove)
		{
			_fds[it->second].events = events; // on set l'event a surveiller de ce fd
			_fds[it->second].revents = 0; // on remet lhistorique (l'event qui s'est deroule)de ce fd a 0
		}
		return ;
	}
	// si le fd n'existait pas on l add a notre struct
	paul.fd = fd; 
	paul.events = events;
	paul.revents = 0;
	_fds.push_back(paul); // on le push dans la struct vector
	_fdIndex[fd] = _fds.size() - 1; // et lajoute dans la map 
}

void PollManager::modify(int fd, short events)
{
	if (fd < 0)
		return ;
	std::map<int, size_t>::iterator it = _fdIndex.find(fd);
	if (it != _fdIndex.end())
	{
		if (it->second < _fds.size() && _fds[it->second].fd == fd)
		{
			_fds[it->second].events = events;
			_fds[it->second].revents = 0;
		}
		return ;
	}
	PollManager::add(fd, events); // si le fd n'existait pas on l add a notre struct
}

void PollManager::addEvent(int fd, short eventFlag)
{
	if (fd < 0 || eventFlag == 0)
		return ;
	std::map<int, size_t>::iterator it = _fdIndex.find(fd);
	if (it != _fdIndex.end())
	{
		if (it->second < _fds.size() && _fds[it->second].fd == fd)
		{
			_fds[it->second].events |= eventFlag; // ajoute un event sans ecraser ceux deja present
			_fds[it->second].revents = 0;
		}
		return ;
	}
	PollManager::add(fd, eventFlag); // si le fd n'existait pas on l add a notre struct avec l'event qu'on devait ajouter
}

void PollManager::removeEvent(int fd, short eventFlag)
{
	if (fd < 0 || eventFlag == 0)
		return ;
	std::map<int, size_t>::iterator it = _fdIndex.find(fd);
	if (it != _fdIndex.end())
	{
		if (it->second < _fds.size() && _fds[it->second].fd == fd)
		{
			_fds[it->second].events &= ~eventFlag; // supprime un event en laissant les autre inchange
			_fds[it->second].revents = 0;
		}
		return ;
	}
}

void PollManager::remove(int fd)
{
	int	new_fd;

	if (fd < 0)
		return ;
	std::map<int, size_t>::iterator it = _fdIndex.find(fd);
	if (it != _fdIndex.end())
	{
		if (!_fds.empty() && it->second == _fds.size() - 1)
		{
			_fds.pop_back(); // si il y a un ou plusieurs fd et que le fd a retirer est le dernier, on fait simplement un pop pour retirer le dernier
			_fdIndex.erase(it); // on erase directement la cle ainsi que la valeur du fd en question dans la map
			return ;
		}
		if (it->second < _fds.size())
		{
			new_fd = _fds[_fds.size() - 1].fd; // si le fd en question n'est pas a la fin, on stock le dernier fd
			_fds[it->second] = _fds[_fds.size() - 1]; // on remplace le fd qu'on souhaite supprimer par le dernier fd
			_fdIndex[new_fd] = it->second; // on actualise l'index du dernier fd de la liste qu'on vient de deplacer a l'emplacement du fd supprime par son nouvel index
			_fds.pop_back(); // on pop pour retirer le dernier fd de la liste (vector)
			_fdIndex.erase(it); // on erase pour retirer la cle ainsi que la valeur du fd qu'on a supprime
		}
		return ;
	}
}

int PollManager::pollOnce(int timeout)
{
	struct pollfd	*fds_ptr;
	nfds_t			nfds;
	int				exit_status;

	for (size_t i = 0; i < _fds.size(); ++i)
		_fds[i].revents = 0;
	fds_ptr = _fds.empty() ? NULL : &_fds[0]; // si jamais on a aucun fd, on initialise le pointeur du tableau de fd a NULL, sinon on le met au premier fd
	nfds = static_cast<nfds_t>(_fds.size());
	while (true)
	{
		exit_status = ::poll(fds_ptr, nfds, timeout);
		if (exit_status < 0)
		{
			if (errno == EINTR)
				continue ; // relancer si interrompu par signal
			return (-1);
			// pour signaler que poll a echoue autrement que par un signal
		}
		break ;
	}
	return (exit_status);
}

const std::vector<struct pollfd> &PollManager::getFds() const
{
	return (_fds);
}
std::vector<struct pollfd> &PollManager::getFds()
{
	return (_fds);
}

bool PollManager::contains(int fd) const
{
	for (size_t i = 0; i < _fds.size(); i++)
	{
		if (_fds[i].fd == fd)
			return (true);
	}
	return (false);
}

size_t PollManager::size(void) const
{
	return (_fds.size());
}

void PollManager::clear(void)
{
	_fds.clear();
	_fdIndex.clear();
}