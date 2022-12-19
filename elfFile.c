//
// Created by Pyrolink on 14/12/2022.
//

#include <stdlib.h>
#include <unistd.h>
#include "elfFile.h"

Elf32_Ehdr ShowElfHeader(FILE *elfFile)
{
    Elf32_Ehdr header;
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
    // check ELF
    if (header.e_ident[EI_MAG0] != ELFMAG0 ||
        header.e_ident[EI_MAG1] != ELFMAG1 ||
        header.e_ident[EI_MAG2] != ELFMAG2 ||
        header.e_ident[EI_MAG3] != ELFMAG3)
    {
        exit(-1);
    }

    printf("ELF Header: \n");
    // afficher Magique
    printf("Magic: ");
    for (int i = 0; i < EI_NIDENT; i++)
    {
        printf("%02hhx ", header.e_ident[i]);
    }
    printf("\n");

    // afficher classe
    printf("  Class: \t");
    if (header.e_ident[EI_CLASS] == ELFCLASS32)
    {
        printf("ELF32");
    }
    else if (header.e_ident[EI_CLASS] == ELFCLASS64)
    {
        printf("ELF64");
    }
    else
    {
        exit(-1);
    }
    printf("\n");

    // afficher Data
    printf("  Data: \t");
    if (header.e_ident[EI_DATA] == ELFDATA2LSB)
    {
        printf("2's complement, little endian");
    }
    else if (header.e_ident[EI_DATA] == ELFDATA2MSB)
    {
        printf("2's complement, Big endian");
    }
    else
    {
        exit(-1);
    }
    printf("\n");

    // afficher version
    printf("  Version: \t %d", header.e_ident[EI_VERSION]);
    printf("\n");

    // afficher OS/ABI
    printf("  OS/ABI: \t");
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
    printf("  ABI Version: \t%d", header.e_ident[EI_ABIVERSION]);
    printf("\n");

    //afficher type
    printf("  Type: \t");
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
    printf("  Machine: \t");
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
            printf("Machine not implemented");
    }
    printf("\n");

    // afficher Version
    printf("  Version: \t");
    switch (header.e_version)
    {
        case EV_NONE:
            printf("Invalid ELF version");
            break;
        case EV_CURRENT:
            printf("0x%x ", header.e_version);
            break;
        default:
            exit(-1);
    }
    printf("\n");

    // afficher Entry point address
    printf("  Entry point address:  \t0x%x", header.e_entry);
    printf("\n");

    // afficher Start of program headers
    printf("  Start of program headers:  \t%d", header.e_phoff);
    printf("\n");

    // afficher Start of section headers:
    printf("  Start of section headers:  \t%d", header.e_shoff);
    printf("\n");

    // afficher Fanions
    printf("  Flags: 0x%x (", header.e_flags);
    if ((header.e_flags & EF_PARISC_TRAPNIL) == EF_PARISC_TRAPNIL)
    {
        printf(" Trap nil pointer dereference");
    }
    if ((header.e_flags & EF_PARISC_EXT) == EF_PARISC_EXT)
    {
        printf(" Program uses arch. extensions");
    }
    if ((header.e_flags & EF_PARISC_LSB) == EF_PARISC_LSB)
    {
        printf(" Program expects little endian");
    }
    if ((header.e_flags & EF_PARISC_WIDE) == EF_PARISC_WIDE)
    {
        printf(" Program expects wide mode.");
    }
    if ((header.e_flags & EF_PARISC_NO_KABP) == EF_PARISC_NO_KABP)
    {
        printf(" No kernel assisted branch");
    }
    if ((header.e_flags & EF_PARISC_LAZYSWAP) == EF_PARISC_LAZYSWAP)
    {
        printf(" Allow lazy swapping");
    }
    if ((header.e_flags & EF_PARISC_ARCH) == EF_PARISC_ARCH)
    {
        printf(" Architecture version , ");
        if ((header.e_flags & EFA_PARISC_1_0) == EFA_PARISC_1_0)
        {
            printf(" PA-RISC 1.0 big-endian");
        }
        if ((header.e_flags & EFA_PARISC_1_1) == EFA_PARISC_1_1)
        {
            printf(" PA-RISC 1.1 big-endian");
        }
        if ((header.e_flags & EFA_PARISC_2_0) == EFA_PARISC_2_0)
        {
            printf(" PA-RISC 2.0 big-endian");
        }
    }
    printf(" )\n");

    // afficher Size of this header
    printf("  Size of this header:  \t%d\n", header.e_ehsize);

    //afficher Size of program headers
    printf("  Size of program headers:  \t%d\n", header.e_phentsize);

    // afficher Number of program headers
    printf("  Number of program headers: \t%d\n", header.e_phnum);

    // afficher Size of section headers
    printf("  Size of section headers:  \t%d\n", header.e_shentsize);

    // afficher Nombre of section header:
    printf("  Number of section header: \t%d\n", header.e_shnum);

    // afficher Section header string table index
    printf("  Section header string table index: \t%d\n", header.e_shstrndx);

    return header;

}

void ShowSectionFromIndex(FILE *elfFile, Elf32_Shdr *table, int index)
{
    Elf32_Shdr section = table[index];

    fseek(elfFile, section.sh_offset, SEEK_SET);
    for (int i = 0; i < section.sh_size; i++)
    {
        unsigned char byte;
        fread(&byte, sizeof(byte), 1, elfFile);
        fprintf(stdout, "%02hhx", byte);
        if ((i + 1) % 4 == 0)
        {
            if ((i + 1) % 16 == 0)
            {
                fprintf(stdout, "\n");
            }
            else
            {
                fprintf(stdout, " ");
            }
        }
    }
}

void ShowSectionFromName(FILE *elfFile, Elf32_Shdr *table, Elf32_Ehdr header, unsigned char *name)
{
    Elf32_Shdr stringTable = table[header.e_shstrndx];

    fseek(elfFile, stringTable.sh_offset, SEEK_SET);

    int nameId = -1;

    int skip = 0;
    int currentIndex = 0;

    for (int i = 1; i < stringTable.sh_size; i++)
    {
        unsigned char currentChar;
        fscanf(elfFile, "%c", &currentChar);

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
                nameId = i - currentIndex;
                for (int j = 0; j < header.e_shentsize; j++)
                {
                    if (table[j].sh_name == nameId)
                    {
                        ShowSectionFromIndex(elfFile, table, j);
                        return;
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
        exit(-2);
    }
}

Elf32_Shdr *ShowSectionTableAndDetails(FILE *elfFile, Elf32_Ehdr header)
{
    Elf32_Shdr *sectionTable = malloc(sizeof(Elf32_Shdr) * header.e_shnum);
    fseek(elfFile, header.e_shoff, SEEK_SET);

    //Lecture de la table des sections
    for (int i = 0; i < header.e_shnum; i++)
    {
        //Lire le nom de la section
        fread(&sectionTable[i].sh_name, sizeof(Elf32_Word), 1, elfFile);

        //Lire le type de la section
        fread(&sectionTable[i].sh_type, sizeof(Elf32_Word), 1, elfFile);

        //Lire le type de la section
        fread(&sectionTable[i].sh_flags, sizeof(Elf32_Word), 1, elfFile);

        //Lire l'adresse de la section
        fread(&sectionTable[i].sh_addr, sizeof(Elf32_Addr), 1, elfFile);

        //Lire la position de la section
        fread(&sectionTable[i].sh_offset, sizeof(Elf32_Word), 1, elfFile);

        //Lire la taille de la section
        fread(&sectionTable[i].sh_size, sizeof(Elf32_Word), 1, elfFile);

        //Lire l'indice de la table des en-têtes de sections
        fread(&sectionTable[i].sh_link, sizeof(Elf32_Word), 1, elfFile);

        //Lire les informations supplémentaires
        fread(&sectionTable[i].sh_info, sizeof(Elf32_Word), 1, elfFile);

        //Lire la taille de l'alignement
        fread(&sectionTable[i].sh_addralign, sizeof(Elf32_Word), 1, elfFile);

        //Lire la taille de l'entrée
        fread(&sectionTable[i].sh_entsize, sizeof(Elf32_Word), 1, elfFile);
    }

    Elf32_Shdr stringTable = sectionTable[header.e_shstrndx];

    //Affichage des informations de la table des sections
    for (int i = 0; i < header.e_shnum; i++)
    {
        printf("Section %d\n", i);
        printf(" Section name: ");
        fseek(elfFile, stringTable.sh_offset + sectionTable[i].sh_name, SEEK_SET);

        char c = ' ';
        while (c != '\0')
        {
            fread(&c, sizeof(char), 1, elfFile);
            printf("%c", c);
        }

        printf("\n");

        printf(" Section type: %u (", sectionTable[i].sh_type);
        switch (sectionTable[i].sh_type)
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

        printf(")\n");

        //l'adresse à  laquelle le premier octet de la section doit se trouver.
        printf(" Section address: %x\n", sectionTable[i].sh_addr);


        printf(" Section offset: %x\n", sectionTable[i].sh_offset);


        printf(" Section size: %x\n", sectionTable[i].sh_size);


        //la taille de l'entrée, pour les sections qui contiennent une table d'entrées de même taille.
        printf(" Entry size if section holds table: %x\n",
               sectionTable[i].sh_entsize);


        printf(" Section flags: %x ( ", sectionTable[i].sh_flags);
        //Cette section contient des données qu'il devrait être possible d'écrire durant l'exécution du processus;
        if ((sectionTable[i].sh_flags & SHF_WRITE) == SHF_WRITE)
            printf("'Write' ");

        //La section fait partie de l'image mémoire du programme à exécuter.");
        if ((sectionTable[i].sh_flags & SHF_ALLOC) == SHF_ALLOC)
            printf("'Alloc' ");


        //La section contient du code exécutable.\n");
        if ((sectionTable[i].sh_flags & SHF_EXECINSTR) == SHF_EXECINSTR)
            printf("'Executable' ");

        //Tous les bits contenus dans ce masque sont réservés à des sémantiques spécifiques au processeur
        if ((sectionTable[i].sh_flags & SHF_MASKPROC) == SHF_MASKPROC)
            printf("'Processor-specific' ");

        printf(")\n");

        //lien vers un indice de la table des en-têtes de  sections,
        printf(" Link to another section: %d\n", sectionTable[i].sh_link);



        //informations supplémentaires, dépendant du type de section.
        printf(" Additional section information: %d\n", sectionTable[i].sh_info);


        //la taille de l'alignement, exprimée en puissance de 2.
        printf(" Section alignment (exponent of 2): %u\n", sectionTable[i].sh_addralign);
        printf("\n");
    }

    return sectionTable;
}

//int nbTableReimplantation(Elf32_Shdr *TableSection, Elf32_Ehdr header)
//{
//    int compteur = 0;
//    int i;
//    for (i = 0; i < header.e_shnum; i++)
//    {
//        if (TableSection[i].sh_type == SHT_REL)
//        {
//            compteur++;
//        }
//    }
//    return compteur;
//}
//
//Elf32_Rel *ShowReimplantationTablesAndDetails(FILE *elfFile, Elf32_Ehdr header)
//{
//    Elf32_Shdr *TableSection = malloc(header.e_shnum * sizeof(Elf32_Shdr));
//    Elf32_Rel *TableReimplantation = malloc(header.e_shnum * sizeof(Elf32_Rel));//e_shnum=nombre des entrées
//    int i;
//    //Pour chaque entrée de la table des sections
//    for (i = 0; i < header.e_shnum; i++)
//    {
//        //Si c'est une table de réimplantation
//        if (TableSection[i].sh_type == SHT_REL)
//        {
//            rewind(elfFile);//OU fseek(elfFile, sectionTable[i].sh_offset, SEEK_SET);
//            int nb = nbTableReimplantation(TableSection, header);
//            fread(TableReimplantation, sizeof(Elf32_Rel), nb, elfFile);
//            int j;
//            for (j = 0; j < nb; j++)
//            {
//                printf("La cible de la réimplantation est: %d\n", TableReimplantation[j].r_offset);
//                printf("Le type de réimplantation est: %d\n", TableReimplantation[j].r_info);
//                printf("L'index de l'entrée concernée dans la table des symboles est: %d\n",
//                       TableReimplantation[j].r_info >> 8);
//
//            }
//        }
//        return TableReimplantation;
//
//    }
//}


int main(int argc, char *argv[])
{
    FILE *elfFile;
    Elf32_Ehdr header[argc - 1];
    Elf32_Shdr *sectionTable[argc - 1];
//    Elf32_Sym *symbolTable[argc - 1];
//    Elf32_Rel *reimplantationTable[argc - 1];

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

        header[i - 1] = ShowElfHeader(elfFile);
        rewind(elfFile);
        printf("\n");
        sectionTable[i - 1] = ShowSectionTableAndDetails(elfFile, header[i - 1]);
        (void) sectionTable;
//        rewind(elfFile);
//        ShowSectionFromIndex(elfFile, sectionTable[i - 1], 0);
//        symbolTable[i - 1] = ShowSymbolsTableAndDetails(elfFile, header[i - 1]);
//        rewind(elfFile);
//        reimplantationTable[i - 1] = ShowReimplantationTablesAndDetails(elfFile, header[i - 1]);
//
//        (void) reimplantationTable;
//        (void) symbolTable;

        fclose(elfFile);
    }

    return 0;
}