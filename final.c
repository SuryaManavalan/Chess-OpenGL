/* Surya Manavalan
 *  Key bindings:
  arrows     Change view angle in overhead
  o/p        Zoom in and out in overhead
  ESC        Exit
  Type in commands to move pieces in the format e2,e4
 */
#include "CSCIx229.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#ifdef USEGLEW
#include <GL/glew.h>
#endif
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
// Tell Xcode IDE to not gripe about OpenGL deprecation
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glut.h>
#endif
//  Default resolution
//  For Retina displays compile with -DRES=2
#ifndef RES
#define RES 1
#endif

int th = 90;      //  Azimuth of view angle
int ph = 55;      //  Elevation of view angle
int axes = 0;     //  Display axes
int mode = 1;     //  Projection mode
int fov = 55;     //  Field of view (for perspective)
double asp = 1;   //  Aspect ratio
double dim = 5.9; //  Size of world
int Tmode = 0;    //  Texture mode

/* Eye coords */
double Ex = 0;
double Ey = 0.75;
double Ez = 5;

/*forward view*/
double Cx = 0;
double Cz = 0;

double t = 0;            // Time
int move = 0;            // Toggle move
int light = 1;           // Toggle light
int local = 0;           // Local Viewer Model
int distance = 3;        // Light distance
int smooth = 1;          // Shading smooth or flat
int emission = 0;        // Emission intensity value
int ambient = 30;        // Ambient intensity value
int diffuse = 100;       // Diffuse intensity value
int specular = 0;        // Specular intensity value
int shininess = 0;       // Shininess (power of two)
float shiny = 1;         // Shininess value
int zh = 90;             // Light azimuth
float ylight = 8;        // Light elevation
unsigned int texture[7]; // Texture names

char typed[5] = "";
int cursor = 0;

int turn = 0; // 0 for white, 1 for black, 2 for game over
int moveCount = 1;

//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.14159265 / 180))
#define Sin(x) (sin((x)*3.14159265 / 180))
#define Acos(x) (acos((x)) * 180 / 3.1415927)
#define Asin(x) (asin((x)) * 180 / 3.1415927)
#define Atan(x) (atan((x)) * 180 / 3.1415927)

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192 //  Maximum length of text string

// Define the size of the chess board
#define BOARD_SIZE 8

// Define the pieces
enum PieceType
{
   EMPTY,
   PAWN,
   KNIGHT,
   BISHOP,
   ROOK,
   QUEEN,
   KING
};

// Define the colors
enum PieceColor
{
   WHITE,
   BLACK,
   NONE
};

// Define a structure to represent a position on the board
struct PiecePosition
{
   int row;
   int col;
};

// Define a structure to represent a chess piece
struct BoardPiece
{
   enum PieceType type;
   enum PieceColor color;
   struct PiecePosition pos;
};

// Declare the chess board as a global variable
struct BoardPiece board[BOARD_SIZE][BOARD_SIZE];

void Print(const char *format, ...)
{
   char buf[LEN];
   char *ch = buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args, format);
   vsnprintf(buf, LEN, format, args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *ch++);
}

//  2D vector
typedef struct
{
   float x, y;
} vec2;

//  Piece position
typedef struct
{
   int x, y, c;
} ppos;

#define Np 50
vec2 pawn[Np] = {
    {0.29, 0.004},
    {0.305, 0.025},
    {0.308, 0.046},
    {0.3, 0.067},
    {0.276, 0.089},
    {0.302, 0.11},
    {0.316, 0.132},
    {0.318, 0.153},
    {0.311, 0.174},
    {0.292, 0.195},
    {0.223, 0.216},
    {0.221, 0.237},
    {0.219, 0.258},
    {0.211, 0.281},
    {0.199, 0.302},
    {0.181, 0.323},
    {0.153, 0.344},
    {0.142, 0.365},
    {0.132, 0.386},
    {0.122, 0.407},
    {0.115, 0.428},
    {0.109, 0.449},
    {0.103, 0.47},
    {0.098, 0.492},
    {0.094, 0.517},
    {0.09, 0.538},
    {0.087, 0.559},
    {0.086, 0.58},
    {0.084, 0.601},
    {0.087, 0.622},
    {0.14, 0.643},
    {0.148, 0.664},
    {0.141, 0.685},
    {0.101, 0.706},
    {0.122, 0.727},
    {0.133, 0.748},
    {0.143, 0.769},
    {0.15, 0.79},
    {0.155, 0.811},
    {0.156, 0.832},
    {0.156, 0.853},
    {0.15, 0.874},
    {0.142, 0.895},
    {0.133, 0.916},
    {0.119, 0.937},
    {0.098, 0.958},
    {0.071, 0.979},
    {0.001, 1.0},
};
vec2 bishop[Np] = {
    {0.221, 0.01},
    {0.221, 0.031},
    {0.219, 0.052},
    {0.219, 0.073},
    {0.23, 0.094},
    {0.231, 0.115},
    {0.231, 0.136},
    {0.226, 0.157},
    {0.188, 0.178},
    {0.155, 0.199},
    {0.147, 0.22},
    {0.144, 0.241},
    {0.122, 0.262},
    {0.113, 0.283},
    {0.107, 0.304},
    {0.102, 0.325},
    {0.097, 0.346},
    {0.092, 0.367},
    {0.088, 0.388},
    {0.084, 0.409},
    {0.081, 0.43},
    {0.079, 0.451},
    {0.079, 0.472},
    {0.079, 0.493},
    {0.101, 0.514},
    {0.136, 0.535},
    {0.143, 0.556},
    {0.136, 0.577},
    {0.092, 0.598},
    {0.074, 0.619},
    {0.086, 0.64},
    {0.076, 0.661},
    {0.089, 0.682},
    {0.097, 0.703},
    {0.105, 0.724},
    {0.112, 0.745},
    {0.115, 0.766},
    {0.114, 0.787},
    {0.11, 0.808},
    {0.099, 0.829},
    {0.083, 0.85},
    {0.066, 0.871},
    {0.047, 0.892},
    {0.026, 0.913},
    {0.015, 0.934},
    {0.026, 0.955},
    {0.026, 0.976},
    {0.0, 0.997},
};
vec2 king[Np] = {
    {0.191, 0.011},
    {0.191, 0.032},
    {0.181, 0.053},
    {0.193, 0.074},
    {0.182, 0.095},
    {0.169, 0.117},
    {0.146, 0.138},
    {0.136, 0.159},
    {0.121, 0.18},
    {0.117, 0.201},
    {0.119, 0.222},
    {0.104, 0.243},
    {0.089, 0.264},
    {0.079, 0.285},
    {0.072, 0.306},
    {0.069, 0.327},
    {0.065, 0.348},
    {0.06, 0.369},
    {0.059, 0.39},
    {0.054, 0.411},
    {0.056, 0.432},
    {0.051, 0.453},
    {0.051, 0.474},
    {0.051, 0.495},
    {0.051, 0.516},
    {0.051, 0.537},
    {0.057, 0.558},
    {0.137, 0.579},
    {0.103, 0.6},
    {0.08, 0.621},
    {0.071, 0.642},
    {0.077, 0.663},
    {0.074, 0.684},
    {0.08, 0.705},
    {0.088, 0.726},
    {0.097, 0.747},
    {0.104, 0.768},
    {0.113, 0.789},
    {0.12, 0.81},
    {0.058, 0.831},
    {0.042, 0.852},
    {0.041, 0.873},
    {0.037, 0.894},
    {0.064, 0.915},
    {0.064, 0.936},
    {0.028, 0.957},
    {0.03, 0.978},
    {0.003, 0.999},
};
vec2 queen[Np] = {
    {0.222, 0.02},
    {0.219, 0.041},
    {0.203, 0.062},
    {0.217, 0.083},
    {0.214, 0.104},
    {0.193, 0.125},
    {0.177, 0.146},
    {0.158, 0.167},
    {0.144, 0.189},
    {0.128, 0.21},
    {0.129, 0.231},
    {0.133, 0.252},
    {0.113, 0.273},
    {0.098, 0.294},
    {0.091, 0.316},
    {0.086, 0.338},
    {0.081, 0.359},
    {0.077, 0.38},
    {0.072, 0.401},
    {0.072, 0.422},
    {0.064, 0.444},
    {0.064, 0.465},
    {0.064, 0.486},
    {0.064, 0.507},
    {0.062, 0.528},
    {0.058, 0.549},
    {0.062, 0.571},
    {0.059, 0.595},
    {0.062, 0.616},
    {0.134, 0.637},
    {0.145, 0.658},
    {0.115, 0.679},
    {0.082, 0.7},
    {0.08, 0.721},
    {0.092, 0.742},
    {0.082, 0.763},
    {0.085, 0.784},
    {0.084, 0.806},
    {0.088, 0.827},
    {0.088, 0.848},
    {0.096, 0.871},
    {0.107, 0.892},
    {0.128, 0.913},
    {0.03, 0.935},
    {0.027, 0.956},
    {0.025, 0.977},
    {0.0, 0.998},
};
vec2 rook[Np] = {
    {0.284, 0.013},
    {0.292, 0.034},
    {0.32, 0.055},
    {0.327, 0.076},
    {0.298, 0.097},
    {0.282, 0.118},
    {0.295, 0.139},
    {0.304, 0.16},
    {0.298, 0.181},
    {0.281, 0.202},
    {0.247, 0.223},
    {0.228, 0.244},
    {0.224, 0.265},
    {0.213, 0.286},
    {0.216, 0.307},
    {0.198, 0.328},
    {0.192, 0.349},
    {0.188, 0.37},
    {0.182, 0.391},
    {0.176, 0.412},
    {0.171, 0.433},
    {0.168, 0.454},
    {0.168, 0.475},
    {0.162, 0.496},
    {0.158, 0.517},
    {0.157, 0.538},
    {0.155, 0.559},
    {0.154, 0.58},
    {0.154, 0.601},
    {0.151, 0.622},
    {0.151, 0.643},
    {0.151, 0.664},
    {0.151, 0.685},
    {0.153, 0.706},
    {0.161, 0.727},
    {0.175, 0.748},
    {0.181, 0.769},
    {0.193, 0.79},
    {0.193, 0.811},
    {0.208, 0.832},
    {0.054, 0.853},
    {0.052, 0.874},
    {0.052, 0.895},
    {0.052, 0.916},
    {0.052, 0.937},
    {0.052, 0.958},
    {0.053, 0.979},
    {0.0, 1.0},
};
vec2 knight[Np] = {
    {0.293, 0.007},
    {0.305, 0.028},
    {0.309, 0.051},
    {0.295, 0.072},
    {0.277, 0.093},
    {0.306, 0.114},
    {0.317, 0.135},
    {0.318, 0.157},
    {0.308, 0.178},
    {0.277, 0.199},
    {0.223, 0.22},
    {0.22, 0.241},
    {0.218, 0.262},
    {0.21, 0.283},
    {0.197, 0.304},
    {0.18, 0.325},
    {0.153, 0.346},
    {0.139, 0.368},
    {0.13, 0.389},
    {0.121, 0.41},
    {0.116, 0.431},
    {0.107, 0.452},
    {0.104, 0.473},
};

vec2 knightHead[72] = {
    {0.352, 0.311},
    {0.328, 0.312},
    {0.305, 0.315},
    {0.307, 0.332},
    {0.313, 0.347},
    {0.32, 0.365},
    {0.328, 0.386},
    {0.332, 0.415},
    {0.333, 0.437},
    {0.331, 0.465},
    {0.326, 0.492},
    {0.323, 0.514},
    {0.316, 0.544},
    {0.31, 0.566},
    {0.306, 0.588},
    {0.302, 0.614},
    {0.3, 0.638},
    {0.306, 0.66},
    {0.31, 0.687},
    {0.319, 0.712},
    {0.333, 0.739},
    {0.345, 0.754},
    {0.351, 0.781},
    {0.369, 0.8},
    {0.397, 0.818},
    {0.414, 0.826},
    {0.437, 0.823},
    {0.45, 0.813},
    {0.466, 0.814},
    {0.481, 0.8},
    {0.491, 0.797},
    {0.519, 0.787},
    {0.548, 0.779},
    {0.569, 0.773},
    {0.597, 0.766},
    {0.627, 0.759},
    {0.665, 0.749},
    {0.695, 0.737},
    {0.707, 0.729},
    {0.714, 0.71},
    {0.719, 0.7},
    {0.714, 0.689},
    {0.716, 0.679},
    {0.715, 0.674},
    {0.707, 0.671},
    {0.707, 0.663},
    {0.699, 0.647},
    {0.699, 0.638},
    {0.7, 0.632},
    {0.7, 0.632},
    {0.685, 0.625},
    {0.663, 0.616},
    {0.652, 0.613},
    {0.642, 0.616},
    {0.62, 0.628},
    {0.603, 0.635},
    {0.584, 0.625},
    {0.564, 0.615},
    {0.543, 0.607},
    {0.532, 0.607},
    {0.53, 0.601},
    {0.543, 0.576},
    {0.561, 0.555},
    {0.576, 0.535},
    {0.59, 0.513},
    {0.607, 0.484},
    {0.626, 0.447},
    {0.639, 0.418},
    {0.648, 0.394},
    {0.656, 0.36},
    {0.63, 0.334},
    {0.605, 0.312},
};

ppos piecePos[32] = {
    {2, 1, 1},
    {2, 2, 1},
    {2, 3, 1},
    {2, 4, 1},
    {2, 5, 1},
    {2, 6, 1},
    {2, 7, 1},
    {2, 8, 1},
    {1, 1, 1},
    {1, 8, 1},
    {1, 2, 1},
    {1, 7, 1},
    {1, 3, 1},
    {1, 6, 1},
    {1, 4, 1},
    {1, 5, 1},
    {7, 1, 0},
    {7, 2, 0},
    {7, 3, 0},
    {7, 4, 0},
    {7, 5, 0},
    {7, 6, 0},
    {7, 7, 0},
    {7, 8, 0},
    {8, 1, 0},
    {8, 8, 0},
    {8, 2, 0},
    {8, 7, 0},
    {8, 3, 0},
    {8, 6, 0},
    {8, 4, 0},
    {8, 5, 0},
};

ppos piecePos_copy[32] = {
    {2, 1, 1},
    {2, 2, 1},
    {2, 3, 1},
    {2, 4, 1},
    {2, 5, 1},
    {2, 6, 1},
    {2, 7, 1},
    {2, 8, 1},
    {1, 1, 1},
    {1, 8, 1},
    {1, 2, 1},
    {1, 7, 1},
    {1, 3, 1},
    {1, 6, 1},
    {1, 4, 1},
    {1, 5, 1},
    {7, 1, 0},
    {7, 2, 0},
    {7, 3, 0},
    {7, 4, 0},
    {7, 5, 0},
    {7, 6, 0},
    {7, 7, 0},
    {7, 8, 0},
    {8, 1, 0},
    {8, 8, 0},
    {8, 2, 0},
    {8, 7, 0},
    {8, 3, 0},
    {8, 6, 0},
    {8, 4, 0},
    {8, 5, 0},
};

//  Make vec2 unit length
vec2 normalize(float x, float y)
{
   float len = sqrt(x * x + y * y);
   if (len > 0)
   {
      x /= len;
      y /= len;
   }
   return (vec2){x, y};
}

//  Calculate normals for Pieces
vec2 normPawn[Np];
vec2 normRook[Np];
vec2 normBishop[Np];
vec2 normQueen[Np];
vec2 normKing[Np];
vec2 normKnight[Np];

void CalcNormPawn()
{
   //  Calculate normals for each facet
   //  y increases with point index so dy>0
   for (int i = 0; i < Np - 2; i++)
   {
      // Vector in the plane of the facet
      float dx = pawn[i + 1].x - pawn[i].x;
      float dy = pawn[i + 1].y - pawn[i].y;
      // Normal is perpendicular
      // dy>0 so normal faces out
      normPawn[i] = normalize(dy, -dx);
   }
   //  The normal at the last point on the pawn is straight up
   //  This is NOT true in general
   normPawn[Np - 1] = (vec2){0, 1};

   //  Average normals of adjacent facets
   //  First and last normal unchanged
   vec2 N2 = normPawn[0];
   for (int i = 1; i < Np - 2; i++)
   {
      vec2 N1 = N2;
      N2 = normPawn[i];
      normPawn[i] = normalize(N1.x + N2.x, N1.y + N2.y);
   }
}
void CalcNormRook()
{
   //  Calculate normals for each facet
   //  y increases with point index so dy>0
   for (int i = 0; i < Np - 2; i++)
   {
      // Vector in the plane of the facet
      float dx = rook[i + 1].x - rook[i].x;
      float dy = rook[i + 1].y - rook[i].y;
      // Normal is perpendicular
      // dy>0 so normal faces out
      normRook[i] = normalize(dy, -dx);
   }
   //  The normal at the last point on the pawn is straight up
   //  This is NOT true in general
   normRook[Np - 1] = (vec2){0, 1};

   //  Average normals of adjacent facets
   //  First and last normal unchanged
   vec2 N2 = normRook[0];
   for (int i = 1; i < Np - 2; i++)
   {
      vec2 N1 = N2;
      N2 = normRook[i];
      normRook[i] = normalize(N1.x + N2.x, N1.y + N2.y);
   }
}
void CalcNormBishop()
{
   //  Calculate normals for each facet
   //  y increases with point index so dy>0
   for (int i = 0; i < Np - 2; i++)
   {
      // Vector in the plane of the facet
      float dx = bishop[i + 1].x - bishop[i].x;
      float dy = bishop[i + 1].y - bishop[i].y;
      // Normal is perpendicular
      // dy>0 so normal faces out
      normBishop[i] = normalize(dy, -dx);
   }
   //  The normal at the last point on the pawn is straight up
   //  This is NOT true in general
   normBishop[Np - 1] = (vec2){0, 1};

   //  Average normals of adjacent facets
   //  First and last normal unchanged
   vec2 N2 = normBishop[0];
   for (int i = 1; i < Np - 2; i++)
   {
      vec2 N1 = N2;
      N2 = normBishop[i];
      normBishop[i] = normalize(N1.x + N2.x, N1.y + N2.y);
   }
}
void CalcNormQueen()
{
   //  Calculate normals for each facet
   //  y increases with point index so dy>0
   for (int i = 0; i < Np - 2; i++)
   {
      // Vector in the plane of the facet
      float dx = queen[i + 1].x - queen[i].x;
      float dy = queen[i + 1].y - queen[i].y;
      // Normal is perpendicular
      // dy>0 so normal faces out
      normQueen[i] = normalize(dy, -dx);
   }
   //  The normal at the last point on the pawn is straight up
   //  This is NOT true in general
   normQueen[Np - 1] = (vec2){0, 1};

   //  Average normals of adjacent facets
   //  First and last normal unchanged
   vec2 N2 = normQueen[0];
   for (int i = 1; i < Np - 2; i++)
   {
      vec2 N1 = N2;
      N2 = normQueen[i];
      normQueen[i] = normalize(N1.x + N2.x, N1.y + N2.y);
   }
}
void CalcNormKing()
{
   //  Calculate normals for each facet
   //  y increases with point index so dy>0
   for (int i = 0; i < Np - 2; i++)
   {
      // Vector in the plane of the facet
      float dx = king[i + 1].x - king[i].x;
      float dy = king[i + 1].y - king[i].y;
      // Normal is perpendicular
      // dy>0 so normal faces out
      normKing[i] = normalize(dy, -dx);
   }
   //  The normal at the last point on the pawn is straight up
   //  This is NOT true in general
   normKing[Np - 1] = (vec2){0, 1};

   //  Average normals of adjacent facets
   //  First and last normal unchanged
   vec2 N2 = normKing[0];
   for (int i = 1; i < Np - 2; i++)
   {
      vec2 N1 = N2;
      N2 = normKing[i];
      normKing[i] = normalize(N1.x + N2.x, N1.y + N2.y);
   }
}
void CalcNormKnight()
{
   //  Calculate normals for each facet
   //  y increases with point index so dy>0
   for (int i = 0; i < Np - 2; i++)
   {
      // Vector in the plane of the facet
      float dx = knight[i + 1].x - knight[i].x;
      float dy = knight[i + 1].y - knight[i].y;
      // Normal is perpendicular
      // dy>0 so normal faces out
      normKnight[i] = normalize(dy, -dx);
   }
   //  The normal at the last point on the pawn is straight up
   //  This is NOT true in general
   normKnight[Np - 1] = (vec2){0, 1};

   //  Average normals of adjacent facets
   //  First and last normal unchanged
   vec2 N2 = normKnight[0];
   for (int i = 1; i < Np - 2; i++)
   {
      vec2 N1 = N2;
      N2 = normKnight[i];
      normKnight[i] = normalize(N1.x + N2.x, N1.y + N2.y);
   }
}

// isvalidmove function prototype
int is_valid_move(struct BoardPiece piece, struct PiecePosition from, struct PiecePosition to);
int is_check(struct BoardPiece king, struct PiecePosition king_pos);

void init_board()
{
   // Set all squares to empty
   for (int row = 0; row < BOARD_SIZE; row++)
   {
      for (int col = 0; col < BOARD_SIZE; col++)
      {
         board[row][col] = (struct BoardPiece){EMPTY, NONE, {row, col}};
      }
   }

   // Set the white pieces
   board[0][0] = (struct BoardPiece){ROOK, WHITE, {0, 0}};
   board[0][1] = (struct BoardPiece){KNIGHT, WHITE, {0, 1}};
   board[0][2] = (struct BoardPiece){BISHOP, WHITE, {0, 2}};
   board[0][3] = (struct BoardPiece){QUEEN, WHITE, {0, 3}};
   board[0][4] = (struct BoardPiece){KING, WHITE, {0, 4}};
   board[0][5] = (struct BoardPiece){BISHOP, WHITE, {0, 5}};
   board[0][6] = (struct BoardPiece){KNIGHT, WHITE, {0, 6}};
   board[0][7] = (struct BoardPiece){ROOK, WHITE, {0, 7}};
   for (int col = 0; col < BOARD_SIZE; col++)
   {
      board[1][col] = (struct BoardPiece){PAWN, WHITE, {1, col}};
   }

   // Set the black pieces
   board[7][0] = (struct BoardPiece){ROOK, BLACK, {7, 0}};
   board[7][1] = (struct BoardPiece){KNIGHT, BLACK, {7, 1}};
   board[7][2] = (struct BoardPiece){BISHOP, BLACK, {7, 2}};
   board[7][3] = (struct BoardPiece){QUEEN, BLACK, {7, 3}};
   board[7][4] = (struct BoardPiece){KING, BLACK, {7, 4}};
   board[7][5] = (struct BoardPiece){BISHOP, BLACK, {7, 5}};
   board[7][6] = (struct BoardPiece){KNIGHT, BLACK, {7, 6}};
   board[7][7] = (struct BoardPiece){ROOK, BLACK, {7, 7}};
   for (int col = 0; col < BOARD_SIZE; col++)
   {
      board[6][col] = (struct BoardPiece){PAWN, BLACK, {6, col}};
   }
}

void move_piece(struct BoardPiece piece, struct PiecePosition from, struct PiecePosition to)
{
   // Update the piece's position on the board
   board[to.row][to.col] = piece;

   // Update the piece's position
   piece.pos = to;

   // Set the original square to empty
   board[from.row][from.col].type = EMPTY;

   // Set the original square's color to none
   board[from.row][from.col].color = NONE;
}

void move_back(struct BoardPiece piece, struct BoardPiece temp, struct PiecePosition from, struct PiecePosition to)
{
   // Update the piece's position on the board
   board[to.row][to.col] = piece;
   piece.pos = to;

   // Update the piece's position
   board[from.row][from.col] = temp;
   temp.pos = from;
}

int is_valid_pawn_move(struct BoardPiece pawn, struct PiecePosition from, struct PiecePosition to)
{
   // Check if the piece is a pawn
   if (pawn.type != PAWN)
   {
      return 0;
   }

   // Check if the destination square is occupied by a piece of the same color
   if (board[to.row][to.col].color == pawn.color)
   {
      return 0;
   }

   // Check the pawn's color
   if (pawn.color == WHITE)
   {
      // Check if the pawn is on its starting rank
      if (from.row == 1)
      {
         // Check if the pawn is moving forward two squares
         if (to.row == from.row + 2 && to.col == from.col)
         {
            // Check if the square in front of the pawn is unoccupied
            if (board[from.row + 1][from.col].type != EMPTY || board[from.row + 2][from.col].type != EMPTY)
            {
               return 0;
            }
            return 1;
         }
      }

      // Check if the pawn is moving forward one square
      if (to.row == from.row + 1 && to.col == from.col)
      {
         // Check if the square in front of the pawn is unoccupied
         if (board[from.row + 1][from.col].type != EMPTY)
         {
            return 0;
         }
         return 1;
      }

      // Check if the pawn is capturing an opponent's piece
      if (to.row == from.row + 1 && (to.col == from.col + 1 || to.col == from.col - 1))
      {
         if (board[to.row][to.col].type == EMPTY)
         {
            // The move is not valid if the pawn is moving diagonally and there is no enemy piece at the destination position
            return 0;
         }
         return 1;
      }
   }
   else if (pawn.color == BLACK)
   {
      // Check if the pawn is on its starting rank
      if (from.row == 6)
      {
         // Check if the pawn is moving forward two squares
         if (to.row == from.row - 2 && to.col == from.col)
         {
            // Check if the square in front of the pawn is unoccupied
            if (board[from.row - 1][from.col].type != EMPTY)
            {
               return 0;
            }
            return 1;
         }
      }

      // Check if the pawn is moving forward one square
      if (to.row == from.row - 1 && to.col == from.col)
      {
         // Check if the square in front of the pawn is unoccupied
         if (board[from.row - 1][from.col].type != EMPTY)
         {
            return 0;
         }
         return 1;
      }

      // Check if the pawn is capturing an opponent's piece
      if (to.row == from.row - 1 && (to.col == from.col + 1 || to.col == from.col - 1))
      {
         if (board[to.row][to.col].type == EMPTY)
         {
            // The move is not valid if the pawn is moving diagonally and there is no enemy piece at the destination position
            return 0;
         }
         return 1;
      }
   }

   // The move is not valid
   return 0;
}

int is_valid_knight_move(struct BoardPiece knight, struct PiecePosition from, struct PiecePosition to)
{
   // Check if the piece is a knight
   if (knight.type != KNIGHT)
   {
      return 0;
   }

   // Check if the destination square is occupied by a piece of the same color
   if (board[to.row][to.col].color == knight.color)
   {
      return 0;
   }

   // Check if the knight is moving in an L shape
   if ((abs(to.row - from.row) == 2 && abs(to.col - from.col) == 1) ||
       (abs(to.row - from.row) == 1 && abs(to.col - from.col) == 2))
   {
      return 1;
   }

   // The move is not valid
   return 0;
}

int is_valid_bishop_move(struct BoardPiece bishop, struct PiecePosition from, struct PiecePosition to)
{
   // Check if the piece is a bishop
   if (bishop.type != BISHOP)
   {
      return 0;
   }

   // Check if the destination square is occupied by a piece of the same color
   if (board[to.row][to.col].color == bishop.color)
   {
      return 0;
   }

   // Check if the bishop is moving diagonally
   if (abs(to.row - from.row) != abs(to.col - from.col))
   {
      return 0;
   }

   // Check if the bishop is blocked
   int row_inc = (to.row - from.row) / abs(to.row - from.row);
   int col_inc = (to.col - from.col) / abs(to.col - from.col);
   for (int row = from.row + row_inc, col = from.col + col_inc;
        row != to.row && col != to.col;
        row += row_inc, col += col_inc)
   {
      if (board[row][col].type != EMPTY)
      {
         return 0;
      }
   }

   // The move is valid
   return 1;
}

int is_valid_rook_move(struct BoardPiece rook, struct PiecePosition from, struct PiecePosition to)
{
   // Check if the piece is a rook
   if (rook.type != ROOK)
   {
      return 0;
   }

   // Check if the destination square is occupied by a piece of the same color
   if (board[to.row][to.col].color == rook.color)
   {
      return 0;
   }

   // Check if the rook is moving horizontally or vertically
   if (from.row != to.row && from.col != to.col)
   {
      return 0;
   }

   // Check if the rook is blocked
   if (from.row == to.row)
   {
      int col_inc = (to.col - from.col) / abs(to.col - from.col);
      for (int col = from.col + col_inc; col != to.col; col += col_inc)
      {
         if (board[from.row][col].type != EMPTY)
         {
            return 0;
         }
      }
   }
   else if (from.col == to.col)
   {
      int row_inc = (to.row - from.row) / abs(to.row - from.row);
      for (int row = from.row + row_inc; row != to.row; row += row_inc)
      {
         if (board[row][from.col].type != EMPTY)
         {
            return 0;
         }
      }
   }

   // The move is valid
   return 1;
}

int is_valid_queen_move(struct BoardPiece queen, struct PiecePosition from, struct PiecePosition to)
{
   // Check if the piece is a queen
   if (queen.type != QUEEN)
   {
      return 0;
   }

   // Check if the destination square is occupied by a piece of the same color
   if (board[to.row][to.col].color == queen.color)
   {
      return 0;
   }

   // Check if the queen is moving horizontally, vertically, or diagonally
   if (from.row != to.row && from.col != to.col && abs(to.row - from.row) != abs(to.col - from.col))
   {
      return 0;
   }

   // Check if the queen is blocked
   if (from.row == to.row)
   {
      int col_inc = (to.col - from.col) / abs(to.col - from.col);
      for (int col = from.col + col_inc; col != to.col; col += col_inc)
      {
         if (board[from.row][col].type != EMPTY)
         {
            return 0;
         }
      }
   }
   else if (from.col == to.col)
   {
      int row_inc = (to.row - from.row) / abs(to.row - from.row);
      for (int row = from.row + row_inc; row != to.row; row += row_inc)
      {
         if (board[row][from.col].type != EMPTY)
         {
            return 0;
         }
      }
   }
   else
   {
      int row_inc = (to.row - from.row) / abs(to.row - from.row);
      int col_inc = (to.col - from.col) / abs(to.col - from.col);
      for (int row = from.row + row_inc, col = from.col + col_inc;
           row != to.row && col != to.col;
           row += row_inc, col += col_inc)
      {
         if (board[row][col].type != EMPTY)
         {
            return 0;
         }
      }
   }

   // The move is valid
   return 1;
}

int is_valid_king_move(struct BoardPiece king, struct PiecePosition from, struct PiecePosition to)
{
   // Check if the piece is a king
   if (king.type != KING)
   {
      return 0;
   }

   // Check if the destination square is occupied by a piece of the same color
   if (board[to.row][to.col].color == king.color)
   {
      return 0;
   }

   // Check if the king is moving one square horizontally, vertically, or diagonally
   if (abs(to.row - from.row) > 1 || abs(to.col - from.col) > 1)
   {
      return 0;
   }

   // The move is valid
   return 1;
}

int is_valid_move(struct BoardPiece piece, struct PiecePosition from, struct PiecePosition to)
{

   switch (piece.type)
   {
   case PAWN:
      return is_valid_pawn_move(piece, from, to);
   case KNIGHT:
      return is_valid_knight_move(piece, from, to);
   case BISHOP:
      return is_valid_bishop_move(piece, from, to);
   case ROOK:
      return is_valid_rook_move(piece, from, to);
   case QUEEN:
      return is_valid_queen_move(piece, from, to);
   case KING:
      return is_valid_king_move(piece, from, to);
   default:
      return 0;
   }
}

int is_check(struct BoardPiece king, struct PiecePosition king_pos)
{
   // Check if the king is in check
   for (int row = 0; row < 8; row++)
   {
      for (int col = 0; col < 8; col++)
      {
         if (board[row][col].color != king.color && board[row][col].type != EMPTY)
         {
            struct PiecePosition piece_pos = {row, col};
            if (is_valid_move(board[row][col], piece_pos, king_pos))
            {
               return 1;
            }
         }
      }
   }

   // The king is not in check
   return 0;
}

// Check if the king is in checkmate
int is_checkmate(struct BoardPiece king, struct PiecePosition king_pos)
{
   // Check if the king is in checkmate
   for (int row = 0; row < 8; row++)
   {
      for (int col = 0; col < 8; col++)
      {
         if (board[row][col].color == king.color && board[row][col].type != EMPTY)
         {
            struct PiecePosition piece_pos = {row, col};
            for (int to_row = 0; to_row < 8; to_row++)
            {
               for (int to_col = 0; to_col < 8; to_col++)
               {
                  struct PiecePosition to_pos = {to_row, to_col};
                  if (is_valid_move(board[row][col], piece_pos, to_pos))
                  {
                     // Check if the king is in check after the move
                     
                     struct BoardPiece temp = board[to_row][to_col];
                     // board[to_row][to_col] = board[row][col];
                     // board[row][col] = (struct BoardPiece){EMPTY, NONE, {row, col}};
                     move_piece(board[piece_pos.row][piece_pos.col], piece_pos, to_pos);

                     struct PiecePosition new_king_pos;

                     // Iterate through the rows and columns of the chess board
                     for (int row = 0; row < BOARD_SIZE; row++)
                     {
                        for (int col = 0; col < BOARD_SIZE; col++)
                        {
                           // Check if the piece at this position is a king
                           if (board[row][col].type == KING && board[row][col].color == king.color)
                           {
                              // Save the position and color of the king
                              new_king_pos = (struct PiecePosition){row, col};
                           }
                        }
                     }

                     if (!is_check(board[new_king_pos.row][new_king_pos.col], new_king_pos))
                     {
                        move_back(board[to_pos.row][to_pos.col], temp, to_pos, piece_pos);
                        return 0; // The king is not in checkmate
                     }
                     move_back(board[to_pos.row][to_pos.col], temp, to_pos, piece_pos);
                  }
               }
            }
         }
      }
   }

   // The king is in checkmate
   return 1;
}

int is_stalemate(struct BoardPiece king, struct PiecePosition king_pos)
{
   // Check if the king is in stalemate
   for (int row = 0; row < 8; row++)
   {
      for (int col = 0; col < 8; col++)
      {
         if (board[row][col].color == king.color && board[row][col].type != EMPTY)
         {
            struct PiecePosition piece_pos = {row, col};
            for (int to_row = 0; to_row < 8; to_row++)
            {
               for (int to_col = 0; to_col < 8; to_col++)
               {
                  struct PiecePosition to_pos = {to_row, to_col};
                  if (is_valid_move(board[row][col], piece_pos, to_pos))
                  {
                     return 0;
                  }
               }
            }
         }
      }
   }

   // The king is in stalemate
   return 1;
}

/*
 *  Check for OpenGL errors
 */
void ErrCheck(const char *where)
{
   int err = glGetError();
   if (err)
      fprintf(stderr, "ERROR: %s [%s]\n", gluErrorString(err), where);
}

/*
 *  Print message to stderr and exit
 */
void Fatal(const char *format, ...)
{
   va_list args;
   va_start(args, format);
   vfprintf(stderr, format, args);
   va_end(args);
   exit(1);
}

/*
 *  Set projection
 */
void Project(double fov, double asp, double dim)
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (fov)
      gluPerspective(fov, asp, dim / 16, 16 * dim);
   //  Orthogonal transformation
   else
      glOrtho(-asp * dim, asp * dim, -dim, +dim, -dim, +dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

void horseHead(double xin, double yin, double zin, double s, int col)
{
   double x = xin - 0.5; // - 5;
   double y = yin;
   double z = zin; // - 4.5;
   //  Set specular color
   float color[] = {1, 1, 1.2, 1};
   if (col == 0)
   {
      color[0] = 0.2;
      color[1] = 0.1;
      color[2] = 0.2;
   }

   float Emission[] = {0.0, 0.0, 0.01 * emission, 1.0};
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Emission);
   //  Save transformation
   // glPushMatrix();
   //  Offset, scale and rotate
   // glTranslated(x, y, z);
   if (col == 1)
   {
      glScaled(s, s * 1.5, s);
   }
   else
   {
      glScaled(-s, s * 1.5, s);
   }

   glColor4fv(color);
   if (mode)
      glEnable(GL_TEXTURE_2D);

   int t = 0;
   int tex = 0;

   glBegin(GL_QUAD_STRIP);
   for (int i = 0; i < 36; i++)
   {
      if (tex == 10)
         tex = 0;
      glNormal3f(0, 0, 1);
      glVertex3f(knightHead[i].x + x, knightHead[i].y + y, 0.115 + z);
      if (tex == 0)
         glTexCoord2f(t, 0);

      glNormal3f(0, 0, 1);
      glVertex3f(knightHead[71 - i].x + x, knightHead[71 - i].y + y, 0.115 + z);
      if (tex == 0)
      {
         glTexCoord2f(t, 1);
         t = 1 - t;
      }
      tex += 1;
   }
   glEnd();

   tex = 0;
   glBegin(GL_QUAD_STRIP);
   for (int i = 0; i < 36; i++)
   {
      if (tex == 10)
         tex = 0;
      glNormal3f(0, 0, -1);
      glVertex3f(knightHead[i].x + x, knightHead[i].y + y, -0.115 + z);
      if (tex == 0)
         glTexCoord2f(t, 0);

      glNormal3f(0, 0, -1);
      glVertex3f(knightHead[71 - i].x + x, knightHead[71 - i].y + y, -0.115 + z);
      if (tex == 0)
      {
         glTexCoord2f(t, 1);
         t = 1 - t;
      }
      tex += 1;
   }
   glEnd();

   tex = 0;
   glBegin(GL_QUAD_STRIP);
   for (int i = 0; i < 72; i++)
   {
      if (tex == 10)
         tex = 0;
      glNormal3f(knightHead[i].x - 0.5, knightHead[i].y - 0.5, 0);
      glVertex3f(knightHead[i].x + x, knightHead[i].y + y, 0.115 + z);
      if (tex == 0)
         glTexCoord2f(t, 0);

      glNormal3f(knightHead[i].x - 0.5, knightHead[i].y - 0.5, 0);
      glVertex3f(knightHead[i].x + x, knightHead[i].y + y, -0.115 + z);
      if (tex == 0)
      {
         glTexCoord2f(t, 1);
         t = 1 - t;
      }
      tex += 1;
   }
   glEnd();
   if (mode)
      glDisable(GL_TEXTURE_2D);
}

/*
 *  Draw the piece
 *     at (x,y,z) size s
 */
static void Piece(vec2 piece[], double xin, double yin, double zin, double s, int col)
{
   int inc = 15;
   double x = xin - 4.5;
   double y = yin;
   double z = zin - 4.5;
   //  Set specular color
   float color[] = {1, 1, 1.2, 1};
   if (col == 0)
   {
      color[0] = 0.2;
      color[1] = 0.1;
      color[2] = 0.2;
   }

   float Emission[] = {0.0, 0.0, 0.01 * emission, 1.0};
   glColor4fv(color);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Emission);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x, y, z);
   glScaled(s, s, s);

   if (piece == knight)
   {
      horseHead(0, 0, 0, s, col);
   }

   //  Draw bottom
   glColor3f(0.0, 0.5, 0.0);
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0, -1, 0);
   glVertex3f(0, piece[0].y, 0);
   for (int th = 0; th <= 360; th += inc)
      glVertex3f(Cos(th) * piece[0].x, piece[0].y, Sin(th) * piece[0].x);
   glEnd();
   //  Draw cylindrical base
   glColor4fv(color);
   if (mode)
      glEnable(GL_TEXTURE_2D);

   //  Draw the curved part of the piece
   for (int i = 0; i < Np - 1; i++)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th = 0; th <= 360; th += inc)
      {
         float c = Cos(th);
         float s = Sin(th);
         glTexCoord2f(th / 360.0, piece[i].y);
         // Calc norms for specific piece
         if (piece == pawn)
            glNormal3f(c * normPawn[i].x, normPawn[i].y, s * normPawn[i].x);
         if (piece == rook)
            glNormal3f(c * normRook[i].x, normRook[i].y, s * normRook[i].x);
         if (piece == bishop)
            glNormal3f(c * normBishop[i].x, normBishop[i].y, s * normBishop[i].x);
         if (piece == queen)
            glNormal3f(c * normQueen[i].x, normQueen[i].y, s * normQueen[i].x);
         if (piece == king)
            glNormal3f(c * normKing[i].x, normKing[i].y, s * normKing[i].x);
         if (piece == knight)
            glNormal3f(c * normKnight[i].x, normKnight[i].y, s * normKnight[i].x);
         glVertex3f(c * piece[i].x, piece[i].y, s * piece[i].x);

         glTexCoord2f(th / 360.0, piece[i + 1].y);
         // Calc Norms for specific piece
         if (piece == pawn)
            glNormal3f(c * normPawn[i + 1].x, normPawn[i + 1].y, s * normPawn[i + 1].x);
         if (piece == rook)
            glNormal3f(c * normRook[i + 1].x, normRook[i + 1].y, s * normRook[i + 1].x);
         if (piece == bishop)
            glNormal3f(c * normBishop[i + 1].x, normBishop[i + 1].y, s * normBishop[i + 1].x);
         if (piece == queen)
            glNormal3f(c * normQueen[i + 1].x, normQueen[i + 1].y, s * normQueen[i + 1].x);
         if (piece == king)
            glNormal3f(c * normKing[i + 1].x, normKing[i + 1].y, s * normKing[i + 1].x);
         if (piece == knight)
            glNormal3f(c * normKnight[i + 1].x, normKnight[i + 1].y, s * normKnight[i + 1].x);
         glVertex3f(c * piece[i + 1].x, piece[i + 1].y, s * piece[i + 1].x);
      }
      glEnd();
   }

   if (mode)
      glDisable(GL_TEXTURE_2D);
   //  Undo transformations and textures
   glPopMatrix();
}

/*
 *  Draw vertex in polar coordinates
 */
static void Vertex2(double th, double ph)
{
   glNormal3d(Sin(th) * Cos(ph), Cos(th) * Cos(ph), Sin(ph));
   glVertex3d(Sin(th) * Cos(ph), Cos(th) * Cos(ph), Sin(ph));
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x, double y, double z, double r)
{
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x, y, z);
   glScaled(r, r, r);
   //  White ball with yellow specular
   float yellow[] = {1.0, 1.0, 0.0, 1.0};
   float Emission[] = {0.0, 0.0, 0.01 * emission, 1.0};
   glColor3f(1, 1, 1);
   glMaterialf(GL_FRONT, GL_SHININESS, shiny);
   glMaterialfv(GL_FRONT, GL_SPECULAR, yellow);
   glMaterialfv(GL_FRONT, GL_EMISSION, Emission);

   //  Bands of latitude
   for (int ph = -90; ph < 90; ph += 10)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th = 0; th <= 360; th += 2 * 10)
      {
         Vertex2(th, ph);
         Vertex2(th, ph + 10);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}

static void displayBoard()
{
   //  Set specular color to white
   float white[] = {1, 1, 1, 1};
   float Emission[] = {0.0, 0.0, 0.01 * emission, 1.0};
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, Emission);

   glColor3f(Cos(th) * Cos(th), Sin(ph) * Sin(ph), Sin(th) * Sin(th));

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, Tmode ? GL_REPLACE : GL_MODULATE);

   glBindTexture(GL_TEXTURE_2D, texture[0]);

   int offset = 0;

   int c = 1;
   glBegin(GL_QUADS);
   for (int i = 0; i < 8; i += 1)
   {
      c = 1 - c;
      for (int j = 0; j < 8; j += 1)
      {
         if (c)
         {
            glColor3f(0.76, 0.82, 0.82);
            c = 1 - c;
         }
         else
         {
            glColor3f(0.43, 0.5, 0.5);
            c = 1 - c;
         }
         glNormal3f(0, 1, 0);
         glVertex3d(-4 + j + offset, 0, -4 + i + offset);
         glTexCoord2f(0, 0);
         glVertex3d(-3 + j + offset, 0, -4 + i + offset);
         glTexCoord2f(1, 0);
         glVertex3d(-3 + j + offset, 0, -3 + i + offset);
         glTexCoord2f(0, 1);
         glVertex3d(-4 + j + offset, 0, -3 + i + offset);
         glTexCoord2f(1, 1);
      }
   }
   glEnd();
   glDisable(GL_TEXTURE_2D);
}

void startPieces()
{
   // White pawns
   for (int i = 0; i < 8; i += 1)
   {
      Piece(pawn, piecePos[i].x, 0, piecePos[i].y, 1.0, 1);
   }
   // White rooks
   Piece(rook, piecePos[8].x, 0, piecePos[8].y, 1.15, 1);
   Piece(rook, piecePos[9].x, 0, piecePos[9].y, 1.15, 1);
   // White Knights
   Piece(knight, piecePos[10].x, 0, piecePos[10].y, 1.0, 1);
   Piece(knight, piecePos[11].x, 0, piecePos[11].y, 1.0, 1);
   // White bishops
   Piece(bishop, piecePos[12].x, 0, piecePos[12].y, 1.5, 1);
   Piece(bishop, piecePos[13].x, 0, piecePos[13].y, 1.5, 1);
   // White Queen
   Piece(queen, piecePos[14].x, 0, piecePos[14].y, 2.0, 1);
   // White King
   Piece(king, piecePos[15].x, 0, piecePos[15].y, 2.0, 1);

   // Black pawns
   for (int i = 16; i < 24; i += 1)
   {
      Piece(pawn, piecePos[i].x, 0, piecePos[i].y, 1.0, 0);
   }
   // Black rooks
   Piece(rook, piecePos[24].x, 0, piecePos[24].y, 1.15, 0);
   Piece(rook, piecePos[25].x, 0, piecePos[25].y, 1.15, 0);
   // White Knights
   Piece(knight, piecePos[26].x, 0, piecePos[26].y, 1.0, 0);
   Piece(knight, piecePos[27].x, 0, piecePos[27].y, 1.0, 0);
   // Black bishops
   Piece(bishop, piecePos[28].x, 0, piecePos[28].y, 1.5, 0);
   Piece(bishop, piecePos[29].x, 0, piecePos[29].y, 1.5, 0);
   // Black Queen
   Piece(queen, piecePos[30].x, 0, piecePos[30].y, 2.0, 0);
   // Black King
   Piece(king, piecePos[31].x, 0, piecePos[31].y, 2.0, 0);
}

/*
 *  Move piece based on command
 */

void movePiece()
{
   int fromx = 0;
   int fromy = 0;
   int tox = 0;
   int toy = 0;

   // printf(typed);
   // printf("\n");
   // int size = strlen(typed);
   // printf("%d",cursor);
   // printf("\n");

   if (typed[0] == 'a')
      fromy = 1;
   else if (typed[0] == 'b')
      fromy = 2;
   else if (typed[0] == 'c')
      fromy = 3;
   else if (typed[0] == 'd')
      fromy = 4;
   else if (typed[0] == 'e')
      fromy = 5;
   else if (typed[0] == 'f')
      fromy = 6;
   else if (typed[0] == 'g')
      fromy = 7;
   else if (typed[0] == 'h')
      fromy = 8;

   fromx = typed[1] - '0';

   if (typed[3] == 'a')
      toy = 1;
   else if (typed[3] == 'b')
      toy = 2;
   else if (typed[3] == 'c')
      toy = 3;
   else if (typed[3] == 'd')
      toy = 4;
   else if (typed[3] == 'e')
      toy = 5;
   else if (typed[3] == 'f')
      toy = 6;
   else if (typed[3] == 'g')
      toy = 7;
   else if (typed[3] == 'h')
      toy = 8;

   tox = typed[4] - '0';

   // printf("%d %d , %d %d\n",fromx, fromy, tox, toy);

   int capture = 0;

   int pindex = -1;
   for (int i = 0; i < 32; i++)
   {
      if ((piecePos[i].x == fromx) && (piecePos[i].y == fromy))
      {
         pindex = i;
      }

      if ((piecePos[i].x == tox) && (piecePos[i].y == toy))
      {
         capture = 1;
      }
   }
   ⠀⠀⠀⠀⠀⢀⣠⠤⠴⠒⠒⠒⠒⠒⠒⠒⠦⢤⣀⠀⠀⠀⠀
⠀⠀⠀⠀⣀⡴⠚⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠳⢦⡀⠀⠀⠀⠀
⠀⠀⢠⠞⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⢳⡀⠀⠀
⠀⣰⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⣆⠀
⢰⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⡆
⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀This is a for loop⠀⠀⠀⣷
⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡿
⢸⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠇
⠀⢳⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⡯⠁
⠀⠀⠙⢦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡴⠋⠁⠀
⠀⠀⠀⠀⠙⠦⣄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⠶⠋⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠉⠛⠲⠤⢤⣀⠀⠀⠀⠀⠀⢶⠶⠛⠉⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⠳⢦⣄⡀⠈⠳⣄⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⠓⢦⣝⣦⡀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⣓⠀⠀⠀⠀⠀⠀
           .--._.--.
          ( O     O )
          /   . .   \
         .`._______.'.
        /(           )\
      _/  \  \   /  /  \_
   .~   `  \  \ /  /  '   ~.
  {    -.   \  V  /   .-    }
_ _`.    \  |  |  |  /    .'_ _
>_       _} |  |  | {_       _<
 /. - ~ ,_-'  .^.  `-_, ~ - .\
         '-'|/   \|`-`

   if (turn != 2)
   {
      if (pindex != -1)
      {
         if (tox >= 1 && tox <= 8)
         {
            if (toy >= 1 && toy <= 8)
            {
               if (turn == board[fromx - 1][fromy - 1].color)
               {
                  if (is_valid_move(board[fromx - 1][fromy - 1], (struct PiecePosition){fromx - 1, fromy - 1}, (struct PiecePosition){tox - 1, toy - 1}))
                  {
                     // Move piece
                     move_piece(board[fromx - 1][fromy - 1], (struct PiecePosition){fromx - 1, fromy - 1}, (struct PiecePosition){tox - 1, toy - 1});

                     // check if move will put king in check
                     struct PiecePosition king_pos;

                     // Iterate through the rows and columns of the chess board
                     for (int row = 0; row < BOARD_SIZE; row++)
                     {
                        for (int col = 0; col < BOARD_SIZE; col++)
                        {
                           // Check if the piece at this position is a king
                           if (board[row][col].type == KING && board[row][col].color == turn)
                           {
                              // Save the position and color of the king
                              king_pos = (struct PiecePosition){row, col};
                           }
                        }
                     }


                     struct BoardPiece temp = board[tox - 1][toy - 1];

                     if (!is_check(board[king_pos.row][king_pos.col], king_pos))
                     {
                        // move_back(piece, temp, to, from);
                        // Capture piece
                        if (capture)
                        {
                           for (int i = 0; i < 32; i++)
                           {
                              if ((piecePos[i].x == tox) && (piecePos[i].y == toy))
                              {
                                 piecePos[i].x = 100;
                                 piecePos[i].y = 100;
                              }
                           }
                        }

                        // Move graphics
                        piecePos[pindex].x = tox;
                        piecePos[pindex].y = toy;

                        // clear command input
                        typed[0] = '\0';
                        typed[1] = '\0';
                        typed[2] = '\0';
                        typed[3] = '\0';
                        typed[4] = '\0';
                        cursor = 0;

                        // check if checkmate or stalemate
                        struct PiecePosition enemy_king_pos;

                        // Iterate through the rows and columns of the chess board
                        for (int row = 0; row < BOARD_SIZE; row++)
                        {
                           for (int col = 0; col < BOARD_SIZE; col++)
                           {
                              // Check if the piece at this position is a king
                              if (board[row][col].type == KING && board[row][col].color != turn)
                              {
                                 // Save the position and color of the king
                                 enemy_king_pos = (struct PiecePosition){row, col};
                              }
                           }
                        }
                        if (is_checkmate(board[enemy_king_pos.row][enemy_king_pos.col], enemy_king_pos) || is_stalemate(board[enemy_king_pos.row][enemy_king_pos.col], enemy_king_pos))
                        {
                           turn = 2;
                        }
                        else
                        {
                           // Change turn
                           turn = 1 - turn;
                           if (turn == 0){
                              th = 90;
                           }
                           else{
                              th = 270;
                              moveCount += 1;
                           }
                        }
                     }
                     else
                     {
                        move_back(board[fromx - 1][fromy - 1], temp, (struct PiecePosition){tox - 1, toy - 1}, (struct PiecePosition){fromx - 1, fromy - 1});
                     }
                  }
               }
            }
         }
      }
   }
}


void Restart()
{
   init_board();
   for (int i = 0; i < 32; i++)
   {
      piecePos[i] = piecePos_copy[i];
   }
   turn = 0;
   th = 90;
}


/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   glClearColor(0.0, 0.0, 0.0, 0.0);
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();

   //  Orthogonal - set world orientation
   if (mode == 0)
   {
      glRotatef(ph, 1, 0, 0);
      glRotatef(th, 0, 1, 0);
   }
   //  Perspective - set eye position
   else if (mode == 1)
   {
      double Ex = -2 * dim * Sin(th) * Cos(ph);
      double Ey = +2 * dim * Sin(ph);
      double Ez = +2 * dim * Cos(th) * Cos(ph);
      gluLookAt(Ex, Ey, Ez, 0, 0, 0, 0, Cos(ph), 0);
   }

   //  Flat or smooth shading
   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   //  Lighting
   if (light)
   {
      //  Translate intensity to color vectors
      float Ambient[] = {0.01 * ambient, 0.01 * ambient, 0.01 * ambient, 1.0};
      float Diffuse[] = {0.01 * diffuse, 0.01 * diffuse, 0.01 * diffuse, 1.0};
      float Specular[] = {0.01 * specular, 0.01 * specular, 0.01 * specular, 1.0};
      //  Light position
      float Position[] = {distance * Cos(zh), ylight, distance * Sin(zh), 1.0};
      //  Draw light position as ball (still no lighting here)
      glColor3f(1, 1, 1);
      ball(Position[0], Position[1], Position[2], 0.1);
      //  OpenGL should normalize normal vectors
      glEnable(GL_NORMALIZE);
      //  Enable lighting
      glEnable(GL_LIGHTING);
      //  Location of viewer for specular calculations
      glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, local);
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      //  Enable light 0
      glEnable(GL_LIGHT0);
      //  Set ambient, diffuse, specular components and position of light 0
      glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
      glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);
      glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
      glLightfv(GL_LIGHT0, GL_POSITION, Position);
   }
   else
      glDisable(GL_LIGHTING);

   startPieces(); // Draw start pieces
   displayBoard();
   // horseHead(3, 0, 4, 1, 1);
   // Piece(knight, 3, 0, 4, 1.0, 1);

   glDisable(GL_LIGHTING);
   //  White
   glColor3f(1, 1, 1);
   //  Draw axes
   if (axes)
   {
      const double len = 1.5; //  Length of axes
      glBegin(GL_LINES);
      glVertex3d(0.0, 0.0, 0.0);
      glVertex3d(len, 0.0, 0.0);
      glVertex3d(0.0, 0.0, 0.0);
      glVertex3d(0.0, len, 0.0);
      glVertex3d(0.0, 0.0, 0.0);
      glVertex3d(0.0, 0.0, len);
      glEnd();
      //  Label axes
      glRasterPos3d(len, 0.0, 0.0);
      Print("X");
      glRasterPos3d(0.0, len, 0.0);
      Print("Y");
      glRasterPos3d(0.0, 0.0, len);
      Print("Z");
   }

   int n = 1;
   if (th > 180 || (th < 0 && th > -180))
      n = -1;

   // Draw Square labels
   glRasterPos3d(-4.5 * n, 0.0, -3.5);
   Print("a");
   glRasterPos3d(-4.5 * n, 0.0, -2.5);
   Print("b");
   glRasterPos3d(-4.5 * n, 0.0, -1.5);
   Print("c");
   glRasterPos3d(-4.5 * n, 0.0, -0.5);
   Print("d");
   glRasterPos3d(-4.5 * n, 0.0, 0.5);
   Print("e");
   glRasterPos3d(-4.5 * n, 0.0, 1.5);
   Print("f");
   glRasterPos3d(-4.5 * n, 0.0, 2.5);
   Print("g");
   glRasterPos3d(-4.5 * n, 0.0, 3.5);
   Print("h");

   glRasterPos3d(-3.5, 0.0, -4.5);
   Print("1");
   glRasterPos3d(-2.5, 0.0, -4.5);
   Print("2");
   glRasterPos3d(-1.5, 0.0, -4.5);
   Print("3");
   glRasterPos3d(-0.5, 0.0, -4.5);
   Print("4");
   glRasterPos3d(0.5, 0.0, -4.5);
   Print("5");
   glRasterPos3d(1.5, 0.0, -4.5);
   Print("6");
   glRasterPos3d(2.5, 0.0, -4.5);
   Print("7");
   glRasterPos3d(3.5, 0.0, -4.5);
   Print("8");

   /*
   PRINT STUFF AT THE BOTTOM OF THE SCREEN vvv
   */
   // Five pixels from the lower left corner of the window
   glWindowPos2i(5, 5);
   //  Print the text string
   // Print("  Pawn Chess Piece");
   Print(typed);

   glWindowPos2i(5, 25);
   if (turn == 0)
   {
      th = 90;
      Print("White's Turn, enter move (ex: \"e2,e4\"). Use arrow keys to look.");
   }
   else if (turn == 1)
   {
      th = 270;
      Print("Black's Turn, enter move (ex: \"e2,e4\"). Use arrow keys to look.");
   }
   else
   {
      Print("Game Over, press \"r\" to restart.");
   }
   // Print("Move piece by entering command: \"e2,e4\". Use arrow keys to move.");
   // Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%d", th, ph, dim, fov, mode);
   // Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d", ambient, diffuse, specular, emission, shiny);
   //  Render the scene
   // ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
   zh = fmod(90 * t, 360.0);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key, int x, int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_UP)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_DOWN && dim > 1)
      dim -= 0.1;
   else if (key == GLUT_KEY_F3)
      distance = (distance == 1) ? 5 : 1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project(mode ? fov : 0, asp, dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch, int x, int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
      
   //  PageUp key - increase dim
   else if (ch == 'o')
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (ch == 'p')
      dim -= 0.1;
   // restart game
   else if ((ch == 'r' || ch == 'R') && turn == 2)
      Restart();
   // DELETE CHARS AT BOTTOM OF SCREEN, 127 is acsii for backspace
   else if (ch == 8)
   {
      if (cursor > -1)
      {
         typed[cursor - 1] = '\0';
         if (cursor > 0)
            cursor -= 1;
      }
   }
   // Enter Move AT BOTTOM OF SCREEN, 13 is acsii for enter
   else if (ch == 13)
   {
      movePiece();
   }
   // ADD CHARS TO BOTTOM OF SCREEN STRING
   else
   {
      if (cursor < 5)
      {
         typed[cursor] = ch;
         cursor += 1;
      }
   }
   // if (move)
   // {
   //    if (ch == 'b' || ch == 'B')
   //    {
   //       zh += 5;
   //    }
   //    zh %= 360;
   // }
   //  Translate shininess power to value (-1 => 0)
   // shiny = shininess < 0 ? 0 : pow(2.0, shininess);
   //  Reproject
   Project(mode ? fov : 0, asp, dim);
   //  Animate if requested
   glutIdleFunc(move ? idle : NULL);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width, int height)
{
   //  Ratio of the width to the height of the window
   asp = (height > 0) ? (double)width / height : 1;
   //  Set the viewport to the entire window
   glViewport(0, 0, RES * width, RES * height);
   //  Set projection

   Project(mode ? fov : 0, asp, dim);
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc, char *argv[])
{
   //  Initialize GLUT
   glutInit(&argc, argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(400, 400);
   glutCreateWindow("Surya Manavalan - Final Project: Chess");
   init_board();
#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit() != GLEW_OK)
      Fatal("Error initializing GLEW\n");
#endif
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   //  Compute normals for pawn
   CalcNormPawn();
   CalcNormRook();
   CalcNormBishop();
   CalcNormQueen();
   CalcNormKing();
   CalcNormKnight();

   texture[0] = LoadTexBMP("marble.bmp");
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
