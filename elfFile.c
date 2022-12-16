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
        fprintf(stdout, "%x", byte);
        if ((i + 1) % 4 == 0)
        {
            fprintf(stdout, " ");
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
    Elf32_Ehdr  header[argc-1];
    Elf32_Shdr  *sectionTable[argc-1];
    Elf32_Sym   *symbolTable[argc-1];
    Elf32_Rel   *reimplantationTable[argc-1];

    //Permet de recuperer toutes les informations d'un fichier
    //et les stockes dans des variables
    //A la fin, le fichier est fermé et on ouvre le fichier suivant
    for (int i = 1; i < argc; i++)
    {
        elfFile = fopen(argv[i], "r");

        header[i-1] = ShowElfHeader(elfFile);
        BackToBegin(elfFile);
        sectionTable[i-1] = ShowSectionTableAndDetails(elfFile, header[i-1]);
        BackToBegin(elfFile);
        ShowSectionFromIndex(elfFile, sectionTable[i-1], 0);
        symbolTable[i-1] = ShowSymbolsTableAndDetails(elfFile, header[i-1]);
        BackToBegin(elfFile);
        reimplantationTable[i-1] = ShowReimplantationTablesAndDetails(elfFile, header[i-1]);

        (void) reimplantationTable;
        (void) symbolTable;

        fclose(elfFile);
    }

    return 0;
}