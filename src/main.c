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

int	ft_strcmp(const char *str1, const char *str2)
{
	int	len1 = strlen(str1);
	int	len2 = strlen(str2);

	if (len1 > len2)
		return (ft_strncmp(str1, str2, len1));
	else
		return (ft_strncmp(str1, str2, len2));
}

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
	if (e_ident[0] != 0x7f || e_ident[1] != 'E' || e_ident[2] != 'L' || e_ident[3] != 'F')
		return (error(file, "file format not recognized", 0));
	int is_64 = (e_ident[4] == ELFCLASS64);
	if (is_64)
	{
		// x64 file
		ft_printf("%s\n", "this file is x64");
		Elf64_Ehdr *ehdr = (Elf64_Ehdr *)mapped;
		Elf64_Shdr	*shdr = (Elf64_Shdr *)((char *)mapped + ehdr->e_shoff);
		char *shtrtab = (char*)mapped + shdr[ehdr->e_shstrndx].sh_offset;
		Elf64_Shdr	*symtab_hdr = NULL;
		Elf64_Shdr	*strtab_hdr = NULL;

		for (int i = 0; i < ehdr->e_shnum; i++)
		{
			const char *name = shtrtab + shdr[i].sh_name;
			if (shdr[i].sh_type == SHT_SYMTAB)
				symtab_hdr = &shdr[i];
			else if (shdr[i].sh_type == SHT_STRTAB && !(ft_strcmp(name, ".strtab")))
				strtab_hdr = &shdr[i];
		}
		Elf64_Sym *symbols = (Elf64_Sym *)((char *)mapped + symtab_hdr->sh_offset);
		int num_symbols = symtab_hdr->sh_size / sizeof(Elf64_Sym);
		char *strtab = (char *)mapped + strtab_hdr->sh_offset;
		for (int i = 0; i < num_symbols; i++)
		{
			Elf64_Sym sym = symbols[i];
			if (sym.st_name == 0) continue;
			const char *name = strtab + sym.st_name;
			char type_char = '?';
			switch (ELF64_ST_TYPE(sym.st_info))
			{
				case STT_FUNC: type_char = 'T'; break;
				case STT_OBJECT: type_char = 'D'; break;
				case STT_SECTION: type_char = 'S'; break;
				case STT_NOTYPE: type_char = 'N'; break;
				default: type_char = '?'; break;
			}
			printf("%016lx %c %s\n", sym.st_value, type_char, name);
		}
	}
	else
	{
		// x86 file
		ft_printf("%s", "this file is x86");
	}
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
