# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dnahon <dnahon@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/08/20 14:42:46 by dnahon            #+#    #+#              #
#    Updated: 2025/12/08 13:52:34 by dnahon           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        =   ircserv
CC          =   c++
CPPFLAGS    =   -g3 -Wall -Wextra -Werror -Wshadow -std=c++98

RM          =   rm -f

SRC         =   srcs/main.cpp \
				srcs/channel/Channel.cpp \
				srcs/common/Client.cpp \
				srcs/network/Connection.cpp \
				srcs/network/PollManager.cpp \
				srcs/network/Server_clients.cpp \
				srcs/network/Server_init.cpp \
				srcs/network/Server_io.cpp \
				srcs/network/Socket.cpp \
				srcs/parsing/Command.cpp \
				srcs/parsing/Parser.cpp

INCLUDES    =   includes/ft_irc.hpp \
				includes/common/Channel.hpp \
				includes/common/Client.hpp \
				includes/common/Utils.hpp \
				includes/network/Connection.hpp \
				includes/network/PollManager.hpp \
				includes/network/Server.hpp \
				includes/network/Socket.hpp \
				includes/parsing/Command.hpp \
				includes/parsing/Parser.hpp

OBJ         =   $(SRC:.cpp=.o)

# Color codes
GREEN       =   \033[0;32m
YELLOW      =   \033[0;33m
RED         =   \033[0;31m
BLUE        =   \033[0;34m
PURPLE      =   \033[0;35m
CYAN        =   \033[0;36m
NC          =   \033[0m # No Color

all: $(NAME)

$(NAME): $(OBJ)
	@echo "$(GREEN)Building $(NC)$(NAME)"
	@$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp $(INCLUDES)
	@echo "$(CYAN)Compiling $(NC)$<"
	@$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	@echo "$(CYAN)Cleaning object files"
	@$(RM) $(OBJ)

fclean: clean
	@echo "$(CYAN)Cleaning $(NC)$(NAME)"
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
