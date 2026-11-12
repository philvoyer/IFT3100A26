// IFT3100A26_SurfaceCoons/main.cpp
// Exemple d'une surface de Coons construite à partir de quatre courbes de Bézier cubiques qui délimitent ses bords.
// La surface est affichée sous forme d'isolignes, avec une caméra qui tourne lentement autour de l'axe y pour la voir en 3D.
// Chaque point de contrôle a une étiquette et la couleur de la ou des courbes auxquelles il appartient.

#include "ofMain.h"
#include "application.h"

int main()
{
  ofGLWindowSettings windowSettings;
  windowSettings.setSize(1920/2.0f, 1080/2.0f);
  windowSettings.setGLVersion(2, 1);
  ofCreateWindow(windowSettings);
  ofRunApp(new Application());
}
