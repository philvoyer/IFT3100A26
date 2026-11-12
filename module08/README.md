# IFT3100A26

## Module 8 : Topologie

### Exemple 8.1 (CurveBezier)

Exemple d'une courbe de Bézier quadratique et d'une courbe de Bézier cubique.

Le code compare deux implémentations équivalentes de chaque courbe, une traduction directe de l'équation et une version précalculée qui fait moins de calculs : environ 1,6 fois moins de multiplications et 1,8 fois moins d'opérations mathématiques pour la courbe quadratique, et environ 2,1 fois moins de multiplications et 2,3 fois moins d'opérations mathématiques pour la courbe cubique.

### Exemple 8.2 (CurveHermite)

Exemple d'une courbe de Hermite cubique et d'une courbe de Bézier cubique.

Les tangentes de la courbe de Hermite sont obtenues par conversion des points de contrôle de la courbe de Bézier (V1 = 3 (P2 - P1), V2 = 3 (P4 - P3)), donc les deux courbes se superposent quand on passe de l'une à l'autre.

Le code compare aussi deux implémentations équivalentes de la courbe de Hermite, une traduction directe de l'équation et une version précalculée qui fait environ 3,7 fois moins de multiplications et 2,8 fois moins d'opérations mathématiques.

### Exemple 8.3 (SurfaceCoons)

Exemple d'une surface de Coons construite à partir de quatre courbes de Bézier cubiques qui délimitent ses bords.

La surface est affichée sous forme d'isolignes et une caméra tourne lentement autour de l'axe y pour la voir en 3D. Chaque point de contrôle a une étiquette et la couleur de la courbe à laquelle il appartient, et les coins, partagés par deux courbes, ont la somme des deux couleurs (ex. rouge + bleu = violet). La tête de lecture anime deux isolignes avec deux oscillations désynchronisées, ce qui lui permet de parcourir toute la surface.
