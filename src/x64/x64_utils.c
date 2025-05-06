/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   x64_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 02:01:38 by samusanc          #+#    #+#             */
/*   Updated: 2025/05/06 15:59:27 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm.h"

char	get_symbol_type_x64(Elf64_Sym *sym, Elf64_Shdr *shdrs, Elf64_Ehdr *ehdr)
{
	if (!sym || !shdrs || !ehdr)
		return 0;
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
	(void)ehdr;
	return c;
}

int	process_elf64(void *mapped, t_list *output)
{
	if (!mapped || !output)
		return 0;
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
