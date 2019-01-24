/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/24 12:44:55 by dde-jesu          #+#    #+#             */
/*   Updated: 2019/01/24 13:42:08 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include <stdlib.h>

static bool		io_expect(t_readable *rd, const char *expected)
{
	while (*expected)
		if (io_peek(rd) == *expected)
		{
			rd->index++;
			expected++;
		}
		else
			return (false);
	return (true);
}

static uint32_t	io_readnum(t_readable *rd)
{
	int			c;
	uint32_t	res;

	res = 0;
	while ((c = io_peek(rd)) >= '0' && c <= '9')
	{
		res = res * 10 + (c - '0');
		rd->index++;
	}
	return (res);
}

static bool		io_skipto(t_readable *rd, char e)
{
	int		c;

	while ((c = io_peek(rd)) != -1 && c != e)
		rd->index++;
	if (c == -1)
		return (false);
	else
	{
		rd->index++;
		return (true);
	}
}

bool			read_player_number(t_readable *stdin, uint32_t *player_number)
{
	if (!io_expect(stdin, "$$$ exec p"))
		return (false);
	if ((*player_number = io_readnum(stdin)) == 0)
		return (false);
	if (!io_expect(stdin, " : "))
		return (false);
	if (!io_skipto(stdin, '\n'))
		return (false);
	return (true);
}

struct s_map	*read_map(t_readable *stdin, bool piece)
{
	struct s_map	*map;
	uint32_t		height;
	uint32_t		width;
	uint32_t		y;

	if (!(io_expect(stdin, piece ? "Piece " : "Plateau ")
		&& (height = io_readnum(stdin)) != 0
		&& io_expect(stdin, " ")
		&& (width = io_readnum(stdin)) != 0
		&& io_expect(stdin, ":\n")
		&& (piece || io_skipto(stdin, '\n'))))
		return (NULL);
	map = malloc(sizeof(*map) + width * height);
	*map = (struct s_map){ .width = width, .height = height };
	y = 0;
	while (y < height)
		if (!((piece || (io_readnum(stdin) == y && io_expect(stdin, " ")))
			&& io_read(stdin, map->data + y * width, width) >= 0
			&& io_expect(stdin, "\n")))
			break ;
		else
			y++;
	if (y < height)
		free(map);
	return (y < height ? NULL : map);
}
