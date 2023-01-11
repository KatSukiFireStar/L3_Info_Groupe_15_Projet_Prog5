//
// Created by Pyrolink on 14/12/2022.
//

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

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


//ToDo a voir si l'on deplace ces fonctions dans le utils


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
        command = GetCharAndClear(stdin);
        switch (command)
        {
            case 'q':
                fin = 1;
                break;
            case 'h':
                LoopOnEachArgs(ShowElfHeader(structureElfs[i - 1].header);)
                printf("\n");
                break;
            case 's':

                LoopOnEachArgs(ShowSectionTableAndDetails(fopen(structureElfs[i - 1].path, "r"),
                                                          structureElfs[i - 1].header,
                                                          structureElfs[i - 1].sectionTable);)
                printf("\n");
                break;
            case 'y':

                LoopOnEachArgs(ShowSymbolsTableAndDetails(fopen(structureElfs[i - 1].path, "r"),
                                                          structureElfs[i - 1].header,
                                                          structureElfs[i - 1].sectionTable,
                                                          structureElfs[i - 1].symbolTable);)
                printf("\n");
                break;
            case 'r':

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

        if (!fin)
        {
            fprintf(stdout, "Press enter to continue!");
            GetCharAndClear(stdin);
        }

#pragma endregion

    }


#pragma endregion

#pragma region LiberationStructure

    LoopOnEachArgs(FreeElf32_Structure(structureElfs[i - 1]);)

#pragma endregion

    return 0;
}