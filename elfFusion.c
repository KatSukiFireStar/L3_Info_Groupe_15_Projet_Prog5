//
// Created by Pyrolink on 07/01/2023.
//

#include <stdbool.h>
#include <string.h>

#include "elfFile.h"
#include "utils.h"

void DoFusionCommand(FILE **elfFiles, Elf32_Structure *structure)
{
    Elf32_SectionFusion fusion = FusionSections(elfFiles, structure);

    printf("%s\n", fusion.tmpFile);
    FreeSectionFusion(fusion);
}

Elf32_SectionFusion FusionSections(FILE **elfFiles, Elf32_Structure *structure)
{
    Elf32_Half numbersection2 = structure[1].header.e_shnum;
    //create the section merge structure
    Elf32_SectionFusion fu = NewSectionFusion(structure[0].header.e_shnum, structure[1].header.e_shnum);
    // open the tmp files with the name elfileW to do the merge in it
    FILE *elfFileW = fopen(fu.tmpFile, "w");
    // table to know which index is merged in the second file
    bool *mergedindex = mallocArray(Elf32_Word, structure[1].header.e_shnum);
    for (int i = 0; i < structure[1].header.e_shnum; i++)
    {
        mergedindex[i] = false;
    }
    // number of sections in tmp file (to calculate the new index of the second file)
    Elf32_Half numbersectiontmp = 0;
    //compare sections
    for (Elf32_Half i = 0; i < structure[0].header.e_shnum; i++)
    {
        bool sectioncreated = false;
        // verify that the type of the first section is PROGBITS
        if (structure[0].sectionTable[i].sh_type == SHT_PROGBITS)
        {
            for (Elf32_Half j = 0; j < structure[1].header.e_shnum; j++)
            {
                bool fusion;
                // compare if the two sections have the same type PROGBITS
                if (structure[1].sectionTable[j].sh_type == SHT_PROGBITS)
                {
                    fusion = true;
                    // create our table of string
                    Elf32_Shdr strtab[2];
                    strtab[0] = structure[0].sectionTable[structure[0].header.e_shstrndx];
                    strtab[1] = structure[1].sectionTable[structure[1].header.e_shstrndx];
                    // get the name of each section by adding the offset of name to the offset of the string table
                    fseek(elfFiles[0], strtab[0].sh_offset + structure[0].sectionTable[i].sh_name, SEEK_SET);
                    fseek(elfFiles[1], strtab[1].sh_offset + structure[1].sectionTable[j].sh_name, SEEK_SET);
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
                fseek(elfFiles[0], structure[0].sectionTable[i].sh_offset, SEEK_SET);
                fseek(elfFiles[1], structure[1].sectionTable[j].sh_offset, SEEK_SET);
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
                    } while (nb < structure[0].sectionTable[i].sh_size);

                    // we write the section from the second file to the tmp file
                    nb = 0;
                    do
                    {
                        freadEndian(&c, sizeof(char), 1, elfFiles[1]);
                        fwrite(&c, sizeof(char), 1, elfFileW);
                        nb++;
                    } while (nb < structure[1].sectionTable[j].sh_size);
                    // we add it to the structure
                    fu.concatenationOffset[i] = structure[0].sectionTable[i].sh_size;
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
            } while (nb < structure[0].sectionTable[i].sh_size);
            // the number of the sections in tmp increased
            numbersectiontmp++;
        }

    }
    // add the Sections not merged of the second file
    for (Elf32_Half i = 0; i < structure[1].header.e_shnum; i++)
    {
        if (mergedindex[i] == false)
        {
            // the new index of the sections
            fu.newIndices[i] = numbersectiontmp;
            // number sections in tmp file increased
            numbersectiontmp++;
            fseek(elfFiles[1], structure[1].sectionTable[i].sh_offset, SEEK_SET);
            char c;
            int nb = 0;
            do
            {
                freadEndian(&c, sizeof(char), 1, elfFiles[1]);
                fwrite(&c, sizeof(char), 1, elfFileW);
                nb++;
            } while (nb < structure[1].sectionTable[i].sh_size);
        }
    }
    fu.numberSection = structure[0].header.e_shnum + numbersection2;
    fclose(elfFileW);
    free(mergedindex);
    return fu;
}

Elf32_SymbolFusion FusionSymbols(FILE **elfFile, Elf32_Structure *structure, Elf32_SectionFusion sectionFusion)
{
    Elf32_Half nbSym1 = GetEntryCountFromType(structure[0].header, structure[0].sectionTable, SHT_SYMTAB);
    Elf32_Half nbSym2 = GetEntryCountFromType(structure[1].header, structure[1].sectionTable, SHT_SYMTAB);

    Elf32_Half nbTotSym = nbSym1 + nbSym2;
    Elf32_SymbolFusion fusionTable;

    fusionTable.newIndices = mallocArray(Elf32_Half, nbSym2);
    for (int i = 0; i < nbSym2; i++)
    {
        fusionTable.newIndices[i] = -1;//au debut aucun indice ne change
    }

    fusionTable.symbolTable = mallocArray(Elf32_Sym, nbTotSym);

    Elf32_Shdr strndx[2];
    strndx[0] = structure[0].sectionTable[structure[0].header.e_shstrndx];
    strndx[1] = structure[1].sectionTable[structure[1].header.e_shstrndx];

    Elf32_Shdr strtab[2];
    strtab[0] = structure[0].sectionTable[GetSectionIndexByName(elfFile[0], structure[0].sectionTable,
                                                                structure[0].header, ".strtab")];
    strtab[1] = structure[1].sectionTable[GetSectionIndexByName(elfFile[1], structure[1].sectionTable,
                                                                structure[1].header, ".strtab")];

    int k = 0;
    for (int i = 0; i < nbSym1; i++)
    {
        if (ELF32_ST_BIND(structure[0].symbolTable[i].st_info) == STB_GLOBAL)
        {
            for (int j = 0; j < nbSym2; j++)
            {
                if (structure[0].symbolTable[i].st_info == STB_LOCAL &&
                    !(isLocalSymbolInFusionTable(fusionTable, structure[0].symbolTable[i])))
                {
                    fusionTable.symbolTable[k].st_size = structure[0].symbolTable[i].st_size;
                    fusionTable.symbolTable[k].st_info = structure[0].symbolTable[i].st_info;
                    fusionTable.symbolTable[k].st_other = structure[0].symbolTable[i].st_other;
                    fusionTable.symbolTable[k].st_shndx = structure[0].symbolTable[i].st_shndx;
                    fusionTable.symbolTable[k].st_name = structure[0].symbolTable[i].st_name;
                    fusionTable.symbolTable[k].st_value =
                            (structure[1].symbolTable[j].st_value) + sectionFusion.concatenationOffset[i];

                    k++;
                    continue;
                }

                if (ELF32_ST_BIND(structure[1].symbolTable[j].st_info) != STB_GLOBAL)
                {
                    continue;
                }

                bool fusion = true;

                unsigned char type0 = ELF32_ST_TYPE(structure[0].symbolTable[j].st_info);
                unsigned char type1 = ELF32_ST_TYPE(structure[1].symbolTable[j].st_info);

                if (type0 == STT_SECTION)
                {
                    fseek(elfFile[0], strndx[0].sh_offset +
                                      structure[0].sectionTable[structure[0].symbolTable[j].st_shndx].sh_name,
                          SEEK_SET);
                }
                else
                {
                    fseek(elfFile[0], strtab[0].sh_offset + structure[0].symbolTable[j].st_name, SEEK_SET);
                }

                if (type1 == STT_SECTION)
                {
                    fseek(elfFile[1], strndx[1].sh_offset +
                                      structure[1].sectionTable[structure[1].symbolTable[j].st_shndx].sh_name,
                          SEEK_SET);
                }
                else
                {
                    fseek(elfFile[1], strtab[1].sh_offset + structure[1].symbolTable[j].st_name, SEEK_SET);
                }

                char c1, c2;
                do
                {
                    freadEndian(&c1, sizeof(char), 1, elfFile[0]);
                    freadEndian(&c2, sizeof(char), 1, elfFile[1]);
                    if (c1 != c2)
                    {
                        fusion = false;
                        break;
                    }
                } while (c1 != '\0' || c2 != '\0');

                if (!fusion)
                {
                    if (structure[0].symbolTable[i].st_shndx != SHN_UNDEF &&
                        structure[1].symbolTable[i].st_shndx != SHN_UNDEF)
                    {
                        printf("Erreur de l'édition de lien: il est interdit pour "
                               "2 fichiers différents que deux symboles globlaux aient le meme nom ");
                        exit(-5);
                    }

                    if (structure[0].symbolTable[i].st_shndx == SHN_UNDEF &&
                        structure[1].symbolTable[j].st_shndx != SHN_UNDEF)
                    {
                        fusionTable.newIndices[j] = i;
                        nbTotSym--;
                        fusionTable.symbolTable[k].st_size = structure[1].symbolTable[j].st_size;
                        fusionTable.symbolTable[k].st_info = structure[1].symbolTable[j].st_info;
                        fusionTable.symbolTable[k].st_other = structure[1].symbolTable[j].st_other;
                        fusionTable.symbolTable[k].st_shndx = structure[1].symbolTable[j].st_shndx;
                        fusionTable.symbolTable[k].st_name = structure[1].symbolTable[j].st_name;
                        fusionTable.symbolTable[k].st_value = (structure[1].symbolTable[j].st_value) +
                                                              sectionFusion.concatenationOffset[i];

                    }
                    else if (structure[0].symbolTable[i].st_shndx != SHN_UNDEF &&
                             structure[1].symbolTable[j].st_shndx == SHN_UNDEF)
                    {
                        fusionTable.newIndices[j] = i;
                        nbTotSym--;
                        fusionTable.symbolTable[k].st_size = structure[0].symbolTable[i].st_size;
                        fusionTable.symbolTable[k].st_info = structure[0].symbolTable[i].st_info;
                        fusionTable.symbolTable[k].st_other = structure[0].symbolTable[i].st_other;
                        fusionTable.symbolTable[k].st_shndx = structure[0].symbolTable[i].st_shndx;
                        fusionTable.symbolTable[k].st_name = structure[0].symbolTable[i].st_name;
                        fusionTable.symbolTable[k].st_value = (structure[1].symbolTable[j].st_value) +
                                                              sectionFusion.concatenationOffset[i];
                    }
                    //Si les 2 sont non définis
                    if (structure[0].symbolTable[i].st_shndx == SHN_UNDEF &&
                        structure[1].symbolTable[j].st_shndx == SHN_UNDEF)
                    {
                        nbTotSym--;
                        fusionTable.symbolTable[k].st_size = structure[0].symbolTable[i].st_size;
                        fusionTable.symbolTable[k].st_info = structure[0].symbolTable[i].st_info;
                        fusionTable.symbolTable[k].st_other = structure[0].symbolTable[i].st_other;
                        fusionTable.symbolTable[k].st_shndx = structure[0].symbolTable[i].st_shndx;
                        fusionTable.symbolTable[k].st_name = structure[0].symbolTable[i].st_name;
                        fusionTable.symbolTable[k].st_value = (structure[1].symbolTable[j].st_value) +
                                                              sectionFusion.concatenationOffset[i];
                    }
                }

                k++;
            }
        }
    }

    fusionTable.nbSymbol = nbTotSym;
    fusionTable.strtab = mallocArray(char, fusionTable.nbSymbol + 1);
    fusionTable.strtab[0] = mallocArray(char, 1);
    fusionTable.strtab[0][0] = '\0';

    for (int i = 0; i < fusionTable.nbSymbol; i++)
    {
        Elf32_Sym sym = fusionTable.symbolTable[i];
        int index;

        if (i < structure[0].symbolCount)
        {
            index = 0;
        }
        else
        {
            index = 1;
        }

        unsigned char type = ELF32_ST_TYPE(sym.st_info);

        if (type == STT_SECTION)
        {
            fseek(elfFile[index], strndx[index].sh_offset +
                              structure[0].sectionTable[structure[index].symbolTable[i].st_shndx].sh_name,
                  SEEK_SET);
        }
        else
        {
            fseek(elfFile[index], strtab[index].sh_offset + sym.st_name, SEEK_SET);
        }

        int nameSize = 0;
        char c;
        do
        {
            freadEndian(&c, sizeof(char), 1, elfFile[0]);
            nameSize++;
        } while (c != '\0');

        fusionTable.strtab[i + 1] = mallocArray(char, nameSize);

        c = ' ';
        for (int j = 0; c != '\0'; c++)
        {
            freadEndian(&c, sizeof(char), 1, elfFile[0]);
            fusionTable.strtab[i + 1][j] = c;
        }
    }

    return fusionTable;
}


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
    fusion.reimplantationCount = nbTotal;

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

            reim1.reimplantation[i].r_info = ELF32_R_INFO(newSymIndex, ELF32_R_TYPE(reim1.reimplantation[i].r_info));

            Elf32_Sym sym = structure[1].symbolTable[lastSymIndex];

            if (ELF32_ST_TYPE(sym.st_info) != STT_SECTION || section1.sh_type != SHT_RELA)
            {
                continue;
            }

            switch (ELF32_R_TYPE(reim1.reimplantation[i].r_info))
            {
                case R_ARM_ABS8:
                case R_ARM_ABS12:
                case R_ARM_ABS16:
                case R_ARM_ABS32:
                case R_ARM_ABS32_NOI:
                    if (offset > 0)
                    {
                        reim1.reimplantation[i].r_addend += offset;
                    }
                    break;

                case R_ARM_CALL:
                case R_ARM_JUMP24:
                    if (offset > 0)
                    {
                        reim1.reimplantation[i].r_addend += (offset / 4);
                    }
                    break;
            }
        }

        currentIndex++;
    }

    return fusion;
}