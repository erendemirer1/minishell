/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asoker <asoker@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 15:08:55 by asoker            #+#    #+#             */
/*   Updated: 2024/05/14 15:08:57 by asoker           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	size_t		i;
	char		*ptr;
	char		a;

	i = 0;
	ptr = 0;
	a = c;
	while (s[i] != '\0')
	{
		if (s[i] == a)
			ptr = (char *)&s[i];
		i++;
	}
	if (s[i] == a)
		ptr = (char *)&s[i];
	return (ptr);
}
