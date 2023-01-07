//
// Created by Pyrolink on 14/12/2022.
//

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "elfFile.h"
#include "elfStructure.h"
#include "utils.h"

#pragma region Define

#define LoopOnEachArgs(action) for (int i = 1; i < argc; i++) \
{ \
    action \
}

#pragma endregion

Elf32_RelFusion FusionReimplantation(FILE **elfFiles, Elf32_Structure *structure, Elf32_SectionFusion sectionFusion,
                                     Elf32_SymbolFusion symbolFusion)
{
    Elf32_RelFusion fusion;

    int nbTotal = structure[0].reimplantationCount + structure[1].reimplantationCount;

    fusion.newIndices = mallocArray(Elf32_Word, structure[1].reimplantationCount);

    for (int i = 0; i < structure[1].reimplantationCount; i++)
    {
        fusion.newIndices[i] = i;
    }

    for (int reimI0 = 0; reimI0 < structure[0].reimplantationCount; reimI0++)
    {
        for (int reimI1 = 0; reimI1 < structure[0].reimplantationCount; reimI1++)
        {
            bool haveFusion = true;

            Elf32_Shdr strtab[2];
            strtab[0] = structure[0].sectionTable[structure[0].header.e_shstrndx];
            strtab[1] = structure[1].sectionTable[structure[1].header.e_shstrndx];
            // get the name of each section by adding the offset of name to the offset of the string table
            fseek(elfFiles[0], strtab[0].sh_offset + structure[0].sectionTable[reimI0].sh_name, SEEK_SET);
            fseek(elfFiles[1], strtab[1].sh_offset + structure[0].sectionTable[reimI1].sh_name, SEEK_SET);

            char c1, c2;
            do
            {
                freadEndian(&c1, sizeof(char), 1, elfFiles[0]);
                freadEndian(&c2, sizeof(char), 1, elfFiles[1]);

                if (c1 != c2)
                {
                    haveFusion = false;
                    break;
                }
            } while (c1 != '\0' || c2 != '\0');

            if (haveFusion)
            {
                fusion.newIndices[reimI1] = reimI0;
                nbTotal--;
                break;
            }
        }
    }

    fusion.reimplantationTable = mallocArray(Elf32_RelaTable, nbTotal);

    for (int reimI0 = 0; reimI0 < structure[0].reimplantationCount; reimI0++)
    {
        Elf32_Reim reim0 = structure[0].reimplantationTable[reimI0];
        Elf32_Shdr section0 = structure[0].sectionTable[reim0.section];
        Elf32_Half entityInTable0 = section0.sh_size / section0.sh_entsize;

        for (int i = 0; i < entityInTable0; i++)
        {
            fusion.reimplantationTable[reimI0].reimplantation[i] = reim0.reimplantation[i];
        }
    }

    int currentIndex = structure[0].reimplantationCount;
    for (int reimI1 = 0; reimI1 < structure[1].reimplantationCount; reimI1++)
    {
        if (fusion.newIndices[reimI1] < structure[0].reimplantationCount)
        {
            continue;
        }

        Elf32_Reim reim1 = structure[1].reimplantationTable[reimI1];
        Elf32_Shdr section1 = structure[1].sectionTable[reim1.section];
        Elf32_Half entityInTable1 = section1.sh_size / section1.sh_entsize;
        int newSectionIndex = sectionFusion.newIndices[reim1.section];

        int offset;
        if (newSectionIndex < structure[0].sectionCount)
        {
            offset = sectionFusion.concatenationOffset[newSectionIndex];
        }
        else
        {
            offset = -1;
        }

        for (int i = 0; i < entityInTable1; i++)
        {
            fusion.reimplantationTable[currentIndex].reimplantation[i] = reim1.reimplantation[i];

            if (offset >= 0)
            {
                fusion.reimplantationTable[currentIndex].reimplantation[i].r_offset += offset;
            }

            int lastSymIndex = ELF32_R_SYM(reim1.reimplantation[i].r_info);
            int newSymIndex = symbolFusion.newIndices[lastSymIndex];
            reim1.reimplantation[i].r_info &= ~(0xf << 8);
            reim1.reimplantation[i].r_info |= (newSymIndex << 8);
        }

        currentIndex++;
    }

    return fusion;
}

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

Elf32_SectionFusion FusionSections(FILE **elfFiles, Elf32_Ehdr *elfHeaders, Elf32_ShdrTable *sectionTables)
{
    Elf32_Half numbersection2 = elfHeaders[1].e_shnum;
    //create the section merge structure
    Elf32_SectionFusion fu = NewSectionFusion(elfHeaders[0].e_shnum, elfHeaders[1].e_shnum);
    // open the tmp files with the name elfileW to do the merge in it
    FILE *elfFileW = fopen(fu.tmpFile, "w");
    // table to know which index is merged in the second file
    bool *mergedindex = mallocArray(Elf32_Word, elfHeaders[1].e_shnum);
    for (int i = 0; i < elfHeaders[1].e_shnum; i++)
    {
        mergedindex[i] = false;
    }
    // number of sections in tmp file (to calculate the new index of the second file)
    Elf32_Half numbersectiontmp = 0;
    //compare sections
    for (Elf32_Half i = 0; i < elfHeaders[0].e_shnum; i++)
    {
        bool sectioncreated = false;
        // verify that the type of the first section is PROGBITS
        if (sectionTables[0][i].sh_type == SHT_PROGBITS)
        {
            for (Elf32_Half j = 0; j < elfHeaders[1].e_shnum; j++)
            {
                bool fusion;
                // compare if the two sections have the same type PROGBITS
                if (sectionTables[1][j].sh_type == SHT_PROGBITS)
                {
                    fusion = true;
                    // create our table of string
                    Elf32_Shdr strtab[2];
                    strtab[0] = sectionTables[0][elfHeaders[0].e_shstrndx];
                    strtab[1] = sectionTables[1][elfHeaders[1].e_shstrndx];
                    // get the name of each section by adding the offset of name to the offset of the string table
                    fseek(elfFiles[0], strtab[0].sh_offset + sectionTables[0][i].sh_name, SEEK_SET);
                    fseek(elfFiles[1], strtab[1].sh_offset + sectionTables[1][j].sh_name, SEEK_SET);
                    // if they have the same type we check if they have the same name
                    char c1, c2;
                    do
                    {
                        freadEndian(&c1, sizeof(char), 1, elfFiles[0]);
                        freadEndian(&c2, sizeof(char), 1, elfFiles[1]);
                        if (c1 != c2)
                        {
                            fusion = false;
                            break;
                        }
                    } while (c1 != '\0' || c2 != '\0');
                }
                else
                {
                    // no merge fusion stay 0
                    fusion = false;
                }
                // fseek on the sections
                fseek(elfFiles[0], sectionTables[0][i].sh_offset, SEEK_SET);
                fseek(elfFiles[1], sectionTables[1][j].sh_offset, SEEK_SET);
                // merge
                if (fusion)
                {
                    //section created
                    sectioncreated = true;
                    // the index of the section merged in the second file turn to 1
                    mergedindex[j] = true;
                    //we write the section from the first file to the tmp file
                    numbersection2--;
                    char c;
                    int nb = 0;
                    do
                    {
                        freadEndian(&c, sizeof(char), 1, elfFiles[0]);
                        fwrite(&c, sizeof(char), 1, elfFileW);
                        nb++;
                    } while (nb < sectionTables[0][i].sh_size);

                    // we write the section from the second file to the tmp file
                    nb = 0;
                    do
                    {
                        freadEndian(&c, sizeof(char), 1, elfFiles[1]);
                        fwrite(&c, sizeof(char), 1, elfFileW);
                        nb++;
                    } while (nb < sectionTables[1][j].sh_size);
                    // we add it to the structure
                    fu.concatenationOffset[i] = sectionTables[0][i].sh_size;
                    // the number of section in the tmp file increased by 1
                    numbersectiontmp++;
                    break;
                }
            }
        }
        // if the section is not merged we create it here
        if (sectioncreated == false)
        {
            // we write the section from the first file to tmp file
            char c;
            int nb = 0;
            do
            {
                freadEndian(&c, sizeof(char), 1, elfFiles[0]);
                fwrite(&c, sizeof(char), 1, elfFileW);
                nb++;
            } while (nb < sectionTables[0][i].sh_size);
            // the number of the sections in tmp increased
            numbersectiontmp++;
        }

    }
    // add the Sections not merged of the second file
    for (Elf32_Half i = 0; i < elfHeaders[1].e_shnum; i++)
    {
        if (mergedindex[i] == false)
        {
            // the new index of the sections
            fu.newIndices[i] = numbersectiontmp;
            // number sections in tmp file increased
            numbersectiontmp++;
            fseek(elfFiles[1], sectionTables[1][i].sh_offset, SEEK_SET);
            char c;
            int nb = 0;
            do
            {
                freadEndian(&c, sizeof(char), 1, elfFiles[1]);
                fwrite(&c, sizeof(char), 1, elfFileW);
                nb++;
            } while (nb < sectionTables[1][i].sh_size);
        }
    }
    fu.numberSection = elfHeaders[0].e_shnum + numbersection2;
    fclose(elfFileW);
    return fu;
}


void DoFusionCommand(FILE **elfFiles, Elf32_Structure *structure)
{
    Elf32_Ehdr headers[2];
    headers[0] = structure[0].header;
    headers[1] = structure[1].header;
    Elf32_ShdrTable sectionTables[2];
    sectionTables[0] = structure[0].sectionTable;
    sectionTables[1] = structure[1].sectionTable;

    Elf32_SectionFusion fusion = FusionSections(elfFiles, headers, sectionTables);

    printf("%s\n", fusion.tmpFile);
    FreeSectionFusion(fusion);
}

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