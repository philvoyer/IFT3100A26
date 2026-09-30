# IFT3100A26

## Module 4 : Géométrie

### Exemple 4.1 (BonjourTriangle)

Exemple où un triangle par seconde est dessiné à des positions aléatoires dans la fenêtre d'affichage.

Chaque triangle a une couleur de remplissage aléatoire, ses trois sommets sont marqués d'un point et leurs coordonnées sont écrites dans la console.

### Exemple 4.2 (EquilateralTriangle)

Exemple de dessin d'un triangle équilatéral centré dans la fenêtre d'affichage, avec son cercle inscrit et son cercle circonscrit.

Certaines de ses propriétés (longueur des arêtes, altitude, rayons des cercles, périmètre et aire) sont calculées et écrites dans la console.

### Exemple 4.3 (RegularPolygon)

Exemple de dessin des polygones réguliers du triangle au dodécagone (de 3 à 12 côtés).

Les touches 1 à 9 et 0 sélectionnent le nombre de côtés (de 3 à 12). Chaque sommet est relié au centre du polygone.

### Exemple 4.4 (TriangleSoup)

Exemple de génération aléatoire et rendu d'une soupe aux triangles (2500 triangles) répartis dans un hémisphère (bol) ou dans une sphère (balle).

Les données des triangles sont stockées dans un bloc de mémoire contigu.

Les flèches déplacent le point de vue et les touches Z et X font pivoter la soupe autour de l'axe Y. Un clic de souris génère une nouvelle soupe et la barre d'espace alterne entre le bol et la balle.

### Exemple 4.5 (TeaParty)

Exemple de chargement et de rendu de plusieurs instances (100) d'un modèle importé à partir d'un fichier de géométrie externe (un teapot en format .obj).

Chaque instance a une position, une rotation autour de l'axe Y et une proportion aléatoires, stockées dans un bloc de mémoire contigu. Un clic de souris redistribue les instances.

Les touches 1, 2 et 3 sélectionnent le mode de rendu (surfaces, fil de fer ou sommets), les flèches déplacent le point de vue, les touches W, E et R activent ou désactivent la translation, la rotation et la proportion, et la touche F inverse l'axe Y.

### Exemple 4.6 (LambertTeapot)

Exemple d'importation et de rendu d'un teapot avec un shader de Lambert.

Un autre shader permet aussi de visualiser les normales sur la surface du modèle par conversion des composantes XYZ en couleur RGB.

Les touches 1 et 2 sélectionnent le shader (Lambert ou normales), la barre d'espace active ou désactive la rotation du teapot, et un panneau permet d'ajuster les couleurs d'arrière-plan, ambiante et diffuse.

Le fichier de géométrie du teapot et les shaders sont dans le répertoire ./bin/data.

### Exemple 4.7 (GL_Mesh)

Exemple d'une classe minimaliste pour stocker les données des sommets d'un maillage triangulaire (structure de sommet, structure de triangle et tableau d'indices).

### Exemple 4.8 (GL_VertexBuffer)

Exemple d'une section de code pour créer un buffer de géométrie (vbo), en version statique et dynamique, puis pour le détruire.

### Exemple 4.9 (GL_VertexAttributeFixedPipeline)

Exemple d'une section de code pour assigner les pointeurs vers chaque attribut de la structure de sommet d'un buffer de géométrie (pipeline fixe).

### Exemple 4.10 (GL_VertexAttributeModernPipeline)

Exemple d'une section de code pour assigner les pointeurs vers chaque attribut de la structure de sommet d'un buffer de géométrie (pipeline moderne, avec un programme de shader).

### Exemple 4.11 (GL_VertexBufferDraw)

Exemple de différentes approches pour dessiner un buffer de géométrie (glDrawArrays et glDrawElements avec divers types de primitives topologiques).

### Exemple 4.12 (GL_VertexBufferSwap)

Exemple d'une section de code qui alterne les rôles de deux buffers de géométrie (mise à jour et rendu) afin d'éviter de bloquer le pipeline de rendu.
