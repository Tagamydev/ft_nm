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

void	print_help()
{
	ft_printf("%s", "Usage: nm [option(s)] [file(s)]\n");
	ft_printf("%s", " List symbols in [file(s)] (a.out by default).\n");
	ft_printf("%s", " The options are:\n");
	ft_printf("%s", "  -a, --debug-syms       Display debugger-only symbols\n");
	ft_printf("%s", "  -A, --print-file-name  Print name of the input file before every symbol\n");
	ft_printf("%s", "  -B                     Same as --format=bsd\n");
	ft_printf("%s", "  -C, --demangle[=STYLE] Decode mangled/processed symbol names\n");
	ft_printf("%s", "                           STYLE can be \"none\", \"auto\", \"gnu-v3\", \"java\",\n");
	ft_printf("%s", "                           \"gnat\", \"dlang\", \"rust\"\n");
	ft_printf("%s", "      --no-demangle      Do not demangle low-level symbol names\n");
	ft_printf("%s", "      --recurse-limit    Enable a demangling recursion limit.  (default)\n");
	ft_printf("%s", "      --no-recurse-limit Disable a demangling recursion limit.\n");
	ft_printf("%s", "  -D, --dynamic          Display dynamic symbols instead of normal symbols\n");
	ft_printf("%s", "  -e                     (ignored)\n");
	ft_printf("%s", "  -f, --format=FORMAT    Use the output format FORMAT.  FORMAT can be `bsd',\n");
	ft_printf("%s", "                           `sysv', `posix' or 'just-symbols'.\n");
	ft_printf("%s", "                           The default is `bsd'\n");
	ft_printf("%s", "  -g, --extern-only      Display only external symbols\n");
	ft_printf("%s", "    --ifunc-chars=CHARS  Characters to use when displaying ifunc symbols\n");
	ft_printf("%s", "  -j, --just-symbols     Same as --format=just-symbols\n");
	ft_printf("%s", "  -l, --line-numbers     Use debugging information to find a filename and\n");
	ft_printf("%s", "                           line number for each symbol\n");
	ft_printf("%s", "  -n, --numeric-sort     Sort symbols numerically by address\n");
	ft_printf("%s", "  -o                     Same as -A\n");
	ft_printf("%s", "  -p, --no-sort          Do not sort the symbols\n");
	ft_printf("%s", "  -P, --portability      Same as --format=posix\n");
	ft_printf("%s", "  -r, --reverse-sort     Reverse the sense of the sort\n");
	ft_printf("%s", "      --plugin NAME      Load the specified plugin\n");
	ft_printf("%s", "  -S, --print-size       Print size of defined symbols\n");
	ft_printf("%s", "  -s, --print-armap      Include index for symbols from archive members\n");
	ft_printf("%s", "      --quiet            Suppress \"no symbols\" diagnostic\n");
	ft_printf("%s", "      --size-sort        Sort symbols by size\n");
	ft_printf("%s", "      --special-syms     Include special symbols in the output\n");
	ft_printf("%s", "      --synthetic        Display synthetic symbols as well\n");
	ft_printf("%s", "  -t, --radix=RADIX      Use RADIX for printing symbol values\n");
	ft_printf("%s", "      --target=BFDNAME   Specify the target object format as BFDNAME\n");
	ft_printf("%s", "  -u, --undefined-only   Display only undefined symbols\n");
	ft_printf("%s", "  -U, --defined-only     Display only defined symbols\n");
	ft_printf("%s", "      --unicode={default|show|invalid|hex|escape|highlight}\n");
	ft_printf("%s", "                         Specify how to treat UTF-8 encoded unicode characters\n");
	ft_printf("%s", "  -W, --no-weak          Ignore weak symbols\n");
	ft_printf("%s", "      --with-symbol-versions  Display version strings after symbol names\n");
	ft_printf("%s", "  -X 32_64               (ignored)\n");
	ft_printf("%s", "  @FILE                  Read options from FILE\n");
	ft_printf("%s", "  -h, --help             Display this information\n");
	ft_printf("%s", "  -V, --version          Display this program's version number\n");
	ft_printf("%s", "nm: supported targets: elf64-x86-64 elf32-i386 elf32-iamcu elf32-x86-64 pei-i386 pe-x86-64 pei-x86-64 elf64-little elf64-big elf32-little elf32-big pe-bigobj-x86-64 pe-i386 pdb srec symbolsrec verilog tekhex binary ihex plugin");
}

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

typedef struct s_flags{
	// u > g > a
	int	a;
	int g;
	int u;
	// p > r
	int	r;
	int p;
	int	total;
	int	error;
} t_flags;

void	parse_flag(t_flags *result, char *str)
{
	if (str[0] != '-')
		return ;
	str++;
	result->total += 1;
	for (int i = 0; str[i]; i++)
	{
		switch (str[i])
		{
			case 'a':
				result->a += 1;
				break;
			case 'g':
				result->g += 1;
				break;
			case 'u':
				result->u += 1;
				break;
			case 'r':
				result->r += 1;
				break;
			case 'p':
				result->p += 1;
				break;
			case 'h':
				print_help();
				exit(0);
				break;
			default:
				char	error_str[3];
				char	*error_ptr;

				error_str[0] = str[i];
				error_str[1] = '\'';
				error_str[2] = '\0';
				error_ptr = ft_strjoin(ft_strdup("invalid option -- '"), ft_strdup(error_str));
				error(NULL, error_ptr, 0);
				free(error_ptr);
				result->error = 1;
				return;
				break;
		}
	}
}

t_flags	parse_flags(char **argv)
{
	t_flags	result;
	char	**array = argv + 1;

	ft_bzero(&result, sizeof(t_flags));
	for (int i = 0; argv[i]; i++)
	{
		parse_flag(&result, argv[i]);
		if (result.error)
			return (result);
	}
	return (result);
}

void	*free_header(void *ptr)
{
	t_header	*cast;

	cast = (t_header *)ptr;
	free(cast->name);
	free(ptr);
	return (NULL);
}

char	*convert_addr(int addr, int is_64)
{
	char	str_64[17];
	char	str_86[9];

	ft_memset(str_64, '0', sizeof(str_64));
	ft_memset(str_86, '0', sizeof(str_86));

	char	*nbr = ft_itoa(addr);
	if (!nbr)
		return (NULL);
	str_64[16 - ft_strlen(nbr)] = '\0';
	str_86[8 - ft_strlen(nbr)] = '\0';
	if (is_64)
		return (ft_strjoin(ft_strdup(str_64), nbr));
	else
		return (ft_strjoin(ft_strdup(str_86), nbr));
}

void	print_content(t_header *content, int is_64, char flag)
{
	if (flag != 'a')
		if (content->type_char == 'a' || content->type_char == 'A')
			return ;
	if (flag == 'u')
		if (content->type_char != 'U' && content->type_char != 'w')
			return ;
	if (flag == 'g')
		if (content->type_char == 't' || content->type_char == 'd' || content->type_char == 'b')
			return ;
	if (content->addr)
	{
		char	*str = convert_addr(content->addr, is_64);
		ft_printf("%s %c %s\n", str, content->type_char, content->name);
		free(str);
	}
	else
	{
		if (is_64)
			ft_printf("                 %c %s\n", content->type_char, content->name);
		else
			ft_printf("         %c %s\n", content->type_char, content->name);
	}
}

void	print_list(t_list *list, int order, int is_64, char flag)
{
		t_node	*tmp;

		if (order)
			tmp = list->head;
		else
			tmp = list->tail;
		for (int i = 0; i < list->size; i++)
		{
			t_header	*tmp2;

			tmp2 = (t_header *)tmp->content;
			print_content(tmp2, is_64, flag);
			if (order)
				tmp = tmp->next;
			else
				tmp = tmp->back;
		}

}

int	ft_strcmpl(char	*str1, char *str2)
{
	char	*cp_str1 = ft_strdup(str1);
	char	*cp_str2 = ft_strdup(str2);

	for (int i = 0; cp_str1[i]; i++)
	{
		cp_str1[i] = ft_tolower(cp_str1[i]);
	}
	for (int i = 0; cp_str2[i]; i++)
	{
		cp_str2[i] = ft_tolower(cp_str2[i]);
	}

	int result = ft_strcmp(cp_str1, cp_str2);
	free(cp_str1);
	free(cp_str2);
	return (result);
}

void	sort_list(t_list *list)
{
	t_node	*tmp1;
	tmp1 = list->head;
	for (int i = 0; i < list->size; i++)
	{
		t_node	*tmp2 = tmp1->next;
		if (tmp2)
		{
			t_header	*content1 = tmp1->content;
			t_header	*content2 = tmp2->content;
			if (ft_strcmpl(content1->name, content2->name) < 0)
			{
				list_swap(tmp1, tmp2);
				i = -1;
				tmp1 = list->head;
				continue;
			}
		}
		tmp1 = tmp2;
	}
}

int	ft_nm(char *file, t_flags flags)
{
	if (file[0] == '-')
		return (0);
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
	t_list	output = list(NULL);
	if (is_64)
	{
		// x64 file
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
		}
	}
	else
	{
		// x86 file
		ft_printf("%s", "this file is x86");
	}

	int	reverse = 0;

	if (!flags.p)
	{
		sort_list(&output);
		if (flags.r)
			reverse = 1;
	}

	if (flags.u)
		print_list(&output, reverse, is_64, 'u');
	else if (flags.g)
		print_list(&output, reverse, is_64, 'g');
	else if (flags.a)
		print_list(&output, reverse, is_64, 'a');
	else
		print_list(&output, reverse, is_64, 'x');
	list_clear(&output);

	return (0);
}

int	main(int argc, char **argv)
{
	int	error_counter = 0;
	t_flags	flags = parse_flags(argv);

	if (flags.error && flags.error != -1)
	{
		print_help();
		return (1);
	}
	if (argc == (flags.total + 1))
	{
		error_counter += ft_nm("a.out", flags);
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{
			error_counter += ft_nm(argv[i], flags);
		}
	}
	return (error_counter);
}
