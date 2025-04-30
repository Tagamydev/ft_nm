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

int	error(char *title, char *message, int i)
{
	write(2, "nm: ", 4);
    if (title)
    {
        if (i)
            write(2, "'", 1);
        write(2, title, ft_strlen(title));
        if (i)
            write(2, "'", 1);
        write(2, ": ", 2);
    }
    write(2, message, ft_strlen(message));
	write(2, "\n", 1);
	return (1);
}

int	ft_nm(char *file)
{
	int fd = open(file, O_RDONLY);
	if (fd < 0)
		return (error(file, "No such file", 1));
	struct stat st;
	fstat(fd, &st);
	void	*mapped = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	unsigned char *e_ident = (unsigned char *)mapped;
	ft_printf("%s", e_ident);
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
