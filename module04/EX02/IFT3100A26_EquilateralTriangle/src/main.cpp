// IFT3100A26_EquilateralTriangle/main.cpp
// Exemple de dessin d'un triangle équilatéral centré dans la fenêtre d'affichage, avec son cercle inscrit et son cercle circonscrit.
// Certaines de ses propriétés (longueur des arêtes, altitude, rayons des cercles, périmètre et aire) sont calculées et écrites dans la console.

#include "ofMain.h"
#include "application.h"

int main()
{
  ofSetupOpenGL(512, 512, OF_WINDOW);
  ofRunApp(new Application());
}
