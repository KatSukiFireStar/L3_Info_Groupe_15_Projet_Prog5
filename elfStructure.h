//
// Created by Pyrolink on 04/01/2023.
//

#ifndef PROJET_PROG_ELFSTRUCTURE_H
#define PROJET_PROG_ELFSTRUCTURE_H

#include <stdlib.h>
#include <elf.h>

/** Représente une table des sections */
typedef Elf32_Shdr *Elf32_ShdrTable;
/** Représente une table des symboles */
typedef Elf32_Sym *Elf32_SymTable;
/** Représente une table des réimplémentations */
typedef Elf32_Rel *Elf32_RelTable;

typedef struct
{
    char *path;

    Elf32_Ehdr header;
    Elf32_ShdrTable sectionTable;
    Elf32_SymTable symbolTable;
    Elf32_RelTable reimplantationTable;
} Elf32_Structure;

Elf32_Structure NewElf32_Structure(char *path,Elf32_Ehdr header,Elf32_ShdrTable sectionTable,
                                          Elf32_SymTable symbolTable,Elf32_RelTable reimplantationTable);

inline Elf32_Structure FreeElf32_Structure(Elf32_Structure structure)
{
    free(structure.path);
    free(structure.symbolTable);
    free(structure.reimplantationTable);
}

#endif //PROJET_PROG_ELFSTRUCTURE_H
