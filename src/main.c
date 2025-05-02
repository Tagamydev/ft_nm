/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 08:18:02 by samusanc          #+#    #+#             */
/*   Updated: 2025/05/02 02:59:54 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm.h"
#include "libft.h"

/**
 * Processes an ELF64 file, extracting symbol information
 * Parses headers, finds symbol and string tables, and builds a list of symbols
 */
int process_elf64(void *mapped, t_list *output)
{
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)mapped;
	Elf64_Shdr *shdr = (Elf64_Shdr *)((char *)mapped + ehdr->e_shoff);
	char *shtrtab = (char*)mapped + shdr[ehdr->e_shstrndx].sh_offset;
	Elf64_Shdr *symtab_hdr = NULL;
	Elf64_Shdr *strtab_hdr = NULL;
	
	for (int i = 0; i < ehdr->e_shnum; i++) {
		const char *name = shtrtab + shdr[i].sh_name;
		if (shdr[i].sh_type == SHT_SYMTAB)
			symtab_hdr = &shdr[i];
		else if (shdr[i].sh_type == SHT_STRTAB && !(ft_strcmp(name, ".strtab")))
			strtab_hdr = &shdr[i];
	}
	
	Elf64_Sym *symbols = (Elf64_Sym *)((char *)mapped + symtab_hdr->sh_offset);
	int num_symbols = symtab_hdr->sh_size / sizeof(Elf64_Sym);
	char *strtab = (char *)mapped + strtab_hdr->sh_offset;
	
	for (int i = 0; i < num_symbols; i++) {
		Elf64_Sym sym = symbols[i];
		if (sym.st_name == 0) 
			continue;
		
		t_header *header = malloc(sizeof(t_header));
		if (!header)
			return error("fatal", "malloc allocation failed", 0);
		
		ft_bzero(header, sizeof(t_header));
		const char *name = strtab + sym.st_name;
		char type_char = get_symbol_type_x64(&sym, shdr, ehdr);
		
		header->addr = sym.st_value;
		header->type_char = type_char;
		header->name = ft_strdup(name);
		
		list_push_b(output, node(header, free_header));
	}
	
	return 0;
}

/**
 * Processes an ELF32 file, extracting symbol information
 * Parses headers, finds symbol and string tables, and builds a list of symbols
 */
int process_elf32(void *mapped, t_list *output)
{
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)mapped;
	Elf32_Shdr *shdr = (Elf32_Shdr *)((char *)mapped + ehdr->e_shoff);
	char *shtrtab = (char*)mapped + shdr[ehdr->e_shstrndx].sh_offset;
	Elf32_Shdr *symtab_hdr = NULL;
	Elf32_Shdr *strtab_hdr = NULL;
	
	for (int i = 0; i < ehdr->e_shnum; i++) {
		const char *name = shtrtab + shdr[i].sh_name;
		if (shdr[i].sh_type == SHT_SYMTAB)
			symtab_hdr = &shdr[i];
		else if (shdr[i].sh_type == SHT_STRTAB && !(ft_strcmp(name, ".strtab")))
			strtab_hdr = &shdr[i];
	}
	
	Elf32_Sym *symbols = (Elf32_Sym *)((char *)mapped + symtab_hdr->sh_offset);
	int num_symbols = symtab_hdr->sh_size / sizeof(Elf32_Sym);
	char *strtab = (char *)mapped + strtab_hdr->sh_offset;
	
	for (int i = 0; i < num_symbols; i++) {
		Elf32_Sym sym = symbols[i];
		if (sym.st_name == 0) 
			continue;
		
		t_header *header = malloc(sizeof(t_header));
		if (!header)
			return error("fatal", "malloc allocation failed", 0);
		
		ft_bzero(header, sizeof(t_header));
		const char *name = strtab + sym.st_name;
		char type_char = get_symbol_type_x86(&sym, shdr, ehdr);
		
		header->addr = sym.st_value;
		header->type_char = type_char;
		header->name = ft_strdup(name);
		
		list_push_b(output, node(header, free_header));
	}
	
	return 0;
}

void display_symbols(t_list *output, t_flags flags, int is_64)
{
	int reverse = 0;
	
	if (!flags.p) {
		sort_list(output);
		if (flags.r)
			reverse = 1;
	}
	
	char format = 'x';
	if (flags.u)
		format = 'u';
	else if (flags.g)
		format = 'g';
	else if (flags.a)
		format = 'a';
	print_list(output, reverse, is_64, format);
}

/**
 * Verifies if the file is a valid ELF file
 * Checks the magic number at the beginning of the file
 */
int is_valid_elf(unsigned char *e_ident, char *file)
{
	if (e_ident[0] != 0x7f || e_ident[1] != 'E' || e_ident[2] != 'L' || e_ident[3] != 'F')
		return error(file, "file format not recognized", 0);
	return 1;
}

int ft_nm(char *file, t_flags flags)
{
	if (file[0] == '-')
		return 0;
	
	int fd = open(file, O_RDONLY);
	if (fd < 0)
		return error(file, "No such file", 1);
	
	struct stat st;
	fstat(fd, &st);
	void *mapped = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	
	unsigned char *e_ident = (unsigned char *)mapped;
	if (!is_valid_elf(e_ident, file))
		return 0;
	
	int is_64 = (e_ident[4] == ELFCLASS64);
	t_list output = list(NULL);
	
	int result = 0;
	if (is_64)
		result = process_elf64(mapped, &output);
	else
		result = process_elf32(mapped, &output);
	
	if (result == 0)
		display_symbols(&output, flags, is_64);
	list_clear(&output);
	return result;
}

int main(int argc, char **argv)
{
	int error_counter = 0;
	t_flags flags = parse_flags(argv);
	
	if (flags.error && flags.error != -1)
	{
		print_help();
		return 1;
	}
	if (argc == (flags.total + 1))
		error_counter += ft_nm("a.out", flags);
	else
	{
		for (int i = 1; i < argc; i++) {
			error_counter += ft_nm(argv[i], flags);
		}
	}
	return error_counter;
}
