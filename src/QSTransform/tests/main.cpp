#include <iostream>
#include <QString>
#include <QDebug>
#include <time.h>
#include "qapplication.h"
#include "qsorthogonaltransform.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSOrthogonalSystem sys0(0,0,"aboba");
    QSOrthogonalSystem sys1(100,100,"aboba1", M_PI/4, QSCoordinateSystem::RIGHT,
                            &sys0);
    QSOrthogonalTransform tr(&sys1);
    tr.setAreaSize({100, 100});
    auto e = tr.subMatrixRectForRotate();
    qDebug() << e.x << " " << e.y;
    qDebug() << e.width << " " << e.height;
    return a.exec();
}
