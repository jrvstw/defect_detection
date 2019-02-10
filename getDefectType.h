#ifndef GETDEFECTTYPE_H
#define GETDEFECTTYPE_H

#include <QImage>
#include <QRect>
#include <QPoint>
#include <QString>
#include <vector>
#include <queue>
using namespace std;

enum defectType{normal, impact, scratch, sponge, water, unrecognized};

defectType getDefectType(vector<QPoint> object, QImage *contour,
                         QRect *bBox, QString *label);
int cross(QPoint O, QPoint A, QPoint B);
int compare(const void* C, const void* D);
void convexHull(QPoint convex[10000], int *top, QPoint input[10000], int ninput);
void getConvex(vector<QPoint> object, int *area, double *perimeter);
void findObject(int x, int y, QImage *src, vector<QPoint> *object);
void visit(int dx, int dy, queue<QPoint> *q, vector<QPoint> *object,
           QImage *src);
void scaleCoords(QRect *bBox, QRect after, QRect before);

#endif//GETDEFECTTYPE_H
