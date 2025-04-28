/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 08:18:02 by samusanc          #+#    #+#             */
/*   Updated: 2025/04/28 10:13:24 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm.h"
#include "libft.h"

char *error_str(char *title, char *error)
{
    char *buff;
    int i = 0;
    int j = 0;
    int size = ft_strlen(title) + ft_strlen(error) + 2 + 1;

    buff = malloc(sizeof(char) * size);
    if (!buff)
        return (NULL);
    ft_bzero(buff, size);
    while (i < ft_strlen(title))
    {
        buff[i] = title[i];
        i++;
    }
    buff[i++] = ':';
    buff[i++] = ' ';
    while (j < ft_strlen(error))
    {
        buff[i] = error[j];
        j++;
        i++;
    }
    buff[i] = '\0';
    return (buff);
}

int	error(char *str)
{
	write(2, "nm: ", 4);
	if (str)
		write(2, str, ft_strlen(str));
	free(str);
	write(2, "\n", 1);
	return (1);
}

int	ft_nm(char *file)
{
	int fd = open(file, O_RDONLY);
	if (fd < 0)
		return (error(error_str("a", "b")));

	return (0);
}

int	main(int argc, char **argv)
{
	int	error_counter = 0;

	if (argc == 1)
	{
		error_counter += ft_nm("a.out");
	}
	else if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
		{
			error_counter += ft_nm(argv[i]);
		}
	}
	return (error_counter);
}
