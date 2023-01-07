//
// Created by Corenthin Z. on 14/12/2022.
//

#ifndef PROJET_PROG_ELFFILE_H
#define PROJET_PROG_ELFFILE_H

/**
 * @file
 * Fichier d'en-tête principal. Contient les signatures des méthodes d'extractions et d'affichage
 */

#include <elf.h>
#include <stdio.h>

#include "elfStructure.h"

#pragma region Global variables

/**
 * Indique si les valeurs doivent être inversées lors d'une lecture ou d'une écriture
 * (fichier en big et machine en little ou inverse)
 */
extern int needReverse;

#pragma endregion

#pragma region Extract

/**
 * Extrait toutes les informations d'un fichier ELF et créer une structure ELF
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf
 * @param path Chemin d'acces au fichier ELF
 * @return Un objet de type ELF32_Structure
 */
Elf32_Structure ExtractElfInformation(FILE *elfFile, char *path);

/**
 * Extrait le header d'un fichier ELF
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf.
 * @return Le header de @p elfFile
 */
Elf32_Ehdr ExtractHeader(FILE *elfFile);

/**
 * Extrait la table des sections d'un fichier ELF
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf.
 * @param header Header du fichier
 * @return La table des sections de @p elfFile
 */
Elf32_ShdrTable ExtractSectionTable(FILE *elfFile, Elf32_Ehdr header);

/**
 * Extrait la tables des symboles d'un fichier ELF
 * @param elfFile Fichier elf
 * @param header Header du fichier @p elfFile
 * @param sectionTable Table des sections du fichier @p elfFile
 * @param symbolCount Paramètre out. La valeur de l'adresse prends le nombre de symboles trouvés
 * @return La table des symboles de @p elfFile
 */
Elf32_SymTable ExtractSymbolsTable(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable, int *symbolCount);

/**
 * Extrait la table de relocation d'un fichier ELF
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf
 * @param header Header du fichier @p elfFile
 * @param sectionTable Table des sections du fichier @p elfFile
 * @param symbolTable Table des symboles du fichier @p elfFile
 * @param reimplantationCount Paramètre out. La valeur à l'adresse prends le nombre de table de réimplémentations trouvées
 * @return Un tableau de Relocation correspondant à la table des sections
 */
Elf32_ReimTable ExtractReimplantationTable(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable,
                                           Elf32_SymTable symbolTable, int *reimplantationCount);

#pragma endregion

#pragma region Display

/**
 * Affiche le header d'un fichier ELF
 * @param header Header du fichier.
 */
void ShowElfHeader(Elf32_Ehdr header);

/**
 * Affiche la table des sections ELF et des détails relatifs à chaque section de @p elfFile
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf.
 * @param header Header du fichier
 * @param sectionTable Table des section du fichier à afficher
 */
void ShowSectionTableAndDetails(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable);

/**
 * Affiche le contenu d'une section elf
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf.
 * @param sectionTable Table des section du fichier @p elfFile
 * @param index Index de la section
 */
void ShowSectionFromIndex(FILE *elfFile, Elf32_ShdrTable sectionTable, Elf32_Word index);

/**
 * Affiche le contenu d'une section elf
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf.
 * @param sectionTable Table des section du fichier @p elfFile
 * @param header Header du fichier @p elfFile
 * @param name Nom de la section à afficher
 */
void ShowSectionFromName(FILE *elfFile, Elf32_ShdrTable sectionTable, Elf32_Ehdr header, char *name);

/**
 * Affiche la table des symboles ELF et des détails relatifs à chaque symboles de @p elfFile
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf
 * @param header Header du fichier @p elfFile
 * @param sectionTable Table des sections du fichier @p elfFile
 * @param symbolTable Table des symboles du fichier @p elfFile à afficher
 */
void ShowSymbolsTableAndDetails(FILE *elfFile, Elf32_Ehdr header, Elf32_ShdrTable sectionTable,
                                Elf32_SymTable symbolTable);

/**
 * Affiche les tables de réimplantation ELF et des détails relatifs à chaque entrée de @p elfFile
 * <br>
 * Précondition : @p elfFile doit être ouvert en mode lecture
 * @param elfFile Fichier elf
 * @param structure Structure du fichier @p elfFile
 */
void ShowReimplantationTablesAndDetails(FILE *elfFile, Elf32_Structure structure);

#pragma endregion

#pragma region Fusion

/**
 * Fusionne les 2 fichiers elf
 * <br>
 * Précondition :
 * <br>
 * - @p elfFiles doit être un tableau de 2 fichiers ouverts en lecture
 * <br>
 * - @p structure doit être un tableau de 2 structures
 * @param elfFiles Fichiers elf à fusionner
 * @param structure Structure des 2 fichiers elf
 */
void DoFusionCommand(FILE **elfFiles, Elf32_Structure *structure);

/**
 * Fusionne les tables de sections.
 * <br>
 * Précondition : chaque fichiers de @p elfFiles doit être ouvert
 * @param elfFiles Fichiers elf à fusionner
 * @param elfHeaders Header des 2 fichiers
 * @param sectionTables Table des sections des 2 fichiers elf
 * @return Résultat de la fusion
 */
Elf32_SectionFusion FusionSections(FILE **elfFiles, Elf32_Ehdr *elfHeaders,
                                   Elf32_ShdrTable *sectionTables);

/**
 * Fusionne les tables de symboles
 * <br>
 * Précondition : chaque fichiers de elfFiles doit être ouvert
 * @param elfFiles Fichiers elf à fusionner
 * @param elfHeaders Header des 2 fichiers
 * @param symbolTables Table de symbole des 2 fichier
 * @param sectionFusion Fusion des sections des 2 fichiers
 * @return Le résultat de la fusion
 */
Elf32_SymbolFusion FusionSymbols(FILE *elfFiles[2], Elf32_Ehdr elfHeaders[2], Elf32_SymTable symbolTables[2],
                                 Elf32_SectionFusion sectionFusion);

/**
 * Fusionne les tables de réimplémentation
 * <br>
 * Précondition : chaque fichiers de elfFiles doit être ouvert
 * @param elfFiles Fichiers elf à fusionner
 * @param structure Structure des 2 fichiers
 * @param sectionFusion Fusion des sections des 2 fichiers
 * @param symbolFusion Fusion des symboles des 2 fichiers
 * @return Le résultat de la fusion
 */
Elf32_RelFusion FusionReimplantation(FILE **elfFiles, Elf32_Structure *structure, Elf32_SectionFusion sectionFusion,
                                     Elf32_SymbolFusion symbolFusion);

/** Affiche l'aide du programme */
void help();

#pragma endregion

#endif //PROJET_PROG_ELFFILE_H
