// IFT3100A26_ToneMapping/main.cpp
// Exemple d'un filtre d'image créé avec un shader de fragments.
// Le filtre applique une correction de couleur sur l'image d'origine avec trois techniques : un facteur d'exposition, un mappage tonal (Reinhard ou ACES filmic) et une correction gamma.

#include "ofMain.h"
#include "application.h"

int main()
{
  ofGLFWWindowSettings windowSettings;
  windowSettings.resizable = false;
  windowSettings.setGLVersion(3, 3);
  windowSettings.numSamples = 4;
  ofCreateWindow(windowSettings);
  ofRunApp(new Application());
}
