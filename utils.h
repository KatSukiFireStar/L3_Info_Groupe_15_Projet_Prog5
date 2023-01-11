//
// Created by Pyrolink on 06/01/2023.
//

#ifndef PROJET_PROG_UTILS_H
#define PROJET_PROG_UTILS_H

/**
 * @file
 * Fichier d'en-tête utilitaires
 */

#include <stdio.h>
#include "elfStructure.h"

#pragma region Exit codes

/** Le fichier ouvert n'est pas un fichier elf */
#define NOT_ELF_FILE    254
/** La valeur de e_ident[EI_CLASS] est invalide */
#define INVALID_CLASS   253
/** La valeur de e_ident[EI_DATA] est invalide */
#define INVALID_DATA    252
/** La valeur de e_version est invalide */
#define INVALID_VERSION 251
/** La valeur de e_ident[EI_OSABI] est invalide */
#define INVALID_OSABI   250
/** La valeur de e_type est invalide */
#define INVALID_TYPE    249
/** Aucune section porte le nom recherché */
#define STRING_ERROR    248
/** Un symbole est invalide (st_info ou st_other) */
#define INVALID_SYMBOL  247
/** 2 fichiers différents ont deux symboles globlaux avec le même nom */
#define SAME_GLOBALS    246
/** Une allocation de mémoire à échouée */
#define MALLOC_FAILED   245

#pragma endregion

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
#define mallocArray(element, size) mallocArrayAndCheck(sizeof(element), (size))

/**
 * Alloue un espace mémoire pour un tableau d'élément.
 * @param elementSize Taille d'un élément en octet
 * @param nbElements Nombre d'élément à allouer
 * @return Le pointeur du tableau alloué
 */
void *mallocArrayAndCheck(size_t elementSize, int nbElements);

/**
 * Obtient le nombre d'entrée dans une / plusieurs section d'un type
 * @param header Header du fichier @p elfFile
 * @param sectionTable Table des sections du fichier @p elfFile
 * @param type Type de la section
 * @return Le nombre d'entrée dans les sections de type @p type
 */
Elf32_Half GetEntryCountFromType(Elf32_Ehdr header, Elf32_ShdrTable sectionTable, Elf32_Half type);

/**
 * Obtient le nombre de sections d'un type
 * @param header Header du fichier @p elfFile
 * @param sectionTable Table des sections du fichier @p elfFile
 * @param type Type de la section
 * @return Le nombre de sections de type @p type
 */
Elf32_Half GetSectionCountFromType(Elf32_Ehdr header, Elf32_ShdrTable sectionTable, Elf32_Half type);

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

/**
 * Permet d'écrire un fichier en fonction de la variable @p needReverse
 * @param ptr Emplacement de stockage des données
 * @param size Taille de l'élément en octets
 * @param number Nombre maximal d'éléments à lire
 * @param file Pointeur vers la structure FILE
 * @return fwrite retourne le nombre d’éléments complets écrits par la fonction,
 * qui peut être inférieur à count si une erreur se produit
 */
size_t fwriteEndian(const void *restrict ptr, size_t size, size_t number, FILE *restrict file);


int isLocalSymbolInFusionTable(Elf32_SymbolFusion fusionTable, Elf32_Sym symbol);

#endif //PROJET_PROG_UTILS_H
