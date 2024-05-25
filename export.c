/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asoker <asoker@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 15:04:27 by asoker            #+#    #+#             */
/*   Updated: 2024/05/14 15:04:29 by asoker           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <unistd.h>
#include "libft/libft.h"
#include <stdio.h>

void	export(t_ms *ms)
{
	char	**env;
	int		i;

	if (ms->cmd->next && ms->cmd->next->token == NONE)
	{
		add_export(ms);
		frees(ms, ms->status);
	}
	env = convert_t_env_to_str_array(ms, NULL, 1);
	i = -1;
	while (env[++i])
	{
		ms->tmp1 = ft_strchr(env[i], '=');
		if (ms->tmp1)
		{
			*ms->tmp1 = 0;
			printf("declare -x %s=\"%s\"\n", env[i], ms->tmp1 + 1);
		}
		else
			printf("declare -x %s\n", env[i]);
	}
	ms->tmp1 = NULL;
	ft_free_malloc(env);
	frees(ms, 0);
}
