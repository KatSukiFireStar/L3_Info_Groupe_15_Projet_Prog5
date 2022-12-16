//
// Created by Corenthin Z. on 14/12/2022.
//

#ifndef PROJET_PROG_ELFFILE_H
#define PROJET_PROG_ELFFILE_H

#include <stdio.h>
#include <elf.h>

/**
 * Affiche l'entête ELF de @p elfFile
 * @param elfFile fichier ELF
 */
Elf32_Ehdr ShowElfHeader(FILE *elfFile);

/**
 * Affiche la table des sections ELF et des détails relatifs à chaque section de @p elfFile
 * @param elfFile Fichier ELF
 * @param header Header du fichier
 */
Elf32_Shdr *ShowSectionTableAndDetails(FILE *elfFile, Elf32_Ehdr header);

/**
 * Affiche le contenu de la section ... ELF de @p elfFile
 * @param elfFile fichier ELF
 * @param header Header du fichier
 */
void ShowSectionFromIndex(FILE *elfFile, Elf32_Shdr *table, int index);

/**
 *
 * @param elfFile
 * @param table
 * @param header
 * @param name
 */
void ShowSectionFromName(FILE *elfFile, Elf32_Shdr *table, Elf32_Ehdr header, Elf32_Word name);

/**
 * Affiche la table des symboles ELF et des détails relatifs à chaque symboles de @p elfFile
 * @param elfFile fichier ELF
 * @param header Header du fichier
 */
Elf32_Sym *ShowSymbolsTableAndDetails(FILE *elfFile, Elf32_Ehdr header);

/**
 * Affiche les tables de réimplantation ELF et des détails relatifs à chaque entrée de @p elfFile
 * @param elfFile fichier ELF
 * @param header Header du fichier
 */
Elf32_Rel *ShowReimplantationTablesAndDetails(FILE *elfFile, Elf32_Ehdr header);

#endif //PROJET_PROG_ELFFILE_H
