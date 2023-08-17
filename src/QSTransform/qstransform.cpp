#include "qstransform.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

QSTransform::QSTransform(QSCoordinateSystem *coor_sys, TransformType t)
{
    sys = coor_sys;
    type = t;
}

QSTransform::~QSTransform()
{
    if(sys)
        delete sys;
}

void    QSTransform::setZeroPoint(const cv::Point2f zero)
{
    sys->setZero(zero);
}

void    QSTransform::setAreaSize(const cv::Size2f size)
{
    areaSize = size;
}

void    QSTransform::setArea(const cv::Rect2i area)
{
    sys->setZero({(float)area.x,(float)area.y});
    areaSize.width = area.width;
    areaSize.height = area.height;
}

QSTransform::TransformType QSTransform::getType() const
{
    return type & TRANSFORM_STATIC;
}
