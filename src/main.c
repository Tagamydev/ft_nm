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

char	get_symbol_type_x64(Elf64_Sym *sym, Elf64_Shdr *shdrs, Elf64_Ehdr *ehdr)
{
	unsigned char bind = ELF64_ST_BIND(sym->st_info);

	if (sym->st_shndx == SHN_UNDEF)
	{
		if (bind == STB_WEAK)
			return ('w');
		else
			return ('U');
	}
	if (sym->st_shndx == SHN_ABS)
		return (bind == STB_LOCAL) ? 'a' : 'A';
	if (sym->st_shndx == SHN_COMMON)
		return (bind == STB_LOCAL) ? 'c' : 'C';
	Elf64_Shdr *sec = &shdrs[sym->st_shndx];
	char c = '?';
	if (sec->sh_type == SHT_NOBITS)
	{
		if (sec->sh_flags & SHF_WRITE)
			c = 'B';
	}
	else if (sec->sh_flags & SHF_EXECINSTR)
		c = 'T';
	else if (sec->sh_flags & SHF_WRITE)
		c = 'D';
	else if (sec->sh_flags & SHF_ALLOC)
		c = 'R';
	if (bind == STB_LOCAL)
		c = ft_tolower(c);

	if (bind == STB_WEAK)
	{
		if (sym->st_shndx == SHN_UNDEF)
			return ('w');
		else
			return ('W');
	}
	return c;
}

typedef	struct s_header{
	size_t	addr;
	char	type_char;
	char	*name;
}	t_header;

void	*free_header(void *ptr)
{
	t_header	*cast;

	cast = (t_header *)ptr;
	free(cast->name);
	free(ptr);
	return (NULL);
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
		//ft_printf("%s\n", "this file is x64");
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
		t_list	output = list(NULL);
		for (int i = 0; i < num_symbols; i++)
		{
			t_node		*new_node;
			t_header	*header;

			Elf64_Sym sym = symbols[i];
			if (sym.st_name == 0) continue;
			header = malloc(sizeof(t_header));
			if (!header)
				return (error("fatal", "malloc allocation failed", 0));
			ft_bzero(header, sizeof(t_header));
			const char *name = strtab + sym.st_name;
			char type_char = get_symbol_type_x64(&sym, shdr, ehdr);
			header->addr = sym.st_value;
			header->type_char = type_char;
			header->name = ft_strdup(name);
			list_push_b(&output, node(header, free_header));
			/*
			if (type_char != 'A' && type_char != 'a')
			{
				if (sym.st_value)
					printf("%016lx %c %s\n", sym.st_value, type_char, name);
				else
					printf("                 %c %s\n", type_char, name);
			}
			*/
		}
		t_node	*tmp;

		tmp = output.head;
		for (int i = 0; i < output.size; i++)
		{
			t_header	*tmp2;


			tmp2 = (t_header *)tmp->content;
			if (tmp2->addr)
				printf("%016lx %c %s\n", tmp2->addr, tmp2->type_char, tmp2->name);
			else
				printf("                 %c %s\n", tmp2->type_char, tmp2->name);
			tmp = tmp->next;
		}
		list_clear(&output);
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
