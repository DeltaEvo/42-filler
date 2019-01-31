/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   filler.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/07 13:14:22 by dde-jesu          #+#    #+#             */
/*   Updated: 2019/01/31 17:58:37 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "filler.h"
#include "parser.h"
#include "ft/io.h"
#include "ft/mem.h"
#include <stdlib.h>

bool		is_valid(struct s_map *map, struct s_map *piece, int32_t pos[2],
		char player_char)
{
	bool		overlap;
	int32_t		x;
	int32_t		y;
	char		c;

	overlap = false;
	y = -1;
	while ((x = -1) && ++y < piece->height)
		while (++x < piece->width)
			if (piece->data[y * piece->width + x] != '*')
				continue ;
			else if (!(pos[1] + y >= 0 && pos[0] + x >= 0
			&& pos[1] + y < map->height && pos[0] + x < map->width))
				return (false);
			else if ((c = map->data[(pos[1] + y) * map->width + (pos[0] + x)])
					!= '.')
			{
				if (!overlap && (c | 0x20) == (player_char | 0x20))
					overlap = true;
				else
					return (false);
			}
	return (overlap);
}

uint32_t	get_distance_with_enemy(struct s_map *map, int32_t xo, int32_t yo,
		char player)
{
	int32_t		x;
	int32_t		y;
	uint32_t	score;
	uint32_t	dist[2];

	score = UINT32_MAX;
	y = 0;
	while (y < map->height)
	{
		x = 0;
		while (x < map->width)
		{
			if (map->data[y * map->width + x] != '.' && (map->data[y *
						map->width + x] | 0x20) != (player | 0x20))
			{
				dist[0] = (x > xo ? x - xo : xo - x);
				dist[1] = (y > yo ? y - yo : yo - y);
				if (dist[0] * dist[0] + dist[1] * dist[1] < score)
					score = dist[0] * dist[0] + dist[1] * dist[1];
			}
			x++;
		}
		y++;
	}
	return (score);
}

uint32_t	get_score(struct s_map *map, struct s_map *piece, int32_t pos[2],
		char player_char)
{
	int32_t		x;
	int32_t		y;
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
				if ((score2 = get_distance_with_enemy(map, pos[0] + x,
								pos[1] + y, player_char)) < score)
					score = score2;
			}
			x++;
		}
		y++;
	}
	return (score);
}

void		play_turn(struct s_map *map, struct s_map *piece, char player_char)
{
	int32_t			x;
	int32_t			y;
	uint32_t		score;
	struct s_move	best;

	best.score = UINT32_MAX;
	y = -piece->height;
	while (y < map->height)
	{
		x = -piece->width;
		while (x < map->width)
		{
			if (is_valid(map, piece, (int32_t[2]){ x, y }, player_char))
				if ((score = get_score(map, piece, (int32_t[2]){ x, y },
						player_char)) < best.score)
					best = (struct s_move) { x, y, score };
			x++;
		}
		y++;
	}
	if (best.score == UINT32_MAX)
		ft_putf("Can't play\n");
	else
		ft_putf("%d %d\n", best.y, best.x);
}

#define STDIN (uintptr_t)0

int			main(void)
{
	t_readable		stdin;
	uint8_t			buffer[4096];
	uint32_t		player_number;
	struct s_map	*map;
	struct s_map	*piece;

	stdin = init_readable(fill_fd, (void *)STDIN, buffer, sizeof(buffer));
	if (!read_player_number(&stdin, &player_number) || player_number > 2)
		return (1);
	while (1)
	{
		map = read_map(&stdin, false);
		piece = read_map(&stdin, true);
		if (!map || !piece)
		{
			free(map);
			free(piece);
			ft_putf("Exit bye");
			return (1);
		}
		play_turn(map, piece, "OX"[player_number - 1]);
		free(map);
		free(piece);
	}
}
