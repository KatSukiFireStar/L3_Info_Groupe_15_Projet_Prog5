//
// Created by Pyrolink on 06/01/2023.
//

#ifndef PROJET_PROG_UTILS_H
#define PROJET_PROG_UTILS_H

#include <stdio.h>
#include "elfStructure.h"

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

#endif //PROJET_PROG_UTILS_H
