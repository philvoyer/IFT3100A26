// IFT3100A26_TriangleSoup/main.cpp
// Exemple de génération aléatoire et rendu d'une soupe aux triangles (2500 triangles) répartis dans un hémisphère (bol) ou dans une sphère (balle).
// Les données des triangles sont stockées dans un bloc de mémoire contigu.
// Les flèches déplacent le point de vue et les touches Z et X font pivoter la soupe autour de l'axe Y.
// Un clic de souris génère une nouvelle soupe et la barre d'espace alterne entre le bol et la balle.

#include "ofMain.h"
#include "application.h"

int main()
{
  ofSetupOpenGL(512, 512, OF_WINDOW);
  ofRunApp(new Application());
}
