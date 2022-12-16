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

Elf32_Sym *listeTableSymbol(FILE *elfFile, Elf32_Ehdr header, Elf32_Shdr *sectionTable){
     for(int i = 0; i < header.e_shnum; i++) {
        if(&sectionTable[i].sh_type == SHT_SYMTAB){
            ShowSymbolsTableAndDetails(elfFile, sectionTable[i]);
        }
    }
}

Elf32_Sym *listeTableSymbolD(FILE *elfFile, Elf32_Ehdr header, Elf32_Shdr *sectionTable){
    for(int i = 0; i < header.e_shnum; i++) {
        if(&sectionTable[i].sh_type == SHT_DYNSYM){
            ShowSymbolsTableAndDetailss(elfFile, sectionTable[i]);
        }
    }
}

Elf32_Sym *ShowSymbolsTableAndDetails(FILE *elfFile, Elf32_Shdr section){
        Elf32_Sym *symbolTable = malloc(sizeof(Elf32_Sym) * section.sh_entsize);
        fseek(elfFile, section.sh_offset, SEEK_SET);

        for (int i = 0; i < section.sh_entsize; i++) {
            fread(&symbolTable[i].st_name, sizeof(Elf32_Word), 1, elfFile);
            printf("Nom du symbol: %ls\n", &symbolTable[i].st_name);
            fread(&symbolTable[i].st_size, sizeof(Elf32_Word), 1, elfFile);
            printf("Taille du symbol : %d\n", symbolTable[i].st_size);
            fread(&symbolTable[i].st_shndx, sizeof(Elf32_Section), 1, elfFile);
            printf("Index de la table : %d\n", symbolTable[i].st_shndx);
            fread(&symbolTable[i].st_value, sizeof(Elf32_Addr), 1, elfFile);
            printf("Valeur du symbol : %d\n", symbolTable[i].st_value);
            fread(&symbolTable[i].st_info, sizeof(unsigned char), 1, elfFile);
            switch (symbolTable[i].st_info) {
                case STB_LOCAL:
                    printf("Local symbol\n");
                case STB_GLOBAL:
                    printf("Global symbol\n");
                case STB_WEAK:
                    printf("Weak symbol\n");
                case STB_NUM:
                    printf("Number of defined types\n");
                case STB_LOOS:
                    printf("Start of OS-specific\n");
                case STB_HIOS:
                    printf("End of OS-specific\n");
                case STB_LOPROC:
                    printf("Start of processor-specific\n");
                case STB_HIPROC:
                    printf("End of processor-specific\n");
            }
            printf("Insertion BIND : %c\n", ELF32_ST_BIND(symbolTable[i].st_value));
            printf("Insertion TYPE : %u\n", ELF32_ST_TYPE(symbolTable[i].st_value));
            printf("Insertion INFO : %c\n", ELF32_ST_INFO(ELF32_ST_BIND(symbolTable[i].st_value),ELF32_ST_TYPE(symbolTable[i].st_value)));

            fread(&symbolTable[i].st_other, sizeof(unsigned char), 1, elfFile);
            switch (symbolTable[i].st_other) {
                case STV_DEFAULT:
                    printf("Default symbol visibility rules\n");
                case STV_INTERNAL:
                    printf("Processor specific hidden class\n");
                case STV_HIDDEN:
                    printf("Sym unavailable in other modules\n");
                case STV_PROTECTED:
                    printf("Not preemptible, not exported\n");
            }

        }
    return symbolTable;}

Elf32_Sym *ShowSymbolsTableDynamicAndDetails(FILE *elfFile, Elf32_Shdr section){
                Elf32_Sym *symbolTable = malloc(sizeof(Elf32_Sym) * section.sh_entsize);
                fseek(elfFile, section.sh_offset, SEEK_SET);


            Elf32_Syminfo *symbolTablee = malloc(sizeof(Elf32_Syminfo) * section.sh_entsize);
            fseek(elfFile, section.sh_offset, SEEK_SET);

            for (int i = 0; i < section.sh_entsize; i++) {
                fread(&symbolTablee[i].si_boundto, sizeof(Elf32_Half), 1, elfFile);
                fread(&symbolTablee[i].si_flags, sizeof(Elf32_Half), 1, elfFile);
                if ((symbolTablee[i].si_boundto & SYMINFO_BT_SELF)== SYMINFO_BT_SELF){
                    printf("Symbol bound to self\n");}
                if ((symbolTablee[i].si_boundto & SYMINFO_BT_PARENT)== SYMINFO_BT_PARENT){
                    printf("Symbol bound to parent\n");}
                if ((symbolTablee[i].si_boundto & SYMINFO_BT_LOWRESERVE)== SYMINFO_BT_LOWRESERVE){
                    printf("Beginning of reserved entries\n");}
                if ((symbolTablee[i].si_flags & SYMINFO_FLG_DIRECT)== SYMINFO_FLG_DIRECT){
                    printf("Direct bound symbol \n");}
                if ((symbolTablee[i].si_flags & SYMINFO_FLG_PASSTHRU)== SYMINFO_FLG_PASSTHRU){
                    printf("Pass-thru symbol for translator\n");}
                if ((symbolTablee[i].si_flags & SYMINFO_FLG_COPY)== SYMINFO_FLG_COPY){
                    printf("Symbol is a copy-reloc\n");}
                if ((symbolTablee[i].si_flags & SYMINFO_FLG_LAZYLOAD)== SYMINFO_FLG_LAZYLOAD){
                    printf("Symbol bound to object to be lazy\n");}
            }
            return symbolTablee;
        }

/*Elf32_Sym *ShowSymbolsTableAndDetails(FILE *elfFile, Elf32_Shdr section){
    if(section.sh_type == SHT_SYMTAB){
    Elf32_Sym *symbolTable = malloc(sizeof(Elf32_Sym) * section.sh_entsize);
    fseek(elfFile, section.sh_offset, SEEK_SET);

        for (int i = 0; i < section.sh_entsize; i++) {
            fread(&symbolTable[i].st_name, sizeof(Elf32_Word), 1, elfFile);
            printf("Nom du symbol: %ls\n", &symbolTable[i].st_name);
            fread(&symbolTable[i].st_size, sizeof(Elf32_Word), 1, elfFile);
            printf("Taille du symbol : %d\n", symbolTable[i].st_size);
            fread(&symbolTable[i].st_shndx, sizeof(Elf32_Section), 1, elfFile);
            printf("Index de la table : %d\n", symbolTable[i].st_shndx);
            fread(&symbolTable[i].st_value, sizeof(Elf32_Addr), 1, elfFile);
            printf("Valeur du symbol : %d\n", symbolTable[i].st_value);
            fread(&symbolTable[i].st_info, sizeof(unsigned char), 1, elfFile);
            switch (symbolTable[i].st_info) {
                case STB_LOCAL:
                    printf("Local symbol\n");
                case STB_GLOBAL:
                    printf("Global symbol\n");
                case STB_WEAK:
                    printf("Weak symbol\n");
                case STB_NUM:
                    printf("Number of defined types\n");
                case STB_LOOS:
                    printf("Start of OS-specific\n");
                case STB_HIOS:
                    printf("End of OS-specific\n");
                case STB_LOPROC:
                    printf("Start of processor-specific\n");
                case STB_HIPROC:
                    printf("End of processor-specific\n");
            }
            printf("Insertion BIND : %c\n", ELF32_ST_BIND(symbolTable[i].st_value));
            printf("Insertion TYPE : %u\n", ELF32_ST_TYPE(symbolTable[i].st_value));
            printf("Insertion INFO : %c\n", ELF32_ST_INFO(ELF32_ST_BIND(symbolTable[i].st_value),ELF32_ST_TYPE(symbolTable[i].st_value)));

            fread(&symbolTable[i].st_other, sizeof(unsigned char), 1, elfFile);
            switch (symbolTable[i].st_other) {
                case STV_DEFAULT:
                    printf("Default symbol visibility rules\n");
                case STV_INTERNAL:
                    printf("Processor specific hidden class\n");
                case STV_HIDDEN:
                    printf("Sym unavailable in other modules\n");
                case STV_PROTECTED:
                    printf("Not preemptible, not exported\n");
            }
            return symbolTable;
        }
        if(section.sh_type == SHT_DYNSYM){
            Elf32_Syminfo *symbolTablee = malloc(sizeof(Elf32_Syminfo) * section.sh_entsize);
            fseek(elfFile, section.sh_offset, SEEK_SET);

            for (int i = 0; i < section.sh_entsize; i++) {
                fread(&symbolTablee[i].si_boundto, sizeof(Elf32_Half), 1, elfFile);
                fread(&symbolTablee[i].si_flags, sizeof(Elf32_Half), 1, elfFile);
                if ((symbolTablee[i].si_boundto & SYMINFO_BT_SELF)== SYMINFO_BT_SELF){
                    printf("Symbol bound to self\n");}
                if ((symbolTablee[i].si_boundto & SYMINFO_BT_PARENT)== SYMINFO_BT_PARENT){
                    printf("Symbol bound to parent\n");}
                if ((symbolTablee[i].si_boundto & SYMINFO_BT_LOWRESERVE)== SYMINFO_BT_LOWRESERVE){
                    printf("Beginning of reserved entries\n");}
                if ((symbolTablee[i].si_flags & SYMINFO_FLG_DIRECT)== SYMINFO_FLG_DIRECT){
                    printf("Direct bound symbol \n");}
                if ((symbolTablee[i].si_flags & SYMINFO_FLG_PASSTHRU)== SYMINFO_FLG_PASSTHRU){
                    printf("Pass-thru symbol for translator\n");}
                if ((symbolTablee[i].si_flags & SYMINFO_FLG_COPY)== SYMINFO_FLG_COPY){
                    printf("Symbol is a copy-reloc\n");}
                if ((symbolTablee[i].si_flags & SYMINFO_FLG_LAZYLOAD)== SYMINFO_FLG_LAZYLOAD){
                    printf("Symbol bound to object to be lazy\n");}
            }
            return symbolTablee;
        }
        else{
            printf("No Symbol Table");
        }
}}*/


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