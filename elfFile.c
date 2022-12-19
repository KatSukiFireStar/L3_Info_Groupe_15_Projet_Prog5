//
// Created by Pyrolink on 14/12/2022.
//

#include <stdlib.h>
#include <unistd.h>
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

void ShowSectionFromName(FILE *elfFile, Elf32_Shdr *table, Elf32_Ehdr header, unsigned char *name)
{
    Elf32_Shdr stringTable = table[header.e_shstrndx];

    fseek(elfFile, stringTable.sh_offset, SEEK_SET);

    int nameId = -1;

    int skip = 0;
    int currentIndex = 0;

    for (int i = 1; i < stringTable.sh_size; i++)
    {
        unsigned char currentChar;
        fscanf(elfFile, "%c", &currentChar);

        if (skip == 1)
        {
            if (currentChar == '\0')
            {
                skip = 0;
            }
            continue;
        }

        if (name[currentIndex] == currentChar)
        {
            if (currentChar == '\0')
            {
                nameId = i - currentIndex;
                break;
            }
            currentIndex++;
        }
        else
        {
            skip = 1;
            currentIndex = 0;
        }
    }

    if (nameId == -1)
    {
        exit(-2);
    }

    for (int i = 0; i < header.e_shentsize; i++)
    {
        if (table[i].sh_name == nameId)
        {
            ShowSectionFromIndex(elfFile, table, i);
            return;
        }
    }
}

int main(int argc, char *argv[])
{
    FILE *elfFile;
    Elf32_Ehdr header[argc - 1];
    Elf32_Shdr *sectionTable[argc - 1];
    Elf32_Sym *symbolTable[argc - 1];
    Elf32_Rel *reimplantationTable[argc - 1];

    if (argc <= 1)
    {
        fprintf(stderr, "Vous n'avez pas mis de fichier en paramètres!");
        return -3;
    }
//    else if (argc <= 2)
//    {
//        fprintf(stderr, "Vous n'avez mis qu'un seul fichier en paramètres!");
//        return -4;
//    }

    for (int i = 1; i < argc; i++)
    {
        if (access(argv[i], R_OK))
        {
            fprintf(stderr, "Le fichier %s ne peut pas etre ouvert!", argv[i]);
            return -5;
        }
    }

    // Permet de recuperer toutes les informations d'un fichier
    // et les stockes dans des variables
    // A la fin, le fichier est fermé et on ouvre le fichier suivant
    for (int i = 1; i < argc; i++)
    {
        elfFile = fopen(argv[i], "r");

        header[i - 1] = ShowElfHeader(elfFile);
        rewind(elfFile);
        sectionTable[i - 1] = ShowSectionTableAndDetails(elfFile, header[i - 1]);
        rewind(elfFile);
        ShowSectionFromIndex(elfFile, sectionTable[i - 1], 0);
        symbolTable[i - 1] = ShowSymbolsTableAndDetails(elfFile, header[i - 1]);
        rewind(elfFile);
        reimplantationTable[i - 1] = ShowReimplantationTablesAndDetails(elfFile, header[i - 1]);

        (void) reimplantationTable;
        (void) symbolTable;

        fclose(elfFile);
    }

    return 0;
}