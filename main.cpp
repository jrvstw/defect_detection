#include <cstring>
#include <QApplication>
#include "myWidget.h"

void import_pics(char source[][30]);

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    int  nSource = 20;
    char source[20][30];
    import_pics(source);
    int  period = 0; // millisec.

    myWidget *defectDetector = new myWidget(source, nSource, period);
    defectDetector->resize(960,540);
    //defectDetector->showFullScreen();
    defectDetector->show();

    return app.exec();
}

void import_pics(char source[][30])
{
    strcpy(source[ 0], "./pics/impact51.jpg");
    strcpy(source[ 1], "./pics/impact52.jpg");
    strcpy(source[ 2], "./pics/impact53.jpg");
    strcpy(source[ 3], "./pics/impact54.jpg");
    strcpy(source[ 4], "./pics/impact55.jpg");
    strcpy(source[ 5], "./pics/scratch271.jpg");
    strcpy(source[ 6], "./pics/scratch272.jpg");
    strcpy(source[ 7], "./pics/scratch273.jpg");
    strcpy(source[ 8], "./pics/scratch274.jpg");
    strcpy(source[ 9], "./pics/scratch275.jpg");
    strcpy(source[10], "./pics/sponge457.jpg");
    strcpy(source[11], "./pics/sponge458.jpg");
    strcpy(source[12], "./pics/sponge459.jpg");
    strcpy(source[13], "./pics/sponge460.jpg");
    strcpy(source[14], "./pics/sponge461.jpg");
    strcpy(source[15], "./pics/water1.jpg");
    strcpy(source[16], "./pics/water2.jpg");
    strcpy(source[17], "./pics/water3.jpg");
    strcpy(source[18], "./pics/water4.jpg");
    strcpy(source[19], "./pics/water5.jpg");
}

