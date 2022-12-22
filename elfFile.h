//
// Created by Corenthin Z. on 14/12/2022.
//

#ifndef PROJET_PROG_ELFFILE_H
#define PROJET_PROG_ELFFILE_H

#include <stdio.h>
#include <elf.h>

/**
 * Affiche et extrait le header d'un fichier ELF
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf.
 * @return Le header de @p elfFile
 */
Elf32_Ehdr ShowElfHeader(FILE *elfFile);

/**
 * Affiche la table des sections ELF et des détails relatifs à chaque section de @p elfFile
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf.
 * @param header Header du fichier
 * @return Un tableau de SectionHeader correpondant à la table des sections
 */
Elf32_Shdr *ShowSectionTableAndDetails(FILE *elfFile, Elf32_Ehdr header);

/**
 * Affiche le contenu d'une section elf
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf.
 * @param sectionTable Table des section du fichier @p elfFile
 * @param index Index de la section
 */
void ShowSectionFromIndex(FILE *elfFile, Elf32_Shdr *sectionTable, int index);

/**
 * Affiche le contenu d'une section elf
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf.
 * @param sectionTable Table des section du fichier @p elfFile
 * @param header Header du fichier @p elfFile
 * @param name Nom de la section à afficher
 */
void ShowSectionFromName(FILE *elfFile, Elf32_Shdr *sectionTable, Elf32_Ehdr header, char *name);

/**
 * Affiche la table des symboles ELF et des détails relatifs à chaque symboles de @p elfFile
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf
 * @param header Header du fichier @p elfFile
 * @param sectionTable Table des sections du fichier @p elfFile
 * @return Un tableau de Symbol correpondant à la table des sections
 */
Elf32_Sym *ShowSymbolsTableAndDetails(FILE *elfFile, Elf32_Ehdr header, Elf32_Shdr *sectionTable);

/**
 * Affiche les tables de réimplantation ELF et des détails relatifs à chaque entrée de @p elfFile
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf
 * @param header Header du fichier @p elfFile
 * @param sectionTable Table des sections du fichier @p elfFile
 * @param symbolTable Table des symboles du fichier @p elfFile
 * @return Un tableau de Relocation correpondant à la table des sections
 */
Elf32_Rel *ShowReimplantationTablesAndDetails(FILE *elfFile, Elf32_Ehdr header, Elf32_Shdr *sectionTable, Elf32_Sym * symbolTable);

#endif //PROJET_PROG_ELFFILE_H
