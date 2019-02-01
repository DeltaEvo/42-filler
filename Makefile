# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/11/06 16:01:17 by dde-jesu          #+#    #+#              #
#    Updated: 2019/02/01 09:09:20 by dde-jesu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME=dde-jesu.filler
CFLAGS=-Wall -Werror -Wextra -Iinclude -Ilibft/include
CC=gcc

include src.mk

OBJS=$(SRCS:.c=.o)

all: $(NAME)

$(OBJS): Makefile src.mk

$(NAME): $(OBJS)
	$(MAKE) -C libft libft.a
	$(CC) -o $(NAME) $(CFLAGS) $(OBJS) libft/libft.a

clean:
	$(MAKE) -C libft clean
	rm -f $(OBJS)

fclean: clean
	$(MAKE) -C libft fclean
	rm -rf $(NAME)

re: fclean $(NAME)

.PHONY: clean fclean re all
