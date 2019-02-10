#include "imgTool.h"
using namespace std;
const QRgb black = 0xFF000000;
const QRgb white = 0xFFFFFFFF;


QImage dwt(QImage src, int level, int width, int height)
{
    QImage copy = src.scaled(width, height, Qt::IgnoreAspectRatio);

    int m[1024][1024];
    int lumi[1024] = {0};
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            m[y][x] = qGray(copy.pixel(x, y));
            lumi[y] += m[y][x];
        }
        lumi[y] >>= 10;
    }

    int average = 0;
    for (int y = 0; y < height; y++)
        average += lumi[y];
    average >>= 10;

    for (int y = 0; y < height; y++) {
        int d = lumi[y] - average;
        for (int x = 0; x < width; x++) {
            m[y][x] -= d;
            if (m[y][x] > 255)
                m[y][x] = 255;
            else if (m[y][x] < 0)
                m[y][x] = 0;
        }
    }

    int k = 0;
    int a, b, c, d;
    for (int i = 0; i < level; i++) {
        k = (k == 0)? 1: (k << 1);
        for (int y = 0; y < height; y += (k << 1))
            for (int x = 0; x < width; x += (k << 1)) {
                a = m[y    ][x    ];
                b = m[y    ][x + k];
                c = m[y + k][x    ];
                d = m[y + k][x + k];
                m[y    ][x    ] = a + b + c + d;
                m[y    ][x + k] = a - b + c - d;
                m[y + k][x    ] = a + b - c - d;
                m[y + k][x + k] = a - b - c + d;
            }
    }

    for (int y = 0; y < height; y += (k << 1))
        for (int x = 0; x < width; x += (k << 1)) {
            m[y    ][x    ] = 0;
            //m[y    ][x + k] *= 2;
            //m[y + k][x    ] *= 2;
            //m[y + k][x + k] *= 2;
        }

    for (int i = 0; i < level; i++) {
        for (int y = 0; y < height; y+= (k << 1))
            for (int x = 0; x < width; x += (k << 1)) {
                a = m[y    ][x    ];
                b = m[y    ][x + k];
                c = m[y + k][x    ];
                d = m[y + k][x + k];
                m[y    ][x    ] = (a + b + c + d) >> 2;
                m[y    ][x + k] = (a - b + c - d) >> 2;
                m[y + k][x    ] = (a + b - c - d) >> 2;
                m[y + k][x + k] = (a - b - c + d) >> 2;
            }
        k >>= 1;
    }

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++) {
            if (-255 <= m[y][x] && m[y][x] <= 0)
                m[y][x] = -m[y][x];
            else
                m[y][x] = 0;
            copy.setPixel(x, y, QColor(m[y][x], m[y][x], m[y][x]).rgb());
        }

    return copy.scaled(src.width()/2, src.height()/2, Qt::IgnoreAspectRatio);
}

QImage toBinary(QImage source, int threshold)
{
    QImage copy = source;
    for (int y = 0; y < copy.height(); y++)
        for (int x = 0; x < copy.width(); x++) {
            if (qGray(copy.pixel(x, y)) >= threshold)
                copy.setPixel(x, y, white);
            else
                copy.setPixel(x, y, black);
        }
    return copy;
}

int autoThreshold(QImage src)
{
    int histogram[256] = {0};
    for (int y = 0; y < src.height(); y++)
        for (int x = 0; x < src.width(); x++) {
            histogram[ qGray(src.pixel(x, y)) ]++;
        }

    int top = 255;
    while(histogram[top] == 0)
        top--;
    if (top > 50)
        return top * 50 / 100;
    else
        return 25;
}

QImage erode(QImage source, int d)
{
    QImage extended(source.width() + 2 * d, source.height() + 2 * d,
                    source.format());
    extended.fill(Qt::white);

    for (int y = 0; y < source.height(); y++)
        for (int x = 0; x < source.width(); x++)
            extended.setPixel(x + d, y + d, source.pixel(x, y));

    QImage output(source);
    output.fill(Qt::black);
    for (int y = d; y < source.height() + d; y++)
        for (int x = d; x < source.width() + d; x++) {
            if (extended.pixel(x    , y    ) == white &&
                extended.pixel(x - 1, y    ) == white &&
                extended.pixel(x    , y - 1) == white &&
                extended.pixel(x + 1, y    ) == white &&
                extended.pixel(x    , y + 1) == white)
                output.setPixel(x - d, y - d, white);
            /*
            if (d < 2)
                continue;
            if (extended.pixel(x + 2, y    ) == black ||
                extended.pixel(x + 1, y + 1) == black ||
                extended.pixel(x    , y + 2) == black ||
                extended.pixel(x - 1, y + 1) == black ||
                extended.pixel(x - 2, y    ) == black ||
                extended.pixel(x - 1, y - 1) == black ||
                extended.pixel(x    , y - 2) == black ||
                extended.pixel(x + 1, y - 1) == black)
                output.setPixel(x - d, y - d, black);
            if (d < 3)
                continue;
            if (extended.pixel(x + 3, y    ) == black ||
                extended.pixel(x + 2, y + 1) == black ||
                extended.pixel(x + 1, y + 2) == black ||
                extended.pixel(x    , y + 3) == black ||
                extended.pixel(x - 1, y + 2) == black ||
                extended.pixel(x - 2, y + 1) == black ||
                extended.pixel(x - 3, y    ) == black ||
                extended.pixel(x - 2, y - 1) == black ||
                extended.pixel(x - 1, y - 2) == black ||
                extended.pixel(x    , y - 3) == black ||
                extended.pixel(x + 1, y - 2) == black ||
                extended.pixel(x + 2, y - 1) == black)
                output.setPixel(x - d, y - d, black);
            if (d < 4)
                continue;
            if (extended.pixel(x + 4, y    ) == black ||
                extended.pixel(x + 3, y + 1) == black ||
                extended.pixel(x + 2, y + 2) == black ||
                extended.pixel(x + 1, y + 3) == black ||
                extended.pixel(x    , y + 4) == black ||
                extended.pixel(x - 1, y + 3) == black ||
                extended.pixel(x - 2, y + 2) == black ||
                extended.pixel(x - 3, y + 1) == black ||
                extended.pixel(x - 4, y    ) == black ||
                extended.pixel(x - 3, y - 1) == black ||
                extended.pixel(x - 2, y - 2) == black ||
                extended.pixel(x - 1, y - 3) == black ||
                extended.pixel(x    , y - 4) == black ||
                extended.pixel(x + 1, y - 3) == black ||
                extended.pixel(x + 2, y - 2) == black ||
                extended.pixel(x + 3, y - 1) == black)
                output.setPixel(x - d, y - d, black);
            */
        }
    return output;
}

QImage dilate(QImage source, int d)
{
    QImage extended(source.width() + 2 * d, source.height() + 2 * d,
                    source.format());
    extended.fill(Qt::black);

    for (int y = 0; y < source.height(); y++)
        for (int x = 0; x < source.width(); x++)
            extended.setPixel(x + d, y + d, source.pixel(x, y));

    QImage output(source);
    output.fill(Qt::white);
    for (int y = d; y < source.height() + d; y++)
        for (int x = d; x < source.width() + d; x++) {
            if (extended.pixel(x    , y    ) == black &&
                extended.pixel(x - 1, y    ) == black &&
                extended.pixel(x    , y - 1) == black &&
                extended.pixel(x + 1, y    ) == black &&
                extended.pixel(x    , y + 1) == black)
                output.setPixel(x - d, y - d, black);
        }
    return output;

    /*
    QImage tmp(source);
    for (int y = 0; y < source.height(); y++)
        for (int x = 0; x < source.width(); x++)
            tmp.setPixel(x, y, white - (0x00FFFFFF & tmp.pixel(x, y)));
    tmp = erode(tmp, d);
    for (int y = 0; y < source.height(); y++)
        for (int x = 0; x < source.width(); x++)
            tmp.setPixel(x, y, white - (0x00FFFFFF & tmp.pixel(x, y)));
    return tmp;
    */
}

