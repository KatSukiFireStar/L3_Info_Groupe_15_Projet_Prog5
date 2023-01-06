//
// Created by Pyrolink on 04/01/2023.
//

#ifndef PROJET_PROG_ELFSTRUCTURE_H
#define PROJET_PROG_ELFSTRUCTURE_H

#include <stdlib.h>
#include <elf.h>

#pragma region Table typedef

typedef Elf32_Shdr *Elf32_ShdrTable;
/** Représente une table des symboles */
typedef Elf32_Sym *Elf32_SymTable;
/** Représente une table des réimplémentations */
typedef Elf32_Rela *Elf32_RelaTable;

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
 * @return Une Elf32_SectionFusion avec ses tableaux alloués (hormis @p tmpOffsets) et un chemin de fichier temporaire
 * généré automatiquement
 */
Elf32_SectionFusion NewSectionFusion(Elf32_Word sectionSize1, Elf32_Word sectionSize2);

/**
 * Désaloue les tableaux de @p fusion et supprime le fichier temporaire
 * @param fusion Elf32_SectionFusion à libérer
 */
void FreeSectionFusion(Elf32_SectionFusion fusion);

#pragma endregion

typedef struct
{
    int section;
    Elf32_RelaTable reimplantation;
    int nbRel;
} Elf32_Reim;

typedef Elf32_Reim *Elf32_ReimTable;

typedef struct
{
    Elf32_Word *newIndices;
    Elf32_SymTable symbolTable;
    char *strtab;
} Elf32_SymbolFusion;

typedef struct
{
    Elf32_Word *newIndices;
    Elf32_ReimTable reimplantationTable;
} Elf32_RelFusion;

typedef struct
{
    char *path;

    Elf32_Ehdr header;
    Elf32_ShdrTable sectionTable;
    Elf32_SymTable symbolTable;
    Elf32_ReimTable reimplantationTable;

    int sectionCount;
    int symbolCount;
    int reimplantationCount;

} Elf32_Structure;

Elf32_Structure NewElf32_Structure(char *path, Elf32_Ehdr header, Elf32_ShdrTable sectionTable,
                                   Elf32_SymTable symbolTable, Elf32_ReimTable reimplantationTable,
                                   int sectionCount, int symbolCount, int reimplantationCount);

void FreeElf32_Structure(Elf32_Structure structure);

#endif //PROJET_PROG_ELFSTRUCTURE_H
