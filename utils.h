//
// Created by Pyrolink on 06/01/2023.
//

#ifndef PROJET_PROG_UTILS_H
#define PROJET_PROG_UTILS_H

/**
 * Alloue un espace mémoire pour un tableau
 * <br>
 * Précondition :
 * <br>
 * - @p element doit être un type c
 * <br>
 * - @p size doit être positif et non nul
 * @param element Type des éléments du tableau
 * @param size Taille du tableau
 */
#define mallocArray(element, size) malloc(sizeof(element) * size)

#endif //PROJET_PROG_UTILS_H
