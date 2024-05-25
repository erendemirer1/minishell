/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putendl_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asoker <asoker@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 15:07:48 by asoker            #+#    #+#             */
/*   Updated: 2024/05/14 15:07:50 by asoker           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <unistd.h>

int	ft_putendl_fd(char *s, int fd)
{
	int	tmp;

	tmp = write(fd, s, ft_strlen(s));
	if (tmp == -1)
		return (-1);
	if (write(fd, "\n", 1) == -1)
		return (-1);
	return (++tmp);
}
