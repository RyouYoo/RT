/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_replace.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abenaiss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/10/16 23:48:54 by abenaiss          #+#    #+#             */
/*   Updated: 2018/10/16 23:48:57 by abenaiss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

char	*ft_replace(char *str, char a, char b)
{
	int i;

	i = 0;
	while (str[i])
	{
		if (str[i] == a)
			str[i] = b;
		i++;
	}
	return (str);
}
