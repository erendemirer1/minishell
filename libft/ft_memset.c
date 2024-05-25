/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asoker <asoker@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 15:07:40 by asoker            #+#    #+#             */
/*   Updated: 2024/05/14 15:07:43 by asoker           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *b, int c, size_t len)
{
	size_t				i;
	unsigned char		a;
	unsigned char		*tmp;

	i = 0;
	a = c;
	tmp = (unsigned char *)b;
	while (i < len)
	{
		tmp[i] = a;
		i++;
	}
	return (b);
}
