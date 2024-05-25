/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asoker <asoker@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 15:06:40 by asoker            #+#    #+#             */
/*   Updated: 2024/05/14 15:06:42 by asoker           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>

static void	find_num(char *num, int n, size_t num_of_char)
{
	num[num_of_char--] = '\0';
	if (n < 0)
	{
		num[0] = '-';
		n = -n;
	}
	else if (n == 0)
		num[0] = '0';
	while (n != 0)
	{
		num[num_of_char--] = n % 10 + '0';
		n /= 10;
	}
}

static size_t	find_num_of_digit(int tmp_n)
{
	size_t	num_of_digit;

	num_of_digit = 0;
	while (tmp_n != 0)
	{
		num_of_digit++;
		tmp_n /= 10;
	}
	return (num_of_digit);
}

char	*ft_itoa(int n)
{
	size_t	num_of_char;
	char	*num;
	int		tmp_n;

	num_of_char = 0;
	tmp_n = n;
	if (n == -2147483648)
	{
		num = ft_strdup("-2147483648");
		if (!num)
			return (0);
		return (num);
	}
	else if (n <= 0)
	{
		num_of_char++;
		tmp_n = -n;
	}
	num_of_char += find_num_of_digit(tmp_n);
	num = (char *)malloc(num_of_char + 1);
	if (!num)
		return (0);
	find_num(num, n, num_of_char);
	return (num);
}
