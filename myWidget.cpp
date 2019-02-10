#include "myWidget.h"
#include "imgTool.h"
#include "getDefectType.h"

#include <QPainter>
#include <QTimer>
#include <fstream>
#include <vector>
#include <QKeyEvent>

using namespace std;
const QRgb black = 0xFF000000;
const QRgb white = 0xFFFFFFFF;


myWidget::myWidget(char pics[][30], int nPics, int period, QWidget *parent) :
    QWidget(parent)
{
    source = pics;
    nSource = nPics;

    if (period > 0) {
        timer = new QTimer(this);
        QObject::connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
        timer->start(period);
    }
}

void myWidget::timeout()
{
    index = (index == nSource - 1)? 0: index + 1;
    this->update();
}

void myWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space ||
        event->key() == Qt::Key_Return ||
        event->key() == Qt::Key_Right) {
        index = (index == nSource - 1)? 0: index + 1;
        this->update();
    } else if (event->key() == Qt::Key_Left ||
               event->key() == Qt::Key_Backspace) {
        index = (index == 0)? nSource - 1: index -1;
        this->update();
    } else if (event->key() == Qt::Key_Escape) {
        this->close();
    }
}

void myWidget::mousePressEvent(QMouseEvent *)
{
    index = (index == nSource - 1)? 0: index + 1;
    this->update();
}

void myWidget::paintEvent(QPaintEvent *)
{
    // 0. setup
    QPainter painter(this);
    const QImage src(source[index]);

    QSize paintSize(src.size());
    paintSize.scale(this->size(), Qt::KeepAspectRatio);

    QRect canvas;
    canvas.setSize(paintSize);
    canvas.moveCenter(this->rect().center());

    // 1. paint original image

    /*
    QRect subCanvas(canvas.x(), canvas.y(),
                    canvas.width() / 2, canvas.height() / 2);
                    */
    QRect subCanvas = canvas;
    painter.drawImage(subCanvas, src);

    // 2. paint dwt result

    QImage copy = dwt(src, 6, 1024, 1024);

    //subCanvas.moveTo(canvas.x(), canvas.y() + canvas.height() / 2);
    //painter.drawImage(subCanvas, copy);

    // 3. paint binary image

    copy = toBinary(copy, autoThreshold(copy));

    //subCanvas.moveTo(canvas.x() + canvas.width() / 2,
                     //canvas.y() + canvas.height() / 2);
    //painter.drawImage(subCanvas, copy);

    // 4. paint processed image

    copy = dilate(copy, 1);
    copy = erode(copy, 1);

    //subCanvas.moveTo(canvas.x() + canvas.width() / 2,
                     //canvas.y());

    // 4.2 paint frames on processed image

    QImage contour = erode(copy, 1);
    for (int y = 0; y < copy.height(); y++)
        for (int x = 0; x < copy.width(); x++)
            contour.setPixel(x, y, copy.pixel(x, y) -
                             (contour.pixel(x, y) & 0xFFFFFF));
    //subCanvas = canvas;
    //painter.drawImage(subCanvas, contour);
    //painter.drawImage(subCanvas, copy);

    QImage tmpImg(copy);

    for (int y = 0; y < copy.height(); y++)
        for (int x = 0; x < copy.width(); x++)
            if (copy.pixel(x, y) != black) {
                vector<QPoint> object;
                findObject(x, y, &tmpImg, &object);
                QRect bBox(QPoint(x, y), QPoint(x, y));
                QString label;
                defectType type = getDefectType(object, &contour, &bBox,
                                                &label);

                if (type == normal)
                    continue;
                else if (type == impact)
                    painter.setPen(QPen(Qt::red, 2));
                else if (type == scratch)
                    painter.setPen(QPen(Qt::green, 2));
                else if (type == sponge)
                    painter.setPen(QPen(Qt::yellow, 2));
                else if (type == water)
                    painter.setPen(QPen(Qt::blue, 2));
                else if (type == unrecognized)
                    painter.setPen(QPen(Qt::white, 2));

                scaleCoords(&bBox, subCanvas, copy.rect());
                bBox.translate(subCanvas.topLeft());
                painter.drawRect(bBox);
                painter.drawText(bBox.topLeft(), label);
            }
    // end of myWidget::paintEvent(QPaintEvent *)
}

