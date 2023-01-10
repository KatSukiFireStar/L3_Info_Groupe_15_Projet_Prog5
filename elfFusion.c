//
// Created by Pyrolink on 07/01/2023.
//

#include <stdbool.h>

#include "elfFile.h"
#include "utils.h"

#define DEBUG

#ifdef DEBUG

void DebugSectionFusion(Elf32_Structure *structure, Elf32_SectionFusion sectionFusion)
{
    fprintf(stderr, "New indices: \n");
    for (int i = 0; i < structure[1].sectionCount; i++)
    {
        fprintf(stderr, "\t%i\n", sectionFusion.newIndices[i]);
    }

    fprintf(stderr, "Concatenation offset: \n");
    for (int i = 0; i < structure[1].sectionCount; i++)
    {
        fprintf(stderr, "\t%i\n", sectionFusion.concatenationOffset[i]);
    }

    fprintf(stderr, "Tmp file: %s\n", sectionFusion.tmpFile);
    fprintf(stderr, "Section number: %i\n", sectionFusion.numberSection);
}

#endif

void DoFusionCommand(FILE **elfFiles, Elf32_Structure *structure)
{
    Elf32_SectionFusion sectionFusion = FusionSections(elfFiles, structure);
    Elf32_SymbolFusion symFusion = FusionSymbols(elfFiles, structure, sectionFusion);
    Elf32_RelFusion relFusion = FusionReimplantation(elfFiles, structure, sectionFusion, symFusion);

    ElfCreation("./debug.o", elfFiles, structure, sectionFusion, symFusion, relFusion);

    FreeSectionFusion(sectionFusion);
    FreeSymbolFusion(symFusion);
    FreeRelFusion(relFusion);
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
                    fu.newIndices[j] = i;
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
            //fu.newIndices[i] = numbersectiontmp;
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
    Elf32_Word nbTotSym = structure[0].symbolCount + structure[1].symbolCount;
    Elf32_SymbolFusion fusionTable;

    fusionTable.newIndices = mallocArray(Elf32_Word, structure[1].symbolCount);
    for (int i = 0; i < structure[1].symbolCount; i++)
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

    int currentSymbolIndex = 0;
    for (int symbolIndex0 = 0; symbolIndex0 < structure[0].symbolCount; symbolIndex0++)
    {
        if (ELF32_ST_BIND(structure[0].symbolTable[symbolIndex0].st_info) == STB_LOCAL)
        {
            fusionTable.symbolTable[currentSymbolIndex] = structure[0].symbolTable[symbolIndex0];

            currentSymbolIndex++;
            continue;
        }

        // If GLOBAL
        bool find = false;
        for (int symbolIndex1 = 0; symbolIndex1 < structure[1].symbolCount; symbolIndex1++)
        {
            if (ELF32_ST_BIND(structure[1].symbolTable[symbolIndex1].st_info) == STB_LOCAL)
            {
                continue;
            }

            unsigned char type0 = ELF32_ST_TYPE(structure[0].symbolTable[symbolIndex0].st_info);
            unsigned char type1 = ELF32_ST_TYPE(structure[1].symbolTable[symbolIndex1].st_info);

            if (type0 == STT_SECTION)
            {
                fseek(elfFile[0], strndx[0].sh_offset +
                                  structure[0].sectionTable[structure[0].symbolTable[symbolIndex0].st_shndx].sh_name,
                      SEEK_SET);
            }
            else
            {
                fseek(elfFile[0], strtab[0].sh_offset + structure[0].symbolTable[symbolIndex0].st_name, SEEK_SET);
            }

            if (type1 == STT_SECTION)
            {
                fseek(elfFile[1], strndx[1].sh_offset +
                                  structure[1].sectionTable[structure[1].symbolTable[symbolIndex1].st_shndx].sh_name,
                      SEEK_SET);
            }
            else
            {
                fseek(elfFile[1], strtab[1].sh_offset + structure[1].symbolTable[symbolIndex1].st_name, SEEK_SET);
            }

            // If global
            bool sameName = true;
            char c1, c2;
            do
            {
                freadEndian(&c1, sizeof(char), 1, elfFile[0]);
                freadEndian(&c2, sizeof(char), 1, elfFile[1]);
                if (c1 != c2)
                {
                    sameName = false;
                    break;
                }
            } while (c1 != '\0' || c2 != '\0');

            if (!sameName)
            {
                continue;
            }

            find = true;
            if (structure[0].symbolTable[symbolIndex0].st_shndx != SHN_UNDEF &&
                structure[1].symbolTable[symbolIndex1].st_shndx != SHN_UNDEF)
            {
                printf("Erreur de l'édition de lien: il est interdit pour "
                       "2 fichiers différents que deux symboles globlaux aient le meme nom ");
                exit(-5);
            }
            else if (structure[0].symbolTable[symbolIndex0].st_shndx == SHN_UNDEF &&
                     structure[1].symbolTable[symbolIndex1].st_shndx != SHN_UNDEF)
            {
                fusionTable.newIndices[symbolIndex1] = currentSymbolIndex;
                nbTotSym--;
                fusionTable.symbolTable[currentSymbolIndex] = structure[1].symbolTable[symbolIndex1];
                fusionTable.symbolTable[currentSymbolIndex].st_shndx = sectionFusion.newIndices[fusionTable.symbolTable[currentSymbolIndex].st_shndx];

                if (sectionFusion.concatenationOffset[symbolIndex0] >= 0)
                {
                    fusionTable.symbolTable[currentSymbolIndex].st_value +=
                            sectionFusion.concatenationOffset[fusionTable.symbolTable[currentSymbolIndex].st_shndx];
                }
            }
            else if (structure[0].symbolTable[symbolIndex0].st_shndx != SHN_UNDEF &&
                     structure[1].symbolTable[symbolIndex1].st_shndx == SHN_UNDEF)
            {
                nbTotSym--;
                fusionTable.symbolTable[currentSymbolIndex] = structure[0].symbolTable[symbolIndex0];
            }
            else if (structure[0].symbolTable[symbolIndex0].st_shndx == SHN_UNDEF &&
                     structure[1].symbolTable[symbolIndex1].st_shndx == SHN_UNDEF) //Si les 2 sont non définis
            {
                nbTotSym--;
                fusionTable.symbolTable[currentSymbolIndex] = structure[0].symbolTable[symbolIndex0];
                fusionTable.newIndices[symbolIndex1] = symbolIndex0;
            }
        }
        if (!find)
        {
            fusionTable.symbolTable[currentSymbolIndex] = structure[0].symbolTable[symbolIndex0];
        }
        currentSymbolIndex++;
    }

    for (int symbolIndex1 = 0; symbolIndex1 < structure[1].symbolCount; symbolIndex1++)
    {
        if (fusionTable.newIndices[symbolIndex1] != -1)
        {
            continue;
        }
        fusionTable.newIndices[symbolIndex1] = currentSymbolIndex;
        fusionTable.symbolTable[currentSymbolIndex] = structure[1].symbolTable[symbolIndex1];

        Elf32_Section lastSection = structure[1].symbolTable[symbolIndex1].st_shndx;

        if (lastSection == SHN_ABS)
        {
            currentSymbolIndex++;
            continue;
        }

        Elf32_Word newSection = sectionFusion.newIndices[lastSection];

        fusionTable.symbolTable[currentSymbolIndex].st_shndx = newSection;

        if (newSection < structure[0].sectionCount)
        {
            fusionTable.symbolTable[currentSymbolIndex].st_value +=
                    sectionFusion.concatenationOffset[lastSection];
        }

        currentSymbolIndex++;
    }


    // Recreer la strtab des symboles
    fusionTable.nbSymbol = nbTotSym;
    fusionTable.strtab = mallocArray(char *, fusionTable.nbSymbol);


    for (int fusionSymbolIndex = 0; fusionSymbolIndex < fusionTable.nbSymbol; fusionSymbolIndex++)
    {
        Elf32_Sym sym = fusionTable.symbolTable[fusionSymbolIndex];
        int index;

        if (fusionSymbolIndex < structure[0].symbolCount)
        {
            index = 0;
        }
        else
        {
            index = 1;
        }

        unsigned char type = ELF32_ST_TYPE(fusionTable.symbolTable[fusionSymbolIndex].st_info);

        if (type == STT_SECTION)
        {
            Elf32_Sym currentSym;
            if (index == 1)
            {
                for (int j = 0; j < structure[1].symbolCount; j++)
                {
                    if (fusionTable.newIndices[j] == fusionSymbolIndex)
                    {
                        currentSym = structure[1].symbolTable[j];
                    }
                }
            }
            else
            {
                currentSym = structure[0].symbolTable[fusionSymbolIndex];
            }

            fseek(elfFile[index], strndx[index].sh_offset +
                                  structure[index].sectionTable[currentSym.st_shndx].sh_name,
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
            freadEndian(&c, sizeof(char), 1, elfFile[index]);
            nameSize++;
        } while (c != '\0');

        if (type == STT_SECTION)
        {
            Elf32_Sym currentSym;
            if (index == 1)
            {
                for (int j = 0; j < structure[1].symbolCount; j++)
                {
                    if (fusionTable.newIndices[j] == fusionSymbolIndex)
                    {
                        currentSym = structure[1].symbolTable[j];
                    }
                }
            }
            else
            {
                currentSym = structure[0].symbolTable[fusionSymbolIndex];
            }

            fseek(elfFile[index], strndx[index].sh_offset +
                                  structure[index].sectionTable[currentSym.st_shndx].sh_name,
                  SEEK_SET);
        }
        else
        {
            fseek(elfFile[index], strtab[index].sh_offset + sym.st_name, SEEK_SET);
        }

        fusionTable.strtab[fusionSymbolIndex] = mallocArray(char, nameSize);

        int j = 0;
        do
        {
            freadEndian(&c, sizeof(char), 1, elfFile[index]);
            fusionTable.strtab[fusionSymbolIndex][j] = c;
            j++;
        } while (c != '\0');
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
        fusion.newIndices[i] = -1;
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

    fusion.reimplantationTable = mallocArray(Elf32_Reim, nbTotal);
    fusion.reimplantationCount = nbTotal;

    for (int reimI0 = 0; reimI0 < structure[0].reimplantationCount; reimI0++)
    {
        Elf32_Reim reim0 = structure[0].reimplantationTable[reimI0];
        Elf32_Shdr section0 = structure[0].sectionTable[reim0.section];
        Elf32_Half entityInTable0 = section0.sh_size / section0.sh_entsize;

        fusion.reimplantationTable[reimI0].reimplantation = mallocArray(Elf32_Rela, entityInTable0);
        fusion.reimplantationTable[reimI0].section = reim0.section;

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

        fusion.newIndices[reimI1] = currentIndex;

        Elf32_Reim reim1 = structure[1].reimplantationTable[reimI1];
        Elf32_Shdr section1 = structure[1].sectionTable[reim1.section];
        Elf32_Half entityInTable1 = section1.sh_size / section1.sh_entsize;

        Elf32_Word newSectionIndex = sectionFusion.newIndices[reim1.section];

        fusion.reimplantationTable[currentIndex].section = newSectionIndex;

        fusion.reimplantationTable[currentIndex].reimplantation = mallocArray(Elf32_Rela, entityInTable1);

        Elf32_Off offset;
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

            if (offset != -1)
            {
                fusion.reimplantationTable[currentIndex].reimplantation[i].r_offset += offset;
            }

            int lastSymIndex = ELF32_R_SYM(reim1.reimplantation[i].r_info);
            Elf32_Word newSymIndex = symbolFusion.newIndices[lastSymIndex];

            fusion.reimplantationTable[currentIndex].reimplantation[i].r_info =
                    ELF32_R_INFO(newSymIndex, ELF32_R_TYPE(reim1.reimplantation[i].r_info));

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
                        fusion.reimplantationTable[currentIndex].reimplantation[i].r_addend += offset;
                    }
                    break;

                case R_ARM_CALL:
                case R_ARM_JUMP24:
                    if (offset > 0)
                    {
                        fusion.reimplantationTable[currentIndex].reimplantation[i].r_addend += (offset / 4);
                    }
                    break;
            }
        }

        currentIndex++;
    }

    return fusion;
}

void ElfCreation(char *output, FILE **inputs, Elf32_Structure *structures, Elf32_SectionFusion sectionFusion,
                 Elf32_SymbolFusion symFusion, Elf32_RelFusion relFusion)
{
    Elf32_Ehdr header;

    header.e_ident[EI_MAG0] = ELFMAG0;
    header.e_ident[EI_MAG1] = ELFMAG1;
    header.e_ident[EI_MAG2] = ELFMAG2;
    header.e_ident[EI_MAG3] = ELFMAG3;

    // Statique avec l'énoncé
    header.e_ident[EI_CLASS] = ELFCLASS32;
    header.e_ident[EI_DATA] = ELFDATA2LSB;
    header.e_ident[EI_VERSION] = 1;
    header.e_ident[EI_OSABI] = ELFOSABI_NONE;
    header.e_ident[EI_ABIVERSION] = 0;
    header.e_ident[EI_PAD] = 0;
    for (int i = EI_PAD; i < EI_NIDENT; i++)
    {
        header.e_ident[i] = 0;
    }
    header.e_type = ET_REL;
    header.e_machine = EM_ARM;
    header.e_version = EV_CURRENT;
    header.e_entry = 0x0;
    header.e_phoff = 0x0;
    header.e_shoff = 0xf; // ToDo
    header.e_flags = EF_MIPS_ARCH_32; // FixMe: À vérifier
    header.e_ehsize = sizeof(Elf32_Ehdr);
    header.e_phentsize = 0; // Pas de programme
    header.e_phnum = 0; // Pas de programme

    header.e_shentsize = 0xf; // ToDo
    header.e_shnum = structures[0].sectionCount + structures[1].sectionCount; // ToDo: Mettre à jour
    header.e_shstrndx = 0; // ToDo

    Elf32_ShdrTable newTable = mallocArray(Elf32_Shdr, header.e_shnum);

    Elf32_Off currentSectionOffset = header.e_ehsize;
    Elf32_Section currentSectionIndex = 0;
    for (int sectionIndexFile0 = 0; sectionIndexFile0 < structures[0].sectionCount; sectionIndexFile0++)
    {
        Elf32_Shdr section0 = structures[0].sectionTable[sectionIndexFile0];
        newTable[currentSectionIndex] = section0;

        Elf32_Shdr *concatenatedSection = NULL;

        switch (section0.sh_type)
        {
            case SHT_PROGBITS:
                if (sectionIndexFile0 == 0)
                {
                    break;
                }

                newTable[currentSectionIndex].sh_offset = currentSectionOffset;

                if (sectionFusion.concatenationOffset[sectionIndexFile0] != -1)
                {
                    concatenatedSection = NULL;
                    for (int sectionIndexFile1 = 0; sectionIndexFile1 < structures[1].sectionCount; sectionIndexFile1++)
                    {
                        if (sectionFusion.newIndices[sectionIndexFile1] == sectionIndexFile0)
                        {
                            concatenatedSection = &structures[1].sectionTable[sectionIndexFile1];
                        }
                    }

                    if (concatenatedSection != NULL)
                    {
                        newTable[currentSectionIndex].sh_size += concatenatedSection->sh_size;
                    }
                }

                currentSectionOffset += newTable[currentSectionIndex].sh_size;

                break;
            case SHT_SYMTAB:
                newTable[currentSectionIndex].sh_size = sizeof(Elf32_Sym) * symFusion.nbSymbol;
                newTable[currentSectionIndex].sh_entsize = sizeof(Elf32_Sym);
                newTable[currentSectionIndex].sh_offset = currentSectionOffset;

                currentSectionOffset += newTable[currentSectionIndex].sh_size;

                break;
            case SHT_REL:
                newTable[currentSectionIndex].sh_offset = currentSectionOffset;
                newTable[currentSectionIndex].sh_entsize = sizeof(Elf32_Rel);

                concatenatedSection = NULL;
                for (int relIndexFile1 = 0; relIndexFile1 < structures[1].reimplantationCount; relIndexFile1++)
                {
                    if (relFusion.reimplantationTable[0].section == sectionIndexFile0)
                    {
                        concatenatedSection = &structures[1].sectionTable[relIndexFile1];
                    }
                }

                if (concatenatedSection != NULL)
                {
                    newTable[currentSectionIndex].sh_size += concatenatedSection->sh_size;
                }

                newTable[currentSectionIndex].sh_offset = currentSectionOffset;

                currentSectionOffset += newTable[currentSectionIndex].sh_size;
                break;
            case SHT_RELA:
                newTable[currentSectionIndex].sh_offset = currentSectionOffset;
                newTable[currentSectionIndex].sh_entsize = sizeof(Elf32_Rela);

                concatenatedSection = NULL;
                for (int sectionIndexFile1 = 0; sectionIndexFile1 < structures[1].sectionCount; sectionIndexFile1++)
                {
                    if (relFusion.newIndices[sectionIndexFile1] == sectionIndexFile0)
                    {
                        concatenatedSection = &structures[1].sectionTable[sectionIndexFile1];
                    }
                }

                if (concatenatedSection != NULL)
                {
                    newTable[currentSectionIndex].sh_size += concatenatedSection->sh_size;
                }

                newTable[currentSectionIndex].sh_offset = currentSectionOffset;

                currentSectionOffset += newTable[currentSectionIndex].sh_size;
                break;
        }
        currentSectionIndex++;
    }

    for (int sectionIndexFile1 = 0; sectionIndexFile1 < structures[1].sectionCount; sectionIndexFile1++)
    {
        Elf32_Shdr section1 = structures[1].sectionTable[sectionIndexFile1];

        switch (section1.sh_type)
        {
            case SHT_PROGBITS:
                if (sectionFusion.newIndices[sectionIndexFile1] != -1)
                {
                    continue;
                }

                newTable[currentSectionIndex] = section1;
                newTable[currentSectionIndex].sh_offset = currentSectionOffset;
                break;
//            case SHT_SYMTAB:
//                break;
            case SHT_REL:
            case SHT_RELA:
                if (sectionFusion.newIndices[sectionIndexFile1] != -1)
                {
                    continue;
                }

                newTable[currentSectionIndex] = section1;
                newTable[currentSectionIndex].sh_offset = currentSectionOffset;
                break;
        }
        currentSectionIndex++;
    }

    (void) header;

    free(newTable);
}