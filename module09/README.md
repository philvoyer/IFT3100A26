# IFT3100A26

## Module 9 : Lancer de rayons

### Exemple 9.1 (RayIntersection)

Exemple des tests d'intersection entre un rayon et les quatre types de surfaces des notes de cours : un plan, une boîte, une sphère et un triangle.

Le rayon balaie la scène et un point est affiché à l'impact avec chaque surface qu'il frappe. Au point d'impact le plus rapproché, on voit aussi la normale de la surface et le rayon de réflexion. Les valeurs intermédiaires de chaque test (D . N pour le plan, la face frappée pour la boîte, le discriminant pour la sphère et les coordonnées barycentriques pour le triangle) sont affichées dans le coin de la fenêtre.

La variable `sample_count` du renderer choisit le nombre d'échantillons lancés à chaque frame. Avec 1, sa valeur par défaut, seul le rayon est lancé. Au-delà de 1, des rayons légèrement différents sont ajoutés, dont la direction est choisie au hasard dans un petit cône autour de celle du rayon, selon l'approche de Monte Carlo. La proportion des échantillons qui frappent chaque surface est alors affichée dans le coin de la fenêtre. Les touches `+` et `-` doublent ou divisent par deux le nombre d'échantillons pendant l'exécution.

### Exemple 9.2 (Raytracer)

Exemple d'un raytracer minimaliste.

Inspiré de *smallpt*.

La scène est une boîte de Cornell éclairée par une sphère lumineuse au plafond, qui contient une sphère miroir et une sphère de verre. Le rendu est fait par path tracing avec roulette russe, il est réparti sur plusieurs fils d'exécution et l'image est sauvegardée dans un fichier `.ppm` dont le nom est horodaté, dans le dossier de l'exécutable (ou dans le dossier choisi avec l'option `--output dossier`).

Ce n'est pas un projet openFrameworks : le programme se compile avec la commande `make` et s'exécute avec `./raytracer largeur hauteur rayons_par_pixel [options]` (par défaut, une image de 256x256 avec 16 rayons par pixel). Chaque pixel est divisé en 2x2 sous-pixels, il reçoit donc 4 fois le nombre de rayons demandé.

Les options `--mirror x y z rayon` et `--glass x y z rayon` déplacent les sphères, `--mirror-count n` et `--glass-count n` ajoutent des sphères de position et de rayon aléatoires, `--mirror-color r g b` et `--glass-color r g b` changent leur couleur, `--random-colors` donne une couleur aléatoire à chaque sphère additionnelle, et `--random-box-colors` donne une couleur aléatoire aux murs de la boîte (sauf le mur avant, qui reste noir) et à la lumière. Les options `--floor-color`, `--ceiling-color`, `--left-color`, `--back-color`, `--right-color` et `--light-color`, suivies de `r g b`, choisissent directement ces couleurs et ont priorité sur les couleurs aléatoires.

Les options `--camera x y z` et `--target x y z` déplacent la caméra et choisissent le point qu'elle vise, et `--fov angle` change son champ de vision vertical (en degrés). La caméra doit être dans la boîte ou devant son ouverture. Par exemple, `--camera 50 40 150 --target 50 30 50 --fov 70` donne une vue de l'intérieur de la boîte.

L'option `--threads n` choisit le nombre de fils d'exécution (par défaut, le nombre de cœurs de l'ordinateur). La graine aléatoire est affichée au début du rendu et l'option `--seed n` permet de reproduire exactement le même rendu.

Par exemple : `./raytracer 256 256 16 --mirror 50 20 60 15 --glass-count 3 --glass-color 0.6 0.9 1`.
