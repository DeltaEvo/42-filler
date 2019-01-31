/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dde-jesu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/24 12:52:41 by dde-jesu          #+#    #+#             */
/*   Updated: 2019/01/31 17:34:57 by dde-jesu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "ft/io.h"
# include <stdbool.h>

struct			s_map {
	int32_t		height;
	int32_t		width;
	char		data[];
};

bool			read_player_number(t_readable *stdin, uint32_t *player_number);
struct s_map	*read_map(t_readable *stdin, bool piece);

#endif
