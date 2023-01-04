//
// Created by Corenthin Z. on 14/12/2022.
//

#ifndef PROJET_PROG_ELFFILE_H
#define PROJET_PROG_ELFFILE_H

/** @file */

#include <stdio.h>
#include <elf.h>

#pragma region typdefs and defines

/**
 * Alloue un espace mémoire pour un tableau
 * <br>
 * Précondition :
 * <br>
 * - @p element doit être un type c
 * <br>
 * - @p size doit être positif et non nul
 * @param element Type des éléments du tableau
 * @param size Taille du tableau
 */
#define mallocArray(element, size) malloc(sizeof(element) * size)

/** Représente une table des sections */
typedef Elf32_Shdr *Elf32_ShdrTable;
/** Représente une table des symboles */
typedef Elf32_Sym *Elf32_SymTable;
/** Représente une table des réimplémentations */
typedef Elf32_Rel *Elf32_RelTable;

#pragma endregion

#pragma region Main methods

/**
 * Affiche et extrait le header d'un fichier ELF
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf.
 * @return Le header de @p elfFile
 */
void ShowElfHeader(Elf32_Ehdr header);

Elf32_Ehdr ExtractHeader(FILE *elfFile);

/**
 * Affiche la table des sections ELF et des détails relatifs à chaque section de @p elfFile
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf.
 * @param header Header du fichier
 * @return Un tableau de SectionHeader correpondant à la table des sections
 */
void ShowSectionTableAndDetails(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable);

Elf32_ShdrTable ExtractSectionTable(FILE *elfFile, Elf32_Ehdr header);

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
void
ShowSymbolsTableAndDetails(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable, Elf32_SymTable symbolTable);

Elf32_SymTable ExtractSymbolsTable(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable);

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
void ShowReimplantationTablesAndDetails(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable,
                                        Elf32_SymTable symbolTable, Elf32_RelTable reimplantationTable);

Elf32_RelTable ExtractReimplantationTable(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable,
                                          Elf32_SymTable symbolTable);

void ExtractElfInformation(FILE *elfFile);

#pragma endregion

#pragma region Utils methods

/**
 * Obtient l'index de la section avec son nom
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf
 * @param sectionTable Table des sections
 * @param header Header du fichier @p elfFile
 * @param name Nom de le section à rechercher
 * @return L'index de la section @p name si elle existe. Sinon exit
 */
Elf32_Word GetSectionIndexByName(FILE *elfFile, Elf32_Shdr *sectionTable, Elf32_Ehdr header, char *name);

/**
 * Affiche la chaîne de caractère depuis une table des strings
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf
 * @param stringTable Table des strings
 * @param offset Offset du string dans la table @p stringTable
 */
void ShowStringFromIndex(FILE *elfFile, Elf32_Shdr stringTable, Elf32_Word offset);

/**
 * Obtient le nombre d'entrée dans une / plusieurs section d'un type
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf
 * @param header Header du fichier @p elfFile
 * @param sectionTable Table des sections du fichier @p elfFile
 * @param type Type de la section
 * @return Le nombre d'entrée dans les sections de type @p type
 */
Elf32_Half GetEntryCountFromType(Elf32_Ehdr header, Elf32_ShdrTable sectionTable, Elf32_Half type);

/**
 * Initialise la variable @p needReverse en fonction de l'endian de la machine et de @p fileEndian
 * @param fileEndian Endian du fichier. Si différent de ELFDATA2MSB ou ELFDATA2LSB, @p fileEndian prendra la valeur 0
 */
void CheckMachineEndian(unsigned char fileEndian);

/**
 * Permet de lire le fichier elf en fonction de la variable @p needReverse
 * @param ptr Emplacement de stockage des données
 * @param size Taille de l’élément en octets
 * @param number Nombre maximal d’éléments à lire
 * @param file Pointeur vers la structure FILE
 * @return Retourne le nombre d’éléments complets lus par la fonction,
 * qui peut être inférieur à count si une erreur se produit,
 * ou si elle rencontre la fin du fichier avant d’atteindre count.
 */
size_t freadEndian(void *restrict ptr, size_t size, size_t number, FILE *restrict file);

#pragma endregion

#pragma region Global variables

/**
 * Indique si les valeurs lues doivent être inversées
 * (fichier en big et machine en little ou inverse)
 */
int needReverse;

#pragma endregion

#endif //PROJET_PROG_ELFFILE_H
