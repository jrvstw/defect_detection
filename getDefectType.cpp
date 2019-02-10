#include "getDefectType.h"
#include <cmath>
#include <vector>
using namespace std;
const QRgb black = 0xFF000000;
const QRgb white = 0xFFFFFFFF;

defectType getDefectType(vector<QPoint> object, QImage *contour,
                         QRect *bBox, QString *label)
{
    int area = object.size();
    label->clear();

    if (area < 40 || area > 3000) {
        return normal;
    }

    int perimeter = 0;
    double xSum = 0, ySum = 0, xxSum = 0, xySum = 0, yySum = 0;

    for (int i = 0; i < area; i++) {
        QPoint p(object[i].x(), object[i].y());
        *bBox = bBox->united(QRect(p, p));
        if (contour->pixel(p) == white)
            perimeter++;
        xSum += p.x();
        ySum += p.y();
        xxSum += p.x() * p.x();
        xySum += p.x() * p.y();
        yySum += p.y() * p.y();
    }

    int convexArea;
    double convexPerimeter;
    getConvex(object, &convexArea, &convexPerimeter);
    double xMean = xSum / area,
           yMean = ySum / area,
           nSxSx = xxSum - xMean * xSum,
           nSySy = yySum - yMean * ySum,
           orientation = -(xySum - xMean*ySum) / nSxSx,
           correlation = orientation * sqrt(nSxSx / nSySy);

    double compactness = convexPerimeter*convexPerimeter/12.56/convexArea;
    //label->setNum(compactness);

    if (true &&
        compactness < 1.3 &&
        area > 100 &&
        true) {
        label->append(QString("water"));
        return water;
    }

    if (true &&
        compactness < 1.5 &&
        area > 1000 &&
        true) {
        label->append(QString("water"));
        return water;
    }

    if (true &&
        compactness > 1.5 &&
        100 < area && area < 220 &&
        bBox->height() < bBox->width() &&
        abs(orientation) < 0.4 &&
        true) {
        label->append(QString("scratch"));
        return scratch;
    }

    if (true &&
        170 < area && area < 800 &&
        bBox->height() < bBox->width() * 3 &&
        abs(correlation) > 0.45 &&
        true) {
        label->append(QString("impact"));
        return impact;
    }

    if (true &&
        75 < area && area < 110 &&
        1.29 < compactness && compactness < 1.42 &&
        true) {
        label->append(QString("sponge"));
        return sponge;
    }

    return normal;
}

int cross(QPoint O, QPoint A, QPoint B)
{
    return (A.x()-O.x()) * (B.y()-A.y()) - (B.x()-A.x()) * (A.y()-O.y());
}

int compare(const void* C, const void* D)
{
    QPoint A = *(QPoint *)C,
           B = *(QPoint *)D;
    if (A.x() != B.x())
        return A.x() - B.x();
    else
        return A.y() - B.y();
}

void convexHull(QPoint convex[10000], int *top, QPoint input[10000], int ninput)
{
    int i = 0;
    for (i = 0; i < ninput; i++) {
        while (*top > 0 &&
               cross(convex[*top - 1], convex[*top], input[i]) <= 0)
            (*top)--;
        convex[++(*top)] = input[i];
    }
    int middle = *top;
    for (i = ninput - 2; i >=0; i--) {
        while (*top > middle &&
               cross(convex[*top - 1], convex[*top], input[i]) <= 0)
            (*top)--;
        convex[++(*top)] = input[i];
    }
    (*top)--;
}

void getConvex(vector<QPoint> object, int *area, double *perimeter)
{
    int size = object.size();
    QPoint array[10000];
    for (int i = 0; i < size; i++)
        array[i] = object[i];
    qsort(array, size, sizeof(QPoint), compare);
    QPoint convex[10000];

    int top = -1;
    convexHull(convex, &top, array, size);

    *area = (convex[top].x() * convex[0].y() -
             convex[top].y() * convex[0].x());
    int dx = convex[top].x() - convex[0].x(),
        dy = convex[top].y() - convex[0].y();
    *perimeter = sqrt(dx * dx + dy * dy);
    for (int i = 0; i <= top - 1; i++) {
        *area += (convex[i].x() * convex[i + 1].y() -
                  convex[i].y() * convex[i + 1].x());
        dx = convex[i + 1].x() - convex[i].x();
        dy = convex[i + 1].y() - convex[i].y();
        *perimeter += sqrt(dx * dx + dy * dy);
    }
   *area = abs(*area) / 2;
}

void findObject(int x0, int y0, QImage *src, vector<QPoint> *object)
{
    object->push_back(QPoint(x0, y0));
    queue<QPoint> q;
    q.push(QPoint(x0, y0));
    src->setPixel(x0, y0, black);
    while (q.size() > 0) {
        visit( 1,  0, &q, object, src);
        visit( 0,  1, &q, object, src);
        visit(-1,  0, &q, object, src);
        visit( 0, -1, &q, object, src);
        visit( 1,  1, &q, object, src);
        visit(-1,  1, &q, object, src);
        visit(-1, -1, &q, object, src);
        visit( 1, -1, &q, object, src);
        visit( 2,  0, &q, object, src);
        visit( 0,  2, &q, object, src);
        visit(-2,  0, &q, object, src);
        visit( 0, -2, &q, object, src);
        visit( 3,  0, &q, object, src);
        visit( 2,  1, &q, object, src);
        visit( 1,  2, &q, object, src);
        visit( 0,  3, &q, object, src);
        visit(-1,  2, &q, object, src);
        visit(-2,  1, &q, object, src);
        visit(-3,  0, &q, object, src);
        visit(-2, -1, &q, object, src);
        visit(-1, -2, &q, object, src);
        visit( 0, -3, &q, object, src);
        visit( 1, -2, &q, object, src);
        visit( 2, -1, &q, object, src);
        q.pop();
    }
}

void visit(int dx, int dy, queue<QPoint> *q, vector<QPoint> *object,
           QImage *src)
{
    QPoint p(q->front().x() + dx,
             q->front().y() + dy);
    if (src->rect().contains(p) && src->pixel(p) != black) {
        q->push(p);
        object->push_back(p);
        src->setPixel(p, black);
    }
}

void scaleCoords(QRect *bBox, QRect after, QRect before)
{
    bBox->setCoords(bBox->left()  * after.width() / before.width(),
                    bBox->top()    * after.height() / before.height(),
                    bBox->right() * after.width() / before.width(),
                    bBox->bottom() * after.height() / before.height());
}

