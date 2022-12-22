//
// Created by Pyrolink on 14/12/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "elfFile.h"

Elf32_Ehdr ShowElfHeader(FILE *elfFile)
{
    Elf32_Ehdr header;

#pragma region Read

    fread(&header.e_ident, sizeof(unsigned char), EI_NIDENT, elfFile);
    fread(&header.e_type, sizeof(Elf32_Half), 1, elfFile);
    fread(&header.e_machine, sizeof(Elf32_Half), 1, elfFile);
    fread(&header.e_version, sizeof(Elf32_Word), 1, elfFile);
    fread(&header.e_entry, sizeof(Elf32_Addr), 1, elfFile);
    fread(&header.e_phoff, sizeof(Elf32_Off), 1, elfFile);
    fread(&header.e_shoff, sizeof(Elf32_Off), 1, elfFile);
    fread(&header.e_flags, sizeof(Elf32_Word), 1, elfFile);
    fread(&header.e_ehsize, sizeof(Elf32_Half), 1, elfFile);
    fread(&header.e_phentsize, sizeof(Elf32_Half), 1, elfFile);
    fread(&header.e_phnum, sizeof(Elf32_Half), 1, elfFile);
    fread(&header.e_shentsize, sizeof(Elf32_Half), 1, elfFile);
    fread(&header.e_shnum, sizeof(Elf32_Half), 1, elfFile);
    fread(&header.e_shstrndx, sizeof(Elf32_Half), 1, elfFile);

#pragma endregion

#pragma region check ELF

    if (header.e_ident[EI_MAG0] != ELFMAG0 ||
        header.e_ident[EI_MAG1] != ELFMAG1 ||
        header.e_ident[EI_MAG2] != ELFMAG2 ||
        header.e_ident[EI_MAG3] != ELFMAG3)
    {
        exit(-1);
    }

#pragma endregion

#pragma region display

    // afficher Magique
    printf("  Magic: \t\t\t");
    for (int i = 0; i < EI_NIDENT; i++)
    {
        printf("%02hhx ", header.e_ident[i]);
    }
    printf("\n");

    // afficher classe
    printf("  Class: \t\t\t");
    switch (header.e_ident[EI_CLASS])
    {
        case ELFCLASS32:
            printf("ELF32");
            break;
        case ELFCLASS64:
            printf("ELF64");
            break;
        default:
            exit(-1);
    }

    printf("\n");

    // afficher Data
    printf("  Data: \t\t\t");
    switch (header.e_ident[EI_DATA])
    {
        case ELFDATA2LSB:
            printf("2's complement, little endian");
            break;
        case ELFDATA2MSB:
            printf("2's complement, Big endian");
            break;
        default:
            exit(-1);
    }

    printf("\n");

    // afficher version
    printf("  Version: \t\t\t%d", header.e_version);
    switch (header.e_version)
    {
        case EV_NONE:
            printf("(Invalid)");
            break;
        case EV_CURRENT:
            printf("(Current)");
            break;
        default:
            exit(-1);
    }
    printf("\n");

    // afficher OS/ABI
    printf("  OS/ABI: \t\t\t");
    switch (header.e_ident[EI_OSABI])
    {
        case ELFOSABI_NONE:
            printf("UNIX System V ABI");
            break;
        case ELFOSABI_HPUX:
            printf("HP-UX");
            break;
        case ELFOSABI_NETBSD:
            printf("NetBSD");
            break;
        case ELFOSABI_GNU:
            printf("Object uses GNU ELF extensions");
            break;
        case ELFOSABI_SOLARIS:
            printf("Sun Solaris");
            break;
        case ELFOSABI_AIX:
            printf("IBM AIX");
            break;
        case ELFOSABI_IRIX:
            printf("SGI Irix");
            break;
        case ELFOSABI_FREEBSD:
            printf("FreeBSD");
            break;
        case ELFOSABI_TRU64:
            printf("Compaq TRU64 UNIX");
            break;
        case ELFOSABI_MODESTO:
            printf("Novell Modesto");
            break;
        case ELFOSABI_OPENBSD:
            printf("OpenBSD");
            break;
        case ELFOSABI_ARM_AEABI:
            printf("ARM EABI");
            break;
        case ELFOSABI_ARM:
            printf("ARM");
            break;
        case ELFOSABI_STANDALONE:
            printf("Standalone (embedded) application");
            break;
        default:
            exit(-1);
    }
    printf("\n");

    // afficher Version ABI
    printf("  ABI Version: \t\t\t%d\n", header.e_ident[EI_ABIVERSION]);

    //afficher type
    printf("  Type: \t\t\t");
    switch (header.e_type)
    {
        case ET_NONE:
            printf("NONE, No file type");
            break;
        case ET_REL:
            printf("REL, Relocatable file");
            break;
        case ET_EXEC:
            printf("EXEC, Executable file");
            break;
        case ET_DYN:
            printf("DYN, Shared object file");
            break;
        case ET_CORE:
            printf("CORE, Core file");
            break;
        case ET_NUM:
            printf("NUM, Fichier Core");
            break;
        default:
            exit(-1);
    }
    printf("\n");

    //afficher machine
    printf("  Machine: \t\t\t");
    switch (header.e_machine)
    {
        case EM_NONE:
            printf("No machine");
            break;
        case EM_M32:
            printf("AT&T WE 32100");
            break;
        case EM_SPARC:
            printf("SUN SPARC");
            break;
        case EM_386:
            printf("Intel 80386");
            break;
        case EM_68K:
            printf("Motorola m68k family");
            break;
        case EM_88K:
            printf("Motorola m88k family");
            break;
        case EM_IAMCU:
            printf("Intel MCU");
            break;
        case EM_860:
            printf("Intel 80860");
            break;
        case EM_MIPS:
            printf("MIPS R3000 big-endian");
            break;
        case EM_BPF:
            printf("Linux BPF -- in-kernel virtual machine");
            break;
        case EM_X86_64:
            printf("AMD x86-64 architecture");
            break;
        default:
            printf("Machine not implemented (%d)", header.e_machine);
    }
    printf("\n");

    // afficher Entry point address
    printf("  Entry point address:  \t0x%x\n", header.e_entry);

    // afficher Start of program headers
    printf("  Start of program headers:  \t%d\n", header.e_phoff);

    // afficher Start of section headers:
    printf("  Start of section headers:  \t%d\n", header.e_shoff);

    // afficher Fanions
    printf("  Flags: \t\t\t0x%x (", header.e_flags);
    if ((header.e_flags & EF_PARISC_TRAPNIL) == EF_PARISC_TRAPNIL)
    {
        printf(" 'Trap nil pointer dereference'");
    }
    if ((header.e_flags & EF_PARISC_EXT) == EF_PARISC_EXT)
    {
        printf(" 'Program uses arch. extensions'");
    }
    if ((header.e_flags & EF_PARISC_LSB) == EF_PARISC_LSB)
    {
        printf(" 'Program expects little endian'");
    }
    if ((header.e_flags & EF_PARISC_WIDE) == EF_PARISC_WIDE)
    {
        printf(" 'Program expects wide mode'");
    }
    if ((header.e_flags & EF_PARISC_NO_KABP) == EF_PARISC_NO_KABP)
    {
        printf(" 'No kernel assisted branch'");
    }
    if ((header.e_flags & EF_PARISC_LAZYSWAP) == EF_PARISC_LAZYSWAP)
    {
        printf(" 'Allow lazy swapping'");
    }
    if ((header.e_flags & EF_PARISC_ARCH) == EF_PARISC_ARCH)
    {
        printf(" 'Architecture version, ");
        if ((header.e_flags & EFA_PARISC_1_0) == EFA_PARISC_1_0)
        {
            printf(" 'PA-RISC 1.0 big-endian'");
        }
        if ((header.e_flags & EFA_PARISC_1_1) == EFA_PARISC_1_1)
        {
            printf(" 'PA-RISC 1.1 big-endian'");
        }
        if ((header.e_flags & EFA_PARISC_2_0) == EFA_PARISC_2_0)
        {
            printf(" 'PA-RISC 2.0 big-endian'");
        }
        printf("'");
    }
    printf(" )\n");

    // afficher Size of this header
    printf("  Size of this header:  \t%d bytes\n", header.e_ehsize);

    //afficher Size of program headers
    printf("  Size of program headers:  \t%d bytes\n", header.e_phentsize);

    // afficher Number of program headers
    printf("  Number of program headers: \t%d\n", header.e_phnum);

    // afficher Size of section headers
    printf("  Size of section headers:  \t%d bytes\n", header.e_shentsize);

    // afficher Nombre of section header:
    printf("  Number of section header: \t%d\n", header.e_shnum);

    // afficher Section header string table index
    printf("  String table section index: \t%d\n", header.e_shstrndx);

#pragma endregion

    return header;
}

void ShowSectionFromIndex(FILE *elfFile, Elf32_ShdrTable sectionTable, Elf32_Word index)
{
    Elf32_Shdr section = sectionTable[index];

    fseek(elfFile, section.sh_offset, SEEK_SET);

    fprintf(stdout, "           00010203 04050607 08090a0b 0c0d0e0f\n"
                    "0x00000000 ");
    for (Elf32_Word i = 0; i < section.sh_size; i++)
    {
        unsigned char byte;
        fread(&byte, sizeof(unsigned char), 1, elfFile);

        fprintf(stdout, "%02hhx", byte);

        if ((i + 1) % 4 != 0)
        {
            continue;
        }

        if ((i + 1) % 16 == 0)
        {
            fprintf(stdout, "\n0x%08x ", i + 1);
        }
        else
        {
            fprintf(stdout, " ");
        }
    }

    fprintf(stdout, "\n");
}

Elf32_Word GetSectionIndexByName(FILE *elfFile, Elf32_Shdr *sectionTable, Elf32_Ehdr header, char *name)
{
    Elf32_Shdr stringTable = sectionTable[header.e_shstrndx];

    fseek(elfFile, stringTable.sh_offset, SEEK_SET);

    Elf32_Word nameId = -1;

    int skip = 0;
    Elf32_Word currentIndex = 0;

    for (Elf32_Word stringOffset = 1; stringOffset <= stringTable.sh_size; stringOffset++)
    {
        char currentChar;
        fread(&currentChar, sizeof(char), 1, elfFile);

        if (skip == 1)
        {
            if (currentChar == '\0')
            {
                skip = 0;
            }

            continue;
        }

        if (name[currentIndex] == currentChar)
        {
            if (currentChar == '\0')
            {
                nameId = stringOffset - currentIndex - 1;
                for (Elf32_Half sectionIndex = 0; sectionIndex < header.e_shentsize; sectionIndex++)
                {
                    if (sectionTable[sectionIndex].sh_name == nameId)
                    {
                        return sectionIndex;
                    }
                }
            }
            currentIndex++;
        }
        else
        {
            skip = 1;
            currentIndex = 0;
        }
    }

    if (nameId == -1)
    {
        printf("The string is not found");
        exit(-2);
    }

    printf("No section have the %s name", name);
    exit(-2);
}

void ShowSectionFromName(FILE *elfFile, Elf32_ShdrTable sectionTable, Elf32_Ehdr header, char *name)
{
    ShowSectionFromIndex(elfFile, sectionTable, GetSectionIndexByName(elfFile, sectionTable, header, name));
}

void ShowStringFromIndex(FILE *elfFile, Elf32_Shdr stringTable, Elf32_Word offset)
{
    fseek(elfFile, stringTable.sh_offset + offset, SEEK_SET);

    char c;
    do
    {
        fread(&c, sizeof(char), 1, elfFile);
        printf("%c", c);
    } while (c != '\0');
}

Elf32_ShdrTable ShowSectionTableAndDetails(FILE *elfFile, Elf32_Ehdr header)
{
    Elf32_ShdrTable sectionTable = mallocArray(Elf32_Shdr, header.e_shnum);
    fseek(elfFile, header.e_shoff, SEEK_SET);

#pragma region Read

    // Lecture de la table des sections
    for (Elf32_Half sectionIndex = 0; sectionIndex < header.e_shnum; sectionIndex++)
    {
        // Lire le nom de la section
        fread(&sectionTable[sectionIndex].sh_name, sizeof(Elf32_Word), 1, elfFile);

        // Lire le type de la section
        fread(&sectionTable[sectionIndex].sh_type, sizeof(Elf32_Word), 1, elfFile);

        // Lire le type de la section
        fread(&sectionTable[sectionIndex].sh_flags, sizeof(Elf32_Word), 1, elfFile);

        // Lire l'adresse de la section
        fread(&sectionTable[sectionIndex].sh_addr, sizeof(Elf32_Addr), 1, elfFile);

        // Lire la position de la section
        fread(&sectionTable[sectionIndex].sh_offset, sizeof(Elf32_Word), 1, elfFile);

        // Lire la taille de la section
        fread(&sectionTable[sectionIndex].sh_size, sizeof(Elf32_Word), 1, elfFile);

        // Lire l'indice de la table des en-têtes de sections
        fread(&sectionTable[sectionIndex].sh_link, sizeof(Elf32_Word), 1, elfFile);

        // Lire les informations supplémentaires
        fread(&sectionTable[sectionIndex].sh_info, sizeof(Elf32_Word), 1, elfFile);

        // Lire la taille de l'alignement
        fread(&sectionTable[sectionIndex].sh_addralign, sizeof(Elf32_Word), 1, elfFile);

        // Lire la taille de l'entrée
        fread(&sectionTable[sectionIndex].sh_entsize, sizeof(Elf32_Word), 1, elfFile);
    }

#pragma endregion

    Elf32_Shdr strndx = sectionTable[header.e_shstrndx];

#pragma region Display

    // Affichage des informations de la table des sections
    for (Elf32_Half sectionIndex = 0; sectionIndex < header.e_shnum; sectionIndex++)
    {
        printf("Section %d\n", sectionIndex);
        printf("  Section name: \t\t\t");

        ShowStringFromIndex(elfFile, strndx, sectionTable[sectionIndex].sh_name);

        printf("\n");

        printf("  Section type: \t\t\t%u ( ", sectionTable[sectionIndex].sh_type);
        switch (sectionTable[sectionIndex].sh_type)
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

        printf(" )\n");

        // L'adresse à  laquelle le premier octet de la section doit se trouver.
        printf("  Section address: \t\t\t0x%x\n", sectionTable[sectionIndex].sh_addr);


        printf("  Section offset: \t\t\t0x%x\n", sectionTable[sectionIndex].sh_offset);


        printf("  Section size: \t\t\t0x%x\n", sectionTable[sectionIndex].sh_size);

        // La taille de l'entrée, pour les sections qui contiennent une table d'entrées de même taille.
        printf("  Entry size if section holds table: \t0x%x\n", sectionTable[sectionIndex].sh_entsize);


        printf("  Section flags: \t\t\t0x%x ( ", sectionTable[sectionIndex].sh_flags);
        // Cette section contient des données qu'il devrait être possible d'écrire durant l'exécution du processus;
        if ((sectionTable[sectionIndex].sh_flags & SHF_WRITE) == SHF_WRITE)
            printf("'Write' ");

        // La section fait partie de l'image mémoire du programme à exécuter.");
        if ((sectionTable[sectionIndex].sh_flags & SHF_ALLOC) == SHF_ALLOC)
            printf("'Alloc' ");

        // La section contient du code exécutable.\n");
        if ((sectionTable[sectionIndex].sh_flags & SHF_EXECINSTR) == SHF_EXECINSTR)
            printf("'Executable' ");

        // Tous les bits contenus dans ce masque sont réservés à des sémantiques spécifiques au processeur
        if ((sectionTable[sectionIndex].sh_flags & SHF_MASKPROC) == SHF_MASKPROC)
            printf("'Processor-specific' ");

        printf(")\n");

        // Lien vers un indice de la table des en-têtes de  sections,
        printf("  Link to another section: \t\t%d\n", sectionTable[sectionIndex].sh_link);

        // Informations supplémentaires, dépendant du type de section.
        printf("  Additional section information: \t%d\n", sectionTable[sectionIndex].sh_info);

        // La taille de l'alignement, exprimée en puissance de 2.
        printf("  Section alignment (exponent of 2): \t%u\n", sectionTable[sectionIndex].sh_addralign);
        printf("\n");
    }

#pragma endregion

    return sectionTable;
}

Elf32_Half GetEntryCountFromType(Elf32_Ehdr header, Elf32_ShdrTable sectionTable, Elf32_Half type)
{
    Elf32_Half count = 0;
    for (Elf32_Half i = 0; i < header.e_shnum; i++)
    {
        if (sectionTable[i].sh_type == type)
        {
            count += sectionTable[i].sh_size / sectionTable[i].sh_entsize;
        }
    }
    return count;
}

Elf32_SymTable ShowSymbolsTableAndDetails(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable)
{
    Elf32_Half symTableSize = GetEntryCountFromType(header, sectionTable, SHT_SYMTAB);

    Elf32_Sym *symbolTable = mallocArray(Elf32_Sym, symTableSize);

    for (Elf32_Half sectionIndex = 0; sectionIndex < header.e_shnum; sectionIndex++)
    {
        Elf32_Shdr section = sectionTable[sectionIndex];

        if (section.sh_type != SHT_SYMTAB)
        {
            continue;
        }

        Elf32_Half count = section.sh_size / section.sh_entsize;

        printf("Symbols from section index %d\n", sectionIndex);

        fseek(elfFile, section.sh_offset, SEEK_SET);

        for (Elf32_Half symbolIndex = 0; symbolIndex < count; symbolIndex++)
        {
            fread(&symbolTable[symbolIndex].st_name, sizeof(Elf32_Word), 1, elfFile);
            fread(&symbolTable[symbolIndex].st_value, sizeof(Elf32_Addr), 1, elfFile);
            fread(&symbolTable[symbolIndex].st_size, sizeof(Elf32_Word), 1, elfFile);
            fread(&symbolTable[symbolIndex].st_info, sizeof(unsigned char), 1, elfFile);
            fread(&symbolTable[symbolIndex].st_other, sizeof(unsigned char), 1, elfFile);
            fread(&symbolTable[symbolIndex].st_shndx, sizeof(Elf32_Section), 1, elfFile);
        }
        // affichage des symboles

        Elf32_Shdr strtab = sectionTable[GetSectionIndexByName(elfFile, sectionTable, header, ".strtab")];
        Elf32_Shdr strndx = sectionTable[header.e_shstrndx];

        for (Elf32_Half symbolIndex = 0; symbolIndex < count; symbolIndex++)
        {
            //affichage numero
            printf("Symbol %d\n", symbolIndex);

            //affichage value
            printf("  Value: \t%08x\n", symbolTable[symbolIndex].st_value);

            //affichage size
            printf("  Size: \t%d\n", symbolTable[symbolIndex].st_size);

            //affichage type
            unsigned char type = ELF32_ST_TYPE(symbolTable[symbolIndex].st_info);
            printf("  Type: \t%d ( ", type);
            switch (ELF32_ST_TYPE(symbolTable[symbolIndex].st_info))
            {
                case STT_NOTYPE:
                    printf("'Symbol type is unspecified' ");
                    break;
                case STT_OBJECT:
                    printf("'Symbol is a data object' ");
                    break;
                case STT_FUNC:
                    printf("'Symbol is a code object' ");
                    break;
                case STT_SECTION:
                    printf("'Symbol associated with a section' ");
                    break;
                case STT_FILE:
                    printf("'Symbol's name is file name' ");
                    break;
                case STT_COMMON:
                    printf("'Symbol is a common data object' ");
                    break;
                case STT_TLS:
                    printf("'Symbol is thread-local data object' ");
                    break;
                case STT_NUM:
                    printf("'Number of defined types' ");
                    break;
                case STT_LOOS:
                    printf("'Start of OS-specific / Symbol is indirect code object' ");
                    break;
                    /*case STT_LOOS:
                        printf("Start of OS-specific");
                        break;
                    case STT_GNU_IFUNC:
                        printf("Symbol is indirect code object");
                        break;*/
                case STT_HIOS:
                    printf("'End of OS-specific' ");
                    break;
                case STT_LOPROC:
                    printf("'Start of processor-specific' ");
                    break;
                case STT_HIPROC:
                    printf("'End of processor-specific' ");
                    break;
                default:
                    exit(-3);
            }
            printf(")\n");

            //affichage bind
            unsigned char bind = ELF32_ST_BIND(symbolTable[symbolIndex].st_info);
            printf("  Bind: \t%d ( ", bind);
            switch (bind)
            {
                case STB_LOCAL:
                    printf("'Local symbol' ");
                    break;
                case STB_GLOBAL:
                    printf("'Global symbol' ");
                    break;
                case STB_WEAK:
                    printf("'Weak symbol' ");
                    break;
                case STB_NUM:
                    printf("'Number of defined types' ");
                    break;
                case STB_LOOS:
                    printf("'Start of OS-specific / Unique symbol' ");
                    break;
                    /*case STB_LOOS:
                        printf("Start of OS-specific");
                        break;
                    case STB_GNU_UNIQUE:
                        printf("Unique symbol");
                        break;*/
                case STB_HIOS:
                    printf("'End of OS-specific' ");
                    break;
                case STB_LOPROC:
                    printf("'Start of processor-specific' ");
                    break;
                case STB_HIPROC:
                    printf("'End of processor-specific' ");
                    break;
                default:
                    exit(-3);
            }
            printf(")\n");

            //affichage Visibility
            unsigned char visibility = ELF32_ST_VISIBILITY(symbolTable[symbolIndex].st_other);
            printf("  Visibility: \t%d (", visibility);
            switch (visibility)
            {
                case STV_DEFAULT:
                    printf("Default symbol visibility rules");
                    break;
                case STV_INTERNAL:
                    printf("Processor specific hidden class");
                    break;
                case STV_HIDDEN:
                    printf("Sym unavailable in other modules");
                    break;
                case STV_PROTECTED:
                    printf("Not preemptible, not exported");
                    break;
                default:
                    exit(-3);
            }
            printf(")\n");

            //afficher
            printf("  Ndx: \t\t");
            switch (symbolTable[symbolIndex].st_shndx)
            {
                case SHN_UNDEF:
                    printf("UNDEF");
                    break;
                case SHN_LORESERVE:
                    printf("LORESERVE / LOPROC / BEFORE");
                    break;
                case SHN_AFTER:
                    printf("AFTER");
                    break;
                case SHN_HIPROC:
                    printf("HIPROC");
                    break;
                case SHN_LOOS:
                    printf("LOOS");
                    break;
                case SHN_HIOS:
                    printf("HIOS");
                    break;
                case SHN_ABS:
                    printf("ABS");
                    break;
                case SHN_COMMON:
                    printf("COMMON");
                    break;
                case SHN_XINDEX:
                    printf("XINDEX / HIRESERVE");
                    break;
                default:
                    printf("%d", symbolTable[symbolIndex].st_shndx);
                    break;
            }

            printf("\n");

            //afficher nom
            printf("  Symbol name: \t");
            if (type == STT_SECTION)
            {
                fseek(elfFile, strndx.sh_offset + sectionTable[symbolTable[symbolIndex].st_shndx].sh_name, SEEK_SET);
            }
            else
            {
                fseek(elfFile, strtab.sh_offset + symbolTable[symbolIndex].st_name, SEEK_SET);
            }

            char c = ' ';
            while (c != '\0')
            {
                fread(&c, sizeof(char), 1, elfFile);
                printf("%c", c);
            }
            printf("\n\n");
        }
    }

    return symbolTable;
}

Elf32_RelTable ShowReimplantationTablesAndDetails(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable,
                                                  Elf32_SymTable symbolTable)
{
    uint32_t reimTableSize = GetEntryCountFromType(header, sectionTable, SHT_REL);

    Elf32_Rel *reimplantationTable = mallocArray(Elf32_Rel, reimTableSize);

    Elf32_Shdr strtab = sectionTable[GetSectionIndexByName(elfFile, sectionTable, header, ".strtab")];
    Elf32_Shdr strndx = sectionTable[header.e_shstrndx];

    Elf32_Half symbolIndex = 0;
    // Pour chaque entrée de la table des sections
    for (Elf32_Half tableIndex = 0; tableIndex < header.e_shnum; tableIndex++)
    {
        if (sectionTable[tableIndex].sh_type != SHT_REL)
        {
            continue;
        }

        printf("Reimplantation from table %d\n", tableIndex);

        fseek(elfFile, sectionTable[tableIndex].sh_offset, SEEK_SET);

        Elf32_Half symbolIndexOffset = symbolIndex;
        Elf32_Half symbolsInTable = sectionTable[tableIndex].sh_size / sectionTable[tableIndex].sh_entsize;

        for (Elf32_Half i = 0; i < symbolsInTable; i++)
        {
            fread(&reimplantationTable[symbolIndex].r_offset, sizeof(Elf32_Addr), 1, elfFile);
            fread(&reimplantationTable[symbolIndex].r_info, sizeof(Elf32_Word), 1, elfFile);
            symbolIndex++;
        }

        symbolIndex = symbolIndexOffset;
        for (Elf32_Half i = 0; i < symbolsInTable; i++)
        {
            printf("Reimplantation %d\n", symbolIndex);

            printf("  Offset: \t0x%08x\n", reimplantationTable[symbolIndex].r_offset);
            printf("  Info: \t%0x08x\n", reimplantationTable[symbolIndex].r_info);

            printf("  Type: \t%d\n", ELF32_R_TYPE(reimplantationTable[symbolIndex].r_info));
            Elf32_Sym sym = symbolTable[ELF32_R_SYM(reimplantationTable[symbolIndex].r_info)];
            printf("  Symbol value: 0x%08x\n", sym.st_value);
            printf("  Symbol name: \t");
            unsigned char type = ELF32_ST_TYPE(sym.st_info);
            if (type == STT_SECTION)
            {
                ShowStringFromIndex(elfFile, strndx, sectionTable[sym.st_shndx].sh_name);
            }
            else
            {
                ShowStringFromIndex(elfFile, strtab, sym.st_name);
            }
            printf("\n\n");
            symbolIndex++;
        }
    }

    return reimplantationTable;
}


int main(int argc, char *argv[])
{
    FILE *elfFile;
    Elf32_Ehdr header[argc - 1];
    Elf32_Shdr *sectionTable[argc - 1];
    Elf32_Sym *symbolTable[argc - 1];
    Elf32_Rel *reimplantationTable[argc - 1];

    if (argc <= 1)
    {
        fprintf(stderr, "Vous n'avez pas mis de fichier en paramètres!");
        return -3;
    }
//    else if (argc <= 2)
//    {
//        fprintf(stderr, "Vous n'avez mis qu'un seul fichier en paramètres!");
//        return -4;
//    }

    for (int i = 1; i < argc; i++)
    {
        if (access(argv[i], R_OK))
        {
            fprintf(stderr, "Le fichier %s ne peut pas etre ouvert!", argv[i]);
            return -5;
        }
    }

    // Permet de recuperer toutes les informations d'un fichier
    // et les stockes dans des variables
    // A la fin, le fichier est fermé et on ouvre le fichier suivant
    for (int i = 1; i < argc; i++)
    {
        elfFile = fopen(argv[i], "r");

        printf("ELF Header: \n");
        header[i - 1] = ShowElfHeader(elfFile);
        rewind(elfFile);
        printf("\n");

        printf("Section table: \n");
        sectionTable[i - 1] = ShowSectionTableAndDetails(elfFile, header[i - 1]);
        rewind(elfFile);
        printf("\n");

        printf("Section .shstrtab\n");
        ShowSectionFromName(elfFile, sectionTable[i - 1], header[i - 1], ".shstrtab");
        rewind(elfFile);
        printf("\n");

        printf("Symbol table: \n");
        symbolTable[i - 1] = ShowSymbolsTableAndDetails(elfFile, header[i - 1], sectionTable[i - 1]);
        rewind(elfFile);
        printf("\n");

        printf("Reimplantation table: \n");
        reimplantationTable[i - 1] = ShowReimplantationTablesAndDetails(
                elfFile, header[i - 1], sectionTable[i - 1], symbolTable[i - 1]);
        rewind(elfFile);
        printf("\n");

        (void) reimplantationTable;

        fclose(elfFile);
    }

    return 0;
}