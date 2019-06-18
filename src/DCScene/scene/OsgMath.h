#ifndef _OSGOSGMATH_H_
#define _OSGOSGMATH_H_

#include <osg/Geometry>

#define MY_PI 3.14159265358979323846

// extract from Capaware

/**
    \brief Contains several common Mathematical functions
 */
class OsgMath
{
public:

    /** Default constructor */
    OsgMath();

    /** Destructor */
    ~OsgMath();

    /** Computes the length of a 3d vector
        \param v is the 3d vector
     */
    static double VectorLength(const osg::Vec3d &v);

    /** Computes the length of a 3d vector
        \param v is the 3d vector
     */
    static float VectorLength(const osg::Vec3f &v);

    /** Normalize a 3d vector
        \param v is the 3d vector
     */
    static void NormalizeVec(osg::Vec3d &v);

    /** Normalize a 3d vector
        \param v is the 3d vector
     */
    static void NormalizeVec(osg::Vec3f &v);

    /** Computes the cross product between two vectors
        \param v1 the first vector
        \param v2 the second vector
     */
    static osg::Vec3d CrossProduct(const osg::Vec3d &v1, const osg::Vec3d &v2);

    /** Computes the cross product between two vectors
        \param v1 the first vector
        \param v2 the second vector
     */
    static osg::Vec3f CrossProduct(const osg::Vec3f &v1, const osg::Vec3f &v2);

    /** Computes the scalar product between two vectors
        \param v1 the first vector
        \param v2 the second vector
     */
    static double DotProduct(const osg::Vec3d &v1, const osg::Vec3d &v2) { return v1 * v2; }

    /** Computes the scalar product between two vectors
        \param v1 the first vector
        \param v2 the second vector
     */
    static float DotProduct(const osg::Vec3f &v1, const osg::Vec3f &v2) { return v1 * v2; }

    /** Computes the distance between two 3d points
        \param p1 the first point
        \param p2 the second point
     */
    static double DistanceBetweenTwoPoints(const osg::Vec3d &p1, const osg::Vec3d &p2);

    /** Computes the distance between two 3d points
        \param p1 the first point
        \param p2 the second point
     */
    static float DistanceBetweenTwoPoints(const osg::Vec3f &p1, const osg::Vec3f &p2);

    /** Computes the angle between two 3d vectors
        \param v1 the first point
        \param v2 the second point
     */
    static double AngleBetweenTwoVectorsInRadians(osg::Vec3d &v1, osg::Vec3d &v2);

    /** Computes the angle between two 3d vectors
        \param v1 the first point
        \param v2 the second point
     */
    static float AngleBetweenTwoVectorsInRadians(osg::Vec3f &v1, osg::Vec3f &v2);

    /** Computes the angle between two 3d vectors in grades
        \param v1 the first point
        \param v2 the second point
     */
    static double AngleBetweenTwoVectorsInGrades(osg::Vec3d &v1, osg::Vec3d &v2);

    /** Computes the angle between two 3d vectors in grades
        \param v1 the first point
        \param v2 the second point
     */
    static float AngleBetweenTwoVectorsInGrades(osg::Vec3f &v1, osg::Vec3f &v2);

    /** Rotates a point with respect to a given axis
        \param point to be rotated
        \param axis for the rotation
        \param angle to be rotated
        \param angle_in_grades determines whether the angle is in grades or radians
     */
    static void RotatePoint(osg::Vec3d &point,const osg::Vec3d &axis, const double &angle, const bool &angle_in_grades = true);

    /** Rotates a point with respect to a given axis
        \param point to be rotated
        \param axis for the rotation
        \param angle to be rotated
        \param angle_in_grades determines whether the angle is in grades or radians
     */
    static void RotatePoint(osg::Vec3f &point,const osg::Vec3f &axis, const float &angle, const bool &angle_in_grades = true);

    /** Returns the value of PI */
    static double PI() { return MY_PI; }

    // Make a Quat from hpr angles, avoiding gimble lock. It does this by
    // creating an orthonormal basis of xyz axis vectors and reorienting them
    // in sequence: first by heading, then pitch, then roll.
    // hpr angles are in degrees.
    // Assumes left-handed and z-up, so:
    //   h rotates around the z axis.
    //   p rotates aroung the x' axis.
    //   r rotates around the y'' axis.
    static osg::Quat makeHPRQuat( double h, double p, double r );

    // Same as above, but hpr angles stored as xyz elements of a Vec3.
    static osg::Quat makeHPRQuat( osg::Vec3 rotAngles );
};

/** Class to represent a geometrical plane
 */
class Plane
{
public:

    /** Default constructor */
    Plane();

    /** Constructor */
    Plane(const double &x, const double &y, const double &z, const double &w);

    /** Copy constructor
        \param plane to copy
     */
    Plane(const Plane &plane);

    /** Destructor */
    ~Plane();

    /** Overload of operator =
        \param plane to copy
     */
    Plane & operator =(const Plane &plane);

    /** Assigns a new value to the x coordinate
        \param x
     */
    void SetX(const double &x) { coords[0] = x; }

    /** Assigns a new value to the y coordinate
        \param y
     */
    void SetY(const double &y) { coords[1] = y; }

    /** Assigns a new value to the z coordinate
        \param z
     */
    void SetZ(const double &z) { coords[2] = z; }

    /** Assigns a new value to the w coordinate
        \param w
     */
    void SetW(const double &w) { coords[3] = w; }

    /** Obtains the x coordinate */
    double & GetX() { return coords[0]; }

    /** Obtains the y coordinate */
    double & GetY() { return coords[1]; }

    /** Obtains the z coordinate */
    double & GetZ() { return coords[2]; }

    /** Obtains the w coordinate */
    double & GetW() { return coords[3]; }

    /** Obtains the x coordinate */
    const double & GetX() const { return coords[0]; }

    /** Obtains the y coordinate */
    const double & GetY() const { return coords[1]; }

    /** Obtains the z coordinate */
    const double & GetZ() const { return coords[2]; }

    /** Obtains the w coordinate */
    const double & GetW() const { return coords[3]; }

    /** Returns the angle between to planes
        \param plane is the second plane
     */
    double Angle(const Plane &plane);

    /** Computes the distance between the plane and a point
        \param point
     */
    double Distance(const osg::Vec3d &point);

    /** Computes the distance between the plane and a point
        \param point
     */
    double Distance(const osg::Vec3f &point);

    /** Computes the distance between the plane and a point
        \param point
     */
    double Distance(const osg::Vec3d &point) const;

    /** Computes the distance between the plane and a point
        \param point
     */
    double Distance(const osg::Vec3f &point) const;

    /** Computes the signed distance between the plane and a point
        \param point
     */
    double DistanceSigned(const osg::Vec3d &point);

    /** Computes the signed distance between the plane and a point
        \param point
     */
    double DistanceSigned(const osg::Vec3f &point);

    /** Computes the signed distance between the plane and a point
        \param point
     */
    double DistanceSigned(const osg::Vec3d &point) const;

    /** Computes the signed distance between the plane and a point
        \param point
     */
    double DistanceSigned(const osg::Vec3f &point) const;

    /** Creates a plane from 3 points
        \param p1
        \param p2
        \param p3
     */
    void MakeFrom3Points(const osg::Vec3d &p1, const osg::Vec3d &p2, const osg::Vec3d &p3);

    /** Creates a plane from 3 points
        \param p1
        \param p2
        \param p3
     */
    void MakeFrom3Points(const osg::Vec3f &p1, const osg::Vec3f &p2, const osg::Vec3f &p3);

    /** Projects a vector on a plane
        \param v
     */
    osg::Vec3d ProjectVectorOnPlane(const osg::Vec3d &v);

    /** Projects a vector on a plane
        \param v
     */
    osg::Vec3f ProjectVectorOnPlane(const osg::Vec3f &v);

private:

    double coords[4];         ///< plane coordinates
};

/** Class used to represent a quaternion
 */
class Quaternion
{
public:

    /** Default constructor */
    Quaternion();

    /** Copy constructor
        \param quat quaternion to copy
     */
    Quaternion(const Quaternion &quat);

    /** Destructor */
    ~Quaternion();

    /** Overload of operator =
        \param quat quaternion to copy
     */
    Quaternion & operator =(const Quaternion &quat);

    /** Overload of operator [] to obtain the quaternion values
        \param index
     */
    double & operator [](int index) { return coords[index]; }

    /** Overload of operator * to multiply two quaternions
        \param quat
     */
    Quaternion operator *(const Quaternion &quat);

    /** Overload of operator * to multiply two quaternions
        \param quat
     */
    const Quaternion operator *(const Quaternion &quat) const;

    /** Assigns a new value to the first coordinate
        \param x
     */
    void SetX(const double &x) { coords[0] = x; }

    /** Assigns a new value to the second coordinate
        \param y
     */
    void SetY(const double &y) { coords[1] = y; }

    /** Assigns a new value to the third coordinate
        \param z
     */
    void SetZ(const double &z) { coords[2] = z; }

    /** Assigns a new value to the fourth coordinate
        \param w
     */
    void SetW(const double &w) { coords[3] = w; }

    /** Returns the first coordinate */
    double & GetX() { return coords[0]; }

    /** Returns the second coordinate */
    double & GetY() { return coords[1]; }

    /** Returns the third coordinate */
    double & GetZ() { return coords[2]; }

    /** Returns the fourth coordinate */
    double & GetW() { return coords[3]; }

    /** Obtains the conjugate quaternion */
    void Conjugate();

private:

    double coords[4];          ///< coordinates of the quaternion
};

#endif // _OSGOSGMATH_H_
