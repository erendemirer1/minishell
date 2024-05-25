/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asoker <asoker@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 15:05:14 by asoker            #+#    #+#             */
/*   Updated: 2024/05/14 15:05:15 by asoker           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <signal.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>

void	ft_signal(int x)
{
	if (x == SIGINT && !g_sig_control)
	{
		printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
	else if (x == SIGINT)
	{
		if (g_sig_control == 4)
			exit(1);
		else
			printf("\n");
		g_sig_control = 2;
	}
	else if (x == SIGQUIT && g_sig_control == 1)
	{
		printf("Quit: 3\n");
		g_sig_control = 3;
	}
	else if (x == SIGQUIT)
		return ;
}

void	sigcontrol(void)
{
	signal(SIGINT, ft_signal);
	signal(SIGQUIT, ft_signal);
}
