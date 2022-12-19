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

    //Pour chaque entrée de la table, il est demand´e d’afficher les principales caractéristiques
    int i;
    for (i = 0; i < header.e_shnum; i++)
    {
        //Le nom de la section
        Elf32_Shdr stringTable = sectionTable[header.e_shstrndx];
        fseek(elfFile, stringTable.sh_offset + sectionTable[i].sh_name, SEEK_SET);
        /*stringTable.sh_offset + sectionTable[i].sh_name permet de se positionner au début du nom de la section
         * et de lire le nom de la section
        fseek(elfFile, stringTable.sh_offset + sectionTable[i].sh_name, SEEK_SET) permet donc de déplacer le curseur 
        au début du nom de la section
         */

        int j = 0;
        char c;
        //On veut lire chaque caractére du nom de la section et le stocker dans une varaible nom_section
        char nom_section[100];
        while (c != '\0')
        {
            fread(&c, sizeof(char), 1, elfFile);
            nom_section[j] = c;
            j++;
        }

        fread(&sectionTable[i].sh_name, sizeof(Elf32_Word), 1, elfFile);

        //Lire le type de la section
        fread(&sectionTable[i].sh_type, sizeof(Elf32_Word), 1, elfFile);
        switch (sectionTable[i].sh_type)
        {
            case 0:
                printf("Type de la section : SHT_NULL\n");
                break;
            case 1:
                printf("Type de la section : SHT_PROGBITS\n");
                break;
            case 2:
                printf("Type de la section : SHT_SYMTAB\n");
                break;
            case 3:
                printf("Type de la section : SHT_STRTAB\n");
                break;
            case 4:
                printf("Type de la section : SHT_RELA\n");
                break;
            case 5:
                printf("Type de la section : SHT_HASH\n");
                break;
            case 6:
                printf("Type de la section : SHT_DYNAMIC\n");
                break;
            case 7:
                printf("Type de la section : SHT_NOTE\n");
                break;
            case 8:
                printf("Type de la section : SHT_NOBITS\n");
                break;
            case 9:
                printf("Type de la section : SHT_REL\n");
                break;
            case 10:
                printf("Type de la section : SHT_SHLIB\n");
                break;
            case 11:
                printf("Type de la section : SHT_DYNSYM\n");
                break;
            case 0x70000000:
                printf("Type de la section : SHT_LOPROC\n");
                break;
            case 0x7fffffff:
                printf("Type de la section : SHT_HIPROC\n");
                break;
            case 0x80000000:
                printf("Type de la section : SHT_LOUSER\n");
                break;
        }


        //Lire l'adresse de la section
        fread(&sectionTable[i].sh_addr, sizeof(Elf32_Addr), 1, elfFile);
        //l'adresse à  laquelle le premier octet de la section doit se trouver.
        printf("Adresse de la section : %x\n", sectionTable[i].sh_addr);

        //Lire la position de la section
        fread(&sectionTable[i].sh_offset, sizeof(Elf32_Word), 1, elfFile);
        printf("Position de la section : %x\n", sectionTable[i].sh_offset);


        //Lire la taille de la section
        fread(&sectionTable[i].sh_size, sizeof(Elf32_Word), 1, elfFile);
        printf("Taille de la section : %x\n", sectionTable[i].sh_size);

        //Lire la taille de l'entrée
        fread(&sectionTable[i].sh_entsize, sizeof(Elf32_Word), 1, elfFile);
        //la taille de l'entrée, pour les sections qui contiennent une table d'entrées de même taille.
        printf("La taille de l'entrée, pour les sections qui contiennent une table d'entrées de même taille : %x\n",
               sectionTable[i].sh_entsize);


        fread(&sectionTable[i].sh_flags, sizeof(Elf32_Word), 1, elfFile);

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


        //Lire l'indice de la table des en-têtes de sections
        fread(&sectionTable[i].sh_link, sizeof(Elf32_Word), 1, elfFile);
        //lien vers un indice de la table des en-têtes de  sections,
        printf("Lien vers un indice de la table des en-têtes de sections : %x\n", sectionTable[i].sh_link);


        //Lire les informations supplémentaires
        fread(&sectionTable[i].sh_info, sizeof(Elf32_Word), 1, elfFile);
        //informations supplémentaires, dépendant du type de section.
        printf("Informations supplémentaires, dépendant du type de section : %x\n", sectionTable[i].sh_info);

        //Lire la taille de l'alignement
        fread(&sectionTable[i].sh_addralign, sizeof(Elf32_Word), 1, elfFile);
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
