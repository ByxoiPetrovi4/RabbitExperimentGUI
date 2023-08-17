#ifndef QSORTHOGONALTRANSFORM_H
#define QSORTHOGONALTRANSFORM_H

#include "qstransform.h"
#include "qsorthogonalsystem.h"

///TODO: major fixes required especially with matrix rotation

class QSOrthogonalTransform : public QSTransform
{
public:
    QSOrthogonalTransform() = delete;
    QSOrthogonalTransform(QSCoordinateSystem *coor_sys);
    QSOrthogonalTransform(QSCoordinateSystem *coor_sys, const float phi,
                          const cv::Point2f zero, const cv::Size2f size);

    void            setTurnAngle(const float phi);
    cv::Rect2f      subMatrixRectForRotate();

    void transform(const cv::Mat &src, cv::Mat &dst) override;
};

#endif // QSORTHOGONALTRANSFORM_H
