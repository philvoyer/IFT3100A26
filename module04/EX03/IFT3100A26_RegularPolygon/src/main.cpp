// IFT3100A26_RegularPolygon/main.cpp
// Exemple de dessin des polygones réguliers du triangle au dodécagone (de 3 à 12 côtés).
// Les touches 1 à 9 et 0 sélectionnent le nombre de côtés (de 3 à 12). Chaque sommet est relié au centre du polygone.

#include "ofMain.h"
#include "application.h"

int main()
{
  ofSetupOpenGL(512, 512, OF_WINDOW);
  ofRunApp(new Application());
}
