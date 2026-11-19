// IFT3100A26_RayIntersection/main.cpp
// Exemple des tests d'intersection entre un rayon et un plan, une boîte, une sphère et un triangle.
// Le rayon balaie la scène et le point d'impact avec chaque surface frappée est affiché.

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
