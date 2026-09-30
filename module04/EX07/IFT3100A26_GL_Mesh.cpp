// IFT3100A26_GL_Mesh.cpp
// Exemple d'une classe minimaliste pour stocker les données des sommets d'un maillage triangulaire (structure de sommet, structure de triangle et tableau d'indices).

// entrée
// • Une structure de sommet, une structure de triangle et des données initiales du maillage triangulaire.
// sortie
// • Une classe qui contient des triangles indexés par leurs sommets.

// structure de sommet
struct Vertex
{
  GLfloat position[3]; // 3 * 4 = 12 octets
  GLfloat normal  [3]; // 3 * 4 = 12 octets
  GLfloat texcoord[2]; // 2 * 4 = 8  octets
  GLubyte color   [4]; // 4 * 1 = 4  octets
};                     //       = 36 octets

// structure d'un triangle
struct Triangle
{
  // indices des 3 sommets
  int vertex_index[3];
};

// classe d'un maillage géométrique
class Mesh
{
public:

  // nombre de sommets
  int vertex_count;

  // séquence des sommets
  Vertex* vertex_array;

  // nombre de triangles
  int triangle_count;

  // séquence des triangles
  Triangle* triangle_array;

  // tableau d'indices (3 indices de sommet par triangle)
  // note : même information que triangle_array, mais en une séquence d'entiers contiguë
  // directement utilisable par une commande de rendu indexé (ex. glDrawElements)
  int* index_array;

  Mesh()
  {
    // initialisation du nombre de sommets et du nombre de triangles du maillage
    vertex_count = ...;
    triangle_count = ...;

    // allocation d'une séquence de sommets
    vertex_array = (Vertex*) std::malloc(vertex_count * sizeof(Vertex));

    // allocation d'un tableau de triangles
    triangle_array = (Triangle*) std::malloc(triangle_count * sizeof(Triangle));

    // allocation d'un tableau d'indices de sommet
    index_array = (int*) std::malloc(triangle_count * 3 * sizeof(int));

    // ...
  }

  // note : cette classe possède des pointeurs bruts et ne doit pas être copiée
  // (une copie partagerait les mêmes blocs de mémoire et les libérerait deux fois)
  ~Mesh()
  {
    std::free(vertex_array);
    std::free(triangle_array);
    std::free(index_array);
  }
};
