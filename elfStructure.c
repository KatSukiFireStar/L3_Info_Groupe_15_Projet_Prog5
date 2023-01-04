//
// Created by Pyrolink on 04/01/2023.
//

#include "elfStructure.h"

Elf32_Structure NewElf32_Structure(char *path, Elf32_Ehdr header, Elf32_ShdrTable sectionTable,
                                   Elf32_SymTable symbolTable, Elf32_RelTable reimplantationTable)
{
    Elf32_Structure result = {path, header, sectionTable, symbolTable, reimplantationTable};
    return result;
}