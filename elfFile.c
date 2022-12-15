//
// Created by Pyrolink on 14/12/2022.
//

#include "elfFile.h"
#include <stdio.h>
#include <elf.h>
#include <stdlib.h>

// Todo

/*L’objectif de cette ´etape est de construire un programme capable de lire et d’afficher la table des sections
d’un fichier ELF. Pour chaque entrée de la table, il est demand´e d’afficher les principales caract´eristiques,
telles que :
— le numéro de section (index dans la table) ;
— le nom de la section ;
— la taille de la section ;
— le type de la section (PROGBITS, SYMTAB, STRTAB, etc.) ;
— les principaux attributs de la section, en particulier :
— les informations d’allocation (la section fait-elle partie de l’image m´emoire du programme `a
ex´ecuter ?) ;
— les permissions (la section contient-elle du code ex´ecutable ? des donn´ees modifiables lors de
l’exécution du programme ?) ;
— la position (offset) de la section par rapport au d´ebut du fichier ;
*/


/*Affichage de la table des sections et des détails relatifs à chaque section ;*/
/*Questcequ'une table des sections ?
Une table des sections est une table qui contient des informations sur les sections d'un fichier ELF.
Chaque entrée de la table contient des informations sur une section particulière. 



*/


Elf32_Shdr *ShowSectionTableAndDetails(FILE *elfFile, Elf32_Ehdr header){
    Elf32_Shdr *sectionTable = malloc(sizeof(Elf32_Shdr) * header.e_shnum);
    fseek(elfFile, header.e_shoff, SEEK_SET);
    
    fread(&sectionTable, header.e_shentsize, header.e_shnum, elfFile);
    //Pour chaque entrée de la table, il est demand´e d’afficher les principales caractéristiques
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
        //Lire l'adresse de la section
        fread(&sectionTable[i].sh_addr, sizeof(Elf32_Word), 1, elfFile);
        //l'adresse à  laquelle le premier octet de la section doit se trouver.
        printf("Adresse de la section : %d\n", sectionTable[i].sh_addr);

        //Lire le décalage de la section
        fread(&sectionTable[i].sh_offset, sizeof(Elf32_Word), 1, elfFile);
        //le déplacement du premier octet de la section par rapport  au  début  du  fichier
        printf("Décalage de la section : %d\n", sectionTable[i].sh_offset);

        //Lire l'indice de la table des en-têtes de sections
        fread(&sectionTable[i].sh_link, sizeof(Elf32_Word), 1, elfFile);
        //lien vers un indice de la table des en-têtes de  sections,
        printf("Lien vers un indice de la table des en-têtes de sections : %d\n", sectionTable[i].sh_link);


        //Lire les informations supplémentaires
        fread(&sectionTable[i].sh_info, sizeof(Elf32_Word), 1, elfFile);
        //informations supplémentaires, dépendant du type de section.
        printf("Informations supplémentaires, dépendant du type de section : %d\n", sectionTable[i].sh_info);

        //Lire la taille de l'alignement
        fread(&sectionTable[i].sh_addralign, sizeof(Elf32_Word), 1, elfFile);
        //la taille de l'alignement, exprimée en puissance de 2.
        printf("La taille de l'alignement, exprimée en puissance de 2 : %d\n", sectionTable[i].sh_addralign);

        //Lire la taille de l'entrée
        fread(&sectionTable[i].sh_entsize, sizeof(Elf32_Word), 1, elfFile);
        //la taille de l'entrée, pour les sections qui contiennent une table d'entrées de même taille.
        printf("La taille de l'entrée, pour les sections qui contiennent une table d'entrées de même taille : %d\n", sectionTable[i].sh_entsize);
        
    }
    return sectionTable;
}





int main(int argc, char *argv[])
{
    return 0;
}