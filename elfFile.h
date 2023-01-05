//
// Created by Corenthin Z. on 14/12/2022.
//

#ifndef PROJET_PROG_ELFFILE_H
#define PROJET_PROG_ELFFILE_H

/** @file */

#include <stdio.h>
#include <elf.h>

#include "elfStructure.h"

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

#pragma endregion

#pragma region Structure

/** Représente les données de fusion des sections de 2 fichiers */
typedef struct
{
    /** Nouveaux indices des sections du deuxième fichier */
    Elf32_Word *newIndices;
    /** Offset des concaténations du premier fichiers. -1 s'il n'y a pas de concaténation */
    Elf32_Off *concatenationOffset;
    /** Chemin vers un fichier temporaire qui contiens les sections */
    char *tmpFile;

} Elf32_SectionFusion;

/**
 * Crée une nouvelle instance de Elf32_SectionFusion
 * @param sectionSize1 Taille de la section du premier fichier
 * @param sectionSize2 Taille de la section du deuxième fichier
 * @return Une Elf32_SectionFusion avec ses tableaux alloués (hormis @p tmpOffsets) et un chemin de fichier temporaire
 * généré automatiquement
 */
inline Elf32_SectionFusion NewSectionFusion(Elf32_Word sectionSize1, Elf32_Word sectionSize2)
{
    Elf32_Word *newIndices = mallocArray(Elf32_Word, sectionSize2);
    for (int i = 0; i < sectionSize2; i++)
    {
        newIndices[i] = i;
    }

    Elf32_Off *concatenationOffset = mallocArray(Elf32_Off, sectionSize1);
    for (int i = 0; i < sectionSize1; i++)
    {
        concatenationOffset[i] = -1;
    }


    Elf32_SectionFusion sf = {newIndices, concatenationOffset, tmpnam(NULL)};
    return sf;
}

/**
 * Désaloue les tableaux de @p fusion et supprime le fichier temporaire
 * @param fusion Elf32_SectionFusion à libérer
 */
inline void FreeSectionFusion(Elf32_SectionFusion fusion)
{
    free(fusion.newIndices);
    free(fusion.concatenationOffset);
    remove(fusion.tmpFile);
    free(fusion.tmpFile);
}

typedef struct
{
    Elf32_Word *newIndices;
    Elf32_SymTable symbolTable;
    char *strtab;
} Elf32_SymbolFusion;

#pragma endregion

#pragma region Main methods

/**
 * Affiche le header d'un fichier ELF
 * @param header Header du fichier.
 */
void ShowElfHeader(Elf32_Ehdr header);

/**
 * Extrait le header d'un fichier ELF
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf.
 * @return Le header de @p elfFile
 */
Elf32_Ehdr ExtractHeader(FILE *elfFile);

/**
 * Affiche la table des sections ELF et des détails relatifs à chaque section de @p elfFile
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf.
 * @param header Header du fichier
 * @param sectionTable Table des section du fichier à afficher
 */
void ShowSectionTableAndDetails(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable);

/**
 * Extrait la table des sections d'un fichier ELF
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf.
 * @param header Header du fichier
 * @return La table des sections de @p elfFile
 */
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
 * @param symbolTable Table des symboles du fichier @p elfFile à afficher
 */
void
ShowSymbolsTableAndDetails(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable, Elf32_SymTable symbolTable);

/**
 * Extrait la tables des symboles d'un fichier ELF
 * @param elfFile Fichier elf
 * @param header Header du fichier @p elfFile
 * @param sectionTable Table des sections du fichier @p elfFile
 * @return La table des symboles de @p elfFile
 */
Elf32_SymTable ExtractSymbolsTable(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable);

/**
 * Affiche les tables de réimplantation ELF et des détails relatifs à chaque entrée de @p elfFile
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf
 * @param header Header du fichier @p elfFile
 * @param sectionTable Table des sections du fichier @p elfFile
 * @param symbolTable Table des symboles du fichier @p elfFile
 * @param reimplantationTable Table de relocation du fichier @p elfFile à afficher
 */
void ShowReimplantationTablesAndDetails(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable,
                                        Elf32_SymTable symbolTable, Elf32_RelTable reimplantationTable);

/**
 * Extrait la table de relocation d'un fichier ELF
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf
 * @param header Header du fichier @p elfFile
 * @param sectionTable Table des sections du fichier @p elfFile
 * @param symbolTable Table des symboles du fichier @p elfFile
 * @return Un tableau de Relocation correspondant à la table des sections
 */
Elf32_RelTable ExtractReimplantationTable(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable,
                                          Elf32_SymTable symbolTable);


/**
 * Extrait toutes les informations d'un fichier ELF et créer une structure ELF
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf
 * @param path Chemin d'acces au fichier ELF
 * @return Un objet de type ELF32_Structure
 */
Elf32_Structure ExtractElfInformation(FILE *elfFile, char *path);

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

#pragma region Fusion methods

/**
 * Fusionne les tables de sections.
 * <br>
 * Précondition : chaque fichiers de @p elfFiles doit être ouvert
 * @param elfFiles Fichiers elf à fusionner
 * @param elfHeaders Headers des 2 fichiers
 * @param sectionTables Table des sections des 2 fichiers elf
 * @return Résultat de la fusion
 */
Elf32_SectionFusion FusionSections(FILE *elfFiles[2], Elf32_Ehdr elfHeaders[2],
                                   Elf32_ShdrTable sectionTables[2]);

Elf32_SymbolFusion FusionSymbols(FILE *elfFiles[2], Elf32_Ehdr elfHeaders[2],Elf32_ShdrTable sectionTable[2],
 Elf32_SymTable symbolTables[2]);
//
//Elf32_RelTable FusionReimplantation(Elf32_Ehdr elfHeaders[2], Elf32_RelTable reimplantationTables[2],
//                          Elf32_SectionFusion sectionFusion, Elf32_SymbolFusion symbolFusion);

#pragma endregion

#endif //PROJET_PROG_ELFFILE_H