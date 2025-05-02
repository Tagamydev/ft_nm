/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   x86_utlis.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 02:01:55 by samusanc          #+#    #+#             */
/*   Updated: 2025/05/02 02:02:13 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm.h"

char	get_symbol_type_x86(Elf32_Sym *sym, Elf32_Shdr *shdrs, Elf32_Ehdr *ehdr)
{
	unsigned char bind = ELF32_ST_BIND(sym->st_info);

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
	Elf32_Shdr *sec = &shdrs[sym->st_shndx];
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
