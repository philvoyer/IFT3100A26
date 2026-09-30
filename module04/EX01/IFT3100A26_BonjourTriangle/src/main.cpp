// IFT3100A26_BonjourTriangle/main.cpp
// Exemple où un triangle par seconde est dessiné à des positions aléatoires dans la fenêtre d'affichage.
// Chaque triangle a une couleur de remplissage aléatoire, ses trois sommets sont marqués d'un point et leurs coordonnées sont écrites dans la console.

#include "ofMain.h"
#include "application.h"

int main()
{
  ofSetupOpenGL(512, 512, OF_WINDOW);
  ofRunApp(new Application());
}
