//
// Created by Pyrolink on 14/12/2022.
//

#include "elfFile.h"
#include <stdio.h>
#include <elf.h>
#include <stdlib.h>

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

Elf32_Shdr *ShowSectionTableAndDetails(FILE *elfFile, Elf32_Ehdr header){
    Elf32_Shdr *sectionTable = malloc(sizeof(Elf32_Shdr) * header.e_shnum);
    fseek(elfFile, header.e_shoff, SEEK_SET);
    
    fread(&sectionTable, header.e_shentsize, header.e_shnum, elfFile);
    for (int i = 0; i < header.e_shnum; i++) {
        //lire le nom de la section
        fread(&sectionTable[i].sh_name, sizeof(Elf32_Word), 1, elfFile);
        printf("Section: %ls\n", &sectionTable[i].sh_name);
        //lire la taille de la section
        fread(&sectionTable[i].sh_size, sizeof(Elf32_Word), 1, elfFile);
        //Lire la taille de la section
        fread(&sectionTable[i].sh_type, sizeof(Elf32_Word), 1, elfFile);
        printf("Taille de la section : %d\n", sectionTable[i].sh_size);
        //Lire le type de la section
        fread(&sectionTable[i].sh_type, sizeof(Elf32_Word), 1, elfFile);
        printf("Type de la section : %d\n", sectionTable[i].sh_type);
        //Lire la position de la section
        fread(&sectionTable[i].sh_offset, sizeof(Elf32_Word), 1, elfFile);
        printf("Position de la section : %d\n", sectionTable[i].sh_offset);

        
         switch (sectionTable[i].sh_flags) {
            //Lire les attributs de la section
            fread(&sectionTable[i].sh_flags, sizeof(Elf32_Word), 1, elfFile);
             case SHF_WRITE:
                  
                 if (sectionTable[i].sh_flags == SHF_WRITE) 
                    printf("Cette section contient des données qu'il devrait être possible d'écrire durant l'exécution du processus.\n");
                else
                    printf("Cette section ne contient pas des données qu'il devrait être possible d'écrire durant l'exécution du processus.\n");
                
                 break;
             case SHF_ALLOC:
                if (sectionTable[i].sh_flags == SHF_ALLOC) 
                    printf("La section fait partie de l'image mémoire du programme à exécuter.\n");
                else
                    printf("La section ne fait pas partie de l'image mémoire du programme à exécuter.\n");
                 break;
             case SHF_EXECINSTR:
                 if(sectionTable[i].sh_flags == SHF_EXECINSTR) 
                    printf("La section contient du code exécutable.\n");
                else
                    printf("La section ne contient pas du code exécutable.\n");
                 break;
             case SHF_MASKPROC:
                if (sectionTable[i].sh_flags == SHF_MASKPROC) 
                    printf("Tous les bits contenus dans ce masque sont réservés à des sémantiques spécifiques au processeur.\n");
                else
                    printf("Tous les bits contenus dans ce masque ne sont pas réservés à des sémantiques spécifiques au processeur.\n");
                 break;
             default:
                 break;
         }
        fread(&sectionTable[i].sh_addr, sizeof(Elf32_Word), 1, elfFile);
        printf("Adresse de la section : %d\n", sectionTable[i].sh_addr);
        fread(&sectionTable[i].sh_offset, sizeof(Elf32_Word), 1, elfFile);
        printf("Décalage de la section : %d\n", sectionTable[i].sh_offset);
        fread(&sectionTable[i].sh_link, sizeof(Elf32_Word), 1, elfFile);
        printf("Lien vers un indice de la table des en-têtes de sections : %d\n", sectionTable[i].sh_link);
        fread(&sectionTable[i].sh_info, sizeof(Elf32_Word), 1, elfFile);
        printf("Informations supplémentaires, dépendant du type de section : %d\n", sectionTable[i].sh_info);
        fread(&sectionTable[i].sh_addralign, sizeof(Elf32_Word), 1, elfFile);
        printf("La taille de l'alignement, exprimée en puissance de 2 : %d\n", sectionTable[i].sh_addralign);
        fread(&sectionTable[i].sh_entsize, sizeof(Elf32_Word), 1, elfFile);
        printf("La taille de l'entrée, pour les sections qui contiennent une table d'entrées de même taille : %d\n", sectionTable[i].sh_entsize);
    }
    return sectionTable;
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