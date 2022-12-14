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
void ShowElfHeader(FILE *elfFile);

/**
 * Affiche la table des sections ELF et des détails relatifs à chaque section de @p elfFile
 * @param elfFile fichier ELF
 */
void ShowSectionTableAndDetails(FILE *elfFile);

/**
 * Affiche le contenu de la section ... ELF de @p elfFile
 * @param elfFile fichier ELF
 */
void ShowSection(FILE *elfFile);

/**
 * Affiche la table des symboles ELF et des détails relatifs à chaque symboles de @p elfFile
 * @param elfFile fichier ELF
 */
void ShowSymbolsTableAndDetails(FILE *elfFile);

/**
 * Affiche les tables de réimplantation ELF et des détails relatifs à chaque entrée de @p elfFile
 * @param elfFile fichier ELF
 */
void ShowReimplantationTablesAndDetails(FILE *elfFile);

#endif //PROJET_PROG_ELFFILE_H
