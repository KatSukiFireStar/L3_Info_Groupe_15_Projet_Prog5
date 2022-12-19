//
// Created by Pyrolink on 14/12/2022.
//

#include <stdlib.h>
#include "elfFile.h"

Elf32_Ehdr ShowElfHeader(FILE *elfFile) {
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
        header.e_ident[EI_MAG3] != ELFMAG3) {
        exit(-1);
    }

    printf("En-tête ELF: \n");
    // afficher Magique
    printf("Magic : ");
    for (int i = 0; i < EI_NIDENT; i++) {
        printf("%x ", header.e_ident[i]);
    }
    printf("\n");

    // afficher classe
    printf("Class : \t");
    if (header.e_ident[EI_CLASS] == ELFCLASS32) {
        printf("ELF32");
    } else if (header.e_ident[EI_CLASS] == ELFCLASS64) {
        printf("ELF64");
    } else {
        exit(-1);
    }
    printf("\n");

    // afficher Data
    printf("Data : \t");
    if (header.e_ident[EI_DATA] == ELFDATA2LSB) {
        printf("2's complement, little endian");
    } else if (header.e_ident[EI_DATA] == ELFDATA2MSB) {
        printf("2's complement, Big endian");
    } else {
        exit(-1);
    }
    printf("\n");

    // afficher version
    printf("Version: \t %d", header.e_ident[EI_VERSION]);
    printf("\n");

    // afficher OS/ABI
    printf("OS/ABI : \t");
    switch (header.e_ident[EI_OSABI]) {
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
    }
    printf("\n");

    // afficher Version ABI
    printf("ABI Version : \t%d",header.e_ident[EI_ABIVERSION]);
    printf("\n");

    //afficher type
    printf("Type : \t");
    switch (header.e_type) {
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
    }
    printf("\n");

    //afficher machine
    printf("Machine : \t");
    switch (header.e_machine) {
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
    }
    printf("\n");

    // afficher Version
    printf("Version : \t");
    switch (header.e_version) {
        case EV_NONE:
            printf("Invalid ELF version");
            break;
        case EV_CURRENT:
            printf("0x%x ", header.e_version);
            break;
    }
    printf("\n");

    // afficher Entry point address
    printf("Entry point address :  \t0x%x", header.e_entry);
    printf("\n");

    // afficher Start of program headers
    printf("Start of program headers :  \t%d", header.e_phoff);
    printf("\n");

    // afficher Start of section headers:
    printf("Start of section headers :  \t%d", header.e_shoff);
    printf("\n");

    // afficher Fanions
    printf("Flags : 0x%x (", header.e_flags);
    if ((header.e_flags & EF_PARISC_TRAPNIL) == EF_PARISC_TRAPNIL) {
        printf(" Trap nil pointer dereference");
    }  if ((header.e_flags & EF_PARISC_EXT) == EF_PARISC_EXT) {
        printf(" Program uses arch. extensions");
    }  if ((header.e_flags & EF_PARISC_LSB) == EF_PARISC_LSB) {
        printf(" Program expects little endian");
    }  if ((header.e_flags & EF_PARISC_WIDE) == EF_PARISC_WIDE) {
        printf(" Program expects wide mode.");
    }  if ((header.e_flags & EF_PARISC_NO_KABP) == EF_PARISC_NO_KABP) {
        printf( " No kernel assisted branch");
    }  if ((header.e_flags & EF_PARISC_LAZYSWAP) == EF_PARISC_LAZYSWAP) {
        printf(" Allow lazy swapping");
    }  if ((header.e_flags & EF_PARISC_ARCH) == EF_PARISC_ARCH) {
        printf(" Architecture version , ");
        if ((header.e_flags & EFA_PARISC_1_0) == EFA_PARISC_1_0) {
            printf(" PA-RISC 1.0 big-endian");
        }  if ((header.e_flags & EFA_PARISC_1_1) == EFA_PARISC_1_1) {
            printf(" PA-RISC 1.1 big-endian");
        }  if ((header.e_flags & EFA_PARISC_2_0) == EFA_PARISC_2_0) {
            printf(" PA-RISC 2.0 big-endian");
        }
    }
    printf(" )\n");

    // afficher Size of this header
    printf("Size of this header:  \t%d\n", header.e_ehsize);

    //afficher Size of program headers
    printf("Size of program headers:  \t%d\n", header.e_phentsize);

    // afficher Number of program headers
    printf("Number of program headers: \t%d\n", header.e_phnum);

    // afficher Size of section headers
    printf("Size of section headers:  \t%d\n", header.e_shentsize);

    // afficher Nombre of section header:
    printf("Number of section header: \t%d\n", header.e_shnum);

    // afficher Section header string table index
    printf("Section header string table index : \t%d\n", header.e_shstrndx);

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
        fprintf(stdout, "%x", byte);
        if ((i + 1) % 4 == 0)
        {
            fprintf(stdout, " ");
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
                break;
            }
            currentIndex++;
        }
        else
        {
            skip = 1;
            currentIndex = 0;
        }
    }

    if(nameId == -1)
    {
        exit(-2);
    }

    for (int i = 0; i < header.e_shentsize; i++)
    {
        if (table[i].sh_name == nameId)
        {
            ShowSectionFromIndex(elfFile, table, i);
            return;
        }
    }
}
int main(int argc, char *argv[])
{
    FILE *elfFile;
    Elf32_Ehdr header[argc - 1];
    Elf32_Shdr *sectionTable[argc - 1];
    Elf32_Sym *symbolTable[argc - 1];
    Elf32_Rel *reimplantationTable[argc - 1];

    //Permet de recuperer toutes les informations d'un fichier
    //et les stockes dans des variables
    //A la fin, le fichier est fermé et on ouvre le fichier suivant
    for (int i = 1; i < argc; i++)
    {
        elfFile = fopen(argv[i], "r");

        header[i - 1] = ShowElfHeader(elfFile);
        rewind(elfFile);
        sectionTable[i - 1] = ShowSectionTableAndDetails(elfFile, header[i - 1]);
        rewind(elfFile);
        ShowSectionFromIndex(elfFile, sectionTable[i - 1], 0);
        symbolTable[i - 1] = ShowSymbolsTableAndDetails(elfFile, header[i - 1]);
        rewind(elfFile);
        reimplantationTable[i - 1] = ShowReimplantationTablesAndDetails(elfFile, header[i - 1]);

        (void) reimplantationTable;
        (void) symbolTable;

        fclose(elfFile);
    }

    return 0;
}