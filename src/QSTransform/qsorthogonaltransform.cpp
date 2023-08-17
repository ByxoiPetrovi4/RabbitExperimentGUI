#include "qsorthogonaltransform.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"


QSOrthogonalTransform::QSOrthogonalTransform(QSCoordinateSystem *coor_sys)
    : QSTransform(coor_sys)
{

}

QSOrthogonalTransform::QSOrthogonalTransform(QSCoordinateSystem *coor_sys, const float phi,
                      const cv::Point2f zero, const cv::Size2f size) : QSTransform(coor_sys)
{
    sys->setZero(zero);
    static_cast<QSOrthogonalSystem*>(sys)->setTurnAngle(phi);
    areaSize = size;
}

void    QSOrthogonalTransform::setTurnAngle(const float phi)
{
    static_cast<QSOrthogonalSystem*>(sys)->setTurnAngle(phi);
}

cv::Rect2f  QSOrthogonalTransform::subMatrixRectForRotate()
{
    auto rect = cv::Rect2f{sys->getRelative(sys->getZero()), cv::Size{(int)areaSize.width,
            (int)areaSize.height}};
    cv::Rect2f ret;
    auto fx = {sys->getRelative({0.f, 0.f}).x,
               sys->getRelative({areaSize.width, 0.f}).x,
               sys->getRelative({0.f, areaSize.height}).x,
               sys->getRelative({areaSize.width, areaSize.height}).x};
    auto fy = {sys->getRelative({0.f, 0.f}).y,
               sys->getRelative({areaSize.width, 0.f}).y,
               sys->getRelative({0.f, areaSize.height}).y,
               sys->getRelative({areaSize.width, areaSize.height}).y,
               sys->getRelative({0.f, 0.f}).y - areaSize.height};
    auto maxx = std::max(fx);
    auto maxy = std::max(fy);
    auto minx = std::min(fx);
    auto miny = std::min(fy);
    ret.y = miny;
    ret.x = minx;
    ret.width = maxx - minx;
    ret.height = maxy - miny;
    return ret;
}

void QSOrthogonalTransform::transform(const cv::Mat &src, cv::Mat &dst)
{
    switch(sys->getType())
    {
    case QSCoordinateSystem::ORTHOGONAL:
        if(QSCoordinateSystem::bigAngle(sys->getPhi()))
        {
            auto rect = subMatrixRectForRotate();
            cv::Mat tmp(src, rect);
            cv::Mat rot_mat = cv::getRotationMatrix2D({0,areaSize.height}, sys->getPhi()*180.f/M_PI, 1.0);
            cv::Mat warp_buf(cv::Size{(int)rect.width, (int)rect.height}, src.type());
            warpAffine( tmp, warp_buf, rot_mat, warp_buf.size() );
            cv::Mat warp_dst(warp_buf, {0,0,(int)areaSize.width,(int)areaSize.height});
            dst = cv::Mat(warp_dst.rows, warp_dst.cols, warp_dst.type(), dst.data);
            warp_dst.copyTo(dst);
        }
        else
        {
            auto z = sys->getZero();
            cv::Mat warp_dst(src, {(int)z.x,(int)z.y,(int)areaSize.width,(int)areaSize.height});
            dst = cv::Mat(warp_dst.rows, warp_dst.cols, warp_dst.type(), dst.data);
            warp_dst.copyTo(dst);
        }
        break;
    }
}
