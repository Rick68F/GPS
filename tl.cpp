#include <stdio.h>
#include <stdint.h>
#include <algorithm>
#include <cmath>
#include <iostream>
// #include "earcut.hpp"

#define Y_MAX 50
#define X_MAX 150
#define Y_MIN 0
#define X_MIN 0

// using namespace std;

enum Quadant {  // les 4 cadrant dans le sens des aiguilles d'une montre
  Q1 = 1,       //entre 12 et 3
  Q2,           //entre 4 et 6
  Q3,           //entre 6 et 9
  Q4,           //entre 9 et 12
  LH,           // Ligne a droite
  LV            // Ligne vers le bas
};

char screen[Y_MAX + 1][X_MAX + 1];

void drawLine(int x0, int y0, int x1, int y1, char symbole);

struct PT {
  int16_t x;
  int16_t y;
} pt[1000];

struct Vector2D {
  int x, y;
};

Vector2D v1;
Vector2D v2;
Vector2D v3;
Vector2D v4;

struct Triangle {
  Vector2D p[3];
};

enum Mode_route {
  ROAD = 0,  // une route epaiiseur + BORD
  THICK,     // epaisseur
  BORDER,    // ligne parallele Bord
  CARRE,
};

int n_pt = 0;

// function for rounding off the pixels
int round(float n) {
  if (n - (int)n < 0.5)
    return (int)n;
  return (int)(n + 1);
}

void init() {
  for (int l = 0; l < Y_MAX; l++) {
    for (int c = 0; c < X_MAX; c++) {
      screen[l][c] = ' ';
    }
  }
  drawLine(X_MIN, Y_MIN, X_MIN, Y_MAX, '|');
  drawLine(X_MAX, Y_MIN, X_MAX, Y_MAX, '|');
  drawLine(X_MIN, Y_MIN, X_MAX, Y_MIN, '-');
  drawLine(X_MIN, Y_MAX, X_MAX, Y_MAX, '-');
}

void print() {
  for (int l = 0; l <= Y_MAX; l++) {
    for (int c = 0; c <= X_MAX; c++) {
      printf("%c", screen[l][c]);
    }
    printf("\n");
  }
}

void setPixel(int x, int y, char color = -1) {  // fct substitution draw direct dans le buffer
  char Sym = '.';
  if (x < X_MIN)
    return;
  if (x > X_MAX)
    return;
  if (y < Y_MIN)
    return;
  if (y > Y_MAX)
    return;
  if (color >= 0)
    Sym = color;
  screen[y][x] = Sym;
}

void drawHLine(int x1, int x2, int y, char color = -1) {
  char sym = '.';

  if (color >= 0) sym = color;
  // printf("x1 %d x2 %d y %d\n", x1, x2, y);
  if (x1 > x2) std::swap(x1, x2);
  for (int x = x1; x <= x2; x++)
    setPixel(x, y, sym);
}

void drawLine(int x0, int y0, int x1, int y1, char color) {
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2; /* error value e_xy */
  for (;;) {             /* loop */
    setPixel(x0, y0, color);
    e2 = 2 * err;
    if (e2 >= dy) { /* e_xy+e_x > 0 */
      if (x0 == x1)
        break;
      err += dy;
      x0 += sx;
    }
    if (e2 <= dx) { /* e_xy+e_y < 0 */
      if (y0 == y1)
        break;
      err += dx;
      y0 += sy;
    }
  }
}

// Function for line generation
void DDALine(int x0, int y0, int x1, int y1, char color) {

  // Calculate dx and dy
  int dx = x1 - x0;
  int dy = y1 - y0;

  // calculate steps required for generating pixels
  int step = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

  // Calculate x-increment and y-increment for each step
  float x_incr = (float)dx / step;
  float y_incr = (float)dy / step;

  // Take the initial points as x and y
  float x = x0;
  float y = y0;

  for (int i = 0; i <= step; i++) {

    // putpixel(round(x), round(y), WHITE);
    setPixel(round(x), round(y), color);
    // cout << round(x) << " " << round(y) << "\n";
    x += x_incr;
    y += y_incr;
    // delay(10);
  }
}

void drawFilledRect(int x, int y, int w, int h, uint8_t color = 'h') {
  int x0, y0, x1, y1;

  x0 = std::max(x, X_MIN);
  x1 = std::min(x + w - 1, X_MAX);
  y0 = std::max(y, Y_MIN);
  y1 = std::min(y + h - 1, Y_MAX);
  // printf("x0=%d y0=%d x1=%d y1=%d w=%d h=%d\n", x0, y0, x1, y1, w, h);
  for (int i = y0; i <= y1; i++) {
    drawHLine(x0, x1, i, color);
  }
}

// DDA Function for line generation
void DDA(int X0, int Y0, int X1, int Y1, char color) {
  // calculate dx & dy
  int dx = X1 - X0;
  int dy = Y1 - Y0;

  // calculate steps required for generating pixels
  int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

  // calculate increment in x & y for each steps
  float Xinc = dx / (float)steps;
  float Yinc = dy / (float)steps;

  // Put pixel for each step
  float X = X0;
  float Y = Y0;
  for (int i = 0; i <= steps; i++) {
    setPixel(round(X), round(Y), color);
    X += Xinc;  // increment in x at each step
    Y += Yinc;  // increment in y at each step
    // delay(100); // for visualization of line-
    // generation step by step
  }
}

void DDALLine(float x1, float y1, float x2, float y2, char color) {

  float x, y, dx, dy, step;
  int i;

  dx = (x2 - x1);
  dy = (y2 - y1);

  if (abs(dx) >= abs(dy))
    step = abs(dx);
  else
    step = abs(dy);

  dx = dx / step;
  dy = dy / step;
  x = x1;
  y = y1;
  i = 0;

  while (i <= step) {
    // setPixel(round(x), round(y), color);
    setPixel(x, y, color);
    x = x + dx;
    y = y + dy;
    i = i + 1;
  }
}

void drawRoadSegment(int x0, int y0, int x1, int y1, uint8_t couleur_externe, uint8_t couleur_interne, float offsetPixels, int_fast8_t mode) {

  offsetPixels = offsetPixels / 2;

  float L = std::sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));

  // float offsetPixels = 2.0;

  // This is the second line
  int x1p = round(x0 + offsetPixels * (y1 - y0) / L);
  int x2p = round(x1 + offsetPixels * (y1 - y0) / L);
  int y1p = round(y0 + offsetPixels * (x0 - x1) / L);
  int y2p = round(y1 + offsetPixels * (x0 - x1) / L);

  int x3p = round(x0 - offsetPixels * (y1 - y0) / L);
  int x4p = round(x1 - offsetPixels * (y1 - y0) / L);
  int y3p = round(y0 - offsetPixels * (x0 - x1) / L);
  int y4p = round(y1 - offsetPixels * (x0 - x1) / L);

  if (mode != BORDER) {  // valide pour ROAD et THICK
    //  couleur = couleur_interne;
    //  drawFilledTriangle(x1p, y1p, x2p, y2p, x3p, y3p, couleur_interne);
    //  drawFilledTriangle(x2p, y2p, x3p, y3p, x4p, y4p, couleur_interne);
  }
  if (mode != THICK) {  // valide pour ROAD et BORDER
    //  couleur_bord = couleur_externe;
    drawLine(x1p, y1p, x2p, y2p, 'o');
    drawLine(x3p, y3p, x4p, y4p, 'o');
  }
  if (mode == CARRE) {  // valide pour ROAD et BORDER
    //  couleur_bord = couleur_externe;
    drawLine(x1p, y1p, x3p, y3p, 'o');
    drawLine(x2p, y2p, x4p, y4p, 'o');
  }
}

void drawRoadSegmentDDA(int x0, int y0, int x1, int y1, uint8_t couleur_externe, uint8_t couleur_interne, float offsetPixels, int_fast8_t mode) {

  offsetPixels = offsetPixels / 2;

  float L = std::sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));

  // float offsetPixels = 2.0;

  // This is the second line
  int x1p = round(x0 + offsetPixels * (y1 - y0) / L);
  int x2p = round(x1 + offsetPixels * (y1 - y0) / L);
  int y1p = round(y0 + offsetPixels * (x0 - x1) / L);
  int y2p = round(y1 + offsetPixels * (x0 - x1) / L);

  int x3p = round(x0 - offsetPixels * (y1 - y0) / L);
  int x4p = round(x1 - offsetPixels * (y1 - y0) / L);
  int y3p = round(y0 - offsetPixels * (x0 - x1) / L);
  int y4p = round(y1 - offsetPixels * (x0 - x1) / L);

  if (mode != BORDER) {  // valide pour ROAD et THICK
    //  couleur = couleur_interne;
    //  drawFilledTriangle(x1p, y1p, x2p, y2p, x3p, y3p, couleur_interne);
    //  drawFilledTriangle(x2p, y2p, x3p, y3p, x4p, y4p, couleur_interne);
  }
  if (mode != THICK) {  // valide pour ROAD et BORDER
    //  couleur_bord = couleur_externe;
    DDALine(x1p, y1p, x2p, y2p, '*');
    DDALine(x3p, y3p, x4p, y4p, '*');
  }
  if (mode == CARRE) {  // valide pour ROAD et BORDER
    //  couleur_bord = couleur_externe;
    DDALine(x1p, y1p, x3p, y3p, '*');
    DDALine(x2p, y2p, x4p, y4p, '*');
  }
}

void drawRoadSegmentDDAL(int x0, int y0, int x1, int y1, uint8_t couleur_externe, uint8_t couleur_interne, float offsetPixels, int_fast8_t mode) {

  offsetPixels = offsetPixels / 2;

  float L = std::sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));

  // float offsetPixels = 2.0;

  // This is the second line
  int x1p = round(x0 + offsetPixels * (y1 - y0) / L);
  int x2p = round(x1 + offsetPixels * (y1 - y0) / L);
  int y1p = round(y0 + offsetPixels * (x0 - x1) / L);
  int y2p = round(y1 + offsetPixels * (x0 - x1) / L);

  int x3p = round(x0 - offsetPixels * (y1 - y0) / L);
  int x4p = round(x1 - offsetPixels * (y1 - y0) / L);
  int y3p = round(y0 - offsetPixels * (x0 - x1) / L);
  int y4p = round(y1 - offsetPixels * (x0 - x1) / L);

  if (mode != BORDER) {  // valide pour ROAD et THICK
    //  couleur = couleur_interne;
    //  drawFilledTriangle(x1p, y1p, x2p, y2p, x3p, y3p, couleur_interne);
    //  drawFilledTriangle(x2p, y2p, x3p, y3p, x4p, y4p, couleur_interne);
  }
  if (mode != THICK) {  // valide pour ROAD et BORDER
    //  couleur_bord = couleur_externe;
    DDALLine(x1p, y1p, x2p, y2p, '*');
    DDALLine(x3p, y3p, x4p, y4p, '*');
  }
  if (mode == CARRE) {  // valide pour ROAD et BORDER
    //  couleur_bord = couleur_externe;
    DDALLine(x1p, y1p, x3p, y3p, '*');
    DDALLine(x2p, y2p, x4p, y4p, '*');
  }
}

void drawRoadSegmentMYT(int x0, int y0, int x1, int y1, uint8_t couleur_externe, uint8_t couleur_interne, float offsetPixels, int_fast8_t mode) {
  int8_t q0 = 0;
  // ligne droite ??
  if (x0 == x1) {
    q0 = LV;                         //vers le bas
    if (y0 > y1) std::swap(y0, y1);  // dans le bon ordre
  } else if (y0 == y1) {
    q0 = LH;                         // vers la droite
    if (x0 > x1) std::swap(x0, x1);  // dans le bon ordre
  }
  // Quel cadrant de depart 'Quarter'
  else if (x0 < x1) {
    if (y0 < y1) q0 = Q1;
    else q0 = Q2;
  } else {
    if (y0 < y1) q0 = Q4;
    else q0 = Q3;
  }
  printf("Q0=%d C=%c\n", q0, couleur_interne);


  offsetPixels = offsetPixels / 2;

  switch (q0) {
    case Q3:  // on inverse les corrdonnées
      std::swap(y0, y1);
      std::swap(x0, x1);
      break;
    case Q4:  // on inverse les corrdonnées
      std::swap(y0, y1);
      std::swap(x0, x1);
      break;
  }

  float L = std::sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));

  // float offsetPixels = 2.0;

  // This is the second line
  int x1p = round(x0 + offsetPixels * (y1 - y0) / L);
  int x2p = round(x1 + offsetPixels * (y1 - y0) / L);
  int y1p = round(y0 + offsetPixels * (x0 - x1) / L);
  int y2p = round(y1 + offsetPixels * (x0 - x1) / L);

  int x3p = round(x0 - offsetPixels * (y1 - y0) / L);
  int x4p = round(x1 - offsetPixels * (y1 - y0) / L);
  int y3p = round(y0 - offsetPixels * (x0 - x1) / L);
  int y4p = round(y1 - offsetPixels * (x0 - x1) / L);

  if (q0 > Q4) {
    // printf("x1p=%d y1p=%d x2p=%d y2p=%d x3p=%d y3p=%d x4p=%d y4p=%d\n", x1p, y1p, x2p, y2p, x3p, y3p, x4p, y4p);
    if (q0 == LH) drawFilledRect(x1p, y1p, x4p - x1p + 1, y4p - y1p + 1, 'h');
    else drawFilledRect(x3p, y3p, x2p - x3p + 1, y2p - y3p + 1, 'v');
    return;
  }

  if ((y0 < y1) && (x0 > x1)) {  // on inverse les points si c'est du bas vers le haut la direction de ligne
    std::swap(y1p, y2p);
    std::swap(x1p, x2p);
    std::swap(y3p, y4p);
    std::swap(x3p, x4p);
  }

  setPixel(x1p, y1p, '1');
  setPixel(x2p, y2p, '2');
  setPixel(x3p, y3p, '3');
  setPixel(x4p, y4p, '4');

  float S1, S2;
  S1 = (float)(x1p - x3p) / (y1p - y3p);
  S2 = (float)(x4p - x3p) / (y4p - y3p);

  int xd, xa;
  if ((q0 == Q1) || (q0 == Q3)) {  // Ligne gauche -> droite et haut->bas ou bas->haut
    float lp_Q4 = x1p, lp_Q3 = x3p, lp_Q1 = x1p, lp_Q2 = x2p;
    for (int i = y1p + 1; i < y4p; i++) {  // ligne du point centre haut vers la gauche partie haute
      if (i < y3p) {
        lp_Q4 += S1;
        setPixel(round(lp_Q4), i, '+');
        xd = round(lp_Q4);
      }
      if (i > y3p) {  // ligne du point gauche vers le centre bas
        lp_Q3 += S2;
        setPixel(round(lp_Q3), i, '+');
        xd = round(lp_Q3);
      }
      if (i == y3p) xd = x3p;
      if (i < y2p) {  // ligne du point centre haut vers la doite partie haute
        lp_Q1 += S2;
        setPixel(round(lp_Q1), i, '+');
        xa = round(lp_Q1);
      }
      if (i > y2p) {  // ligne du point droite vers le centre bas
        lp_Q2 += S1;
        setPixel(round(lp_Q2), i, '+');
        xa = round(lp_Q2);
      }
      if (i == y2p) xa = x2p;
      setPixel(xd, i, '*');
      setPixel(xa, i, '-');
      for (int x = xd; x <= xa; x++) setPixel(x, i, '0');
    }
  } else {  // Ligne droite -> gauche et haut->bas ou bas->haut
    float lp_Q4 = x2p, lp_Q3 = x4p, lp_Q1 = x1p, lp_Q2 = x2p;
    for (int i = y2p + 1; i < y3p; i++) {
      if (i < y1p) {  // ligne du point centre haut vers la gauche partie haute
        lp_Q2 += S2;
        setPixel(round(lp_Q2), i, '+');
        xd = round(lp_Q2);
      }
      if (i > y1p) {  // ligne du point gauche vers le centre bas
        lp_Q1 += S1;
        setPixel(round(lp_Q1), i, '+');
        xd = round(lp_Q1);
      }
      if (i == y1p) xd = x1p;
      if (i < y4p) {  // ligne du point centre haut vers la doite partie haute
        lp_Q4 += S1;
        setPixel(round(lp_Q4), i, '+');
        xa = round(lp_Q4);
      }
      if (i > y4p) {  // ligne du point droite vers le centre bas
        lp_Q3 += S2;
        setPixel(round(lp_Q3), i, '+');
        xa = round(lp_Q3);
      }
      if (i == y4p) xa = x4p;
      setPixel(xd, i, '*');
      setPixel(xa, i, '-');
      for (int x = xd; x <= xa; x++) setPixel(x, i, '0');
    }
  }

  DDALine(x0, y0, x1, y1, couleur_interne);
}


int main() {
  init();
  // drawRoadSegment(10, 10, 70, 50, 1, 0, 4, CARRE);
  drawRoadSegmentMYT(40, 10, 70, 30, 1, 'a', 4, CARRE);
  // drawRoadSegmentMYT(10, 40, 70, 40, 1, 'b', 4, CARRE);
  drawRoadSegmentMYT(70, 45, 80, 30, 1, 'c', 4, CARRE);

  drawRoadSegmentMYT(140, 45, 100, 30, 1, 'd', 4, CARRE);
  drawRoadSegmentMYT(130, 5, 105, 25, 1, 'e', 4, CARRE);
  // drawRoadSegmentMYT(10, 10, 10, 30, 1, 'f', 4, CARRE);
  // drawRoadSegmentMY(10, 10, 40, 30, 1, 0, 4, CARRE);
  // drawRoadSegment(110, 50, 50, 30, 1, 0, 4, CARRE);
  // drawRoadSegmentDDA(110, 40, 50, 20, 1, 0, 4, CARRE);
  // drawRoadSegment(130, 50, 70, 30, 1, 0, 4, CARRE);
  // drawRoadSegmentNEW(130, 40, 70, 20, 1, 0, 4, CARRE);
  print();
  return (0);
}
