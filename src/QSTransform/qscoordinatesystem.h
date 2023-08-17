#ifndef QSCOORDINATESYSTEM_H
#define QSCOORDINATESYSTEM_H

#include <inttypes.h>
#include <string>
#include <functional>
#include <opencv2/core.hpp>

class QSCoordinateSystem
{
public:
    enum Type : uint8_t
    {
        ORTHOGONAL = 0,
        POLAR,
        COPY
    };
    enum Orientation : uint8_t
    {
        LEFT = 0,
        RIGHT
    };

protected:
    ///const coordiante system properties
    const QSCoordinateSystem*   parent;
    const Type                  type;
    const Orientation           orient;
    ///Generated when object is creating
    const size_t                hashcode;
    ///Angle between this and parents axes if it has same directions than its 0.f
    float       phi = 0.f;
    ///Coordinate of 0 in parent's coordinate system
    cv::Point2f zero;

public:
    QSCoordinateSystem() = delete;
    QSCoordinateSystem(std::string objIdent, QSCoordinateSystem::Type sysType,
                       QSCoordinateSystem::Orientation sysOrient, QSCoordinateSystem* parent = nullptr);

    virtual ~QSCoordinateSystem();

    static constexpr float      toRads = M_PI/180.f;
    static constexpr float      toDegs = 180.f/M_PI;
    ///return true if |angle|>0.05f rad else return false
    static bool          bigAngle(float angle);

    //DELETE FUNCS
    static cv::Point2f          toPolar(const cv::Point2f p);
    static cv::Point2f          toOrthogonal(const cv::Point2f p);

    void        setZero(cv::Point2f _zero);

    QSCoordinateSystem::Type            getType() const;
    QSCoordinateSystem::Orientation     getOrientation() const;
    cv::Point2f                         getZero() const;

    ///Return angle between this system and parent's system
    float               getPhi() const;
    ///distance between 2 points
    virtual float       distance(const cv::Point2f a, const cv::Point2f b) const = 0;
    ///returns coordinates in absolute coordinate system
    virtual cv::Point2f getAbsolute(const cv::Point2f point) const = 0;
    ///returns coordinates in parents coordinate system
    virtual cv::Point2f getRelative(const cv::Point2f point) const = 0;
    ///returns coordinates in parents coordinate system
    /// /// when end_hashcode received return in specified coordinate system
    virtual cv::Point2f getRelative(const cv::Point2f point, const size_t end_hashcode)
    const = 0;
    ///returns coordinates in absolute coordinate system with specified type
    virtual cv::Point2f getAbsolute(const cv::Point2f point, QSCoordinateSystem::Type t)
    const = 0;
    ///returns coordinates in parents coordinate system with sppecified type
    virtual cv::Point2f getRelative(const cv::Point2f point, QSCoordinateSystem::Type t)
    const = 0;
    ///returns coordinates in parents coordinate system with sppecified type
    /// when end_hashcode received return in specified coordinate system
    /// if end_hashcode isnt found throw error
    virtual cv::Point2f getRelative(const cv::Point2f point, const size_t end_hashcode,
                            QSCoordinateSystem::Type t) const = 0;
    ///returns relative coordinates in orthogonal system
    virtual cv::Point2f getOrthogonal(const cv::Point2f point) const = 0;
    ///returns relative coordinates in polar system
    virtual cv::Point2f getPolar(const cv::Point2f point) const = 0;
};

#endif // QSCOORDINATESYSTEM_H
