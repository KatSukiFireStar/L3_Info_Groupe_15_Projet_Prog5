//
// Created by Pyrolink on 14/12/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "elfFile.h"
#include "elfStructure.h"

#pragma region Endian

int needReverse = 0;

void CheckMachineEndian(unsigned char fileEndian)
{
    uint32_t one = 1;
    int isBig = (*(uint8_t *) &one) == 0;

    needReverse = ((isBig && (fileEndian != ELFDATA2MSB)) ||
                   (!isBig && (fileEndian != ELFDATA2LSB)));
}

size_t freadEndian(void *restrict ptr, size_t size, size_t number, FILE *restrict file)
{
    size_t result = fread(ptr, size, number, file);

    if (!needReverse)
        return result;

    for (int i = 0; i < number; i++)
    {
        switch (size)
        {
            case 2:
            {
                uint16_t *intPtr = (uint16_t *) ptr;
                intPtr += i;
                *intPtr = ((((*intPtr) & 0xFF) << 8) | (((*intPtr) >> 8) & 0xFF));
            }
            case 4:
            {
                uint32_t *intPtr = (uint32_t *) ptr;
                intPtr += i;
                *intPtr = ((((*intPtr) & 0xFF) << 24) | ((((*intPtr) >> 8) & 0xFF) << 16) |
                           ((((*intPtr) >> 16) & 0xFF) << 8) | (((*intPtr) >> 24) & 0xFF));
            }
        }
    }

    return result;
}

#pragma endregion

Elf32_Ehdr ExtractHeader(FILE *elfFile)
{
    Elf32_Ehdr header;

    freadEndian(&header.e_ident, sizeof(unsigned char), EI_NIDENT, elfFile);
    CheckMachineEndian(header.e_ident[EI_DATA]);
    freadEndian(&header.e_type, sizeof(Elf32_Half), 1, elfFile);
    freadEndian(&header.e_machine, sizeof(Elf32_Half), 1, elfFile);
    freadEndian(&header.e_version, sizeof(Elf32_Word), 1, elfFile);
    freadEndian(&header.e_entry, sizeof(Elf32_Addr), 1, elfFile);
    freadEndian(&header.e_phoff, sizeof(Elf32_Off), 1, elfFile);
    freadEndian(&header.e_shoff, sizeof(Elf32_Off), 1, elfFile);
    freadEndian(&header.e_flags, sizeof(Elf32_Word), 1, elfFile);
    freadEndian(&header.e_ehsize, sizeof(Elf32_Half), 1, elfFile);
    freadEndian(&header.e_phentsize, sizeof(Elf32_Half), 1, elfFile);
    freadEndian(&header.e_phnum, sizeof(Elf32_Half), 1, elfFile);
    freadEndian(&header.e_shentsize, sizeof(Elf32_Half), 1, elfFile);
    freadEndian(&header.e_shnum, sizeof(Elf32_Half), 1, elfFile);
    freadEndian(&header.e_shstrndx, sizeof(Elf32_Half), 1, elfFile);

    return header;
}

void ShowElfHeader(Elf32_Ehdr header)
{

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
    printf("  Version: \t\t\t%d (", header.e_version);
    switch (header.e_version)
    {
        case EV_NONE:
            printf("Invalid");
            break;
        case EV_CURRENT:
            printf("Current");
            break;
        default:
            exit(-1);
    }
    printf(")\n");

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
        freadEndian(&byte, sizeof(unsigned char), 1, elfFile);

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
        freadEndian(&currentChar, sizeof(char), 1, elfFile);

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
        freadEndian(&c, sizeof(char), 1, elfFile);
        printf("%c", c);
    } while (c != '\0');
}

Elf32_ShdrTable ExtractSectionTable(FILE *elfFile, Elf32_Ehdr header)
{
    Elf32_ShdrTable sectionTable = mallocArray(Elf32_Shdr, header.e_shnum);
    fseek(elfFile, header.e_shoff, SEEK_SET);
    // Lecture de la table des sections
    for (Elf32_Half sectionIndex = 0; sectionIndex < header.e_shnum; sectionIndex++)
    {
        // Lire le nom de la section
        freadEndian(&sectionTable[sectionIndex].sh_name, sizeof(Elf32_Word), 1, elfFile);

        // Lire le type de la section
        freadEndian(&sectionTable[sectionIndex].sh_type, sizeof(Elf32_Word), 1, elfFile);

        // Lire le type de la section
        freadEndian(&sectionTable[sectionIndex].sh_flags, sizeof(Elf32_Word), 1, elfFile);

        // Lire l'adresse de la section
        freadEndian(&sectionTable[sectionIndex].sh_addr, sizeof(Elf32_Addr), 1, elfFile);

        // Lire la position de la section
        freadEndian(&sectionTable[sectionIndex].sh_offset, sizeof(Elf32_Word), 1, elfFile);

        // Lire la taille de la section
        freadEndian(&sectionTable[sectionIndex].sh_size, sizeof(Elf32_Word), 1, elfFile);

        // Lire l'indice de la table des en-têtes de sections
        freadEndian(&sectionTable[sectionIndex].sh_link, sizeof(Elf32_Word), 1, elfFile);

        // Lire les informations supplémentaires
        freadEndian(&sectionTable[sectionIndex].sh_info, sizeof(Elf32_Word), 1, elfFile);

        // Lire la taille de l'alignement
        freadEndian(&sectionTable[sectionIndex].sh_addralign, sizeof(Elf32_Word), 1, elfFile);

        // Lire la taille de l'entrée
        freadEndian(&sectionTable[sectionIndex].sh_entsize, sizeof(Elf32_Word), 1, elfFile);
    }

    return sectionTable;
}

void ShowSectionTableAndDetails(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable)
{
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







Elf32_SymTable ExtractSymbolsTable(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable)
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

        fseek(elfFile, section.sh_offset, SEEK_SET);

        for (Elf32_Half symbolIndex = 0; symbolIndex < count; symbolIndex++)
        {
            freadEndian(&symbolTable[symbolIndex].st_name, sizeof(Elf32_Word), 1, elfFile);
            freadEndian(&symbolTable[symbolIndex].st_value, sizeof(Elf32_Addr), 1, elfFile);
            freadEndian(&symbolTable[symbolIndex].st_size, sizeof(Elf32_Word), 1, elfFile);
            freadEndian(&symbolTable[symbolIndex].st_info, sizeof(unsigned char), 1, elfFile);
            freadEndian(&symbolTable[symbolIndex].st_other, sizeof(unsigned char), 1, elfFile);
            freadEndian(&symbolTable[symbolIndex].st_shndx, sizeof(Elf32_Section), 1, elfFile);
        }
    }

    return symbolTable;
}

void
ShowSymbolsTableAndDetails(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable, Elf32_SymTable symbolTable)
{
    Elf32_Half symTableSize = GetEntryCountFromType(header, sectionTable, SHT_SYMTAB);
    // affichage des symboles

    Elf32_Shdr strtab = sectionTable[GetSectionIndexByName(elfFile, sectionTable, header, ".strtab")];
    Elf32_Shdr strndx = sectionTable[header.e_shstrndx];

    for (Elf32_Half symbolIndex = 0; symbolIndex < symTableSize; symbolIndex++)
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
            freadEndian(&c, sizeof(char), 1, elfFile);
            printf("%c", c);
        }
        printf("\n\n");
    }


}

Elf32_RelTable ExtractReimplantationTable(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable,
                                          Elf32_SymTable symbolTable)
{
    uint32_t reimTableSize = GetEntryCountFromType(header, sectionTable, SHT_REL);
    Elf32_Rel *reimplantationTable = mallocArray(Elf32_Rel, reimTableSize);
    Elf32_Half symbolIndex = 0;
    for (Elf32_Half tableIndex = 0; tableIndex < header.e_shnum; tableIndex++)
    {
        if (sectionTable[tableIndex].sh_type != SHT_REL)
        {
            continue;
        }

        fseek(elfFile, sectionTable[tableIndex].sh_offset, SEEK_SET);

        // Elf32_Half symbolIndexOffset = symbolIndex;
        Elf32_Half symbolsInTable = sectionTable[tableIndex].sh_size / sectionTable[tableIndex].sh_entsize;

        for (Elf32_Half i = 0; i < symbolsInTable; i++)
        {
            freadEndian(&reimplantationTable[symbolIndex].r_offset, sizeof(Elf32_Addr), 1, elfFile);
            freadEndian(&reimplantationTable[symbolIndex].r_info, sizeof(Elf32_Word), 1, elfFile);
            symbolIndex++;
        }
    }

    return reimplantationTable;
}

void ShowReimplantationTablesAndDetails(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable,
                                        Elf32_SymTable symbolTable, Elf32_RelTable reimplantationTable)
{
    uint32_t reimTableSize = GetEntryCountFromType(header, sectionTable, SHT_REL);

    Elf32_Shdr strtab = sectionTable[GetSectionIndexByName(elfFile, sectionTable, header, ".strtab")];
    Elf32_Shdr strndx = sectionTable[header.e_shstrndx];

    Elf32_Half symbolIndex = 0;

    for (Elf32_Half i = 0; i < reimTableSize; i++)
    {
        printf("Reimplantation %d\n", symbolIndex);

        printf("  Offset: \t0x%08x\n", reimplantationTable[symbolIndex].r_offset);
        printf("  Info: \t0x%08x\n", reimplantationTable[symbolIndex].r_info);

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

Elf32_Structure ExtractElfInformation(FILE *elfFile, char *path)
{
    Elf32_Ehdr header = ExtractHeader(elfFile);
    Elf32_ShdrTable sectionTable = ExtractSectionTable(elfFile, header);
    Elf32_SymTable symbolTable = ExtractSymbolsTable(elfFile, header, sectionTable);
    Elf32_RelTable reimplatationTable = ExtractReimplantationTable(elfFile, header, sectionTable, symbolTable);

    Elf32_Structure structure = NewElf32_Structure(path, header, sectionTable, symbolTable, reimplatationTable);

    return structure;
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

#define LoopOnEachArgs(action) for (int i = 1; i < argc; i++) \
{                                                             \
    action                                                    \
}

Elf32_SectionFusion NewSectionFusion(Elf32_Word sectionSize1, Elf32_Word sectionSize2)
{
    Elf32_Word *newIndices = mallocArray(Elf32_Word, sectionSize2);
    for (int i = 0; i < sectionSize2; i++)
    {
        newIndices[i] = i;
    }

    Elf32_Off *concatenationOffset = mallocArray(Elf32_Off, sectionSize1);
    for (int i = 0; i < sectionSize1; i++)
    {
        concatenationOffset[i] = -1;
    }


    Elf32_SectionFusion sf = {newIndices, concatenationOffset, tmpnam(NULL)};
    return sf;
}

Elf32_SectionFusion FusionSections(FILE **elfFiles, Elf32_Ehdr *elfHeaders, Elf32_ShdrTable *sectionTables)
{
    //create the section merge structure
    Elf32_SectionFusion fu = NewSectionFusion(elfHeaders[0].e_shnum, elfHeaders[1].e_shnum);
    // open the tmp files with the name elfileW to do the merge in it
    FILE *elfFileW = fopen(fu.tmpFile, "w");
    // table to know which index is merged in the second file
    Elf32_Word *mergedindex = mallocArray(Elf32_Word, elfHeaders[1].e_shnum);
    for (int i = 0; i < elfHeaders[1].e_shnum; i++)
    {
        mergedindex[i] = -1;
    }
    // number of section of the second file (to calculate the new number of section of the second file after the merge)
    Elf32_Half numbersection2 = elfHeaders[1].e_shnum;
    // number of sections in tmp file (to calculate the new index of the second file)
    Elf32_Half numbersectiontmp = 0;
    //compare sections
    for (Elf32_Half i = 0; i < elfHeaders[0].e_shnum; i++)
    {
        int sectioncreated = 0;
        for (Elf32_Half j = 0; j < elfHeaders[1].e_shnum; j++)
        {
            int fusion;
            // compare if the two sections have the same type PROGBITS
            if (sectionTables[0][i].sh_type == SHT_PROGBITS && sectionTables[1][j].sh_type == SHT_PROGBITS)
            {
                fusion = 1;
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
                        fusion = 0;
                        break;
                    }
                } while (c1 != '\0' || c2 != '\0');
            }
            else
            {
                // no merge fusion stay 0
                fusion = 0;
            }
            // fseek on the sections
            fseek(elfFiles[0], sectionTables[0][i].sh_offset, SEEK_SET);
            fseek(elfFiles[1], sectionTables[1][j].sh_offset, SEEK_SET);
            // merge
            if (fusion)
            {
                //section created
                sectioncreated = 1;
                // the index of the section merged in the second file turn to 1
                mergedindex[j] = 1;
                // the number of sections in the second file is missing one
                numbersection2--;
                //we write the section from the first file to the tmp file
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
                // we get the size of the section from the first file to calculate the offset of the merged section
                Elf32_Word sectionsize = sectionTables[0][i].sh_size;
                // we add it to the structure
                fu.concatenationOffset[i] = sectionsize;
                // the number of section in the tmp file increased by 1
                numbersectiontmp++;
                break;
            }
        }
        // if the section is not merged we create it here
        if (sectioncreated == 0)
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
    for (Elf32_Half i = 0; i < numbersection2; i++)
    {
        if (mergedindex[i] == -1)
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
    fclose(elfFileW);
    return fu;
}




Elf32_SectionFusion NewSectionFusion(Elf32_Word sectionSize1, Elf32_Word sectionSize2){
    Elf32_Word *newIndices = mallocArray(Elf32_Word, sectionSize2);
    for (int i = 0; i < sectionSize2; i++)
    {
        newIndices[i] = i;
    }

    Elf32_Off *concatenationOffset = mallocArray(Elf32_Off, sectionSize1);
    for (int i = 0; i < sectionSize1; i++)
    {
        concatenationOffset[i] = -1;
    }


    Elf32_SectionFusion sf = {newIndices, concatenationOffset, tmpnam(NULL)};
    return sf;
}
Elf32_SectionFusion FusionSections(FILE **elfFiles, Elf32_Ehdr *elfHeaders, Elf32_ShdrTable *sectionTables){
    //create the section merge structure
    Elf32_SectionFusion fu = NewSectionFusion(elfHeaders[0].e_shnum,elfHeaders[1].e_shnum);
    // open the tmp files with the name elfileW to do the merge in it
    FILE *elfFileW = fopen(fu.tmpFile,"w");
    // table to know which index is merged in the second file
    Elf32_Word *mergedindex = mallocArray(Elf32_Word, elfHeaders[1].e_shnum);
    for (int i = 0; i < elfHeaders[1].e_shnum; i++)
    {
        mergedindex[i] = -1;
    }
    // number of section of the second file (to calculate the new number of section of the second file after the merge)
    Elf32_Half numbersection2=elfHeaders[1].e_shnum;
    // number of sections in tmp file (to calculate the new index of the second file)
    Elf32_Half numbersectiontmp=0;
    //compare sections
    for(Elf32_Half i=0; i<elfHeaders[0].e_shnum; i++) {
        int sectioncreated =0;
        for (Elf32_Half j = 0; j < elfHeaders[1].e_shnum; j++) {
            int fusion;
            // compare if the two sections have the same type PROGBITS
            if ( sectionTables[0][i].sh_type==SHT_PROGBITS && sectionTables[1][j].sh_type == SHT_PROGBITS) {
                fusion = 1;
            // create our table of string
                Elf32_Shdr strtab[2];
                strtab[0] = sectionTables[0][elfHeaders[0].e_shstrndx];
                strtab[1] = sectionTables[1][elfHeaders[1].e_shstrndx];
            // get the name of each section by adding the offset of name to the offset of the string table
                fseek(elfFiles[0], strtab[0].sh_offset + sectionTables[0][i].sh_name, SEEK_SET);
                fseek(elfFiles[1], strtab[1].sh_offset + sectionTables[1][j].sh_name, SEEK_SET);
            // if they have the same type we check if they have the same name
                char c1, c2;
                do {
                    freadEndian(&c1, sizeof(char), 1, elfFiles[0]);
                    freadEndian(&c2, sizeof(char), 1, elfFiles[1]);
                    if (c1 != c2) {
                        fusion = 0;
                        break;
                    }
                } while (c1 != '\0' || c2 != '\0');
            }
            else
            {
                // no merge fusion stay 0
                fusion = 0;
            }
            // fseek on the sections
            fseek(elfFiles[0], sectionTables[0][i].sh_offset, SEEK_SET);
            fseek(elfFiles[1], sectionTables[1][j].sh_offset, SEEK_SET);
            // merge
            if (fusion)
            {
                //section created
                sectioncreated=1;
                // the index of the section merged in the second file turn to 1
                mergedindex[j]=1;
                // the number of sections in the second file is missing one
                numbersection2--;
                //we write the section from the first file to the tmp file
                char c;
                int nb=0;
                do {
                    freadEndian(&c, sizeof(char), 1, elfFiles[0]);
                    fwrite(&c, sizeof(char), 1, elfFileW);
                    nb++;
                } while (nb<sectionTables[0][i].sh_size);

                // we write the section from the second file to the tmp file
                nb = 0;
                do {
                    freadEndian(&c, sizeof(char), 1, elfFiles[1]);
                    fwrite(&c, sizeof(char), 1, elfFileW);
                    nb++;
                } while (nb<sectionTables[1][j].sh_size);
                // we get the size of the section from the first file to calculate the offset of the merged section
                Elf32_Word sectionsize = sectionTables[0][i].sh_size;
                // we add it to the structure
                fu.concatenationOffset[i] = sectionsize;
                // the number of section in the tmp file increased by 1
                numbersectiontmp++;
                break;
            }
        }
        // if the section is not merged we create it here
        if(sectioncreated==0){
            // we write the section from the first file to tmp file
            char c;
            int nb=0;
            do {
                freadEndian(&c, sizeof(char), 1, elfFiles[0]);
                fwrite(&c, sizeof(char), 1, elfFileW);
                nb++;
            } while (nb < sectionTables[0][i].sh_size);
            // the number of the sections in tmp increased
            numbersectiontmp++;
        }

    }
    // add the Sections not merged of the second file
    for(Elf32_Half i=0; i<numbersection2; i++)
    {
        if(mergedindex[i] == -1)
        {
            // the new index of the sections
            fu.newIndices[i]=numbersectiontmp;
            // number sections in tmp file increased
            numbersectiontmp++;
            fseek(elfFiles[1], sectionTables[1][i].sh_offset, SEEK_SET);
            char c;
            int nb = 0;
            do {
                freadEndian(&c, sizeof(char), 1, elfFiles[1]);
                fwrite(&c, sizeof(char), 1, elfFileW);
                nb++;
            } while (nb < sectionTables[1][i].sh_size);
        }
    }
    fclose(elfFileW);
    return fu;
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


Elf32_SymbolFusion FusionSymbols(FILE **elfFile, Elf32_Ehdr *elfHeaders, Elf32_ShdrTable *sectionTable, Elf32_SectionFusion sectionFusion, Elf32_SymTable *symbolTables){
    Elf32_Half symTableSize1 = GetEntryCountFromType(elfHeaders[0], sectionTable[0], SHT_SYMTAB);
    Elf32_Half symTableSize2 = GetEntryCountFromType(elfHeaders[1], sectionTable[1], SHT_SYMTAB);
    Elf32_SymTable fusionTable=mallocArray(Elf32_Sym, symTableSize1 + symTableSize2);
    int i;
    int j;
    Elf32_SymbolFusion TableSymFusion;
    char *strtab;
    char *name;
    for(i=0; i<2; i++){
        for(j=0; j<elfHeaders[i].e_shnum; j++){
          
             Elf32_Half count = sectionTable[i][j].sh_size / sectionTable[i][j].sh_entsize;
             fseek(elfFile, sectionTable[i][j].sh_offset, SEEK_SET);
            //Parcourir les symboles de la section
            for (Elf32_Half k = 0; k < count; k++){

                //Si le symbole est local
                if(ELF32_ST_BIND(symbolTables[i][k].st_info) == STB_LOCAL){

                    fusionTable[k].st_name = symbolTables[i][k].st_name;
                    if(i)
                        fusionTable[k].st_value = symbolTables[i][k].st_value + elfHeaders[0].e_shnum;
                    else
                        fusionTable[k].st_value = symbolTables[i][k].st_value;
            
                    fusionTable[k].st_size = symbolTables[i][k].st_size;
                    fusionTable[k].st_info = symbolTables[i][k].st_info;
                    fusionTable[k].st_other = symbolTables[i][k].st_other;
                    fusionTable[k].st_shndx = symbolTables[i][k].st_shndx;
                    //Ajouter le nom du symbole a strtab
                   
                } 
                //Si le symbole est global
                else if(ELF32_ST_BIND(symbolTables[i][k].st_info) == STB_GLOBAL){
            
                    if(isGlobalSymbol1InTable2(symbolTables[2] , sectionTable[2],elfHeaders ,elfFile[2])){
                        //Si les noms sont identiques, on affiche un message d'erreur et on quitte le programme
                         printf("Erreur de l'édition de lien: il est interdit que pour 2 fichiers différents que deux symboles globlaux aient le meme nom ");
                        exit(1);
                    }
                


                /* si un symbole défini dans l’une des deux tables apparaıt comme non défini dans l’autre, 
                seule la définition devra apparaitre dans la table de sortie*/

                if (symbolTables[0][k].st_shndx == SHN_UNDEF && symbolTables[1][k].st_shndx != SHN_UNDEF){
                    
                        fusionTable[k].st_name = symbolTables[1][k].st_name;
                        fusionTable[k].st_value = symbolTables[1][k].st_value + elfHeaders[0].e_shnum;;
                        fusionTable[k].st_size = symbolTables[1][k].st_size;
                        fusionTable[k].st_info = symbolTables[1][k].st_info;
                        fusionTable[k].st_other = symbolTables[1][k].st_other;
                        fusionTable[k].st_shndx = symbolTables[1][k].st_shndx;
                       
                }
                else if (symbolTables[0][j].st_shndx != SHN_UNDEF && symbolTables[1][j].st_shndx == SHN_UNDEF){
                
                    fusionTable[j].st_name = symbolTables[0][j].st_name;
                    fusionTable[j].st_value = symbolTables[0][j].st_value; 
                    fusionTable[j].st_size = symbolTables[0][j].st_size;
                    fusionTable[j].st_info = symbolTables[0][j].st_info;
                    fusionTable[j].st_other = symbolTables[0][j].st_other;
                    fusionTable[j].st_shndx = symbolTables[0][j].st_shndx;
                }
                /*si deux symboles apparaissent comme non définis dans les deux tables en entrée, 
                une seule entrée pour ce symbole devra être présente dans la table de sortie*/
                if (symbolTables[0][j].st_shndx == SHN_UNDEF && symbolTables[1][j].st_shndx == SHN_UNDEF){
                    
                    fusionTable[k].st_name = symbolTables[0][k].st_name;
                    fusionTable[k].st_value = symbolTables[0][k].st_value;
                    fusionTable[k].st_size = symbolTables[0][k].st_size;
                    fusionTable[k].st_info = symbolTables[0][k].st_info;
                    fusionTable[k].st_other = symbolTables[0][k].st_other;
                    fusionTable[k].st_shndx = symbolTables[0][k].st_shndx;
                }

                /*Si un symbole n'apparait que dans l'une des deux tables en entrée,il  devra apparaitre dans 
                    la table de sortie 
                    */
                if  (!(isGlobalSymbolInTable(symbolTables[2], sectionTable[2], elfHeaders[2],elfFile[2]) )){
                    fusionTable[k].st_name = symbolTables[0][k].st_name;
                    fusionTable[k].st_value = symbolTables[0][k].st_value; 
                    fusionTable[k].st_size = symbolTables[0][k].st_size;
                    fusionTable[k].st_info = symbolTables[0][k].st_info;
                    fusionTable[k].st_other = symbolTables[0][k].st_other;
                    fusionTable[k].st_shndx = symbolTables[0][k].st_shndx;
                }
                else if (!(isGlobalSymbolInTable(symbolTables[2], sectionTable[2], elfHeaders[2],elfFile[2]) )){
                    fusionTable[k].st_name = symbolTables[1][k].st_name;
                    fusionTable[k].st_value = symbolTables[1][k].st_value;
                    fusionTable[k].st_size = symbolTables[1][k].st_size;
                    fusionTable[k].st_info = symbolTables[1][k].st_info;
                    fusionTable[k].st_other = symbolTables[1][k].st_other;
                    fusionTable[k].st_shndx = symbolTables[1][k].st_shndx;
                }
            }
            name=getSymbolName(elfFile[i], sectionTable[i],symbolTables[i], k,elfFile[i]);
            if(strstr(strtab,name)!=NULL){
                //On concaténe name à strtab
                strcat(strtab,name);
                name=strcat(strtab," ");
            }
            }
        }
    }
    TableSymFusion.symbolTable=fusionTable;
    TableSymFusion.strtab=strtab;
    return TableSymFusion;
}





//Ecrire une fonction qui permet de vérifier si un symbole global associé a une table de symbole 1 est dans une table de symbole 2
int isGlobalSymbol1InTable2(Elf32_SymTable tableSym[2] , Elf32_ShdrTable sectionTable[2],Elf32_Ehdr header[2],FILE *elfFile[2]){
    int i;
    Elf32_Half nbsym0 = sectionTable[0][0].sh_size / sectionTable[0][0].sh_entsize;
    Elf32_Half nbsym1 = sectionTable[1][1].sh_size / sectionTable[1][1].sh_entsize;
    for(i=0; i< nbsym0; i++){
        if(ELF32_ST_BIND(tableSym[0][i].st_info) == STB_GLOBAL){
            int j; 
            for(j=0; j< nbsym1; j++){
                if(ELF32_ST_BIND(tableSym[1][j].st_info) == STB_GLOBAL){
                   //Comparer le nom des symboles.Pour obtenir le noms des symboles,utiliser la fonction GetSymbolName
                   if(strcmp( GetSymbolName(header[0], sectionTable[0], tableSym[0],i,elfFile[0]), GetSymbolName(header[1], sectionTable[1], tableSym[1],j,elfFile[1]) ) == 0)
                        return 1;   
                }
            }
        }
    }
    return 0;
}

void DoFusionCommand(FILE **elfFiles, Elf32_Ehdr *elfHeaders, Elf32_ShdrTable *sectionTables)
{
    Elf32_SectionFusion fusion = FusionSections(elfFiles, elfHeaders, sectionTables);
    printf("%s\n", fusion.tmpFile);
    
}

void DoFusionSymbolCommand(FILE **elfFiles, Elf32_Ehdr *elfHeaders, Elf32_ShdrTable *sectionTables,Elf32_SymTable *symbolTables){
    Elf32_SymbolFusion fusion = FusionSymbols(elfFiles,elfHeaders,sectionTables,symbolTables );

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
                                                                  structureElfs[i - 1].header,
                                                                  structureElfs[i - 1].sectionTable,
                                                                  structureElfs[i - 1].symbolTable,
                                                                  structureElfs[i - 1].reimplantationTable);)
                printf("\n");
                break;
            case 'f':
                elfFiles[0] = fopen(argv[1], "r");
                elfFiles[1] = fopen(argv[2], "r");
                Elf32_Ehdr headers[2];
                headers[0] = structureElfs[0].header;
                headers[1] = structureElfs[1].header;
                Elf32_ShdrTable sectiontables[2];
                sectiontables[0] = structureElfs[0].sectionTable;
                sectiontables[1] = structureElfs[1].sectionTable;

                DoFusionCommand(elfFiles, headers, sectiontables);

                fclose(elfFiles[0]);
                fclose(elfFiles[1]);
                break;
            case 'fs':
                elfFiles[0] = fopen(argv[1], "r");
                elfFiles[1] = fopen(argv[2], "r");
                Elf32_Ehdr headers[2];
                headers[0] = structureElfs[0].header;
                headers[1] = structureElfs[1].header;
                Elf32_ShdrTable sectiontables[2];
                sectiontables[0] = structureElfs[0].sectionTable;
                sectiontables[1] = structureElfs[1].sectionTable;
                Elf32_ShdrTable symbolTables[2];
                symbolTables[0] = structureElfs[0]. symbolTable;
                symbolTables[1] = structureElfs[1]. symbolTable;
                DoFusionSymbolCommand(elfFiles,headers,sectiontables,symbolTables);
                
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