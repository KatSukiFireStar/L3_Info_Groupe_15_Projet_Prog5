//
// Created by Pyrolink on 14/12/2022.
//

#include <stdio.h>
#include <stdlib.h>
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

Elf32_Sym *ShowSymbolsTableAndDetails(FILE *elfFile, Elf32_Shdr section){
    if(section.sh_type == 2){
    Elf32_Sym *symbolTable = malloc(sizeof(Elf32_Sym) * section.sh_entsize);
    fseek(elfFile, section.sh_offset, SEEK_SET);

        int i;
        for (i = 0; i < section.sh_entsize; i++) {
            fread(&symbolTable[i].st_name, sizeof(Elf32_Word), 1, elfFile);
            printf("Nom du symbol: %ls\n", &symbolTable[i].st_name);
            fread(&symbolTable[i].st_size, sizeof(Elf32_Word), 1, elfFile);
            printf("Taille du symbol : %d\n", symbolTable[i].st_size);
            fread(&symbolTable[i].st_shndx, sizeof(Elf32_Section), 1, elfFile);
            printf("Index de la table : %d\n", symbolTable[i].st_shndx);
            fread(&symbolTable[i].st_value, sizeof(Elf32_Addr), 1, elfFile);
            printf("Valeur du symbol : %d\n", symbolTable[i].st_value);



}}}


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

    elfFile = fopen(argv[1], "r");

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
    return 0;
}