#ifndef QSTRANSFORM_H
#define QSTRANSFORM_H

#include "qscoordinatesystem.h"

class QSTransform
{
public:
    typedef uint8_t TransformType;
protected:
    QSCoordinateSystem  *sys = nullptr;
    cv::Size2f          areaSize;
    TransformType       type;
public:
    const static TransformType TRANSFORM_STATIC = 0x10u,
        TRANSFORM_DYNAMIC = 0x00u,
        TRANSFORM_READY = 0x01u,
        TRANSFORM_AWAITS = 0x00u;

    QSTransform() = delete;
    QSTransform(QSCoordinateSystem *coor_sys, TransformType t = TRANSFORM_STATIC);

    virtual ~QSTransform();

    void            setZeroPoint(const cv::Point2f zero);
    void            setAreaSize(const cv::Size2f size);
    void            setArea(const cv::Rect2i area);

    TransformType   getType() const;

    virtual void    transform(const cv::Mat &src, cv::Mat &dst) = 0;
};

#endif // QSTRANSFORM_H
