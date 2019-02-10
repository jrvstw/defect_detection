#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QTimer>

class myWidget : public QWidget
{
    Q_OBJECT

    public:
        explicit myWidget(char pics[][30], int nPics,
                          int period, QWidget *parent = 0);

    public slots:
        void timeout();

    protected:
        void mousePressEvent(QMouseEvent *);
        void keyPressEvent(QKeyEvent *event);
        void paintEvent(QPaintEvent *);

    private:
        char    (*source)[30];
        int     nSource;
        int     index;
        QTimer  *timer;
};

#endif //MYWIDGET_H
