//
// Created by Pyrolink on 14/12/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include "elfFile.h"

Elf32_Ehdr ShowElfHeader(FILE *elfFile){
    Elf32_Ehdr header;
    fread(&header.e_ident,sizeof(unsigned char),EI_NIDENT,elfFile);
    fread(&header.e_type,sizeof (Elf32_Half),1,elfFile);
    fread(&header.e_machine,sizeof (Elf32_Half),1,elfFile);
    fread(&header.e_version,sizeof (Elf32_Word),1,elfFile);
    fread(&header.e_entry,sizeof (Elf32_Addr),1,elfFile);
    fread(&header.e_phoff,sizeof (Elf32_Off),1,elfFile);
    fread(&header.e_shoff,sizeof (Elf32_Off),1,elfFile);
    fread(&header.e_flags,sizeof (Elf32_Word),1,elfFile);
    fread(&header.e_ehsize,sizeof (Elf32_Half),1,elfFile);
    fread(&header.e_phentsize,sizeof (Elf32_Half),1,elfFile);
    fread(&header.e_phnum,sizeof (Elf32_Half),1,elfFile);
    fread(&header.e_shentsize,sizeof (Elf32_Half),1,elfFile);
    fread(&header.e_shnum,sizeof (Elf32_Half),1,elfFile);
    fread(&header.e_shstrndx,sizeof (Elf32_Half),1,elfFile);
    printf("En-tête ELF: \n");
    // afficher Magique
    printf("Magique : ");
    for(int i=0;i<EI_NIDENT;i++)
    {
        printf("%c " , header.e_ident[i]);
    }
    // afficher classe
    printf("\nClasse : \t");
    if(header.e_ident[4]==1)
    {
        printf("ELF32\n");
    }
    else if(header.e_ident[4]==2)
    {
        printf("ELF64\n");
    }
    else
    {
        exit(-1);
    }

    // afficher Data
    printf("\nClasse : \t");
    if(header.e_ident[5]==1)
    {
        printf("1's complement, Big endian\n");
    }
    else if(header.e_ident[5]==2)
    {
        printf("2's complement, little endian\n");
    }
    else
    {
        exit(-1);
    }


    // afficher version
    printf("Version: \t %d",header.e_ident[6]);

    // afficher OS/ABI
    printf("OS/ABI : \t");
    switch (header.e_machine)
    {
        case ELFOSABI_NONE:
            printf("UNIX System V ABI\n");
        case ELFOSABI_HPUX:
            printf("HP-UX\n");
        case ELFOSABI_NETBSD:
            printf("NetBSD\n");
        case ELFOSABI_GNU:
            printf("Object uses GNU ELF extensions\n");
        case ELFOSABI_SOLARIS:
            printf("Sun Solaris\n");
        case ELFOSABI_AIX:
            printf("IBM AIX\n");
        case ELFOSABI_IRIX:
            printf("SGI Irix\n");
        case ELFOSABI_FREEBSD:
            printf("FreeBSD\n");
        case ELFOSABI_TRU64:
            printf("Compaq TRU64 UNIX\n");
        case ELFOSABI_MODESTO:
            printf("Novell Modesto\n");
        case ELFOSABI_OPENBSD:
            printf("OpenBSD\n");
        case ELFOSABI_ARM_AEABI:
            printf("ARM EABI\n");
        case ELFOSABI_ARM:
            printf("ARM\n");
        case ELFOSABI_STANDALONE:
            printf("Standalone (embedded) application\n");
    }

    // afficher Version ABI
    printf("\nVersion ABI : \t");
    if(header.e_version==0)
    {
        printf("Aucune\n");
    }
    else if(header.e_version==1)
    {
        printf("%d\n", header.e_version);
    }

    //afficher type
    printf("type : \t");
    switch (header.e_type)
    {
        case ET_NONE:
            printf("No file type\n");
        case ET_REL:
            printf("Relocatable file\n");
        case ET_EXEC:
            printf("Executable file\n");
        case ET_DYN:
            printf("Shared object file\n");
        case ET_CORE:
            printf("Core file\n");
        case ET_NUM:
            printf("Fichier Core\n");
    }

    //afficher machine
    printf("machine : \t");
    switch (header.e_machine)
    {
        case EM_NONE:
            printf("No machine\n");
        case EM_M32:
            printf("AT&T WE 32100\n");
        case EM_SPARC:
            printf("SUN SPARC\n");
        case EM_386:
            printf("Intel 80386\n");
        case EM_68K:
            printf("Motorola m68k family\n");
        case EM_88K:
            printf("Motorola m88k family\n");
        case EM_IAMCU:
            printf("Intel MCU\n");
        case EM_860:
            printf("Intel 80860\n");
        case EM_MIPS:
            printf("MIPS R3000 big-endian\n");
        case EM_BPF:
            printf("Linux BPF -- in-kernel virtual machine\n");
    }

    // afficher Version
    printf("Version ABI : \t");
    switch (header.e_version)
    {
        case EV_NONE:
            printf("Invalid ELF version\n");
        case EV_CURRENT:
            printf("%d \n",header.e_version);
    }

    // afficher Entry point address
    printf("Entry point address :  \t%d\n",header.e_entry);

    // afficher Start of program headers
    printf("Start of program headers :  \t%d\n",header.e_phoff);

    // afficher Start of section headers:
    printf("Start of section headers :  \t%d\n",header.e_phoff);

    // afficher Fanions
    printf("Fanions : %x (", header.e_flags);
    if((header.e_flags & EF_PARISC_TRAPNIL) == EF_PARISC_TRAPNIL)
    {
        printf("X, Trap nil pointer dereference");
    }
    else if((header.e_flags & EF_PARISC_EXT) == EF_PARISC_EXT)
    {
        printf("X, Program uses arch. extensions");
    }
    else if((header.e_flags & EF_PARISC_LSB) == EF_PARISC_LSB)
    {
        printf("X, Program expects little endian");
    }
    else if((header.e_flags & EF_PARISC_WIDE) == EF_PARISC_WIDE)
    {
        printf("X, Program expects wide mode.");
    }
    else if((header.e_flags & EF_PARISC_NO_KABP) == EF_PARISC_NO_KABP)
    {
        printf("X, No kernel assisted branch");
    }
    else if((header.e_flags & EF_PARISC_LAZYSWAP) == EF_PARISC_LAZYSWAP)
    {
        printf("X, Allow lazy swapping");
    }
    else if((header.e_flags & EF_PARISC_ARCH) == EF_PARISC_ARCH)
    {
        printf("X, Architecture version , ");
        if((header.e_flags & EFA_PARISC_1_0) == EFA_PARISC_1_0)
        {
            printf("PA-RISC 1.0 big-endian");
        }
        else if((header.e_flags & EFA_PARISC_1_1) == EFA_PARISC_1_1)
        {
            printf("PA-RISC 1.1 big-endian");
        }
        else if((header.e_flags & EFA_PARISC_2_0) == EFA_PARISC_2_0)
        {
            printf("PA-RISC 2.0 big-endian");
        }
    }
    printf(" )\n");

    // afficher Size of this header
    printf("Size of this header:  \t%d\n",header.e_ehsize);

    //afficher Size of program headers
    printf("Size of program headers:  \t%d\n",header.e_phentsize);

    // afficher Number of program headers
    printf("Number of program headers: \t%d\n",header.e_phnum);

    // afficher Size of section headers
    printf("Size of section headers:  \t%d\n",header.e_shentsize);

    // afficher Nombre of section header:
    printf("Nombre of section header: \t%d\n",header.e_shnum);

    // afficher Section header string table index
    printf("Section header string table index : \t%d",header.e_shstrndx);

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
        fprintf(stdin, "%x", byte);
        if ((i + 1) % 4 == 0)
        {
            fprintf(stdin, " ");
        }
    }
}

void ShowSectionFromName(FILE *elfFile, Elf32_Shdr *table, Elf32_Ehdr header, Elf32_Word name)
{
    for (int i = 0; i < header.e_shentsize; i++)
    {
        if (table[i].sh_name == name)
        {
            ShowSectionFromIndex(elfFile, table, i);
            return;
        }
    }
}

void BackToBegin(FILE *file)
{
    fseek(file, 0, SEEK_SET);
}

int main(int argc, char *argv[])
{
    FILE        *elfFile;
    Elf32_Ehdr  header;
    Elf32_Shdr  *sectionTable;
    Elf32_Sym   *symbolTable;
    Elf32_Rel   *reimplantationTable;

    elfFile = fopen(argv[1], "r");

    header = ShowElfHeader(elfFile);
    BackToBegin(elfFile);
    sectionTable = ShowSectionTableAndDetails(elfFile, header);
    BackToBegin(elfFile);
    ShowSectionFromIndex(elfFile, sectionTable, 0);
    symbolTable = ShowSymbolsTableAndDetails(elfFile, header);
    BackToBegin(elfFile);
    reimplantationTable = ShowReimplantationTablesAndDetails(elfFile, header);

    (void) reimplantationTable;
    (void) symbolTable;

    fclose(elfFile);
    return 0;
}