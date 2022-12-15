//
// Created by Pyrolink on 14/12/2022.
//

#include "elfFile.h"

void ShowSectionFromIndex(FILE *elfFile, Elf32_Shdr *table, int index)
{
    Elf32_Shdr section = table[index];


}

void ShowSectionFromName(FILE *elfFile, Elf32_Shdr *table, Elf32_Word name)
{
    size_t tableSize = sizeof(table) / sizeof(table[0]);

    for (int i = 0; i < tableSize; i++)
    {
        if (table[i].sh_name == name)
        {
            ShowSectionFromIndex(elfFile, table, i);
            return;
        }
    }
}

int main(int argc, char *argv[])
{
    return 0;
}