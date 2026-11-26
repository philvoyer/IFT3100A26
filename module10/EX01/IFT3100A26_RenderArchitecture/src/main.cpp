// IFT3100A26_RenderArchitecture/main.cpp
// Exemple des deux architectures de rendu : le rendu immédiat et le shading en différé.
// Une grille de 2x2 objets animés (triangle, cube, sphère et teapot) est éclairée par quatre lumières ponctuelles avec le modèle de Blinn-Phong.
// Avec le rendu immédiat (touche 1), chaque objet est éclairé au moment où il est dessiné.
// Avec le shading en différé (touche 2), une passe géométrie rend les attributs des surfaces visibles dans un g-buffer, puis une passe éclairage évalue le modèle d'illumination une fois par pixel.
// Les touches 3, 4, 5 et 6 affichent une couche du g-buffer (position, normale, couleur ou profondeur) et la touche 7 les affiche toutes.
// La touche I affiche ou masque les informations sur le g-buffer.
// La touche L active ou désactive le mouvement des lumières et la touche R celui des objets.

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
