//
// Created by Pyrolink on 14/12/2022.
//

#include "elfFile.h"
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


/*Commentrécupérer le nom du type de la section et le nom de la section???????
*/
Elf32_Shdr *ShowSectionTableAndDetails(FILE *elfFile, Elf32_Ehdr header)
{
    Elf32_Shdr *sectionTable = malloc(sizeof(Elf32_Shdr) * header.e_shnum);
    fseek(elfFile, header.e_shoff, SEEK_SET);

    //Lecture de la table des sections
    for (int i = 0; i < header.e_shnum; i++)
    {
        //Lire le nom de la section
        fread(&sectionTable[i].sh_name, sizeof(Elf32_Word), 1, elfFile);

        //Lire le type de la section
        fread(&sectionTable[i].sh_type, sizeof(Elf32_Word), 1, elfFile);

        //Lire l'adresse de la section
        fread(&sectionTable[i].sh_addr, sizeof(Elf32_Addr), 1, elfFile);

        //Lire la position de la section
        fread(&sectionTable[i].sh_offset, sizeof(Elf32_Word), 1, elfFile);

        //Lire la taille de la section
        fread(&sectionTable[i].sh_size, sizeof(Elf32_Word), 1, elfFile);

        //Lire la taille de l'entrée
        fread(&sectionTable[i].sh_entsize, sizeof(Elf32_Word), 1, elfFile);

        //Lire le type de la section
        fread(&sectionTable[i].sh_flags, sizeof(Elf32_Word), 1, elfFile);

        //Lire l'indice de la table des en-têtes de sections
        fread(&sectionTable[i].sh_link, sizeof(Elf32_Word), 1, elfFile);

        //Lire les informations supplémentaires
        fread(&sectionTable[i].sh_info, sizeof(Elf32_Word), 1, elfFile);

        //Lire la taille de l'alignement
        fread(&sectionTable[i].sh_addralign, sizeof(Elf32_Word), 1, elfFile);
    }

    Elf32_Shdr stringTable = sectionTable[header.e_shstrndx];

    //Affichage des informations de la table des sections
    for (int i = 0; i < header.e_shnum; i++)
    {
        printf("Nom de la section : ");
        fseek(elfFile, stringTable.sh_offset + sectionTable[i].sh_name, SEEK_SET);
        /*stringTable.sh_offset + sectionTable[i].sh_name permet de se positionner au début du nom de la section
         et de lire le nom de la section*/

        fseek(elfFile, stringTable.sh_offset + sectionTable[i].sh_name, SEEK_SET); /*permet donc de déplacer le curseur 
        au début du nom de la section*/

        char c = ' ';
        while (c != '\0')
        {
            fread(&c, sizeof(char), 1, elfFile);
            printf("%c", c);
        }

        printf("Type de la section : %u (", sectionTable[i].sh_type);
        switch (sectionTable[i].sh_type)
        {
            case SHT_NULL:
                printf("Section header table entry unused");
                break;
            case SHT_PROGBITS:
                printf("Program data");
                break;
            case SHT_SYMTAB:
                printf("Symbol table");
                break;
            case SHT_STRTAB:
                printf("String table");
                break;
            case SHT_RELA:
                printf("Relocation entries with addends");
                break;
            case SHT_HASH:
                printf("Symbol hash table");
                break;
            case SHT_DYNAMIC:
                printf("Dynamic linking information");
                break;
            case SHT_NOTE:
                printf("Notes");
                break;
            case SHT_NOBITS:
                printf("Program space with no data (bss)");
                break;
            case SHT_REL:
                printf("Relocation entries, no addends");
                break;
            case SHT_SHLIB:
                printf("Reserved");
                break;
            case SHT_DYNSYM:
                printf(" Dynamic linker symbol table");
                break;
            case SHT_LOPROC:
                printf("TStart of processor-specific");
                break;
            case SHT_HIPROC:
                printf("End of processor-specific");
                break;
            case SHT_LOUSER:
                printf("Start of application-specific");
                break;
            case SHT_HIUSER:
                printf("End of application-specific");
                break;
        }

        printf(")");

        //l'adresse à  laquelle le premier octet de la section doit se trouver.
        printf("Adresse de la section : %x\n", sectionTable[i].sh_addr);


        printf("Position de la section : %x\n", sectionTable[i].sh_offset);


        printf("Taille de la section : %u\n", sectionTable[i].sh_size);


        //la taille de l'entrée, pour les sections qui contiennent une table d'entrées de même taille.
        printf("La taille de l'entrée, pour les sections qui contiennent une table d'entrées de même taille : %x\n",
               sectionTable[i].sh_entsize);


        printf("Fanions de la section : %x\n", sectionTable[i].sh_flags);
        //Cette section contient des données qu'il devrait être possible d'écrire durant l'exécution du processus;
        if ((sectionTable[i].sh_flags & SHF_WRITE) == SHF_WRITE)
            printf("W");

        //La section fait partie de l'image mémoire du programme à exécuter.");
        if ((sectionTable[i].sh_flags & SHF_ALLOC) == SHF_ALLOC)
            printf("A");


        //La section contient du code exécutable.\n");
        if ((sectionTable[i].sh_flags & SHF_EXECINSTR) == SHF_EXECINSTR)
            printf("X");

        //Tous les bits contenus dans ce masque sont réservés à des sémantiques spécifiques au processeur
        if ((sectionTable[i].sh_flags & SHF_MASKPROC) == SHF_MASKPROC)
            printf("M");



        //lien vers un indice de la table des en-têtes de  sections,
        printf("Lien vers un indice de la table des en-têtes de sections : %x\n", sectionTable[i].sh_link);



        //informations supplémentaires, dépendant du type de section.
        printf("Informations supplémentaires, dépendant du type de section : %x\n", sectionTable[i].sh_info);


        //la taille de l'alignement, exprimée en puissance de 2.
        printf("La taille de l'alignement, exprimée en puissance de 2 : %x\n", sectionTable[i].sh_addralign);
    }

    return sectionTable;
}


int nbTableReimplantation(Elf32_Shdr *TableSection, Elf32_Ehdr header)
{
    int compteur = 0;
    int i;
    for (i = 0; i < header.e_shnum; i++)
    {
        if (TableSection[i].sh_type == SHT_REL)
        {
            compteur++;
        }
    }
    return compteur;
}

Elf32_Rel *ShowReimplantationTablesAndDetails(FILE *elfFile, Elf32_Ehdr header)
{
    Elf32_Shdr *TableSection = malloc(header.e_shnum * sizeof(Elf32_Shdr));
    Elf32_Rel *TableReimplantation = malloc(header.e_shnum * sizeof(Elf32_Rel));//e_shnum=nombre des entrées
    int i;
    //Pour chaque entrée de la table des sections
    for (i = 0; i < header.e_shnum; i++)
    {
        //Si c'est une table de réimplantation
        if (TableSection[i].sh_type == SHT_REL)
        {
            rewind(elfFile);//OU fseek(elfFile, sectionTable[i].sh_offset, SEEK_SET);
            int nb = nbTableReimplantation(TableSection, header);
            fread(TableReimplantation, sizeof(Elf32_Rel), nb, elfFile);
            int j;
            for (j = 0; j < nb; j++)
            {
                printf("La cible de la réimplantation est : %d\n", TableReimplantation[j].r_offset);
                printf("Le type de réimplantation est : %d\n", TableReimplantation[j].r_info);
                printf("L'index de l'entrée concernée dans la table des symboles est : %d\n",
                       TableReimplantation[j].r_info >> 8);

            }
        }
        return TableReimplantation;

    }
}
