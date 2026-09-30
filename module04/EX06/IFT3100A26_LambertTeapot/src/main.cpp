// IFT3100A26_LambertTeapot/main.cpp
// Exemple d'importation et de rendu d'un teapot avec un shader de Lambert.
// Un autre shader permet aussi de visualiser les normales sur la surface du modèle par conversion des composantes XYZ en couleur RGB.
// Les touches 1 et 2 sélectionnent le shader (Lambert ou normales), la barre d'espace active ou désactive la rotation du teapot, et un panneau permet d'ajuster les couleurs d'arrière-plan, ambiante et diffuse.
// Le fichier de géométrie du teapot et les shaders sont dans le répertoire ./bin/data.

#include "ofMain.h"
#include "application.h"

int main()
{
  // paramètres du contexte de rendu
  ofGLFWWindowSettings window_settings;

  // résolution de la fenêtre d'affichage
  window_settings.setSize(512, 512);

  // sélection de la version d'OpenGL
  window_settings.setGLVersion(3, 3);

  // création de la fenêtre
  ofCreateWindow(window_settings);

  // démarrer l'exécution de l'application
  ofRunApp(new Application());
}
