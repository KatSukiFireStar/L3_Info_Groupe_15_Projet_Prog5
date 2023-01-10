# L3_Info_Groupe_15_Projet_Prog5

### La gestion de ce README a été effectué par Corenthin Zozor et Flavien Gonin.

## Mode d'emploi

Pour compiler le programme, il faut entrer l'une des commandes suivantes dans un terminal :

    make all 
    make elfFile

Ensuite pour le lancer, il faut utiliser la commande suivante :

    ./elfFile fichier1.o fichier2.o

Pour supprimer les fichiers objets ou les fichiers résiduels, il faut utiliser la commande suivante :

    make clean

Pour lancer les tests qui sont prévus avec le programme, il faut installer dotnet. Ensuite, il
faut se placer dans un terminal dans le fichier du projet. 
Pour terminer, il faut utiliser la commande suivante :
  
    dotnet test test/Test/Test.csproj

## Journal de travail

Le travail est fait par équipe donc les taches de travail sont données à l'équipe 
même si certains membres manquent à l'appel. Voici ci-après la liste d'appel :

### Feuille d'appel

| Nom / Date | Mhammad Hajj | Flavien Gonin | Corenthin Zozor | Diagne Babakar | Mohamed Sbartai | Mehdi Chedad            |
|------------|--------------|---------------|-----------------|----------------|-----------------|-------------------------|
| 15/12/2022 | Présentiel   | Présentiel    | Présentiel      | Présentiel     | **Absent**      | Présentiel              |
| 16/12/2022 | Présentiel   | Présentiel    | Présentiel      | Présentiel     | **Absent**      | **Absent** l'après-midi |
| 17/12/2022 | Présentiel   | Présentiel    | Présentiel      | Présentiel     | **Absent**      | Présentiel              |
| 19/12/2022 | _Distanciel_ | _Distanciel_  | _Distanciel_    | _Distanciel_   | **Absent**      | **Absent**              |
| 20/12/2022 | _Distanciel_ | _Distanciel_  | _Distanciel_    | _Distanciel_   | **Absent**      | **Absent**              |
| 03/01/2023 | Présentiel   | Présentiel    | Présentiel      | Présentiel     | _Distantiel_    | **Absent**              |
| 04/01/2023 | Présentiel   | Présentiel    | Présentiel      | Présentiel     | _Distantiel_    | **Absent**              |
| 05/01/2023 | Présentiel   | Présentiel    | Présentiel      | Présentiel     | _Distantiel_    | **Absent**              |
| 06/01/2023 | Présentiel   | Présentiel    | Présentiel      | Présentiel     | Présentiel      | Présentiel pendant 1h   |
| 09/01/2023 | Présentiel   | Présentiel    | Présentiel      | Présentiel     | Présentiel      | **Absent** le matin     |
| 10/01/2023 | _Distanciel_ | Présentiel    | Présentiel      | Présentiel     | Présentiel      | **Absent**              |
| 11/01/2023 |              |               |                 |                |                 |                         |

Mercredi 14 decembre : Mise en place de la methode de travail et 
initialisation des différents outils (Git / Trello / Fichier de code C)

Jeudi 15 decembre : Repartition des taches de travail comme suit :

- Medhi Chedad et Mhammad Hajj : Extraction et affichage de l'en-tête
- Babakar Diagne et Mohamed Sbartai : Extraction et affichage de la table des sections
- Corenthin Zozor et Flavien Gonin : Extraction et affichage du contenu d'une section +
Vérification et correction du code des autres parties

Vendredi 16 decembre : Chaque personne a effectué les mêmes taches 
que le jour précédent

Lundi 19 decembre : Repartition du travail comme suit :

- Medhi Chedad et Mhammad Hajj : Extraction et affichage de la table des symboles
- Babakar Diagne et Mohamed Sbartai: Extraction et affichage des tables de 
réimplantations
- Corenthin Zozor et Flavien Gonin: Correction du code des trois premieres étapes
et gestion des conflits entre les branches + merge des différentes branches sur la 
branche de dev

Mardi 20 decembre : Fin du travail sur la phase 1

- Medhi Chedad et Mhammad Hajj : Extraction et affichage de la table des symboles
- Babakar Diagne et Mohamed Sbartai : Extraction et affichage des tables de réimplantations
- Corenthin Zozor et Flavien Gonin : Correction du code et gestion des conflits 
entre les branches + merge des différentes branches sur la branche de dev + merge de 
la branche de dev sur le main à la fin de la phase 1 + Création des taches de travail
pour la phase 2 + Ajout de la documentation dans le fichier .h

Mardi 3 janvier :

- Medhi Chedad et Mhammad Hajj : Debut de comprehension sur la fusion des sections
- Babakar Diagne et Mohamed Sbartai : Debut de comprehension sur la fusion des symboles
- Corenthin Zozor et Flavien Gonin : Debut de comprehension sur la fusion des tables
de réimplantations + Repartition des taches de travail + Gestion du git + Ajout des 
vérifications pour le big/little endian

Mercredi 4 janvier :

- Medhi Chedad et Mhammad Hajj : Debut de la fonction pour la fusion des sections
- Babakar Diagne et Mohamed Sbartai : Debut de la fonction pour la fusion des symboles
- Corenthin Zozor et Flavien Gonin : Separation affichage et extraction des valeurs + ajout
des commandes au niveau applicatif + création des structures qui seront utilisés plus tard + 
correction du programme à la suite de l'audit de code + Création des tests pour la phase 1 + 
Redefinition de la documentation des fonctions dans le fichier elfFile.h + Correction du code 
sur la fusion des sections

Jeudi 5 janvier :

- Medhi Chedad et Mhammad Hajj : Travaille sur la fonction pour la fusion des sections
- Babakar Diagne et Mohamed Sbartai : Travaille sur la fonction pour la fusion des symboles
- Corenthin Zozor et Flavien Gonin : Correction du code pour la fusion des sections + ajout de 
la commande de fusion + Travaille sur les fonctions de tests + Recuperation fichier fusionné
donné par le prof + Changement de la structure Elf32_Structure

Vendredi 6 janvier :

- Medhi Chedad et Mhammad Hajj : Travaille sur la fonction pour la fusion des sections
- Babakar Diagne et Mohamed Sbartai : Travaille sur la fonction pour la fusion des symboles
- Corenthin Zozor et Flavien Gonin : Correction du code pour la fusion des sections + correction
de la commande de fusion + Changement de la structure Elf32_SectionFusion

Lundi 9 janvier :

- Medhi Chedad et Mhammad Hajj : Aide sur la fonction de fusion des symboles
- Babakar Diagne et Mohamed Sbartai : Travaille sur la fonction pour la fusion des symboles
- Corenthin Zozor et Flavien Gonin : Fin de la fonction de fusion des tables de reimplantation + correction du code pour
la fusion des symboles + ajout des fonctions de désallocations mémoire + tri des fonctions selon leurs utilités + 
correction des problèmes d'allocation et de libération de mémoire

Mardi 10 janvier :

- Medhi Chedad et Mhammad Hajj : Travail sur la presentation
- Babakar Diagne et Mohamed Sbartai : Travail sur la presentation
- Corenthin Zozor et Flavien Gonin : Fin des corrections des methodes de fusion + fin des corrections des problèmes de 
mémoire + Début création du fichier elf final

## Liste des fonctionnalités

- Test plusieurs conditions avant de lancer l'application
  - L'application test s'il y a bien au moins deux fichiers passés en paramètres
  - L'application test aussi si les fichiers peuvent être ouverts
- Extrait toutes les données des fichiers passées en paramètres
- Affichage des informations des fichiers passer en paramètres
- Fusionne et produit un nouveau fichier avec les fichiers en paramètres

## Liste des tests

- Test de comparaison des headers 
  - On compare notre header avec le header afficher par readelf
- Test de comparaison des tables de sections
  - On compare notre table des sections avec la table des sections par readelf
- Test de comparaison des tables de symboles
  - On compare notre table des symboles avec la table des symboles par readelf
- Test de comparaison des tables de reimplantation
  - On compare notre table de reimplantation avec la table de reimplantation de readelf

## Liste des bugs connus

