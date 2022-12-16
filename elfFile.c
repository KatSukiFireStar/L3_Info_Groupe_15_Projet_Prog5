//
// Created by Pyrolink on 14/12/2022.
//

#include "elfFile.h"

void ShowSectionFromIndex(FILE *elfFile, Elf32_Shdr *table, int index)
{
    Elf32_Shdr section = table[index];

    fseek(elfFile, section.sh_offset, SEEK_SET);
    for (int i = 0; i < section.sh_size; i++)
    {
        unsigned char byte;
        fread(&byte, sizeof(byte), 1, elfFile);
        fprintf(stdin, "%x", byte);
        if ((i + 1) % 4 == 0)
        {
            fprintf(stdin, " ");
        }
    }
}

void ShowSectionFromName(FILE *elfFile, Elf32_Shdr *table, Elf32_Ehdr header, Elf32_Word name)
{
    for (int i = 0; i < header.e_shentsize; i++)
    {
        if (table[i].sh_name == name)
        {
            ShowSectionFromIndex(elfFile, table, i);
            return;
        }
    }
}

void BackToBegin(FILE *file)
{
    fseek(file, 0, SEEK_SET);
}

int main(int argc, char *argv[])
{
    FILE        *elfFile;
    Elf32_Ehdr  header;
    Elf32_Shdr  *sectionTable;
    Elf32_Sym   *symbolTable;
    Elf32_Rel   *reimplantationTable;

    for (int i = 1; i < argc; i++)
    {
        elfFile = fopen(argv[i], "r");

        header = ShowElfHeader(elfFile);
        BackToBegin(elfFile);
        sectionTable = ShowSectionTableAndDetails(elfFile, header);
        BackToBegin(elfFile);
        ShowSectionFromIndex(elfFile, sectionTable, 0);
        symbolTable = ShowSymbolsTableAndDetails(elfFile, header);
        BackToBegin(elfFile);
        reimplantationTable = ShowReimplantationTablesAndDetails(elfFile, header);

        (void) reimplantationTable;
        (void) symbolTable;

        fclose(elfFile);
    }

    return 0;
}