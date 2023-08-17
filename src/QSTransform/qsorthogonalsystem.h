#ifndef QSORTHOGONALSYSTEM_H
#define QSORTHOGONALSYSTEM_H

#include "qscoordinatesystem.h"

class QSOrthogonalSystem : public QSCoordinateSystem
{
    bool        rflag = false;
    cv::Mat2f   rmatrix;
public:
    QSOrthogonalSystem() = delete;
    QSOrthogonalSystem(const float x, const float y,
                       std::string objIdent, const float angle = 0.f,
                       QSCoordinateSystem::Orientation sysOrient = RIGHT,
                       QSCoordinateSystem* parent = nullptr);
    QSOrthogonalSystem(const cv::Point2f &_zero,
                       std::string objIdent, const float angle = 0.f,
                       QSCoordinateSystem::Orientation sysOrient = RIGHT,
                       QSCoordinateSystem* parent = nullptr);

    void        setTurnAngle(float angle);

    inline cv::Point2f rotate(const cv::Point2f p) const;

    ///distance between 2 points
    float       distance(const cv::Point2f a, const cv::Point2f b) const override;
    ///returns coordinates in absolute coordinate system
    cv::Point2f getAbsolute(const cv::Point2f point) const override;
    ///returns coordinates in parents coordinate system
    cv::Point2f getRelative(const cv::Point2f point) const override;
    ///returns coordinates in parents coordinate system
    /// /// when end_hashcode received return in specified coordinate system
    cv::Point2f getRelative(const cv::Point2f point, const size_t end_hashcode) const override;
    ///returns coordinates in absolute coordinate system with specified type
    cv::Point2f getAbsolute(const cv::Point2f point, QSCoordinateSystem::Type t) const override;
    ///returns coordinates in parents coordinate system with sppecified type
    cv::Point2f getRelative(const cv::Point2f point, QSCoordinateSystem::Type t) const override;
    ///returns coordinates in parents coordinate system with sppecified type
    /// when end_hashcode received return in specified coordinate system
    /// if end_hashcode isnt found throw error
    cv::Point2f getRelative(const cv::Point2f point, const size_t end_hashcode,
                            QSCoordinateSystem::Type t) const override;
    ///returns relative coordinates in orthogonal system
    cv::Point2f getOrthogonal(const cv::Point2f point) const override;
    ///returns relative coordinates in polar system
    cv::Point2f getPolar(const cv::Point2f point) const override;

};

#endif // QSORTHOGONALSYSTEM_H
