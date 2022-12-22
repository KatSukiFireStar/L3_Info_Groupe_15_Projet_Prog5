//
// Created by Corenthin Z. on 14/12/2022.
//

#ifndef PROJET_PROG_ELFFILE_H
#define PROJET_PROG_ELFFILE_H

/** @file */

#include <stdio.h>
#include <elf.h>

/** Représente une table des sections */
typedef Elf32_Shdr *Elf32_ShdrTable;
/** Représente une table des symboles */
typedef Elf32_Sym *Elf32_SymTable;
/** Représente une table des réimplémentations */
typedef Elf32_Rel *Elf32_RelTable;

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
Elf32_ShdrTable ShowSectionTableAndDetails(FILE *elfFile, Elf32_Ehdr header);

/**
 * Affiche le contenu d'une section elf
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf.
 * @param sectionTable Table des section du fichier @p elfFile
 * @param index Index de la section
 */
void ShowSectionFromIndex(FILE *elfFile, Elf32_ShdrTable sectionTable, Elf32_Word index);

/**
 * Affiche le contenu d'une section elf
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf.
 * @param sectionTable Table des section du fichier @p elfFile
 * @param header Header du fichier @p elfFile
 * @param name Nom de la section à afficher
 */
void ShowSectionFromName(FILE *elfFile, Elf32_ShdrTable sectionTable, Elf32_Ehdr header, char *name);

/**
 * Affiche la table des symboles ELF et des détails relatifs à chaque symboles de @p elfFile
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf
 * @param header Header du fichier @p elfFile
 * @param sectionTable Table des sections du fichier @p elfFile
 * @return Un tableau de Symbol correpondant à la table des sections
 */
Elf32_SymTable ShowSymbolsTableAndDetails(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable);

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
Elf32_RelTable ShowReimplantationTablesAndDetails(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable,
                                                  Elf32_SymTable symbolTable);

#endif //PROJET_PROG_ELFFILE_H
