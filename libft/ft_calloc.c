/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asoker <asoker@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 15:06:18 by asoker            #+#    #+#             */
/*   Updated: 2024/05/14 15:06:20 by asoker           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>

void	*ft_calloc(size_t count, size_t size)
{
	unsigned int	s;
	char			*ptr;

	s = count * size;
	ptr = malloc(s * sizeof(char));
	if (!ptr)
		return (0);
	ft_bzero(ptr, s);
	return (ptr);
}
