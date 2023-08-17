#include "qsorthogonalsystem.h"

QSOrthogonalSystem::QSOrthogonalSystem(const float x, const float y,
        std::string objIdent, float angle,
        QSCoordinateSystem::Orientation sysOrient,
        QSCoordinateSystem* parent) :
    QSCoordinateSystem(objIdent, ORTHOGONAL, sysOrient, parent)
{
    zero.x = x;
    zero.y = y;
    rmatrix.create(2, 2);
    setTurnAngle(angle);
}

QSOrthogonalSystem::QSOrthogonalSystem(const cv::Point2f &_zero,
        std::string objIdent, float angle,
        QSCoordinateSystem::Orientation sysOrient,
        QSCoordinateSystem* parent) :
    QSCoordinateSystem(objIdent, ORTHOGONAL, sysOrient, parent)
{
    zero = _zero;
    rmatrix.create(2, 2);
    setTurnAngle(angle);
}

inline cv::Point2f QSOrthogonalSystem::rotate(const cv::Point2f point) const
{
    return {rmatrix.at<float>(0,0)*point.x + rmatrix.at<float>(0,1)*point.y,
            rmatrix.at<float>(1,0)*point.x + rmatrix.at<float>(1,1)*point.y};
    /*switch (type)
    {
    case ORTHOGONAL:

        break;
    case POLAR:
        return {point.x, point.y + phi};
        break;
    case COPY:
        return point;
        break;
    }*/
}

float QSOrthogonalSystem::distance(const cv::Point2f a, const cv::Point2f b) const
{
    return sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y));
}

void QSOrthogonalSystem::setTurnAngle(float angle)
{
    phi = angle;
    rflag = QSCoordinateSystem::bigAngle(phi);
    if(!rflag)
    {
        rmatrix.at<float>(0,0) = 1.f;
        rmatrix.at<float>(1,0) = 0.f;
        rmatrix.at<float>(0,1) = 0.f;
        rmatrix.at<float>(1,1) = 1.f;
        return;
    }
    if(orient)
    {
        rmatrix.at<float>(0,0) = cos(angle);
        rmatrix.at<float>(1,0) = -sin(angle);
        rmatrix.at<float>(0,1) = sin(angle);
        rmatrix.at<float>(1,1) = cos(angle);
    }
    else
    {
        rmatrix.at<float>(0,0) = cos(angle);
        rmatrix.at<float>(1,0) = sin(angle);
        rmatrix.at<float>(0,1) = -sin(angle);
        rmatrix.at<float>(1,1) = cos(angle);
    }

}

cv::Point2f QSOrthogonalSystem::getAbsolute(const cv::Point2f point) const
{
    if(!parent)
        return point;
    return parent->getAbsolute(getRelative(point));
}

//rewrite!
cv::Point2f QSOrthogonalSystem::getRelative(const cv::Point2f point) const
{
    if(!parent)
        return point;
    switch (type)
    {
    case ORTHOGONAL:
        switch (parent->getType())
        {
        case ORTHOGONAL:
            return zero + (rflag ? rotate(point) : point);
            break;
        case POLAR:
            return toPolar((rflag ? rotate(point) : point) + toOrthogonal(zero));
            break;
        case COPY:
            return parent->getRelative(point);
            break;
        }
        break;
    case POLAR:
        switch (parent->getType())
        {
        case ORTHOGONAL:
            return zero + toOrthogonal(rflag ? rotate(point) : point);
            break;
        case POLAR:
            return toPolar(toOrthogonal(zero) + toOrthogonal(rflag ? rotate(point) : point));
            break;
        case COPY:
            return parent->getRelative(point);
            break;
        }
        break;
    case COPY:
        return point;
        break;
    }
}

cv::Point2f QSOrthogonalSystem::getRelative(const cv::Point2f point, const size_t end_hashcode) const
{
    if(hashcode==end_hashcode)
        return point;
    if(!parent)
        throw 1; //TODO: add proper exceptino
    return parent->getRelative(getRelative(point), end_hashcode);
}

cv::Point2f QSOrthogonalSystem::getAbsolute(const cv::Point2f point, QSOrthogonalSystem::Type t) const
{
    switch (type)
    {
    case ORTHOGONAL:
        break;
    case POLAR:
        break;
    case COPY:
        break;
    }
}

cv::Point2f QSOrthogonalSystem::getRelative(const cv::Point2f point, QSOrthogonalSystem::Type t) const
{
    switch (type)
    {
    case ORTHOGONAL:
        break;
    case POLAR:
        break;
    case COPY:
        break;
    }
}

cv::Point2f QSOrthogonalSystem::getRelative(const cv::Point2f point, const size_t end_hashcode,
                        QSCoordinateSystem::Type t) const
{

}

cv::Point2f QSOrthogonalSystem::getOrthogonal(const cv::Point2f point) const
{
    switch (type)
    {
    case ORTHOGONAL:
        break;
    case POLAR:
        break;
    case COPY:
        break;
    }
}

cv::Point2f QSOrthogonalSystem::getPolar(const cv::Point2f point) const
{
    switch (type)
    {
    case ORTHOGONAL:
        break;
    case POLAR:
        break;
    case COPY:
        break;
    }
}
