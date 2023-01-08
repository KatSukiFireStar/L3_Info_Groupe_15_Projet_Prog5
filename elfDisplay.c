//
// Created by Pyrolink on 07/01/2023.
//
#include <stdlib.h>
#include <stdio.h>

#include "elf.h"
#include "elfFile.h"
#include "utils.h"

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
        printf("  Section address: \t\t\t0x%08x\n", sectionTable[sectionIndex].sh_addr);


        printf("  Section offset: \t\t\t0x%08x\n", sectionTable[sectionIndex].sh_offset);


        printf("  Section size: \t\t\t0x%08x\n", sectionTable[sectionIndex].sh_size);

        // La taille de l'entrée, pour les sections qui contiennent une table d'entrées de même taille.
        printf("  Entry size if section holds table: \t0x%08x\n", sectionTable[sectionIndex].sh_entsize);


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

void ShowSectionFromName(FILE *elfFile, Elf32_ShdrTable sectionTable, Elf32_Ehdr header, char *name)
{
    ShowSectionFromIndex(elfFile, sectionTable, GetSectionIndexByName(elfFile, sectionTable, header, name));
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

void ShowSymbolsTableAndDetails(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable,
                                Elf32_SymTable symbolTable)
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
            ShowStringFromIndex(elfFile, strndx, sectionTable[symbolTable[symbolIndex].st_shndx].sh_name);
        }
        else
        {
            ShowStringFromIndex(elfFile, strtab, symbolTable[symbolIndex].st_name);
        }

        printf("\n\n");
    }


}

void ShowReimplantationTablesAndDetails(FILE *elfFile, Elf32_Structure structure)
{
    Elf32_Shdr strtab = structure.sectionTable[GetSectionIndexByName(elfFile, structure.sectionTable, structure.header,
                                                                     ".strtab")];
    Elf32_Shdr strndx = structure.sectionTable[structure.header.e_shstrndx];

    for (int i = 0; i < structure.reimplantationCount; i++)
    {
        Elf32_Reim reim = structure.reimplantationTable[i];
        Elf32_Shdr section = structure.sectionTable[reim.section];
        Elf32_Half entityInTable = section.sh_size / section.sh_entsize;

        printf("Table %d de nom : ", i);
        ShowStringFromIndex(elfFile, strndx, section.sh_name);
        printf("\n");

        for (int j = 0; j < entityInTable; j++)
        {
            printf("  Offset: \t0x%08x\n", reim.reimplantation[j].r_offset);
            printf("  Info: \t0x%08x\n", reim.reimplantation[j].r_info);
            printf("  Type: \t%d\n", ELF32_R_TYPE(reim.reimplantation[j].r_info));

            Elf32_Sym sym = structure.symbolTable[ELF32_R_SYM(reim.reimplantation[j].r_info)];
            printf("  Symbol value: 0x%08x\n", sym.st_value);
            printf("  Symbol name: \t");
            unsigned char type = ELF32_ST_TYPE(sym.st_info);
            if (type == STT_SECTION)
            {
                ShowStringFromIndex(elfFile, strndx, structure.sectionTable[sym.st_shndx].sh_name);
            }
            else
            {
                ShowStringFromIndex(elfFile, strtab, sym.st_name);
            }
            printf("\n\n");
        }
    }
}