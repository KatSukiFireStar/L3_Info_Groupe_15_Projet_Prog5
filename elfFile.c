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

Elf32_Shdr *ShowSectionTableAndDetails(FILE *elfFile, Elf32_Ehdr header){
    Elf32_Shdr *sectionTable = malloc(sizeof(Elf32_Shdr) * header.e_shnum);
    fseek(elfFile, header.e_shoff, SEEK_SET);
    
    //Pour chaque entrée de la table, il est demand´e d’afficher les principales caractéristiques
    int i;
    for (i = 0; i < header.e_shnum; i++) {
        //lire le nom de la section
        fread(&sectionTable[i].sh_name, sizeof(Elf32_Word), 1, elfFile);
        printf("Section: %ls\n", &sectionTable[i].sh_name);
        //Lire la taille de la section
        fread(&sectionTable[i].sh_size, sizeof(Elf32_Word), 1, elfFile);
        printf("Taille de la section : %d\n", sectionTable[i].sh_size);
        //Lire le type de la section
        fread(&sectionTable[i].sh_type, sizeof(Elf32_Word), 1, elfFile);
        printf("Type de la section : %d\n", sectionTable[i].sh_type);
        //Lire la position de la section
        fread(&sectionTable[i].sh_offset, sizeof(Elf32_Word), 1, elfFile);
        printf("Position de la section : %d\n", sectionTable[i].sh_offset);

        fread(&sectionTable[i].sh_flags, sizeof( Elf32_Word), 1, elfFile);
        if ((sectionTable[i].sh_flags & SHF_WRITE) == SHF_WRITE)
            printf("Cette section contient des données qu'il devrait être possible d'écrire durant l'exécution du processus.\n");
                   
        if ((sectionTable[i].sh_flags & SHF_ALLOC)== SHF_ALLOC) 
            printf("La section fait partie de l'image mémoire du programme à exécuter.\n");
        
                
        if ((sectionTable[i].sh_flags & SHF_EXECINSTR)== SHF_EXECINSTR)
            printf("La section contient du code exécutable.\n");
       
        if ((sectionTable[i].sh_flags & SHF_MASKPROC) == SHF_MASKPROC) 
            printf("Tous les bits contenus dans ce masque sont réservés à des sémantiques spécifiques au processeur.\n");
       
        //Lire l'adresse de la section
        fread(&sectionTable[i].sh_addr, sizeof(Elf32_Addr), 1, elfFile);
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

/*L’objectif de cette étape est de construire un programme capable de lire et d’afficher les tables de
réimplantation(repositionnement) d’un fichier ELF pour machine ARM. Pour chaque entrée, il est demandé d’afficher les
informations suivantes :
— la cible de la réimplantation 3
;
— le type de réimplantation à effectuer (noter que chaque architecture mat´erielle dispose d’une
spécification distincte pour les types de réimplantation) ;
— l’index de l’entrée concernée dans la table des symboles (le symbole impliqu´e dans la réimplantation,
par exemple, dans le cas d’un appel de fonction, le symbole impliqu´e est le nom de la fonction appel´ee).
3. Le champ correspondant est nomme offset mais son contenu dépend du type de fichier ELF qui le contient. Pour un fichier
translatable, il s’agit effectivement d’un offset à partir du début de la section concernée. Pour un fichier ELF ex´ecutable ou une
biblioth`eque partag´ee, le champ correspond en fait à l’adresse mémoire de l’element concerné.
*/
/*La fonction ShowReimplantationTablesAndDetails permet d'afficher des tables de réimplantation*/
Elf32_Rel *ShowReimplantationTablesAndDetails(FILE *elfFile, Elf32_Ehdr header){

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