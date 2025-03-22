## Simulation de Règne

Un jeu de simulation inspiré de Reigns où vous incarnez un roi qui doit gérer son royaume à travers des décisions stratégiques.
Chaque choix influence l'équilibre de quatre ressources essentielles : l'argent, la population, l'armée et la
religion.

À droite est le choix "OUI" et à gauche le choix "NON".
Vous pouvez choisir entre 2 histoires, elles contiennent toutes les 2 différents événements.

Pour faire votre choix, faites glisser la carte :

- Vers la droite pour répondre "OUI"
- Vers la gauche pour répondre "NON"

Chaque choix aura un impact différent sur vos ressources :

- L'argent du royaume
- La population
- L'armée
- La religion

Attention : si une ressource atteint 0 ou 100, votre règne prendra fin !

## Prérequis

- MSYS2 MINGW64
- Compilateur GCC
- Bibliothèques SDL
- SQLite

## Compilation

```shell
gcc -I./include -I/mingw64/include/SDL -I/mingw64/include src/*.c icon.res -o reigns -lmingw32 -lSDLmain -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -lSDL_gfx -lsqlite3
```

## Configuration

Le fichier config.txt permet de personnaliser plusieurs paramètres :

# Chemin vers les fichiers audio

music_path=assets/music/test_music/test1.mp3

# Dimensions de la fenêtre

window_width=800
window_height=600

# Paramètres audio

volume=100
sound_volume=100

## Caractéristiques du Jeu

## Mécaniques de Jeu

- Système de choix binaires pour chaque situation
- Gestion de jauges multiples à maintenir en équilibre
- Système de cartes d'événements
- Personnages uniques avec leurs propres événements bonus

## Bonus Spéciaux

## Bonus à durée limitée

- Bouclier : Protège contre les impacts négatifs en les ramenant à 10
- Équilibre : Divise par 2 l'impact négatif

## Bonus permanent

- Vision divine : Permet de visualiser les impacts avant de prendre une décision

## Système de Règne

- Compteur d'années de règne actuel
- Record personnel (règne le plus long)
- Nombre total d'années de règne cumulées

## A savoir

- Après un game over, l'image se fige pendant 3 secondes avant d'afficher l'interface pour recommencer
- L'interface de nouvelle partie s'affichera avec les différentes années de règne

## Structure du Projet

## Répertoires

- src/ : Fichiers source (.c)
- include/ : Fichiers d'en-tête (.h)
- assets/ : Ressources (images, sons, musiques)

## Fichiers Principaux

    game.c : Logique et affichage principal du jeu
    card.c : Gestion et affichage des cartes
    config.c : Configuration et modification du config.txt
    database.c : Gestion de la base de données SQLite
    event.c : Gestion des événements et des ressources
    image.c : Chargement et affichage des images
    audio.c :   Gestion de l'audio du jeu
    init.c : Initialisation de SDL et configuration de base
    main.c : Point d'entrée du programme
    resource.c : Gestion des ressources du jeu
    story.c : Chargement des histoires
    constants.h : Définition de toutes les constantes
# Reigns
