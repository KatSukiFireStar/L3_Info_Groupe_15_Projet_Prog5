//
// Created by Pyrolink on 04/01/2023.
//

#include <stdio.h>

#include "elfStructure.h"
#include "utils.h"

Elf32_SectionFusion NewSectionFusion(Elf32_Word sectionSize1, Elf32_Word sectionSize2)
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

    Elf32_SectionFusion sf = {newIndices, concatenationOffset, tmpnam(NULL), 0};
    return sf;
}

void FreeSectionFusion(Elf32_SectionFusion fusion)
{
    free(fusion.newIndices);
    free(fusion.concatenationOffset);
    //remove(fusion.tmpFile);
}

Elf32_Structure NewElf32_Structure(char *path, Elf32_Ehdr header, Elf32_ShdrTable sectionTable,
                                   Elf32_SymTable symbolTable, Elf32_RelTable reimplantationTable,
                                   int sectionCount, int symbolCount, int reimplantationCount)
{
    Elf32_Structure result = {path, header, sectionTable, symbolTable, reimplantationTable, sectionCount, symbolCount,
                              reimplantationCount};
    return result;
}

void FreeElf32_Structure(Elf32_Structure structure)
{
    free(structure.sectionTable);
    free(structure.symbolTable);
    free(structure.reimplantationTable);
}