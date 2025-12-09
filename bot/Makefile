# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dnahon <dnahon@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/08/20 14:42:46 by dnahon            #+#    #+#              #
#    Updated: 2025/12/09 20:43:50 by dnahon           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        =   ircbot
CC          =   c++
CPPFLAGS    =   -g3 -Wall -Wextra -Werror -Wshadow -std=c++98

RM          =   rm -f

SRC         =   srcs/bot.cpp \

INCLUDES    =	includes/bot.hpp \

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
