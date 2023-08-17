#include "qscoordinatesystem.h"

QSCoordinateSystem::QSCoordinateSystem(std::string objIdent,
    QSCoordinateSystem::Type sysType, QSCoordinateSystem::Orientation sysOrient,
    QSCoordinateSystem* parent)
    :   type(sysType), orient(sysOrient),
      hashcode(std::hash<std::string>{}(objIdent)), parent{parent}
{

}

QSCoordinateSystem::~QSCoordinateSystem()
{

}

float QSCoordinateSystem::getPhi() const
{
    return phi;
}

bool   QSCoordinateSystem::bigAngle(float angle)
{
    return !(angle > -0.05f && angle < 0.05f);
}

cv::Point2f QSCoordinateSystem::toPolar(const cv::Point2f p)
{
    return {float(sqrt(p.x*p.x + p.y*p.y)), float(cv::fastAtan2(p.y, p.x)*toRads)};
}

cv::Point2f QSCoordinateSystem::toOrthogonal(const cv::Point2f p)
{
    return {float(p.x*cos(p.y)), float(p.x*sin(p.y))};
}


QSCoordinateSystem::Type QSCoordinateSystem::getType() const
{
    return type;
}

QSCoordinateSystem::Orientation     QSCoordinateSystem::getOrientation() const
{
    return orient;
}

cv::Point2f QSCoordinateSystem::getZero() const
{
    return zero;
}

void QSCoordinateSystem::setZero(cv::Point2f _zero)
{
    zero = _zero;
}


