#ifndef IMGTOOL_H
#define IMGTOOL_H

#include <QImage>
#include <QRect>
#include <QPoint>
#include <QColor>
using namespace std;

QImage dwt(QImage source, int level, int width, int height);
QImage toBinary(QImage source, int threshold);
int autoThreshold(QImage source);
QImage erode(QImage source, int d);
QImage dilate(QImage source, int d);

#endif //IMGTOOL_H

