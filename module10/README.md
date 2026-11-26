# IFT3100A26

## Module 10 : Illumination moderne

### Exemple 10.1 (RenderArchitecture)

Exemple des deux architectures de rendu présentées dans les notes de cours (section « Architectures de rendu ») : le rendu immédiat (*forward rendering*) et le rendu en différé (*deferred rendering*), dans sa variante en deux passes, le shading en différé (*deferred shading*).

Une grille de 2x2 objets est éclairée par quatre lumières ponctuelles qui tournent autour de la scène, avec le modèle d'illumination de Blinn-Phong : 1.1 un triangle équilatéral qui s'incline avec deux oscillations désynchronisées, 1.2 un cube tourné de 45 degrés autour des axes x et y qui se déplace avec deux oscillations désynchronisées, 2.1 une sphère qui s'étire et s'écrase avec deux oscillations désynchronisées (courbe de Lissajous) et 2.2 un teapot qui tangue et qui monte et descend avec deux oscillations désynchronisées.

Avec le rendu immédiat (touche 1), chaque objet est éclairé au moment où il est dessiné, directement dans la fenêtre d'affichage. Avec le shading en différé (touche 2), une passe géométrie rend d'abord les attributs des surfaces visibles dans les couches d'un g-buffer (un framebuffer avec plusieurs buffers de rendu, MRT), puis une passe éclairage lit ces couches sur un quadrilatère en pleine fenêtre et évalue le modèle d'illumination une seule fois par pixel. Les deux architectures utilisent la même fonction d'illumination et donnent la même image.

Les touches 3, 4, 5 et 6 affichent une des couches du g-buffer (position, normale, couleur ou profondeur) et la touche 7 affiche les quatre couches en même temps. La touche I affiche ou masque les informations sur le g-buffer : sa résolution, le format interne de chaque couche (`GL_RGBA32F` pour la position, la normale et la couleur, la profondeur sur 24 bits), le contenu et l'espace des valeurs de chaque couche et le poids total en mémoire (52 octets par pixel). La touche L active ou désactive le mouvement des lumières et la touche R le mouvement des objets.

### Exemple 10.2 (SSAO)

Exemple d'occlusion ambiante en espace écran (*screen space ambient occlusion*, SSAO) présentée dans les notes de cours (section « Réflexion ambiante »), à partir du rendu en différé de l'exemple 10.1.

La scène est un coin de pièce (un plancher et deux murs) avec un teapot, des cubes empilés, une pyramide de 6 cubes alignés sur 3 étages (3, 2 et 1 cubes), un cube seul légèrement tourné au milieu d'un espace vide, un tas de 52 billes de dimensions variables et deux sphères posées sur le plancher, pour créer des coins, des creux et des points de contact. Toute la scène a le même matériau mat et pâle, comme un modèle en argile, pour que l'occlusion ambiante soit facile à voir.

Le rendu se fait en quatre passes. La passe géométrie rend la position et la normale des surfaces visibles dans un g-buffer, dans l'espace de vue. La passe SSAO place, pour chaque pixel, des échantillons autour de la surface, les projette dans le g-buffer et compte ceux qui sont cachés par la géométrie, ce qui donne un facteur d'occlusion ambiante entre 0 et 1. Une petite texture de 4x4 vecteurs aléatoires fait tourner les échantillons d'un pixel à l'autre, et la passe de flou fait la moyenne de 4x4 pixels pour effacer ce motif. La passe éclairage multiplie la réflexion ambiante par le facteur d'occlusion ambiante, puis une lumière ponctuelle qui tourne au-dessus de la scène ajoute une réflexion diffuse de Lambert.

Les touches 1 à 5 sélectionnent le mode de rendu : l'éclairage avec ou sans SSAO, la réflexion ambiante seulement (avec SSAO), et le facteur d'occlusion ambiante avant ou après le flou. La touche 6 affiche les couches du SSAO en même temps : la position et la normale du g-buffer, et le facteur d'occlusion ambiante avant et après le flou. La touche H alterne entre un noyau d'échantillons en hémisphère orienté selon la normale de la surface et un noyau en sphère qui n'utilise pas la normale, la technique originale de Crytek pour le jeu Crysis : environ la moitié des échantillons de la sphère sont sous la surface, ce qui assombrit aussi les surfaces planes. Les flèches ↑ et ↓ changent le rayon de la zone d'échantillonnage et les flèches ← et → le nombre d'échantillons (de 4 à 128), dont le coût se voit au nombre d'images par seconde affiché à l'écran. La touche L active ou désactive le mouvement de la lumière et la touche R l'oscillation de la scène. La touche V active ou désactive la limite de 60 images par seconde et la synchronisation verticale : sans la limite, le nombre d'images par seconde affiché montre le vrai coût du rendu.

### Exemple 10.3 (PBR)

Dans cet exemple, différents maillages géométriques sont rendus avec un modèle d'illumination de type PBR.

Le modèle d'illumination est celui qui est couramment appelé GGX.

La fonction BRDF est celle de *Cook-Torrance*.

La fonction de distribution des microfacettes est celle de *Trowbridge-Reitz*.

La fonction géométrique qui permet de calculer l'impact de l'occlusion et de l'ombrage des microfacettes est celle de *Schlick-Beckmann* et elle est utilisée par la méthode de *Smith*.

La fonction qui calcule l'effet de Fresnel est celle de *Schlick-Fresnel*.

L'éclairage de la scène provient d'une lumière ponctuelle qui est transformée en fonction de la position du curseur.

Le matériau utilise 4 textures, une par propriété : la couleur de base (texture diffuse), la métallicité, la rugosité et l'occlusion ambiante (source : Poly Haven, anciennement Texture Haven, https://polyhaven.com/).

Différents attributs du matériau peuvent être configurés à partir d'une interface graphique (ex. couleurs ambiante, diffuse et spéculaire du matériau, facteur de rugosité et métallicité, couleur et intensité de la lumière, paramètres de mappage tonal, correction gamma, etc.).
