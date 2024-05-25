/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asoker <asoker@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 15:03:52 by asoker            #+#    #+#             */
/*   Updated: 2024/05/25 13:48:42 by asoker           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <unistd.h>
#include <stdio.h>
#include "libft/libft.h"

static int	control_tire_n(char *str)
{
	int	i;

	i = 1;
	if (str[0] != '-')
		return (0);
	while (str[i] == 'n')
		i++;
	return (str[i] == 0);
}

void	echo(t_ms *ms)
{
	int		has_n;
	t_cmd	*tmp;

	has_n = ms->cmd->next != NULL
		&& control_tire_n(ms->cmd->next->value);
	tmp = ms->cmd->next;
	while (tmp && control_tire_n(tmp->value))
		tmp = tmp->next;
	if (tmp)
	{
		printf("%s", tmp->value);
		tmp = tmp->next;
	}
	while (tmp)
	{
		printf(" %s", tmp->value);
		tmp = tmp->next;
	}
	if (!has_n)
		printf("\n");
	frees(ms, 0);
}
