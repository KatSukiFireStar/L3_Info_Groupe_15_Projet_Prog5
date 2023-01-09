//
// Created by Pyrolink on 14/12/2022.
//

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "elfFile.h"
#include "elfStructure.h"
#include "utils.h"

#pragma region Define

#define LoopOnEachArgs(action) for (int i = 1; i < argc; i++) \
{ \
    action \
}

#pragma endregion

void help()
{
    fprintf(stdout, "Aide: \n");
    fprintf(stdout, "Saisir l'une des commandes suivantes\n\n");
    fprintf(stdout, "h\t: Afficher le header des fichier en paramètres\n");
    fprintf(stdout, "s\t: Afficher la table des sections des fichier en paramètres\n");
    fprintf(stdout, "y\t: Afficher la table des symboles des fichier en paramètres\n");
    fprintf(stdout, "r\t: Afficher la table de reimplantation des fichier en paramètres\n");
    fprintf(stdout, "f\t: Fusionne les header(temporaire)\n");
    fprintf(stdout, "q\t: Quitter ce programme\n");
}


char * GetSymbolName(Elf32_Ehdr header,Elf32_ShdrTable sectionTable, Elf32_SymTable symbolTable, Elf32_Half symbolIndex, FILE *elfFile){
        char *nom;
        unsigned char type = ELF32_ST_TYPE(symbolTable[symbolIndex].st_info);
        Elf32_Shdr strtab = sectionTable[GetSectionIndexByName(elfFile, sectionTable, header, ".strtab")];
        Elf32_Shdr strndx = sectionTable[header.e_shstrndx];
         if (type == STT_SECTION){
            fseek(elfFile, strndx.sh_offset + sectionTable[symbolTable[symbolIndex].st_shndx].sh_name, SEEK_SET);
         }
        else{
            fseek(elfFile, strtab.sh_offset + symbolTable[symbolIndex].st_name, SEEK_SET);
        }
        char c = ' ';
        while (c != '\0')
        {
            freadEndian(&c, sizeof(char), 1, elfFile);
            nom+=c;
        }
        return nom;
}


//fonction qui vérifie si un symbole local donné en parametre est dans la table de symbole fusion
int isLocalSymbolInFusionTable(Elf32_SymbolFusion fusionTable, Elf32_Sym symbol){
    for(int i=0; i<fusionTable.nbSymbol; i++){
        if(fusionTable.symbolTable[i].st_info==STB_LOCAL && fusionTable.symbolTable[i].st_name==symbol.st_name){
            return 1;
        }
    }
    return 0;
}

Elf32_SymbolFusion FusionSymbols(FILE **elfFile,Elf32_Structure *structure, Elf32_SectionFusion sectionFusion){
    Elf32_Half nbSym1 = GetEntryCountFromType(structure[0].header,structure[0].sectionTable, SHT_SYMTAB);
    Elf32_Half nbSym2 = GetEntryCountFromType(structure[1].header,structure[1].sectionTable, SHT_SYMTAB);

    Elf32_Half nbTotSym = nbSym1 + nbSym2;
    Elf32_SymbolFusion fusionTable;

    
    char *name1;
    char *name2;
    
    char *strtab=mallocArray(char, nbTotSym );
    memset(strtab, 0, nbTotSym);


   fusionTable.newIndices = mallocArray(Elf32_Half ,nbSym2);
   for (int i = 0; i < nbSym2; i++){
        fusionTable.newIndices[i] = -1;//au debut aucun indice ne change
    }

    fusionTable.symbolTable = mallocArray(Elf32_Sym ,nbTotSym);
    int i,j,k;
    for(k=0;k<nbTotSym;k++){
        for(i=0;i<nbSym1;j++){
            if(ELF32_ST_BIND(structure[0].symbolTable[i].st_info) == STB_GLOBAL){
                for(j=0;j<nbSym2;j++){
                  
                    if (ELF32_ST_BIND(structure[1].symbolTable[j].st_info) == STB_GLOBAL){
                        if(strcmp( GetSymbolName(structure[0].header, structure[0].sectionTable, structure[0].symbolTable,i,elfFile[0]), GetSymbolName(structure[1].header, structure[1].sectionTable, structure[1].symbolTable,i,elfFile[1]) ) == 0){
                            if (structure[0].symbolTable[i].st_shndx != SHN_UNDEF && structure[1].symbolTable[i].st_shndx != SHN_UNDEF){
                                printf("Erreur de l'édition de lien: il est interdit que pour 2 fichiers différents que deux symboles globlaux aient le meme nom ");
                                exit(1);
                                
                            }
                            else{
                                if (structure[0].symbolTable[i].st_shndx == SHN_UNDEF && structure[1].symbolTable[j].st_shndx != SHN_UNDEF){
                                   
                                    fusionTable.newIndices[j] = i;
                                    nbTotSym--;
                                    fusionTable.symbolTable[k].st_size = structure[1].symbolTable[j].st_size;
                                   fusionTable.symbolTable[k].st_info = structure[1].symbolTable[j].st_info;
                                    fusionTable.symbolTable[k].st_other = structure[1].symbolTable[j].st_other;
                                    fusionTable.symbolTable[k].st_shndx = structure[1].symbolTable[j].st_shndx;
                                    fusionTable.symbolTable[k].st_name= structure[1].symbolTable[j].st_name;
                                     fusionTable.symbolTable[k].st_value= (structure[1].symbolTable[j].st_value)+sectionFusion.concatenationOffset[i]    ;
                               
                                }
                                else if (structure[0].symbolTable[i].st_shndx != SHN_UNDEF &&  structure[1].symbolTable[j].st_shndx == SHN_UNDEF){
                                    fusionTable.newIndices[j] = i;
                                    nbTotSym--;
                                    fusionTable.symbolTable[k].st_size = structure[0].symbolTable[i].st_size;
                                   fusionTable.symbolTable[k].st_info =structure[0].symbolTable[i].st_info;
                                    fusionTable.symbolTable[k].st_other = structure[0].symbolTable[i].st_other;
                                    fusionTable.symbolTable[k].st_shndx =structure[0].symbolTable[i].st_shndx;
                                    fusionTable.symbolTable[k].st_name= structure[0].symbolTable[i].st_name;
                                   fusionTable.symbolTable[k].st_value= (structure[1].symbolTable[j].st_value)+sectionFusion.concatenationOffset[i] ;
                                }
                                //Si les 2 sont non définis
                                if (structure[0].symbolTable[i].st_shndx == SHN_UNDEF && structure[1].symbolTable[j].st_shndx == SHN_UNDEF){
                                   nbTotSym--;
                                   fusionTable.symbolTable[k].st_size = structure[0].symbolTable[i].st_size;
                                   fusionTable.symbolTable[k].st_info = structure[0].symbolTable[i].st_info;
                                    fusionTable.symbolTable[k].st_other = structure[0].symbolTable[i].st_other;
                                    fusionTable.symbolTable[k].st_shndx = structure[0].symbolTable[i].st_shndx;
                                    fusionTable.symbolTable[k].st_name= structure[0].symbolTable[i].st_name;
                                    fusionTable.symbolTable[k].st_value= (structure[1].symbolTable[j].st_value)+sectionFusion.concatenationOffset[i] ;
                                }
                            }
                        
                        }
                        //Aucun symbole global de la table de symbole 1 se trouve dans la table 2.On ajoutera dans la table fusion ce symbole qui n'est pas dans la table de symboole 2 mais qui est dans la table de symbole 1
                        else{
                            nbTotSym--;
                            fusionTable.symbolTable[k].st_size = structure[0].symbolTable[i].st_size;
                            fusionTable.symbolTable[k].st_info = structure[0].symbolTable[i].st_info;
                            fusionTable.symbolTable[k].st_other = structure[0].symbolTable[i].st_other;
                            fusionTable.symbolTable[k].st_shndx = structure[0].symbolTable[i].st_shndx;
                            fusionTable.symbolTable[k].st_name= structure[0].symbolTable[i].st_name;
                            fusionTable.symbolTable[k].st_value= (structure[1].symbolTable[j].st_value)+sectionFusion.concatenationOffset[i] ;
                            

                        }
                    }
                    name1=GetSymbolName(structure[0].header, structure[0].sectionTable,structure[0].symbolTable, i,elfFile[0]);
                    name2=GetSymbolName(structure[1].header, structure[1].sectionTable,structure[1].symbolTable, i,elfFile[1]);
                    if( (strstr(strtab,name1)!=NULL) ){
                        strcat(strtab,name1);
                        strcat(strtab," ");
                        
                    }
                    if  (strstr(strtab,name2)!=NULL){
                        strcat(strtab,name2);
                        strcat(strtab," ");
                    }


                }
            }
            else if(structure[0].symbolTable[i].st_info== STB_LOCAL){
                if (!(isLocalSymbolInFusionTable(fusionTable,structure[0].symbolTable[i]))){
                    fusionTable.symbolTable[k].st_size = structure[0].symbolTable[i].st_size;
                    fusionTable.symbolTable[k].st_info = structure[0].symbolTable[i].st_info;
                    fusionTable.symbolTable[k].st_other = structure[0].symbolTable[i].st_other;
                    fusionTable.symbolTable[k].st_shndx = structure[0].symbolTable[i].st_shndx;
                    fusionTable.symbolTable[k].st_name= structure[0].symbolTable[i].st_name;
                    fusionTable.symbolTable[k].st_value= (structure[1].symbolTable[j].st_value)+sectionFusion.concatenationOffset[i] ;
            
                }
               
            }
            
        }
    }
    fusionTable.strtab=strtab;
    fusionTable.nbSymbol=nbTotSym;
    return fusionTable;
}






void AffichageTableSymbolFusion(Elf32_SymbolFusion fusionTableSym){
    /*
     for( Elf32_Half symbolIndex = 0; symbolIndex < fusionTableSym.nbSymbol; symbolIndex++){
            freadEndian(&fusionTableSym.symbolTable[symbolIndex].st_name, sizeof(Elf32_Word), 1, elfFile);
            freadEndian(&fusionTableSym.symbolTable[symbolIndex].st_value, sizeof(Elf32_Addr), 1, elfFile);
            freadEndian(&fusionTableSym.symbolTable[symbolIndex].st_size, sizeof(Elf32_Word), 1, elfFile);
            freadEndian(&fusionTableSym.symbolTable[symbolIndex].st_info, sizeof(unsigned char), 1, elfFile);
            freadEndian(&fusionTableSym.symbolTable[symbolIndex].st_other, sizeof(unsigned char), 1, elfFile);
            freadEndian
            (&fusionTableSym.symbolTable[symbolIndex].st_shndx, sizeof(Elf32_Section), 1, elfFile);
    }
    */
    //Affichage de chaque infos de la table de symbole fusionnée
    for(Elf32_Half symbolIndex = 0; symbolIndex < fusionTableSym.nbSymbol; symbolIndex++) {
        printf("Symbol %d\n", symbolIndex);
        printf("st_name: %d\n", fusionTableSym.symbolTable[symbolIndex].st_name);
        printf("st_value: %d\n", fusionTableSym.symbolTable[symbolIndex].st_value);
        printf("st_size: %d\n", fusionTableSym.symbolTable[symbolIndex].st_size);
        printf("st_info: %d\n", fusionTableSym.symbolTable[symbolIndex].st_info);
        printf("st_other: %d\n", fusionTableSym.symbolTable[symbolIndex].st_other);
        printf("st_shndx: %d\n", fusionTableSym.symbolTable[symbolIndex].st_shndx);
    }



}

/*
int main(int argc, char *argv[]){
     #pragma region DeclarationVariables
     FILE **elfFile;
      Elf32_Structure structureElfs[2];

    #pragma endregion

    #pragma region ConditionsArguments
    if (argc <= 1){
        fprintf(stderr, "Vous n'avez pas mis de fichier en paramètres!\n");
       
    }
    for (int i = 1; i < argc; i++){
        if (access(argv[i], R_OK)){
            fprintf(stderr, "Le fichier %s ne peut pas etre ouvert!\n", argv[i]);
        }
    }
    #pragma endregion

    #pragma region ExtractInformations
     for (int i = 1; i < argc; i++){
        elfFile[i] = fopen(argv[i], "r");
        structureElfs[i - 1] = ExtractElfInformation(elfFile[i], argv[i]);
        fclose(elfFile[i]);
    }
    #pragma endregion

    for (int i = 1; i < argc; i++) { 
        elfFile[0] = fopen(argv[1], "r");
        elfFile[1] = fopen(argv[2], "r");
        
         Elf32_SectionFusion fusion = FusionSections(elfFile, structureElfs);
        Elf32_SymbolFusion fusionTableSym= FusionSymbols(elfFile,structureElfs,fusion);
        AffichageTableSymbolFusion(fusionTableSym);
   }
      fclose(elfFile[0]);
        fclose(elfFile[1]);
}
*/


int main(int argc, char *argv[])
{

#pragma region DeclarationVariables

    int fin = 0;
    int command;

    FILE *elfFile;
    FILE *elfFiles[2];
    Elf32_Structure structureElfs[2]; // Le tableau a une taille de 2, car on ne fusionne que 2 fichiers

#pragma endregion

#pragma region ConditionsArguments

    if (argc <= 1)
    {
        fprintf(stderr, "Vous n'avez pas mis de fichier en paramètres!\n");
        return -3;
    }
//    else if (argc <= 2)
//    {
//        fprintf(stderr, "Vous n'avez mis qu'un seul fichier en paramètres!\n");
//        return -4;
//    }

    for (int i = 1; i < argc; i++)
    {
        if (access(argv[i], R_OK))
        {
            fprintf(stderr, "Le fichier %s ne peut pas etre ouvert!\n", argv[i]);
            return -5;
        }
    }

#pragma endregion

#pragma region ExtractInformations

    // Permet de recuperer toutes les informations d'un fichier
    // et les stocks dans des variables
    // A la fin, le fichier est fermé et on ouvre le fichier suivant
    for (int i = 1; i < argc; i++)
    {
        elfFile = fopen(argv[i], "r");
        structureElfs[i - 1] = ExtractElfInformation(elfFile, argv[i]);
        fclose(elfFile);
    }

#pragma endregion

#pragma region Commande&GestionBuffer

    while (!fin)
    {

#pragma region Commande

        help();
        command = getchar();
        switch (command)
        {
            case 'q':
                fin = 1;
                break;
            case 'h':
                printf("ELF Header: \n");
                LoopOnEachArgs(ShowElfHeader(structureElfs[i - 1].header);)

                printf("\n");
                break;
            case 's':
                printf("Section table: \n");
                LoopOnEachArgs(ShowSectionTableAndDetails(fopen(structureElfs[i - 1].path, "r"),
                                                        structureElfs[i - 1].header,
                                                          structureElfs[i - 1].sectionTable);)
               
                
                printf("\n");
                break;
            case 'y':
                printf("Symbol table: \n");
                LoopOnEachArgs(ShowSymbolsTableAndDetails(fopen(structureElfs[i - 1].path, "r"),
                                                          structureElfs[i - 1].header,
                                                          structureElfs[i - 1].sectionTable,
                                                          structureElfs[i - 1].symbolTable);)
                printf("\n");
                break;
            case 'r':
                printf("Reimplantation table: \n");
                LoopOnEachArgs(ShowReimplantationTablesAndDetails(fopen(structureElfs[i - 1].path, "r"),
                                                                  structureElfs[i - 1]);)
                printf("\n");
                break;
            case 'f':
                elfFiles[0] = fopen(argv[1], "r");
                elfFiles[1] = fopen(argv[2], "r");

                DoFusionCommand(elfFiles, structureElfs);

                fclose(elfFiles[0]);
                fclose(elfFiles[1]);
                break;
            default:
                fprintf(stdout, "La commande n'est pas reconnu!\n");
        }

#pragma endregion

#pragma region GestionBuffer

        char buffer[128];
        if (command != '\n')
        {
            fgets(buffer, 128, stdin);
        }

        if (!fin)
        {
            fprintf(stdout, "Press enter to continue!");
            int c = getchar();
            while (c != '\n')
            {
                fgets(buffer, 128, stdin);
                c = getchar();
            }

        }

#pragma endregion

    }


#pragma endregion

#pragma region LiberationStructure

    LoopOnEachArgs(FreeElf32_Structure(structureElfs[i - 1]););

#pragma endregion

    return 0;
}
