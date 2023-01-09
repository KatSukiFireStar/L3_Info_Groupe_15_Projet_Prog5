//
// Created by Pyrolink on 07/01/2023.
//
#include <elf.h>
#include "utils.h"

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
                break;
            }
            case 4:
            {
                uint32_t *intPtr = (uint32_t *) ptr;
                intPtr += i;
                *intPtr = ((((*intPtr) & 0xFF) << 24) | ((((*intPtr) >> 8) & 0xFF) << 16) |
                           ((((*intPtr) >> 16) & 0xFF) << 8) | (((*intPtr) >> 24) & 0xFF));
                break;
            }
        }
    }

    return result;
}

#pragma endregion

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

Elf32_Half GetSectionCountFromType(Elf32_Ehdr header, Elf32_ShdrTable sectionTable, Elf32_Half type)
{
    Elf32_Half count = 0;
    for (Elf32_Half i = 0; i < header.e_shnum; i++)
    {
        if (sectionTable[i].sh_type == type)
        {
            count++;
        }
    }
    return count;
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