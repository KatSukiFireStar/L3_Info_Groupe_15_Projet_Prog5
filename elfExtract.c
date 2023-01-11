//
// Created by Pyrolink on 07/01/2023.
//

#include "elfFile.h"
#include "utils.h"

Elf32_Structure ExtractElfInformation(FILE *elfFile, char *path)
{
    int sectionCount;
    int symbolCount = 0;
    int reimplantationCount = 0;
    Elf32_Ehdr header = ExtractHeader(elfFile);
    Elf32_ShdrTable sectionTable = ExtractSectionTable(elfFile, header);
    Elf32_SymTable symbolTable = ExtractSymbolsTable(elfFile, header, sectionTable, &symbolCount);
    Elf32_ReimTable reimplatationTable = ExtractReimplantationTable(elfFile, header, sectionTable,
                                                                    &reimplantationCount);
    sectionCount = header.e_shnum;
    Elf32_Structure structure = NewElf32_Structure(path, header, sectionTable, symbolTable, reimplatationTable,
                                                   sectionCount, symbolCount, reimplantationCount);

    return structure;
}

Elf32_Ehdr ExtractHeader(FILE *elfFile)
{
    Elf32_Ehdr header;

    freadEndian(&header.e_ident, sizeof(unsigned char), EI_NIDENT, elfFile);
    CheckMachineEndian(header.e_ident[EI_DATA]);
    freadEndian(&header.e_type, sizeof(Elf32_Half), 1, elfFile);
    freadEndian(&header.e_machine, sizeof(Elf32_Half), 1, elfFile);
    freadEndian(&header.e_version, sizeof(Elf32_Word), 1, elfFile);
    freadEndian(&header.e_entry, sizeof(Elf32_Addr), 1, elfFile);
    freadEndian(&header.e_phoff, sizeof(Elf32_Off), 1, elfFile);
    freadEndian(&header.e_shoff, sizeof(Elf32_Off), 1, elfFile);
    freadEndian(&header.e_flags, sizeof(Elf32_Word), 1, elfFile);
    freadEndian(&header.e_ehsize, sizeof(Elf32_Half), 1, elfFile);
    freadEndian(&header.e_phentsize, sizeof(Elf32_Half), 1, elfFile);
    freadEndian(&header.e_phnum, sizeof(Elf32_Half), 1, elfFile);
    freadEndian(&header.e_shentsize, sizeof(Elf32_Half), 1, elfFile);
    freadEndian(&header.e_shnum, sizeof(Elf32_Half), 1, elfFile);
    freadEndian(&header.e_shstrndx, sizeof(Elf32_Half), 1, elfFile);

    return header;
}

Elf32_ShdrTable ExtractSectionTable(FILE *elfFile, Elf32_Ehdr header)
{
    Elf32_ShdrTable sectionTable = mallocArray(Elf32_Shdr, header.e_shnum);
    fseek(elfFile, header.e_shoff, SEEK_SET);
    // Lecture de la table des sections
    for (Elf32_Half sectionIndex = 0; sectionIndex < header.e_shnum; sectionIndex++)
    {
        // Lire le nom de la section
        freadEndian(&sectionTable[sectionIndex].sh_name, sizeof(Elf32_Word), 1, elfFile);

        // Lire le type de la section
        freadEndian(&sectionTable[sectionIndex].sh_type, sizeof(Elf32_Word), 1, elfFile);

        // Lire le type de la section
        freadEndian(&sectionTable[sectionIndex].sh_flags, sizeof(Elf32_Word), 1, elfFile);

        // Lire l'adresse de la section
        freadEndian(&sectionTable[sectionIndex].sh_addr, sizeof(Elf32_Addr), 1, elfFile);

        // Lire la position de la section
        freadEndian(&sectionTable[sectionIndex].sh_offset, sizeof(Elf32_Word), 1, elfFile);

        // Lire la taille de la section
        freadEndian(&sectionTable[sectionIndex].sh_size, sizeof(Elf32_Word), 1, elfFile);

        // Lire l'indice de la table des en-têtes de sections
        freadEndian(&sectionTable[sectionIndex].sh_link, sizeof(Elf32_Word), 1, elfFile);

        // Lire les informations supplémentaires
        freadEndian(&sectionTable[sectionIndex].sh_info, sizeof(Elf32_Word), 1, elfFile);

        // Lire la taille de l'alignement
        freadEndian(&sectionTable[sectionIndex].sh_addralign, sizeof(Elf32_Word), 1, elfFile);

        // Lire la taille de l'entrée
        freadEndian(&sectionTable[sectionIndex].sh_entsize, sizeof(Elf32_Word), 1, elfFile);
    }

    return sectionTable;
}

Elf32_SymTable ExtractSymbolsTable(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable, int *symbolCount)
{
    Elf32_Half symTableSize = GetEntryCountFromType(header, sectionTable, SHT_SYMTAB);

    Elf32_Sym *symbolTable = mallocArray(Elf32_Sym, symTableSize);
    for (Elf32_Half sectionIndex = 0; sectionIndex < header.e_shnum; sectionIndex++)
    {
        Elf32_Shdr section = sectionTable[sectionIndex];

        if (section.sh_type != SHT_SYMTAB)
        {
            continue;
        }

        Elf32_Half count = section.sh_size / section.sh_entsize;

        fseek(elfFile, section.sh_offset, SEEK_SET);

        for (Elf32_Half symbolIndex = 0; symbolIndex < count; symbolIndex++)
        {
            freadEndian(&symbolTable[symbolIndex].st_name, sizeof(Elf32_Word), 1, elfFile);
            freadEndian(&symbolTable[symbolIndex].st_value, sizeof(Elf32_Addr), 1, elfFile);
            freadEndian(&symbolTable[symbolIndex].st_size, sizeof(Elf32_Word), 1, elfFile);
            freadEndian(&symbolTable[symbolIndex].st_info, sizeof(unsigned char), 1, elfFile);
            freadEndian(&symbolTable[symbolIndex].st_other, sizeof(unsigned char), 1, elfFile);
            freadEndian(&symbolTable[symbolIndex].st_shndx, sizeof(Elf32_Section), 1, elfFile);
        }
    }
    *symbolCount = symTableSize;
    return symbolTable;
}

Elf32_ReimTable ExtractReimplantationTable(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable,
                                           int *reimplantationCount)
{
    Elf32_Half reimTableSize = 0;
    for (Elf32_Half i = 0; i < header.e_shnum; i++)
    {
        if (sectionTable[i].sh_type == SHT_REL || sectionTable[i].sh_type == SHT_RELA)
        {
            reimTableSize++;
        }
    }

    Elf32_ReimTable reimplantationTable = mallocArray(Elf32_Reim, reimTableSize);
    Elf32_Half reimplantationIndex = 0;
    for (Elf32_Half tableIndex = 0; tableIndex < header.e_shnum; tableIndex++)
    {
        if (sectionTable[tableIndex].sh_type != SHT_REL && sectionTable[tableIndex].sh_type != SHT_RELA)
        {
            continue;
        }

        fseek(elfFile, sectionTable[tableIndex].sh_offset, SEEK_SET);

        // Elf32_Half symbolIndexOffset = symbolIndex;
        Elf32_Half symbolsInTable = sectionTable[tableIndex].sh_size / sectionTable[tableIndex].sh_entsize;

        reimplantationTable[reimplantationIndex].nbRel = symbolsInTable;
        reimplantationTable[reimplantationIndex].section = tableIndex;
        reimplantationTable[reimplantationIndex].reimplantation = mallocArray(Elf32_Rela, symbolsInTable);

        for (Elf32_Half i = 0; i < symbolsInTable; i++)
        {
            freadEndian(&reimplantationTable[reimplantationIndex].reimplantation[i].r_offset,
                        sizeof(Elf32_Addr), 1, elfFile);
            freadEndian(&reimplantationTable[reimplantationIndex].reimplantation[i].r_info,
                        sizeof(Elf32_Word), 1, elfFile);

            if (sectionTable[tableIndex].sh_type == SHT_RELA)
            {
                freadEndian(&reimplantationTable[reimplantationIndex].reimplantation[i].r_addend,
                            sizeof(Elf32_Sword), 1, elfFile);
            }
        }

        reimplantationIndex++;
    }
    *reimplantationCount = reimTableSize;
    return reimplantationTable;
}