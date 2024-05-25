/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asoker <asoker@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 15:07:26 by asoker            #+#    #+#             */
/*   Updated: 2024/05/14 15:07:28 by asoker           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	size_t			i;
	const char		*tmp_s;
	char			a;

	i = 0;
	tmp_s = (char *)s;
	a = (char)c;
	while (i < n)
	{
		if (tmp_s[i] == a)
			return ((void *)(s + i));
		i++;
	}
	return (0);
}
