// IFT3100A26_TeaParty/main.cpp
// Exemple de chargement et de rendu de plusieurs instances (100) d'un modèle importé à partir d'un fichier de géométrie externe (un teapot en format .obj).
// Chaque instance a une position, une rotation autour de l'axe Y et une proportion aléatoires, stockées dans un bloc de mémoire contigu. Un clic de souris redistribue les instances.
// Les touches 1, 2 et 3 sélectionnent le mode de rendu (surfaces, fil de fer ou sommets), les flèches déplacent la scène, les touches W, E et R activent ou désactivent la translation, la rotation et la proportion, et la touche F inverse l'axe Y.

#include "ofMain.h"
#include "application.h"

int main()
{
  ofSetupOpenGL(512, 512, OF_WINDOW);
  ofRunApp(new Application());
}
