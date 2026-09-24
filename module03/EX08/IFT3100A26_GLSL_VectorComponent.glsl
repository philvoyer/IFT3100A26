// IFT3100A26_GLSL_VectorComponent.glsl
// Exemples d’accès aux composantes d’un vecteur en GLSL.

// déclaration de variables
vec3 v = vec3(1.0, 2.0, 3.0, 4.0);
vec3 result;
float component;

// 1. lecture par index de composante
component = v[0];
component = v[1];
component = v[2];
component = v[3];

// 2. écriture par index de composante
v[0] = 1.0;
v[1] = 2.0;
v[2] = 3.0;
v[3] = 4.0;

// 3. lecture par étiquette de composante de transformation (XYZW)
component = v.x;
component = v.y;
component = v.z;
component = v.w;

// 4. écriture par étiquette de composante de transformation (XYZW)
v.x = 1.0;
v.y = 2.0;
v.z = 3.0;
v.w = 4.0;

// 5. lecture par étiquette de composante de couleur (RGBA)
component = v.r;
component = v.g;
component = v.b;
component = v.a;

// 6. écriture par étiquette de composante de couleur (RGBA)
v.r = 1.0;
v.g = 2.0;
v.b = 3.0;
v.a = 3.0;

// 7. lecture par étiquette de composante de texture (STPQ)
component = v.s;
component = v.t;
component = v.p;
component = v.q;

// 8. écriture par étiquette de composante de texture (STPQ)
v.s = 1.0;
v.t = 2.0;
v.p = 3.0;
v.q = 4.0;

// 9. swizzle par étiquette de composante de transformation (XYZW)
result = v.xyz; // result = (1.0, 2.0, 3.0)
result = v.zyx; // result = (3.0, 2.0, 1.0)
result = v.xzy; // result = (1.0, 3.0, 2.0)
result = v.yxz; // result = (2.0, 1.0, 3.0)
result = v.xxx; // result = (1.0, 1.0, 1.0)

// 10. swizzle par étiquette de composante de couleur (RGBA)
result = v.rgb; // result = (1.0, 2.0, 3.0)
result = v.bgr; // result = (3.0, 2.0, 1.0)
result = v.rbg; // result = (1.0, 3.0, 2.0)
result = v.grb; // result = (2.0, 1.0, 3.0)
result = v.rrr; // result = (1.0, 1.0, 1.0)

// 11. swizzle par étiquette de composante de texture (STPQ)
result = v.stp;  // result = (1.0, 2.0, 3.0)
result = v.pts;  // result = (3.0, 2.0, 1.0)
result = v.spt;  // result = (1.0, 3.0, 2.0)
result = v.tsp;  // result = (2.0, 1.0, 3.0)
result = v.sss;  // result = (1.0, 1.0, 1.0)
