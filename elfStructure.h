//
// Created by Pyrolink on 04/01/2023.
//

#ifndef PROJET_PROG_ELFSTRUCTURE_H
#define PROJET_PROG_ELFSTRUCTURE_H

/**
 * @file
 * Fichier d'en-tête des structure d'un fichier ELF
 */

#include <stdlib.h>
#include <elf.h>

#pragma region Table typedef

/** Représente une table des sections (tableau des en-tête de sections) */
typedef Elf32_Shdr *Elf32_ShdrTable;
/** Représente une table des symboles (tableau des en-tête de symboles) */
typedef Elf32_Sym *Elf32_SymTable;
/** Représente une table des réimplémentations (tableau des en-tête de réimplémentation avec addend) */
typedef Elf32_Rela *Elf32_RelaTable;

#pragma endregion

#pragma region Elf32_Structure

/**
 * Représente les réimplémentation d'une section
 */
typedef struct
{
    /** Section de réimplémentation */
    int section;
    /** Table des réimplémentation de la section #section */
    Elf32_RelaTable reimplantation;
    /** Nombre de réimplémentation contenu dans la section #section */
    int nbRel;
} Elf32_Reim;

/** Représente une table de réimplémentation par section */
typedef Elf32_Reim *Elf32_ReimTable;

/** Représente la structure d'un fichier elf */
typedef struct
{
    /** Chemin relatif ou absolu du fichier */
    char *path;

    /** Header */
    Elf32_Ehdr header;

    /** Table des sections */
    Elf32_ShdrTable sectionTable;
    /** Nombre de sections */
    int sectionCount;

    /** Table des symboles */
    Elf32_SymTable symbolTable;
    /** Nombre de symboles */
    int symbolCount;

    /** Réimplémentations par tables */
    Elf32_ReimTable reimplantationTable;
    /** Nombre de tables de réimplémentation */
    int reimplantationCount;
} Elf32_Structure;

/**
 * Crée un Elf32_Structure
 * @param path Voir Elf32_Structure.path
 * @param header Voir Elf32_Structure.header
 * @param sectionTable Voir Elf32_Structure.sectionTable
 * @param symbolTable Voir Elf32_Structure.symbolTable
 * @param reimplantationTable Voir Elf32_Structure.reimplantationTable
 * @param sectionCount Voir Elf32_Structure.sectionCount
 * @param symbolCount Voir Elf32_Structure.symbolCount
 * @param reimplantationCount Voir Elf32_Structure.reimplantationCount
 * @return Une nouvelle instance de Elf32_Structure
 */
Elf32_Structure NewElf32_Structure(char *path, Elf32_Ehdr header, Elf32_ShdrTable sectionTable,
                                   Elf32_SymTable symbolTable, Elf32_ReimTable reimplantationTable,
                                   int sectionCount, int symbolCount, int reimplantationCount);

/**
 * Libère un Elf32_Structure
 * @param structure L'Elf32_Structure à libérer
 */
void FreeElf32_Structure(Elf32_Structure structure);

#pragma endregion

#pragma region Elf32_SectionFusion

/** Représente les données de fusion des sections de 2 fichiers */
typedef struct
{
    /** Nouveaux indices des sections du deuxième fichier */
    Elf32_Word *newIndices;
    /** Offset des concaténations du premier fichiers. -1 s'il n'y a pas de concaténation */
    Elf32_Off *concatenationOffset;
    /** Chemin vers un fichier temporaire qui contiens les sections */
    char *tmpFile;
    /** Nombre de section dans la fusion*/
    Elf32_Half numberSection;

} Elf32_SectionFusion;

/**
 * Crée une nouvelle instance de Elf32_SectionFusion
 * @param sectionSize1 Taille de la section du premier fichier
 * @param sectionSize2 Taille de la section du deuxième fichier
 * @return Une Elf32_SectionFusion avec ses tableaux alloués et un chemin de fichier temporaire
 * généré automatiquement
 */
Elf32_SectionFusion NewSectionFusion(Elf32_Word sectionSize1, Elf32_Word sectionSize2);

/**
 * Désaloue les tableaux de @p fusion et supprime le fichier temporaire
 * @param fusion Elf32_SectionFusion à libérer
 */
void FreeSectionFusion(Elf32_SectionFusion fusion);

#pragma endregion

/** Représente les données de fusion des symboles de 2 fichiers */
typedef struct
{
    /** Nouveaux indices des symboles du deuxième fichier */
    Elf32_Word *newIndices;
    /** Nouvelle table des symboles */
    Elf32_SymTable symbolTable;
    /** Nouvelle table de string des symboles */
    char **strtab;
    Elf32_Half nbSymbol;
} Elf32_SymbolFusion;

/** Représente les données de fusion des sections de 2 fichiers */
typedef struct
{
    int reimplantationCount;
    /** Nouveaux indices des réimplémentations par table du deuxième fichier */
    Elf32_Word *newIndices;
    /** Nouvelle table de réimplémentation */
    Elf32_ReimTable reimplantationTable;
} Elf32_RelFusion;

void FreeRelFusion(Elf32_RelFusion relFusion);



#endif //PROJET_PROG_ELFSTRUCTURE_H
