// IFT3100A26_Raytracer/raytracer.cpp
// Exemple d'un raytracer minimaliste.
// Inspiré de smallpt.
// make : g++ -std=c++11 -O2 -pthread raytracer.cpp -o raytracer
// run  : ./raytracer image_width image_height ray_per_pixel [options]
// options :
//   --mirror x y z radius  position et rayon de la sphère miroir (défaut : 30 30 40 22.5)
//   --glass x y z radius   position et rayon de la sphère de verre (défaut : 75 40 75 17.5)
//   --mirror-count n       nombre de sphères miroirs (défaut : 1)
//   --glass-count n        nombre de sphères de verre (défaut : 1)
//   --mirror-color r g b   couleur des sphères miroirs, composantes entre 0 et 1 (défaut : 1 1 1)
//   --glass-color r g b    couleur des sphères de verre, composantes entre 0 et 1 (défaut : 1 1 1)
//   --random-colors        couleur aléatoire pour chaque sphère additionnelle (au lieu de la couleur de son type)
//   --random-box-colors    couleur aléatoire pour les murs de la boîte (sauf le mur avant, qui reste noir) et pour la lumière
//   --floor-color r g b    couleur du plancher, composantes entre 0 et 1 (défaut : 0.75 0.75 0.75)
//   --ceiling-color r g b  couleur du plafond, composantes entre 0 et 1 (défaut : 0.75 0.75 0.75)
//   --left-color r g b     couleur du mur gauche, composantes entre 0 et 1 (défaut : 0.75 0.25 0.25)
//   --back-color r g b     couleur du mur du fond, composantes entre 0 et 1 (défaut : 0.25 0.75 0.25)
//   --right-color r g b    couleur du mur droit, composantes entre 0 et 1 (défaut : 0.25 0.25 0.75)
//   --light-color r g b    teinte de la lumière, composantes entre 0 et 1 (défaut : 1 1 1)
//   --camera x y z         position de la caméra, dans la boîte ou devant son ouverture (défaut : 50 50 300)
//   --target x y z         point visé par la caméra (défaut : direction 0 -0.042612 -1, comme dans smallpt)
//   --fov angle            champ de vision vertical de la caméra, en degrés (défaut : environ 29)
//   --threads n            nombre de fils d'exécution du rendu (défaut : nombre de cœurs de l'ordinateur)
//   --seed n               graine des nombres aléatoires, pour un rendu reproductible (défaut : graine aléatoire)
//   --output dir           dossier où l'image est sauvegardée (défaut : dossier de l'exécutable)
// la première sphère de chaque type utilise la position et le rayon en paramètre,
// les sphères additionnelles ont une position et un rayon aléatoires
// les couleurs des murs et de la lumière en paramètre ont priorité sur les couleurs aléatoires de --random-box-colors
// ex.  : ./raytracer 256 256 16 --mirror 50 20 60 15 --glass-count 3 --glass-color 0.6 0.9 1
// ex.  : ./raytracer 256 256 64 --camera 50 40 150 --target 50 30 50 --fov 70 (vue de l'intérieur de la boîte)

// la boîte de Cornell est une scène de test classique en infographie, qui vient du Program of Computer Graphics de l'Université Cornell :
// elle apparaît pour la première fois dans un article sur la radiosité de Goral, Torrance, Greenberg et Battaile (SIGGRAPH 1984),
// et une vraie boîte a ensuite été construite pour comparer des photographies aux images calculées
// la vraie boîte a cinq faces (plancher, plafond, fond, mur gauche rouge et mur droit vert), un panneau lumineux au plafond,
// et elle est photographiée à travers son côté avant, qui est ouvert (la version la plus connue contient aussi deux blocs blancs)
// ici, comme dans smallpt, la seule géométrie disponible est la sphère, ce qui impose trois astuces :
//   1. les murs sont des sphères énormes dont la surface paraît plane à l'échelle de la boîte
//   2. l'ouverture avant est remplacée par un mur noir, car les sphères des autres murs continuent bien au-delà de la boîte
//   3. les rayons de la caméra partent de l'intérieur de la boîte, car le mur noir cacherait tout à une caméra placée devant

// c
#define _USE_MATH_DEFINES // pour M_PI avec le compilateur de Visual Studio
#include <cmath>    // sqrt pow fabs cos sin
#include <cstdio>   // fprintf stderr remove
#include <cstdlib>  // atoi atof strtoul
#include <ctime>    // time_t tm localtime strftime

// c++
#include <iostream> // std cout endl
#include <sstream>  // stringstream
#include <fstream>  // ofstream
#include <string>   // string
#include <iomanip>  // setw setfill
#include <limits>   // numeric_limits

// c++11
#include <cstdint>  // uint32_t
#include <vector>
#include <random>
#include <chrono>
#include <thread>   // thread hardware_concurrency
#include <atomic>   // atomic

// paramètres du programme
const int max_depth = 5; // profondeur de récursion à partir de laquelle la roulette russe peut arrêter un rayon
// probabilité maximale qu'un rayon survive à la roulette russe : avec 0.95, un rayon qui rebondit entre des surfaces
// parfaitement réfléchissantes s'arrête en moyenne après 20 rebonds (1 / (1 - 0.95)) au-delà de la profondeur maximale
// (smallpt évite plutôt le problème en donnant une couleur de 0.999 au miroir et au verre, mais il faut alors en moyenne 1000 rebonds)
const double max_survival_probability = 0.95;
// champ de vision de la caméra, sous forme de la hauteur de l'image à une distance de 1 devant la caméra
// (0.5135 donne un champ de vision vertical d'environ 29 degrés, soit 2 * atan(0.5135 / 2), la valeur de smallpt)
double camera_fov = 0.5135;
constexpr double gamma_correction = 1 / 2.2;

// variables du programme
int image_width = 0;
int image_height = 0;
int ray_per_pixel = 0;
int pixel_count = 0;

// paramètres des sphères à l'intérieur de la boîte de Cornell (valeurs par défaut de la scène originale)
double mirror_x = 30.0;
double mirror_y = 30.0;
double mirror_z = 40.0;
double mirror_radius = 22.5;
int mirror_count = 1;
double mirror_r = 1.0;
double mirror_g = 1.0;
double mirror_b = 1.0;

double glass_x = 75.0;
double glass_y = 40.0;
double glass_z = 75.0;
double glass_radius = 17.5;
int glass_count = 1;
double glass_r = 1.0;
double glass_g = 1.0;
double glass_b = 1.0;

// couleur aléatoire pour chaque sphère additionnelle
bool random_colors = false;

// couleur aléatoire pour les murs de la boîte de Cornell (sauf le mur avant) et pour la sphère lumineuse
bool random_box_colors = false;

// intervalle des composantes des couleurs aléatoires (assez claires pour que les sphères restent lumineuses)
const double random_color_min = 0.3;
const double random_color_max = 1.0;

// intervalle des composantes des couleurs aléatoires des murs (sous 1, pour que les murs absorbent une partie de la lumière comme de vrais murs)
const double random_wall_color_min = 0.1;
const double random_wall_color_max = 0.8;

// intensité de la sphère lumineuse (valeur de sa composante d'émission la plus forte)
const double light_intensity = 15.0;

// intervalle des rayons aléatoires des sphères additionnelles
const double random_radius_min = 4.0;
const double random_radius_max = 12.0;

// nombre maximal de tentatives pour placer une sphère aléatoire sans chevauchement
const int random_placement_attempts = 1000;

// limites de l'intérieur de la boîte de Cornell (position des murs)
constexpr double box_x_min = 1.0;   // mur gauche
constexpr double box_x_max = 99.0;  // mur droit
constexpr double box_y_min = 0.0;   // plancher
constexpr double box_y_max = 81.6;  // plafond
constexpr double box_z_min = 0.0;   // mur arrière
constexpr double box_z_max = 170.0; // mur avant

// distance à l'intérieur du mur avant d'où partent les rayons d'une caméra placée devant la boîte
const double front_wall_margin = 1.0;

// types de matériau
enum class SurfaceType {diffuse, specular, refraction};

// fonction pour borner une valeur numérique entre 0 et 1
double clamp(double x)
{
  return x < 0 ? 0 : x > 1 ? 1 : x;
}

// type vecteur 3D
struct Vector
{
  double x, y, z;

  // constructeurs
  constexpr Vector() : x(0.0), y(0.0), z(0.0) {}
  constexpr Vector(double x) : x(x), y(0.0), z(0.0) {}
  constexpr Vector(double x, double y) : x(x), y(y), z(0.0) {}
  constexpr Vector(double x, double y, double z) : x(x), y(y), z(z) {}

  // produit scalaire (dot product)
  double dot(const Vector& v) const
  {
    return x * v.x + y * v.y + z * v.z;
  }

  // produit vectoriel (cross product)
  Vector cross(const Vector& v) const
  {
    return Vector(
      y * v.z - z * v.y,
      z * v.x - x * v.z,
      x * v.y - y * v.x);
  }

  // multiplication composante par composante
  Vector multiply(const Vector& v) const
  {
    return Vector(
      x * v.x,
      y * v.y,
      z * v.z);
  }

  // multiplication scalaire
  Vector operator*(double s) const
  {
    return Vector(
      x * s,
      y * s,
      z * s);
  }

  // addition vectorielle
  Vector operator+(const Vector& v) const
  {
    return Vector(
      x + v.x,
      y + v.y,
      z + v.z);
  }

  // soustraction vectorielle
  Vector operator-(const Vector& v) const
  {
    return Vector(
      x - v.x,
      y - v.y,
      z - v.z);
  }

  // normalisation
  Vector& normalize()
  {
    return *this = *this * (1.0 / sqrt(x * x + y * y + z * z));
  }
};

// structure d'un rayon sous forme de droite paramétrique
// équation d'un rayon des notes de cours : rayon(t) = P + t D, où P est l'origine et D la direction normalisée
struct Ray
{
  Vector origin;
  Vector direction;

  Ray(Vector o, Vector d) : origin(o), direction(d) {}
};

// type Sphere sous forme d'un point central et d'un rayon
struct Sphere
{
  double radius;   // rayon de la sphère
  Vector position; // position du centre de la sphère
  Vector emission; // couleur émissive de la sphère
  Vector color;    // couleur de la sphère (couleur diffuse, ou teinte du miroir et du verre)

  SurfaceType material; // type de réflexion de la sphère

  // constructeur
  Sphere(double r, Vector p, Vector e, Vector c, SurfaceType m): radius(r), position(p), emission(e), color(c), material(m) {}

  // fonction d'intersection entre la sphère et un rayon
  double intersect(const Ray& ray) const
  {
    // distance de l'intersection la plus proche si elle existe
    double distance;

    // seuil de tolérance numérique du test d'intersection : distance minimale d'une intersection valide,
    // pour qu'un rayon qui repart d'une surface ne la retouche pas à cause des erreurs d'arrondi
    double epsilon = 0.0001;

    // distance du point d'intersection
    double t;

    // équation quadratique des notes de cours : a t^2 + b t + c = 0, où P = origine du rayon - centre de la sphère
    // comme la direction du rayon est normalisée, a = D . D = 1, et l'équation se simplifie avec la moitié de b
    // ici, delta = centre - origine = -P, donc half_b = delta . D = -b / 2 et les solutions sont t = half_b -/+ racine(discriminant)

    // vecteur entre l'origine du rayon et le centre de la sphère
    Vector delta = position - ray.origin;

    // calculer la moitié de b (avec le signe inversé)
    double half_b = delta.dot(ray.direction);

    // calculer c = P . P - r^2
    double c = delta.dot(delta) - radius * radius;

    // calculer le discriminant de l'équation quadratique simplifiée, qui vaut (b^2 - 4ac) / 4
    double discriminant = half_b * half_b - c;

    // valider si le discriminant est négatif
    if (discriminant < 0)
    {
      // il n'y a pas d'intersection avec cette sphère
      return distance = 0;
    }

    // calculer la racine carrée du discriminant seulement s'il n'est pas négatif
    discriminant = sqrt(discriminant);

    // déterminer la distance de la première intersection
    t = half_b - discriminant;

    // valider si la distance de la première intersection est supérieure au seuil de tolérance
    if (t > epsilon)
      distance = t;
    else
    {
      // déterminer la distance de la seconde intersection, qui est la seule devant le rayon quand son origine est à l'intérieur
      // de la sphère (ex. un rayon dans une sphère de verre, ou dans la boîte de Cornell, qui est à l'intérieur des sphères des murs)
      t = half_b + discriminant;

      // valider si la distance de la seconde intersection est supérieure au seuil de tolérance
      if (t > epsilon)
        distance = t;
      else
        distance = 0;
    }

    // retourner la distance du point d'intersection
    return distance;
  }
};

// structure d'une caméra
struct Camera
{
  Vector position;    // position de la caméra
  Vector orientation; // direction normalisée de la caméra
  Vector target;      // point visé par la caméra (utilisé seulement s'il est choisi en paramètre)

  // axes du repère de la caméra, mis à l'échelle pour couvrir l'image à une distance de 1 devant la caméra
  Vector axis_x;
  Vector axis_y;
  Vector axis_z;

  Vector up; // direction du haut dans la scène

  double viewport_width;
  double viewport_height;

  double fov;

  Camera(Vector p, Vector o) : position(p), orientation(o), target(), up(0, 1, 0), viewport_width(1.0), viewport_height(1.0), fov(1.0) {}

  // fonction qui oriente la caméra vers le point visé
  void look_at(const Vector& point)
  {
    target = point;
    orientation = (target - position).normalize();
  }

  // fonction qui permet de calibrer la caméra en fonction de la valeur courante de ses attributs
  void calibrate()
  {
    // axe z : direction de la caméra
    axis_z = orientation;

    // axe x : vers la droite de l'image, perpendiculaire à la direction de la caméra et au vecteur up,
    // avec une longueur égale à la largeur de l'image à une distance de 1 (champ de vision multiplié par le ratio de l'image)
    axis_x = axis_z.cross(up).normalize() * (viewport_width * fov / viewport_height);

    // axe y : vers le haut de l'image, perpendiculaire aux deux autres axes, avec une longueur égale au champ de vision
    axis_y = axis_x.cross(axis_z).normalize() * fov;
  }
};

// structure d'une image
struct Image
{
  int width;  // largeur de l'image
  int height; // hauteur de l'image
  int count;  // nombre de pixels

  double size; // poids en mémoire (en mégaoctets)

  // pointeur vers une séquence de pixels sous forme vectorielle
  Vector* pixel;

  // constructeurs
  Image() : width(1), height(1), count(0), size(0.0), pixel(nullptr) {}
  Image(int w) : width(w), height(w), count(0), size(0.0), pixel(nullptr) {}
  Image(int w, int h) : width(w), height(h), count(0), size(0.0), pixel(nullptr) {}

  // fonction qui redimensionne l'image
  void resize(int w, int h)
  {
    if (w > 0 && h > 0)
    {
      width = w;
      height = h;

      // calculer le nombre de pixels dans l'image
      count = width * height;

      // calculer la taille de l'espace mémoire (en mégaoctets)
      size = sizeof(Vector) * count / pow(1024.0, 2);

      // libérer la mémoire d'une allocation précédente s'il y a lieu
      if (pixel != nullptr)
        delete[] pixel;

      // allocation de la mémoire pour les pixels de l'image
      pixel = new Vector[count];
    }
  }

  ~Image()
  {
    if (pixel != nullptr)
      delete[] pixel;
  }
};

// variables globales

// déclaration du graphe de scène
std::vector<Sphere> scene;

// structure d'une couleur optionnelle, qui indique si elle a été choisie en paramètre
struct ColorOption
{
  bool is_set;
  Vector color;

  ColorOption() : is_set(false), color() {}
};

// couleurs des murs et teinte de la lumière choisies en paramètre
ColorOption floor_color_option;
ColorOption ceiling_color_option;
ColorOption left_color_option;
ColorOption back_color_option;
ColorOption right_color_option;
ColorOption light_color_option;

// orientation initiale de la caméra (valeur de smallpt, légèrement vers le bas)
Vector camera_orientation(Vector(0, -0.042612, -1).normalize());

// position initiale de la caméra
constexpr Vector camera_position(50, 50, 300);

// déclaration de la caméra utilisée pour rendre la scène
Camera camera(camera_position, camera_orientation);

// indicateur que le point visé par la caméra a été choisi en paramètre
bool camera_target_is_set = false;

// dossier où l'image est sauvegardée (par défaut, le dossier de l'exécutable)
std::string output_directory;

// déclaration d'une image
Image image;

// nombre de fils d'exécution du rendu (0 = nombre de cœurs de l'ordinateur)
int thread_count = 0;

// graine des nombres aléatoires et indicateur qu'elle a été choisie en paramètre
std::uint32_t seed = 0;
bool seed_is_set = false;

// générateur de nombres pseudo-aléatoires (algorithme Mersenne Twister) utilisé pour construire la scène,
// le rendu utilise plutôt un générateur par rangée de pixels (voir la fonction render)
std::mt19937 rng;

// fonction qui retourne un nombre aléatoire uniforme entre 0 et 1 à partir d'un générateur
double random01(std::mt19937& generator)
{
  return std::uniform_real_distribution<double>(0.0, 1.0)(generator);
}

// fonction qui retourne une couleur aléatoire dont les composantes sont entre min et max, à partir du générateur de la scène
Vector random_color(double min, double max)
{
  // trois instructions séparées, pour que l'ordre des tirages aléatoires ne dépende pas du compilateur
  // (l'ordre d'évaluation des arguments d'un constructeur n'est pas défini en C++)
  double r = min + (max - min) * random01(rng);
  double g = min + (max - min) * random01(rng);
  double b = min + (max - min) * random01(rng);

  return Vector(r, g, b);
}

// traitement de la couleur d'un pixel
int format_color_component(double value)
{
  // appliquer la correction gamma
  value = pow(clamp(value), gamma_correction);

  // convertir la valeur dans l'espace de couleur
  value = value * 255 + 0.5;

  // conversion numérique de réel vers entier
  return static_cast<int>(value);
}

// fonction qui valide s'il y a intersection entre un rayon et les géométries de la scène
// retourne l'intersection la plus proche de l'origine du rayon (distance et index de l'élément)
bool raycast(const Ray& ray, double& distance, int& id)
{
  // variable temporaire pour la distance d'une intersection entre un rayon et une sphère
  double d;

  // initialiser la distance à l'infini, pour que la première intersection trouvée soit toujours plus proche
  double infinity = distance = std::numeric_limits<double>::infinity();

  // nombre d'éléments dans la scène
  int n = static_cast<int>(scene.size());

  // parcourir tous les éléments de la scène
  for (int index = 0; index < n; ++index)
  {
    // test d'intersection entre le rayon et la géométrie à cet index
    d = scene[index].intersect(ray);

    // valider s'il y a eu intersection (une distance de 0 indique qu'il n'y en a pas) et si la distance est inférieure aux autres intersections
    if (d && d < distance)
    {
      // nouvelle valeur courante de la distance et de l'index de l'intersection la plus rapprochée de l'origine du rayon
      distance = d;
      id = index;
    }
  }

  // il y a eu intersection si la distance est plus petite que l'infini
  if (distance < infinity)
    return true;
  else
    return false;
}

// fonction récursive qui calcule la radiance, avec le générateur de nombres aléatoires de la rangée en cours
Vector compute_radiance(const Ray& ray, int depth, std::mt19937& generator)
{
  // valeur de la radiance
  Vector radiance;

  // distance de l'intersection
  double distance;

  // identifiant de la géométrie en intersection
  int id = 0;

  // valider s'il n'y a pas d'intersection
  if (!raycast(ray, distance, id))
    return Vector(); // couleur par défaut (noir)

  // référence vers une géométrie en intersection avec un rayon
  const Sphere& obj = scene[id];

  // calculer les coordonnées du point d'intersection avec l'équation du rayon : rayon(t) = P + t D
  Vector x = ray.origin + ray.direction * distance;

  // calculer la normale au point d'intersection, orientée vers l'extérieur de la sphère
  Vector n = (x - obj.position).normalize();

  // normale orientée du côté d'où vient le rayon (inversée quand le rayon frappe la sphère par l'intérieur,
  // ce qui arrive pour les murs, dont la boîte est à l'intérieur, et pour un rayon qui sort d'une sphère de verre)
  Vector na = n.dot(ray.direction) < 0 ? n : n * -1;

  // isoler la composante de couleur la plus puissante
  Vector f = obj.color;
  double threshold = f.x > f.y && f.x > f.z ? f.x : f.y > f.z ? f.y : f.z;

  // plafonner la probabilité de survie, sinon un rayon sur une surface parfaitement réfléchissante (composante de 1.0),
  // comme le miroir et le verre par défaut, survivrait toujours et ne s'arrêterait qu'en frappant une surface diffuse
  if (threshold > max_survival_probability)
    threshold = max_survival_probability;

  // au-delà de la profondeur maximale, la récursion n'est pas arrêtée net, elle continue par roulette russe :
  // le rayon survit avec une probabilité égale à la composante la plus forte de la couleur de la surface (plafonnée),
  // et sa contribution est alors divisée par cette probabilité pour ne pas biaiser le résultat
  if (++depth > max_depth)
  {
    // test de probabilité de la roulette russe
    if (random01(generator) < threshold)
      f = f * (1 / threshold);
    else
      return obj.emission;
  }

  if (obj.material == SurfaceType::diffuse)
  {
    // matériau avec réflexion diffuse (Lambert) : un seul rayon est relancé, dans une direction choisie au hasard
    // dans l'hémisphère orienté selon la normale, avec une probabilité proportionnelle au cosinus de l'angle avec la normale
    // comme cette distribution suit déjà la loi des cosinus de Lambert, la radiance reçue n'a pas à être multipliée par N . L

    // angle aléatoire autour de la normale, entre 0 et 2π
    double r1 = 2 * M_PI * random01(generator);

    // distance aléatoire de la normale, dans un disque de rayon 1 (la racine carrée répartit les points uniformément sur le disque)
    double r2 = random01(generator);
    double r2s = sqrt(r2);

    // repère orthonormé local (u, v, w) dont l'axe w est la normale orientée du côté du rayon
    Vector w = na;

    // l'axe u est perpendiculaire à w, à partir de l'axe y si w a une composante en x, sinon à partir de l'axe x
    // (pour ne jamais faire le produit vectoriel de deux vecteurs presque parallèles)
    Vector u = ((fabs(w.x) > 0.1 ? Vector(0, 1) : Vector(1)).cross(w)).normalize();
    Vector v = w.cross(u);

    // direction du rayon : point aléatoire du disque (dans le plan u v) projeté sur l'hémisphère, à une hauteur de racine(1 - r2) selon w
    Vector d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).normalize();

    radiance = obj.emission + f.multiply(compute_radiance(Ray(x, d), depth, generator));

    return radiance;
  }
  else if (obj.material == SurfaceType::specular)
  {
    // matériau avec réflexion spéculaire (miroir), avec l'équation de réflexion des notes de cours : I - 2 (N . I) N
    // (où I = direction du rayon incident et N = normale, le résultat est le même avec la normale inversée)

    radiance = obj.emission + f.multiply(compute_radiance(Ray(x, ray.direction - n * 2.0 * n.dot(ray.direction)), depth, generator));

    return radiance;
  }
  else if (obj.material == SurfaceType::refraction)
  {
    // matériau avec réfraction (verre) : une partie de la lumière est reflétée et l'autre est réfractée,
    // selon l'algorithme de réfraction des notes de cours, où I = direction du rayon incident et N = normale orientée du côté du rayon

    // rayon de réflexion, avec l'équation de réflexion des notes de cours : I - 2 (N . I) N
    Ray reflection_ray(x, ray.direction - n * 2.0 * n.dot(ray.direction));

    // valider si le rayon entre dans la sphère de verre (sinon, il en sort)
    bool into = n.dot(na) > 0;

    // indices de réfraction de l'air et du verre
    double ior_air = 1.0;
    double ior_glass = 1.5;

    // ratio entre l'indice de réfraction du milieu d'où vient le rayon et celui du milieu où il entre : n = n1 / n2
    double ratio = into ? ior_air / ior_glass : ior_glass / ior_air;

    // produit scalaire N . I (négatif, car la normale est orientée du côté d'où vient le rayon)
    double n_dot_i = ray.direction.dot(na);

    // k = 1 - n^2 (1 - (N . I)^2), le carré du cosinus de l'angle de réfraction
    double k = 1.0 - ratio * ratio * (1.0 - n_dot_i * n_dot_i);

    // si k est négatif, il n'y a pas de rayon réfracté (réflexion totale interne) : toute la lumière est reflétée
    if (k < 0.0)
    {
      radiance = obj.emission + f.multiply(compute_radiance(reflection_ray, depth, generator));

      return radiance;
    }

    // direction du rayon réfracté : n I - (n (N . I) + racine(k)) N
    Vector refraction_direction = (ray.direction * ratio - na * (ratio * n_dot_i + sqrt(k))).normalize();

    // effet de Fresnel avec l'équation de Schlick-Fresnel du module 10 : f0 + (1 - f0) (1 - cos θ)^5,
    // où f0 = ((n2 - n1) / (n2 + n1))^2 est la réflectance avec une incidence normale (0.04 pour le verre)
    double ior_difference = ior_glass - ior_air;
    double ior_sum = ior_glass + ior_air;
    double f0 = ior_difference * ior_difference / (ior_sum * ior_sum);

    // cos θ est mesuré du côté de l'air : angle d'incidence si le rayon entre dans le verre, angle de réfraction s'il en sort
    double cos_theta = into ? -n_dot_i : refraction_direction.dot(n);
    double c = 1.0 - cos_theta;

    // proportion de la lumière qui est reflétée (réflectance) et proportion qui est réfractée (transmittance)
    double reflectance = f0 + (1.0 - f0) * c*c*c*c*c;
    double transmittance = 1.0 - reflectance;

    // probabilité de suivre le rayon de réflexion quand un seul des deux rayons est suivi (entre 0.25 et 0.75, comme dans smallpt)
    double p = 0.25 + 0.5 * reflectance;

    // contribution de chaque rayon divisée par sa probabilité d'être choisi, pour ne pas biaiser le résultat
    double weight_reflection = reflectance / p;
    double weight_refraction = transmittance / (1.0 - p);

    if (depth > 2)
    {
      // à partir de la profondeur 3, suivre un seul des deux rayons, choisi au hasard avec une probabilité p pour la réflexion,
      // sinon le nombre de rayons doublerait à chaque rencontre avec le verre
      if (random01(generator) < p)
        radiance = obj.emission + f.multiply(compute_radiance(reflection_ray, depth, generator) * weight_reflection);
      else
        radiance = obj.emission + f.multiply(compute_radiance(Ray(x, refraction_direction), depth, generator) * weight_refraction);
    }
    else
    {
      // suivre les deux rayons, pondérés par la réflectance et la transmittance
      Vector radiance_reflection = compute_radiance(reflection_ray, depth, generator);
      Vector radiance_refraction = compute_radiance(Ray(x, refraction_direction), depth, generator);

      radiance = obj.emission + f.multiply(radiance_reflection * reflectance + radiance_refraction * transmittance);
    }

    return radiance;
  }
  else
  {
    return radiance;
  }
}

// fonction qui permet de sauvegarder des pixels dans un fichier image (format .ppm)
void save_image_file(int width, int height, int ray_per_pixel, const Vector* pixel)
{
  // horodatage de la sauvegarde, pour que chaque rendu produise un fichier unique
  auto now = std::chrono::system_clock::now();
  std::time_t now_time = std::chrono::system_clock::to_time_t(now);
  int milliseconds = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000);

  // date et heure locales sous la forme YYMMDD_HHMMSS
  char timestamp[16];
  std::strftime(timestamp, sizeof(timestamp), "%y%m%d_%H%M%S", std::localtime(&now_time));

  // nom du fichier image de type .ppm (portable pixmap), ex. image256x256_16_261117_201530123.ppm
  std::stringstream ss;
  ss << "image" << width << "x" << height << "_" << ray_per_pixel << "_" << timestamp << std::setw(3) << std::setfill('0') << milliseconds;
  std::string basename = output_directory + "/" + ss.str();
  std::string filename = basename + ".ppm";

  // si un fichier du même nom existe déjà (ex. deux rendus terminés dans la même milliseconde),
  // ajouter une lettre de a à z à la fin du nom pour ne pas l'écraser
  for (char letter = 'a'; std::ifstream(filename).good() && letter <= 'z'; ++letter)
    filename = basename + letter + ".ppm";

  // déclaration et ouverture du fichier en mode écriture
  std::ofstream file;
  file.open(filename, std::ios::out);

  if (!file.is_open())
  {
    std::cerr << "error: cannot write the image file " << filename << std::endl;
    return;
  }

  std::cout << "image file: " << filename << std::endl;

  // en-tête du fichier pour une image avec un espace de couleur RGB 24 bits (P3 pour pixmap)
  file << "P3\n";

  // largeur et hauteur de l'image sur la deuxième ligne de l'en-tête
  file << width << ' ' << height << '\n';

  // valeur maximale de l'espace de couleur sur la troisième ligne de l'en-tête
  file << "255\n";

  // écriture des pixels dans le fichier image
  for (int index = 0; index < width * height; ++index)
  {
    // écrire la couleur du pixel dans le fichier image
    file << static_cast<std::uint32_t>(format_color_component(pixel[index].x)) << " ";
    file << static_cast<std::uint32_t>(format_color_component(pixel[index].y)) << " ";
    file << static_cast<std::uint32_t>(format_color_component(pixel[index].z)) << " ";
  }

  // fermeture du fichier
  file.close();
}

// fonction qui ajoute à la scène une sphère de position et de rayon aléatoires à l'intérieur de la boîte de Cornell,
// sans chevauchement avec les sphères déjà placées à partir de l'index first_inner_index
bool add_random_sphere(SurfaceType material, Vector color, int first_inner_index)
{
  // limites de l'intérieur de la boîte de Cornell (murs gauche et droit en x, plancher et plafond en y, mur arrière en z)
  // en z, les sphères restent devant la caméra, loin du mur avant
  const double x_min = 1.0;
  const double x_max = 99.0;
  const double y_min = 0.0;
  const double y_max = 81.0; // sous la sphère lumineuse qui dépasse légèrement du plafond
  const double z_min = 0.0;
  const double z_max = 110.0;

  for (int attempt = 0; attempt < random_placement_attempts; ++attempt)
  {
    // rayon aléatoire dans l'intervalle permis
    double radius = random_radius_min + (random_radius_max - random_radius_min) * random01(rng);

    // position aléatoire telle que la sphère est entièrement à l'intérieur de la boîte
    // (trois instructions séparées, pour que l'ordre des tirages aléatoires ne dépende pas du compilateur)
    double x = x_min + radius + (x_max - x_min - 2.0 * radius) * random01(rng);
    double y = y_min + radius + (y_max - y_min - 2.0 * radius) * random01(rng);
    double z = z_min + radius + (z_max - z_min - 2.0 * radius) * random01(rng);

    Vector position(x, y, z);

    // valider qu'il n'y a pas de chevauchement avec les sphères déjà placées
    bool overlap = false;

    for (int index = first_inner_index; index < static_cast<int>(scene.size()); ++index)
    {
      Vector delta = position - scene[index].position;

      double gap = radius + scene[index].radius;

      if (delta.dot(delta) < gap * gap)
      {
        overlap = true;
        break;
      }
    }

    if (!overlap)
    {
      // couleur aléatoire au lieu de la couleur du type si l'option est activée
      if (random_colors)
        color = random_color(random_color_min, random_color_max);

      scene.push_back(Sphere(radius, position, Vector(), color, material));
      return true;
    }
  }

  // aucune position libre trouvée après le nombre maximal de tentatives
  return false;
}

// fonction qui convertit une teinte de lumière en émission, mise à l'échelle pour que sa composante la plus forte
// garde l'intensité de la lumière blanche par défaut (ex. une teinte de 1 0.5 0 donne une émission de 15 7.5 0)
Vector light_emission_from_color(const Vector& color)
{
  double strongest = color.x > color.y && color.x > color.z ? color.x : color.y > color.z ? color.y : color.z;

  // une teinte noire éteint la lumière
  if (strongest <= 0.0)
    return Vector();

  return color * (light_intensity / strongest);
}

// fonction d'initialisation de la scène
void init()
{
  std::cout << "scene setup" << std::endl;

  // rayon des sphères surdimensionnées qui simulent les murs (100 000 unités, assez grand pour que leur surface paraisse plane dans la boîte) :
  // à 85 unités du point le plus près, la surface d'une sphère de rayon r s'écarte d'un plan d'environ 85² / 2r = 0.04 unité,
  // ce qui est invisible dans une boîte de 100 unités de large
  constexpr double wall_radius = 100000.0;

  // centre de la boîte de Cornell (voir ses limites au début du programme)
  constexpr double box_center_x = (box_x_min + box_x_max) / 2.0;
  constexpr double box_center_y = (box_y_min + box_y_max) / 2.0;

  // coordonnée z du centre des sphères du plancher, du plafond, des murs gauche et droit et de la sphère lumineuse (valeur de smallpt)
  constexpr double wall_center_z = 81.6;

  // couleurs des murs (valeurs de smallpt, sauf le fond vert) et émission de la sphère lumineuse (blanche)
  Vector floor_color(0.75, 0.75, 0.75);
  Vector ceiling_color(0.75, 0.75, 0.75);
  Vector left_color(0.75, 0.25, 0.25);
  Vector back_color(0.25, 0.75, 0.25);
  Vector right_color(0.25, 0.25, 0.75);
  Vector light_emission(light_intensity, light_intensity, light_intensity);

  // couleurs aléatoires pour les murs (le mur avant reste noir) et pour la lumière
  if (random_box_colors)
  {
    floor_color = random_color(random_wall_color_min, random_wall_color_max);
    ceiling_color = random_color(random_wall_color_min, random_wall_color_max);
    left_color = random_color(random_wall_color_min, random_wall_color_max);
    back_color = random_color(random_wall_color_min, random_wall_color_max);
    right_color = random_color(random_wall_color_min, random_wall_color_max);

    // teinte aléatoire de la lumière
    light_emission = light_emission_from_color(random_color(random_color_min, random_color_max));
  }

  // couleurs choisies en paramètre, qui ont priorité sur les couleurs par défaut et sur les couleurs aléatoires
  if (floor_color_option.is_set)
    floor_color = floor_color_option.color;
  if (ceiling_color_option.is_set)
    ceiling_color = ceiling_color_option.color;
  if (left_color_option.is_set)
    left_color = left_color_option.color;
  if (back_color_option.is_set)
    back_color = back_color_option.color;
  if (right_color_option.is_set)
    right_color = right_color_option.color;
  if (light_color_option.is_set)
    light_emission = light_emission_from_color(light_color_option.color);

  // vider la scène de son contenu
  scene.clear();

  // génération du contenu de la scène
  scene.insert(scene.begin(), {

    // approximation d'une boîte de Cornell avec des sphères surdimensionnées qui simulent des surfaces planes :
    // le centre de chaque sphère est placé à une distance égale à son rayon de la position du mur, vers l'intérieur de la boîte,
    // donc la boîte est à l'intérieur de chacune de ces sphères et un rayon frappe le mur au moment où il sort de la sphère
    Sphere(wall_radius, Vector(box_center_x,            box_y_min + wall_radius, wall_center_z),           Vector(), floor_color,              SurfaceType::diffuse), // plancher
    Sphere(wall_radius, Vector(box_center_x,            box_y_max - wall_radius, wall_center_z),           Vector(), ceiling_color,            SurfaceType::diffuse), // plafond
    Sphere(wall_radius, Vector(box_x_min + wall_radius, box_center_y,            wall_center_z),           Vector(), left_color,               SurfaceType::diffuse), // mur gauche
    Sphere(wall_radius, Vector(box_center_x,            box_center_y,            box_z_min + wall_radius), Vector(), back_color,               SurfaceType::diffuse), // mur arrière
    Sphere(wall_radius, Vector(box_x_max - wall_radius, box_center_y,            wall_center_z),           Vector(), right_color,              SurfaceType::diffuse), // mur droit

    // mur avant noir (ni couleur, ni émission) : la vraie boîte de Cornell est ouverte à l'avant, mais ici les sphères des autres murs
    // continuent bien au-delà de la boîte, et un rayon qui sortirait par l'avant finirait par les frapper beaucoup plus loin ;
    // le mur noir absorbe plutôt toute la lumière qui sort par l'avant, comme l'ouverture de la vraie boîte qui donne sur une pièce sombre
    // (vu de l'extérieur, ce mur cache l'intérieur de la boîte, d'où le décalage de l'origine des rayons de la caméra dans render_row)
    Sphere(wall_radius, Vector(box_center_x,            box_center_y,            box_z_max - wall_radius), Vector(), Vector(0.0,  0.0,  0.0),  SurfaceType::diffuse), // mur avant

    // sphère lumineuse de rayon 600 au-dessus du plafond, dont le bas dépasse du plafond de 0.27 unité :
    // la partie qui dépasse est une calotte d'environ 18 unités de rayon (racine(2 * 600 * 0.27)),
    // qui joue le rôle d'un disque lumineux presque plat au plafond
    Sphere(600,  Vector(box_center_x, 600.0 + box_y_max - 0.27, wall_center_z), light_emission, Vector(0.0, 0.0, 0.0), SurfaceType::diffuse) // sphère lumineuse
  });

  // index de la première sphère située à l'intérieur de la boîte de Cornell
  int first_inner_index = static_cast<int>(scene.size());

  // ensemble des sphères situées à l'intérieur de la boîte de Cornell, dont la position et le rayon sont en paramètre
  // couleur des sphères de chaque type
  Vector mirror_color(mirror_r, mirror_g, mirror_b);
  Vector glass_color(glass_r, glass_g, glass_b);

  scene.push_back(Sphere(mirror_radius, Vector(mirror_x, mirror_y, mirror_z), Vector(), mirror_color, SurfaceType::specular));  // sphère miroir
  scene.push_back(Sphere(glass_radius,  Vector(glass_x,  glass_y,  glass_z),  Vector(), glass_color,  SurfaceType::refraction)); // sphère de verre

  // sphères additionnelles de position et de rayon aléatoires
  for (int index = 1; index < mirror_count; ++index)
    if (!add_random_sphere(SurfaceType::specular, mirror_color, first_inner_index))
      std::cout << "no free space for an additional mirror sphere" << std::endl;

  for (int index = 1; index < glass_count; ++index)
    if (!add_random_sphere(SurfaceType::refraction, glass_color, first_inner_index))
      std::cout << "no free space for an additional glass sphere" << std::endl;

  std::cout << "scene with " << mirror_count << " mirror sphere(s) and " << glass_count << " glass sphere(s)" << std::endl;

  // allocation de la mémoire de l'image en fonction des paramètres du programme
  image.resize(image_width, image_height);

  std::cout << "image resize to " << image.width << "x" << image.height << " (" << image.count << " pixels, " << image.size << " MB)" << std::endl;

  // orienter la caméra vers le point visé s'il a été choisi en paramètre
  if (camera_target_is_set)
    camera.look_at(camera.target);

  // calibration de la caméra
  camera.viewport_width = image.width;
  camera.viewport_height = image.height;
  camera.fov = camera_fov;
  camera.calibrate();
}

// fonction appelée quand le rendu de la scène est terminé
void post_render()
{
  // écrire les pixels dans un fichier image
  save_image_file(image.width, image.height, ray_per_pixel, image.pixel);

  std::cout << "raytracer task is done" << std::endl;
}

// fonction qui rend une rangée de pixels de l'image
void render_row(int y, std::mt19937& generator)
{
  double r1, r2, dx, dy;

  Vector radiance;
  Vector direction;
  Vector origin;

  // itération sur les colonnes de pixels
  for (int x = 0; x < image_width; ++x)
  {
    // déterminer l'index du pixel
    int index = (image_height - y - 1) * image_width + x;

    // chaque pixel est divisé en un bloc de 2x2 sous-pixels, et chaque sous-pixel reçoit ray_per_pixel rayons,
    // donc un pixel reçoit 4 * ray_per_pixel rayons au total

    // itération sur les rangées du bloc de 2x2 échantillons
    for (int sy = 0; sy < 2; ++sy)
    {
      // itération sur les colonnes du bloc de 2x2 échantillons
      for (int sx = 0; sx < 2; ++sx)
      {
        // initialiser la radiance
        radiance = Vector();

        // itération sur les rayons par pixel
        for (int s = 0; s < ray_per_pixel; ++s)
        {
          // filtre en tente
          r1 = 2.0 * random01(generator);
          dx = r1 < 1.0 ? sqrt(r1) - 1.0 : 1.0 - sqrt(2.0 - r1);

          r2 = 2.0 * random01(generator);
          dy = r2 < 1.0 ? sqrt(r2) - 1.0 : 1.0 - sqrt(2.0 - r2);

          // calculer la direction du rayon de l'échantillon
          direction = camera.axis_x * (((sx + 0.5 + dx) / 2.0 + x) / image_width - 0.5) +
                      camera.axis_y * (((sy + 0.5 + dy) / 2.0 + y) / image_height - 0.5) + camera.axis_z;

          // calculer l'origine du rayon avant de normaliser sa direction
          // par défaut, la caméra est à z = 300, devant le mur avant noir de la boîte de Cornell (z = 170) : un rayon lancé à partir
          // de la caméra frapperait l'extérieur de ce mur et l'image serait noire, donc l'origine du rayon est avancée jusqu'à l'intérieur
          // de la boîte, juste derrière le mur avant (smallpt l'avance plutôt d'une distance fixe de 140 unités) ;
          // la caméra reste ainsi loin de la boîte avec un champ de vision étroit, comme pour photographier la vraie boîte de Cornell
          // à travers son ouverture, et les origines de tous les rayons sont sur le plan de l'ouverture, ce qui joue le rôle
          // du plan d'occlusion avant (near) du frustum de vision
          // si la caméra est à l'intérieur de la boîte, les rayons partent simplement de la caméra
          double start_distance = 0.0;

          // indicateur qu'un rayon d'une caméra placée devant la boîte passe à côté de son ouverture
          bool outside_opening = false;

          if (camera.position.z > box_z_max)
          {
            if (direction.z < 0.0)
              start_distance = (camera.position.z - (box_z_max - front_wall_margin)) / -direction.z;

            origin = camera.position + direction * start_distance;

            // un rayon qui ne va pas vers la boîte, ou qui passe à côté de son ouverture, voit seulement la pièce sombre
            // autour de la boîte (sinon, il partirait de l'extérieur des murs ou de l'intérieur de la sphère lumineuse)
            outside_opening = direction.z >= 0.0 ||
              origin.x <= box_x_min || origin.x >= box_x_max ||
              origin.y <= box_y_min || origin.y >= box_y_max;
          }
          else
            origin = camera.position;

          // normaliser la direction du rayon
          direction.normalize();

          // appel récursif du calcul de la radiance (noir pour un rayon qui passe à côté de l'ouverture de la boîte)
          if (!outside_opening)
            radiance = radiance + compute_radiance(Ray(origin, direction), 0, generator) * (1.0 / ray_per_pixel);
        }

        // chaque pixel appartient à une seule rangée, donc un seul fil écrit à cet endroit de l'image
        image.pixel[index] = image.pixel[index] + Vector(clamp(radiance.x), clamp(radiance.y), clamp(radiance.z)) * 0.25;
      }
    }
  }
}

// fonction de rendu de la scène, répartie sur plusieurs fils d'exécution
void render()
{
  std::cout << "render start (" << thread_count << " threads)" << std::endl;

  // prochaine rangée à rendre, partagée entre les fils (répartition dynamique : un fil qui termine prend la rangée suivante)
  std::atomic<int> next_row(0);

  // nombre de rangées terminées, pour afficher la progression
  std::atomic<int> rows_done(0);

  // fonction exécutée par chaque fil
  auto worker = [&](int thread_index)
  {
    int y;

    while ((y = next_row++) < image_height)
    {
      // générateur propre à la rangée, dont la graine dépend seulement de la graine globale et de la rangée :
      // le résultat est donc le même peu importe le fil qui rend la rangée et l'ordre des rangées
      std::mt19937 generator(seed + static_cast<std::uint32_t>(y));

      render_row(y, generator);

      int done = ++rows_done;

      // seul le premier fil affiche le pourcentage de progression du rendu dans la console
      if (thread_index == 0)
        fprintf(stderr, "\rraytracing (%d rays per pixel) : %4.1f %%", ray_per_pixel, 100.0f * done / image_height);
    }
  };

  // lancer les fils d'exécution
  std::vector<std::thread> threads;

  for (int index = 0; index < thread_count; ++index)
    threads.emplace_back(worker, index);

  // attendre la fin de tous les fils
  for (std::thread& thread : threads)
    thread.join();

  fprintf(stderr, "\rraytracing (%d rays per pixel) : %4.1f %%", ray_per_pixel, 100.0f);

  std::cout << "\nrender done" << std::endl;
}

// fonction de mise à jour
void update()
{
  auto t1 = std::chrono::high_resolution_clock::now();

  // rendu de la scène
  render();

  auto t2 = std::chrono::high_resolution_clock::now();

  // temps de calcul
  auto render_time = t2 - t1;

  std::cout << "render time : " << std::chrono::duration_cast<std::chrono::seconds>(render_time).count() << "s" << std::endl;
}

// fonction qui lit les 3 composantes d'une couleur en paramètre, à la suite de l'option à l'index courant
// les composantes sont bornées entre 0 et 1, car une couleur plus grande que 1 ajouterait de l'énergie à chaque rebond
ColorOption parse_color_option(char* argv[], int& index)
{
  ColorOption option;

  // trois instructions séparées, pour lire les composantes dans l'ordre
  double r = clamp(atof(argv[++index]));
  double g = clamp(atof(argv[++index]));
  double b = clamp(atof(argv[++index]));

  option.color = Vector(r, g, b);
  option.is_set = true;

  return option;
}

// fonction qui lit les 3 coordonnées d'un vecteur en paramètre, à la suite de l'option à l'index courant
Vector parse_vector(char* argv[], int& index)
{
  // trois instructions séparées, pour lire les coordonnées dans l'ordre
  double x = atof(argv[++index]);
  double y = atof(argv[++index]);
  double z = atof(argv[++index]);

  return Vector(x, y, z);
}

// fonction qui retourne le dossier d'un chemin de fichier (ex. le dossier de l'exécutable à partir de argv[0])
std::string directory_of(const std::string& path)
{
  // position du dernier séparateur de dossier (/ sous macOS et Linux, \ ou / sous Windows)
  std::string::size_type position = path.find_last_of("/\\");

  // sans séparateur, le programme a été lancé à partir du dossier courant (ou trouvé dans le PATH)
  if (position == std::string::npos)
    return ".";

  return path.substr(0, position);
}

// fonction qui valide qu'on peut écrire dans le dossier de sortie, avant un rendu qui peut durer longtemps
bool validate_output_directory()
{
  // créer puis supprimer un fichier de test dans le dossier de sortie
  std::string test_filename = output_directory + "/.raytracer_write_test";
  std::ofstream test_file(test_filename);

  if (!test_file.is_open())
  {
    std::cerr << "error: cannot write in the output directory " << output_directory << std::endl;
    return false;
  }

  test_file.close();
  std::remove(test_filename.c_str());

  return true;
}

// fonction qui valide la position et l'orientation de la caméra
bool validate_camera()
{
  const Vector& p = camera.position;

  // la caméra doit être dans la boîte, ou devant son ouverture : ailleurs, elle verrait l'extérieur des sphères des murs
  if (p.x <= box_x_min || p.x >= box_x_max || p.y <= box_y_min || p.y >= box_y_max || p.z <= box_z_min)
  {
    std::cerr << "error: the camera must be inside the box or in front of its opening (" << box_x_min << " < x < " << box_x_max << ", "
              << box_y_min << " < y < " << box_y_max << ", z > " << box_z_min << ")" << std::endl;
    return false;
  }

  if (camera_target_is_set)
  {
    Vector delta = camera.target - p;

    // le point visé doit être différent de la position de la caméra
    if (delta.dot(delta) == 0.0)
    {
      std::cerr << "error: the camera target must be different from the camera position" << std::endl;
      return false;
    }

    // la caméra ne doit pas viser directement vers le haut ou vers le bas, car le vecteur up ne permettrait plus de trouver l'axe x
    if (fabs(delta.normalize().dot(camera.up)) > 0.999)
    {
      std::cerr << "error: the camera cannot look straight up or down" << std::endl;
      return false;
    }
  }

  return true;
}

// fonction principale du programme
int main(int argc, char* argv[])
{
  std::cout << "raytracer start" << std::endl;

  std::cout << "parse program arguments:" << std::endl;

  std::cout << "argc:\t" << argc << std::endl;

  for (int index = 0; index < argc; ++index)
    std::cout << "arg[" << index << "]:\t" << argv[index] << std::endl;

  // arguments positionnels (dimensions de l'image et nombre de rayons par pixel)
  char* positional[3];
  int positional_count = 0;

  // séparer les options des arguments positionnels
  for (int index = 1; index < argc; ++index)
  {
    std::string arg = argv[index];

    if (arg == "--mirror" && index + 4 < argc)
    {
      mirror_x = atof(argv[++index]);
      mirror_y = atof(argv[++index]);
      mirror_z = atof(argv[++index]);
      mirror_radius = atof(argv[++index]);
    }
    else if (arg == "--glass" && index + 4 < argc)
    {
      glass_x = atof(argv[++index]);
      glass_y = atof(argv[++index]);
      glass_z = atof(argv[++index]);
      glass_radius = atof(argv[++index]);
    }
    else if (arg == "--mirror-color" && index + 3 < argc)
    {
      // composantes bornées entre 0 et 1, car une couleur plus grande que 1 ajouterait de l'énergie à chaque rebond
      mirror_r = clamp(atof(argv[++index]));
      mirror_g = clamp(atof(argv[++index]));
      mirror_b = clamp(atof(argv[++index]));
    }
    else if (arg == "--glass-color" && index + 3 < argc)
    {
      glass_r = clamp(atof(argv[++index]));
      glass_g = clamp(atof(argv[++index]));
      glass_b = clamp(atof(argv[++index]));
    }
    else if (arg == "--floor-color" && index + 3 < argc)
      floor_color_option = parse_color_option(argv, index);
    else if (arg == "--ceiling-color" && index + 3 < argc)
      ceiling_color_option = parse_color_option(argv, index);
    else if (arg == "--left-color" && index + 3 < argc)
      left_color_option = parse_color_option(argv, index);
    else if (arg == "--back-color" && index + 3 < argc)
      back_color_option = parse_color_option(argv, index);
    else if (arg == "--right-color" && index + 3 < argc)
      right_color_option = parse_color_option(argv, index);
    else if (arg == "--light-color" && index + 3 < argc)
      light_color_option = parse_color_option(argv, index);
    else if (arg == "--camera" && index + 3 < argc)
      camera.position = parse_vector(argv, index);
    else if (arg == "--target" && index + 3 < argc)
    {
      camera.target = parse_vector(argv, index);
      camera_target_is_set = true;
    }
    else if (arg == "--fov" && index + 1 < argc)
    {
      // convertir l'angle vertical en hauteur de l'image à une distance de 1 devant la caméra
      double angle = atof(argv[++index]);

      if (angle <= 0.0 || angle >= 180.0)
      {
        std::cerr << "error: the field of view must be between 0 and 180 degrees" << std::endl;
        return 1;
      }

      camera_fov = 2.0 * tan(angle * M_PI / 180.0 / 2.0);
    }
    else if (arg == "--random-colors")
      random_colors = true;
    else if (arg == "--random-box-colors")
      random_box_colors = true;
    else if (arg == "--threads" && index + 1 < argc)
      thread_count = atoi(argv[++index]);
    else if (arg == "--output" && index + 1 < argc)
      output_directory = argv[++index];
    else if (arg == "--seed" && index + 1 < argc)
    {
      seed = static_cast<std::uint32_t>(strtoul(argv[++index], nullptr, 10));
      seed_is_set = true;
    }
    else if (arg == "--mirror-count" && index + 1 < argc)
      mirror_count = atoi(argv[++index]);
    else if (arg == "--glass-count" && index + 1 < argc)
      glass_count = atoi(argv[++index]);
    else if (arg.compare(0, 2, "--") == 0)
    {
      // option inconnue (ex. faute de frappe) ou option à laquelle il manque des valeurs
      std::cerr << "error: unknown option or missing values: " << arg << std::endl;
      std::cerr << "usage: " << argv[0] << " image_width image_height ray_per_pixel [options]" << std::endl;
      return 1;
    }
    else if (positional_count < 3)
      positional[positional_count++] = argv[index];
    else
    {
      // il y a au plus 3 arguments positionnels
      std::cerr << "error: too many arguments: " << arg << std::endl;
      std::cerr << "usage: " << argv[0] << " image_width image_height ray_per_pixel [options]" << std::endl;
      return 1;
    }
  }

  // nombre de fils par défaut selon le nombre de cœurs de l'ordinateur
  if (thread_count < 1)
    thread_count = static_cast<int>(std::thread::hardware_concurrency());
  if (thread_count < 1)
    thread_count = 1;

  // graine aléatoire si elle n'est pas choisie en paramètre
  if (!seed_is_set)
  {
    std::random_device rd;
    seed = rd();
  }

  // initialiser le générateur de la scène avec la graine, pour que les sphères aléatoires soient aussi reproductibles
  rng.seed(seed);

  std::cout << "seed : " << seed << " (option --seed " << seed << " pour reproduire ce rendu)" << std::endl;

  // au moins une sphère de chaque type
  if (mirror_count < 1)
    mirror_count = 1;
  if (glass_count < 1)
    glass_count = 1;

  switch (positional_count)
  {
    case 0:
      image_width = 256;
      image_height = 256;
      ray_per_pixel = 16;
      break;

    case 1:
      image_width = 256;
      image_height = 256;
      ray_per_pixel = atoi(positional[0]);
      break;

    case 2:
      image_width = atoi(positional[0]);
      image_height = image_width;
      ray_per_pixel = atoi(positional[1]);
      break;

    default:
    case 3:
      image_width = atoi(positional[0]);
      image_height = atoi(positional[1]);
      ray_per_pixel = atoi(positional[2]);
      break;
  }

  // valider les dimensions de l'image et le nombre de rayons par pixel (atoi retourne 0 si l'argument n'est pas un nombre)
  if (image_width <= 0 || image_height <= 0 || ray_per_pixel <= 0)
  {
    std::cerr << "error: image width, image height and ray per pixel must be greater than 0" << std::endl;
    std::cerr << "usage: " << argv[0] << " image_width image_height ray_per_pixel [options]" << std::endl;
    return 1;
  }

  // valider la position et l'orientation de la caméra
  if (!validate_camera())
    return 1;

  // par défaut, l'image est sauvegardée dans le dossier de l'exécutable, peu importe le dossier courant
  if (output_directory.empty())
    output_directory = directory_of(argv[0]);

  std::cout << "output directory: " << output_directory << std::endl;

  // valider le dossier de sortie avant le rendu
  if (!validate_output_directory())
    return 1;

  std::cout << "program init done" << std::endl;

  // initialisation de la scène
  init();

  // faire un rendu de la scène par lancer de rayons
  update();

  // procédure post-rendu (sauvegarde de l'image)
  post_render();
}
