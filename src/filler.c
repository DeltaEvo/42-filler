/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   filler.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/07 13:14:22 by dde-jesu          #+#    #+#             */
/*   Updated: 2019/01/07 18:26:27 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/io.h"
#include "ft/mem.h"
#include <stdbool.h>
#include <stdio.h>
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

static	uint32_t	io_readnum(t_readable *rd)
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

static bool	read_player_number(t_readable *stdin, uint32_t *player_number)
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

struct	s_map {
	uint32_t	height;
	uint32_t	width;
	char		data[];
};

static struct s_map *read_map(t_readable *stdin, bool piece)
{
	struct s_map	*map;
	uint32_t	height;
	uint32_t	width;
	uint32_t	y;

	if (!io_expect(stdin, piece ? "Piece " : "Plateau "))
		return (NULL);
	if ((height = io_readnum(stdin)) == 0)
        return (NULL);
	if (!io_expect(stdin, " "))
		return (NULL);
    if ((width = io_readnum(stdin)) == 0)
	    return (NULL);
    if (!io_expect(stdin, ":\n"))
	    return (NULL);
    if (!piece && !io_skipto(stdin, '\n'))
	    return (NULL);
    map = malloc(sizeof(*map) + width * height);
	map->width = width;
	map->height = height;
	y = 0;
    //fprintf(stderr, "map: %d %d\n", height, width);
    while (y < height)
    {
	    if (!piece && io_readnum(stdin) != y)
		    return (NULL);
	    if (!piece && !io_expect(stdin, " "))
			return (NULL);
	    io_read(stdin, map->data + y * width, width);
		if (!io_expect(stdin, "\n"))
		    return (NULL);
	    y++;
    }
    //fprintf(stderr, "map: %.*s\n", width * height, map->data);
    return (map);
}

bool	is_valid(struct s_map *map, struct s_map *piece, uint32_t xo, uint32_t yo, char player_char)
{
	bool	overlap;
	uint32_t	x;
	uint32_t	y;

	overlap = false;
	y = 0;
	while (y < piece->height)
	{
		x = 0;
		while (x < piece->width)
		{
			if (piece->data[y * piece->width + x] == '*')
			{
				char c = map->data[(yo + y) * map->width + (xo + x)];
				if (c != '.') { 
					if (!overlap && (c | 0x20) == (player_char | 0x20))
						overlap = true;
					else
						return (false);
				}
			}
			x++;
		}
		y++;
	}
	return (overlap);
}

struct	s_move {
	uint32_t	x;
	uint32_t	y;
	uint32_t	score;
};

uint32_t	get_distance_with_enemy(struct s_map *map, uint32_t xo, uint32_t yo, char player)
{
	uint32_t	x;
	uint32_t	y;
	uint32_t	score;
	uint32_t	distance[2];

	score = UINT32_MAX;
	y = 0;
	while (y < map->height)
	{
		x = 0;
		while (x < map->width)
		{
			if (map->data[y * map->width + x] != '.' && (map->data[y * map->width + x] | 0x20) != (player | 0x20))
			{
				distance[0] = (x > xo ? x - xo : xo - x);
				distance[1] = (y > yo ? y - yo : yo - y);
				if (distance[0] * distance[0] + distance[1] * distance[1] < score)
					score = distance[0] * distance[0] + distance[1] * distance[1];
			}
			x++;
		}
		y++;
	}
	return (score);
}

uint32_t	get_score(struct s_map *map, struct s_map *piece, uint32_t xo, uint32_t yo, char player_char)
{
	uint32_t	x;
	uint32_t	y;
	uint32_t	score;
	uint32_t	score2;

	score = UINT32_MAX;
	y = 0;
	while (y < piece->height)
	{
		x = 0;
		while (x < piece->width)
		{
			if (piece->data[y * piece->width + x] == '*')
			{
				if ((score2 = get_distance_with_enemy(map, xo + x, yo + y, player_char)) < score)
					score = score2;
			}
			x++;
		}
		y++;
	}
	return (score);
}

void	play_turn(struct s_map *map, struct s_map *piece, char player_char)
{
	uint32_t	x;
	uint32_t	y;
	uint32_t	score;
	struct	s_move	best;

	best.score = UINT32_MAX;
	y = 0;
	while (y < map->height - piece->height)
	{
		x = 0;
		while (x < map->width - piece->width)
		{
			if (is_valid(map, piece, x, y, player_char))
				if ((score = get_score(map, piece, x, y, player_char)) < best.score)
					best = (struct s_move) { x, y, score };
			x++;
		}
		y++;
	}
    //fprintf(stderr, "score %u\n", best.score);
    if (best.score == UINT32_MAX)
	    ft_putf("Can't play\n");
   	else
		ft_putf("%d %d\n", best.y, best.x);
}

int	main()
{
	t_readable	stdin;
	uint8_t		buffer[4096];
	uint32_t	player_number;
	char		player_char;
	struct s_map	*map;
	struct s_map	*piece;

	stdin = init_readable(fill_fd, (void *)(uintptr_t)0, buffer, sizeof(buffer));
	if (!read_player_number(&stdin, &player_number) || player_number > 2)
	{
		ft_putf("Exit bye %d %d\n", (int)stdin.len, (int)stdin.index);
		return (1);
	}
	player_char = "OX"[player_number - 1];
    //fprintf(stderr, "player %d %c\n", player_number, player_char);
    while (1)
    {
		map = read_map(&stdin, false);
		piece = read_map(&stdin, true);
		if (!map || !piece)
		{
			ft_putf("Exit bye 2%d %d\n", (int)stdin.len, (int)stdin.index);
			return (1);
		}
		play_turn(map, piece, player_char);
		free(map);
		free(piece);
    }
}
