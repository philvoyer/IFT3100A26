// IFT3100A26_SSAO/main.cpp
// Exemple d'occlusion ambiante en espace écran (SSAO : screen space ambient occlusion) des notes de cours (module 10, section « Réflexion ambiante »),
// à partir du rendu en différé de l'exemple 10.1 : un coin de pièce avec un teapot, des cubes empilés, une pyramide de 6 cubes, un cube seul, un tas de billes et des sphères, dans un matériau mat comme de l'argile.
// Passe 1 (géométrie) : la position et la normale des surfaces visibles sont rendues dans un g-buffer, dans l'espace de vue.
// Passe 2 (SSAO) : pour chaque pixel, des échantillons placés autour de la surface sont projetés dans le g-buffer, et ceux qui sont cachés par la géométrie contribuent à l'occlusion.
// Passe 3 (flou) : la moyenne de 4x4 pixels efface le motif de la petite texture de vecteurs aléatoires qui fait tourner les échantillons d'un pixel à l'autre.
// Passe 4 (éclairage) : la réflexion ambiante est multipliée par le facteur d'occlusion ambiante, puis une lumière ponctuelle ajoute une réflexion diffuse.
// Les touches 1 à 5 sélectionnent le mode de rendu : éclairage avec ou sans SSAO, réflexion ambiante seulement, et le facteur d'occlusion ambiante avant ou après le flou.
// La touche 6 affiche les couches du SSAO en même temps : la position et la normale du g-buffer, et le facteur d'occlusion ambiante avant et après le flou.
// La touche H alterne entre un noyau d'échantillons en hémisphère orienté selon la normale et un noyau en sphère sans normale (la technique originale de Crytek), qui assombrit aussi les surfaces planes.
// Les flèches ↑ et ↓ changent le rayon de la zone d'échantillonnage, et les flèches ← et → le nombre d'échantillons (de 4 à 128), dont le coût se voit au nombre d'images par seconde affiché à l'écran.
// La touche L active ou désactive le mouvement de la lumière et la touche R l'oscillation de la scène.
// La touche V active ou désactive la limite de 60 images par seconde (et la synchronisation verticale), pour mesurer le vrai coût du rendu.

#include "ofMain.h"
#include "application.h"

int main()
{
  // paramètres du contexte de rendu
  ofGLFWWindowSettings windowSettings;

  // résolution de la fenêtre d'affichage
  windowSettings.setSize(1024, 768);

  // sélection de la version de OpenGL
  windowSettings.setGLVersion(3, 3);

  // création de la fenêtre
  ofCreateWindow(windowSettings);

  // démarrer l'exécution de l'application
  ofRunApp(new Application());
}
