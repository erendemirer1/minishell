/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asoker <asoker@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 15:05:10 by asoker            #+#    #+#             */
/*   Updated: 2024/05/25 14:18:48 by asoker           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "libft/libft.h"
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

int	ft_token(t_ms *ms, t_cmd *iter, int *fd)
{
	if (iter->token == INPUT)
		return (ft_input(iter, fd));
	if (iter->token == HEREDOC)
		ft_heredoc(ms, fd);
	else if (iter->token == WRITE)
		ft_write(iter, fd);
	else if (iter->token == REWRITE)
		ft_rewrite(iter, fd);
	return (0);
}

void	ft_control(t_cmd **iter, t_ms *ms)
{
	t_cmd	*tmp;

	if (*iter == ms->cmd)
	{
		*iter = (*iter)->next->next;
		ms->cmd = ms->cmd->next->next;
	}
	else
	{
		tmp = ms->cmd;
		while (tmp->next != *iter)
			tmp = tmp->next;
		*iter = tmp;
		tmp = tmp->next;
		(*iter)->next = (*iter)->next->next->next;
		free(tmp->next->value);
		free(tmp->next);
		free(tmp->value);
		free(tmp);
	}
}

int	redirection(t_ms *ms)
{
	t_cmd	*iter;
	int		fd[2];

	iter = ms->cmd;
	fd[0] = 0;
	fd[1] = 1;
	while (iter && iter->token != PIPE)
	{
		if (iter->token != NONE)
		{
			if (ft_token(ms, iter, fd))
			{
				ms->status = 1;
				dup2(fd[0], 0);
				dup2(fd[1], 1);
				ft_control(&iter, ms);
				return (ms->num_of_pipe == 0);
			}
			ft_control(&iter, ms);
		}
		else
			iter = iter->next;
	}
	return (dup2(fd[0], 0), dup2(fd[1], 1), 0);
}
