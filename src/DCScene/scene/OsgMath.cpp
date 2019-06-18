#include "OsgMath.h"

OsgMath::OsgMath(void)
{}

OsgMath::~OsgMath(void)
{}

double OsgMath::VectorLength(const osg::Vec3d &v)
{
    return v.length ();
}

float OsgMath::VectorLength(const osg::Vec3f &v)
{
    return v.length ();
}

void OsgMath::NormalizeVec(osg::Vec3d &v)
{
    v.normalize ();
}

void OsgMath::NormalizeVec(osg::Vec3f &v)
{
    v.normalize ();
}

osg::Vec3d OsgMath::CrossProduct(const osg::Vec3d &p1, const osg::Vec3d &p2)
{
    return p1 ^ p2;
}

osg::Vec3f OsgMath::CrossProduct(const osg::Vec3f &p1, const osg::Vec3f &p2)
{
    return p1 ^ p2;
}

double OsgMath::DistanceBetweenTwoPoints(const osg::Vec3d &p1, const osg::Vec3d &p2)
{
    double a = p1.x() - p2.x();
    double b = p1.y() - p2.y();
    double c = p1.z() - p2.z();

    return sqrt( (a * a) + (b * b) + (c * c) );
}

float OsgMath::DistanceBetweenTwoPoints(const osg::Vec3f &p1, const osg::Vec3f &p2)
{
    float a = p1.x() - p2.x();
    float b = p1.y() - p2.y();
    float c = p1.z() - p2.z();

    return sqrt( (a * a) + (b * b) + (c * c) );
}

double OsgMath::AngleBetweenTwoVectorsInRadians(osg::Vec3d &v1, osg::Vec3d &v2)
{
    return acos( OsgMath::DotProduct(v1, v2) / ( OsgMath::VectorLength(v1) * OsgMath::VectorLength(v2) ) );
}

float OsgMath::AngleBetweenTwoVectorsInRadians(osg::Vec3f &v1, osg::Vec3f &v2)
{
    if (v1.length() == 0.0 || v2.length() == 0.0)
        return 0.0f;

    float cost = OsgMath::DotProduct(v1, v2) / ( OsgMath::VectorLength(v1) * OsgMath::VectorLength(v2) );

    if (cost < -1)
        cost = -1;
    else if (cost >  1)
        cost = 1;

    return acos(cost);
}

double OsgMath::AngleBetweenTwoVectorsInGrades(osg::Vec3d &v1, osg::Vec3d &v2)
{
    double dot_product = OsgMath::DotProduct(v1, v2);
    double angle_in_radians = acos( dot_product / ( OsgMath::VectorLength(v1) * OsgMath::VectorLength(v2) ) );

    return ( (angle_in_radians * 360) / (2 * MY_PI) );
}

float OsgMath::AngleBetweenTwoVectorsInGrades(osg::Vec3f &v1, osg::Vec3f &v2)
{
    float cost = OsgMath::DotProduct(v1, v2) / ( OsgMath::VectorLength(v1) * OsgMath::VectorLength(v2) );

    if (cost < -1)
        cost = -1;
    else if (cost >  1)
        cost = 1;

    float angle_in_radians = acos(cost);

    return ( (angle_in_radians * 360) / (2 * MY_PI) );
}

void OsgMath::RotatePoint(osg::Vec3d &point,const osg::Vec3d &axis, const double &angle, const bool &angle_in_grades)
{
    double angle_in_radians;

    if (angle_in_grades)
    {
        angle_in_radians = angle * ( (2.0 * MY_PI) / 360.0 );
        angle_in_radians *= 0.5;
    }
    else
        angle_in_radians = angle;

    double sinus = sin(angle_in_radians);
    double cosinus = cos(angle_in_radians);

    Quaternion rotation_q, rotation_q_conjugate, point_q, result1, result2;

    rotation_q[0] = axis.x() * sinus;
    rotation_q[1] = axis.y() * sinus;
    rotation_q[2] = axis.z() * sinus;
    rotation_q[3] = cosinus;

    rotation_q_conjugate[0] = -rotation_q[0];
    rotation_q_conjugate[1] = -rotation_q[1];
    rotation_q_conjugate[2] = -rotation_q[2];
    rotation_q_conjugate[3] = cosinus;

    point_q[0] = point.x();
    point_q[1] = point.y();
    point_q[2] = point.z();
    point_q[3] = 0.0f;

    result1 = rotation_q * point_q;
    result2 = result1 * rotation_q_conjugate;

    point.x() = result2[0];
    point.y() = result2[1];
    point.z() = result2[2];
}

void OsgMath::RotatePoint(osg::Vec3f &point,const osg::Vec3f &axis, const float &angle, const bool &angle_in_grades)
{
    float angle_in_radians;

    if (angle_in_grades)
    {
        angle_in_radians = angle * ( (2.0f * MY_PI) / 360.0f );
        angle_in_radians *= 0.5f;
    }
    else
        angle_in_radians = angle;

    double sinus = sin(angle_in_radians);
    double cosinus = cos(angle_in_radians);

    Quaternion rotation_q, rotation_q_conjugate, point_q, result1, result2;

    rotation_q[0] = axis.x() * sinus;
    rotation_q[1] = axis.y() * sinus;
    rotation_q[2] = axis.z() * sinus;
    rotation_q[3] = cosinus;

    rotation_q_conjugate[0] = -rotation_q[0];
    rotation_q_conjugate[1] = -rotation_q[1];
    rotation_q_conjugate[2] = -rotation_q[2];
    rotation_q_conjugate[3] = cosinus;

    point_q[0] = point.x();
    point_q[1] = point.y();
    point_q[2] = point.z();
    point_q[3] = 0.0f;

    result1 = rotation_q * point_q;
    result2 = result1 * rotation_q_conjugate;

    point.x() = result2[0];
    point.y() = result2[1];
    point.z() = result2[2];
}

osg::Quat OsgMath::makeHPRQuat( osg::Vec3 rotAngles )
{
    return( makeHPRQuat( rotAngles[ 0 ], rotAngles[ 1 ], rotAngles[ 2 ] ) );
}

osg::Quat OsgMath::makeHPRQuat( double h, double p, double r )
{
    // Given h, p, and r angles in degrees, build a Quat to affect these rotatiions.
    // We do this by creating a Matrix that contains correctly-oriented x, y, and
    // z axes. Then we create the Quat from the Matrix.
    //
    // First, create x, y, and z axes that represent the h, p, and r angles.
    //   Rotate x and y axes by the heading.
    osg::Vec3 z( 0., 0., 1. );
    osg::Quat qHeading( osg::DegreesToRadians( h ), z );
    osg::Vec3 x = qHeading * osg::Vec3( 1., 0., 0. );
    osg::Vec3 y = qHeading * osg::Vec3( 0., 1., 0. );

    //   Rotate z and y axes by the pitch.
    osg::Quat qPitch( osg::DegreesToRadians( p ), x );
    y = qPitch * y;
    z = qPitch * z;

    //   Rotate x and z axes by the roll.
    osg::Quat qRoll( osg::DegreesToRadians( r ), y );
    x = qRoll * x;
    z = qRoll * z;

    // Use x, y, and z axes to create an orientation matrix.
    osg::Matrix m( x[0], x[1], x[2], 0.,
        y[0], y[1], y[2], 0.,
        z[0], z[1], z[2], 0.,
        0., 0., 0., 1. );

    osg::Quat quat;
    quat.set( m );
    return( quat );
}

Plane::Plane(void)
{
    coords[0] = coords[1] = coords[2] = coords[3] = 0.0;
}

Plane::Plane(const Plane &plane)
{
    coords[0] = plane.coords[0];
    coords[1] = plane.coords[1];
    coords[2] = plane.coords[2];
    coords[3] = plane.coords[3];
}

Plane::Plane(const double &x, const double &y, const double &z, const double &w)
{
    coords[0] = x;
    coords[1] = y;
    coords[2] = z;
    coords[3] = w;
}

Plane::~Plane(void)
{}

Plane & Plane::operator =(const Plane &plane)
{
    coords[0] = plane.coords[0];
    coords[1] = plane.coords[1];
    coords[2] = plane.coords[2];
    coords[3] = plane.coords[3];
    return *this;
}

double Plane::Angle(const Plane &plane)
{
    osg::Vec3d n1(coords[0], coords[1], coords[2]);
    osg::Vec3d n2(plane.coords[0], plane.coords[1], plane.coords[2]);

    return OsgMath::AngleBetweenTwoVectorsInGrades(n1, n2);
}

double Plane::Distance(const osg::Vec3d &point)
{
    double a = fabs( ( coords[0] * point.x() ) + ( coords[1] * point.y() ) + ( coords[2] * point.z() ) + coords[3] );
    double b = sqrt( (coords[0] * coords[0]) + (coords[1] * coords[1]) + (coords[2] * coords[2]) );
    return a / b;
}

double Plane::Distance(const osg::Vec3f &point)
{
    double a = fabs( ( coords[0] * point.x() ) + ( coords[1] * point.y() ) + ( coords[2] * point.z() ) + coords[3] );
    double b = sqrt( (coords[0] * coords[0]) + (coords[1] * coords[1]) + (coords[2] * coords[2]) );
    return a / b;
}

double Plane::Distance(const osg::Vec3d &point) const
{
    double a = fabs( ( coords[0] * point.x() ) + ( coords[1] * point.y() ) + ( coords[2] * point.z() ) + coords[3] );
    double b = sqrt( (coords[0] * coords[0]) + (coords[1] * coords[1]) + (coords[2] * coords[2]) );
    return a / b;
}

double Plane::Distance(const osg::Vec3f &point) const
{
    double a = fabs( ( coords[0] * point.x() ) + ( coords[1] * point.y() ) + ( coords[2] * point.z() ) + coords[3] );
    double b = sqrt( (coords[0] * coords[0]) + (coords[1] * coords[1]) + (coords[2] * coords[2]) );
    return a / b;
}

double Plane::DistanceSigned(const osg::Vec3d &point)
{
    double a = ( coords[0] * point.x() ) + ( coords[1] * point.y() ) + ( coords[2] * point.z() ) + coords[3];
    double b = sqrt( (coords[0] * coords[0]) + (coords[1] * coords[1]) + (coords[2] * coords[2]) );
    return a / b;
}

double Plane::DistanceSigned(const osg::Vec3f &point)
{
    double a = ( coords[0] * point.x() ) + ( coords[1] * point.y() ) + ( coords[2] * point.z() ) + coords[3];
    double b = sqrt( (coords[0] * coords[0]) + (coords[1] * coords[1]) + (coords[2] * coords[2]) );
    return a / b;
}

double Plane::DistanceSigned(const osg::Vec3d &point) const
{
    double a = ( coords[0] * point.x() ) + ( coords[1] * point.y() ) + ( coords[2] * point.z() ) + coords[3];
    double b = sqrt( (coords[0] * coords[0]) + (coords[1] * coords[1]) + (coords[2] * coords[2]) );
    return a / b;
}

double Plane::DistanceSigned(const osg::Vec3f &point) const
{
    double a = ( coords[0] * point.x() ) + ( coords[1] * point.y() ) + ( coords[2] * point.z() ) + coords[3];
    double b = sqrt( (coords[0] * coords[0]) + (coords[1] * coords[1]) + (coords[2] * coords[2]) );
    return a / b;
}

void Plane::MakeFrom3Points(const osg::Vec3d &p1, const osg::Vec3d &p2, const osg::Vec3d &p3)
{
    osg::Vec3d v1, v2, normal;

    v1.x() = p2.x() - p1.x();
    v1.y() = p2.y() - p1.y();
    v1.z() = p2.z() - p1.z();

    v2.x() = p3.x() - p1.x();
    v2.y() = p3.y() - p1.y();
    v2.z() = p3.z() - p1.z();

    OsgMath::NormalizeVec(v1);
    OsgMath::NormalizeVec(v2);

    normal = OsgMath::CrossProduct(v1, v2);

    OsgMath::NormalizeVec(normal);

    coords[0] = normal.x();
    coords[1] = normal.y();
    coords[2] = normal.z();
    coords[3] = 0.0;
    coords[3] -= normal.x() * p1.x();
    coords[3] -= normal.y() * p1.y();
    coords[3] -= normal.z() * p1.z();
}

void Plane::MakeFrom3Points(const osg::Vec3f &p1, const osg::Vec3f &p2, const osg::Vec3f &p3)
{
    osg::Vec3d v1, v2, normal;

    v1.x() = p2.x() - p1.x();
    v1.y() = p2.y() - p1.y();
    v1.z() = p2.z() - p1.z();

    v2.x() = p3.x() - p1.x();
    v2.y() = p3.y() - p1.y();
    v2.z() = p3.z() - p1.z();

    OsgMath::NormalizeVec(v1);
    OsgMath::NormalizeVec(v2);

    normal = OsgMath::CrossProduct(v1, v2);

    OsgMath::NormalizeVec(normal);

    coords[0] = normal.x();
    coords[1] = normal.y();
    coords[2] = normal.z();
    coords[3] = 0.0;
    coords[3] -= normal.x() * p1.x();
    coords[3] -= normal.y() * p1.y();
    coords[3] -= normal.z() * p1.z();
}

osg::Vec3d Plane::ProjectVectorOnPlane(const osg::Vec3d &vec)
{
    osg::Vec3d projected_vector, plane_normal, u;

    plane_normal.x() = coords[0];
    plane_normal.y() = coords[1];
    plane_normal.z() = coords[2];

    double dot = OsgMath::DotProduct(vec, plane_normal);

    u.x() = plane_normal.x() * dot;
    u.y() = plane_normal.y() * dot;
    u.z() = plane_normal.z() * dot;

    projected_vector.x() = vec.x() - u.x();
    projected_vector.y() = vec.y() - u.y();
    projected_vector.z() = vec.z() - u.z();

    return projected_vector;
}

osg::Vec3f Plane::ProjectVectorOnPlane(const osg::Vec3f &vec)
{
    osg::Vec3f projected_vector, plane_normal, u;

    plane_normal.x() = coords[0];
    plane_normal.y() = coords[1];
    plane_normal.z() = coords[2];

    double dot = OsgMath::DotProduct(vec, plane_normal);

    u.x() = plane_normal.x() * dot;
    u.y() = plane_normal.y() * dot;
    u.z() = plane_normal.z() * dot;

    projected_vector.x() = vec.x() - u.x();
    projected_vector.y() = vec.y() - u.y();
    projected_vector.z() = vec.z() - u.z();

    return projected_vector;
}

Quaternion::Quaternion(void)
{
    coords[0] = coords[1] = coords[2] = coords[3] = 0.0;
}

Quaternion::Quaternion(const Quaternion &quat)
{
    coords[0] = quat.coords[0];
    coords[1] = quat.coords[1];
    coords[2] = quat.coords[2];
    coords[3] = quat.coords[3];
}

Quaternion::~Quaternion(void)
{}

Quaternion & Quaternion::operator =(const Quaternion &quat)
{
    coords[0] = quat.coords[0];
    coords[1] = quat.coords[1];
    coords[2] = quat.coords[2];
    coords[3] = quat.coords[3];
    return *this;
}

Quaternion Quaternion::operator *(const Quaternion &quat)
{
    Quaternion out;

    out.coords[0] = (coords[3] * quat.coords[0]) + ( coords[0] * quat.coords[3] ) + ( coords[1] * quat.coords[2] ) - ( coords[2] * quat.coords[1] );
    out.coords[1] = (coords[3] * quat.coords[1]) - ( coords[0] * quat.coords[2] ) + ( coords[1] * quat.coords[3] ) + ( coords[2] * quat.coords[0] );
    out.coords[2] = (coords[3] * quat.coords[2]) + ( coords[0] * quat.coords[1] ) - ( coords[1] * quat.coords[0] ) + ( coords[2] * quat.coords[3] );
    out.coords[3] = (coords[3] * quat.coords[3]) - ( coords[0] * quat.coords[0] ) - ( coords[1] * quat.coords[1] ) - ( coords[2] * quat.coords[2] );

    return out;
}

const Quaternion Quaternion::operator *(const Quaternion &quat) const
{
    Quaternion out;

    out.coords[0] = (coords[3] * quat.coords[0]) + ( coords[0] * quat.coords[3] ) + ( coords[1] * quat.coords[2] ) - ( coords[2] * quat.coords[1] );
    out.coords[1] = (coords[3] * quat.coords[1]) - ( coords[0] * quat.coords[2] ) + ( coords[1] * quat.coords[3] ) + ( coords[2] * quat.coords[0] );
    out.coords[2] = (coords[3] * quat.coords[2]) + ( coords[0] * quat.coords[1] ) - ( coords[1] * quat.coords[0] ) + ( coords[2] * quat.coords[3] );
    out.coords[3] = (coords[3] * quat.coords[3]) - ( coords[0] * quat.coords[0] ) - ( coords[1] * quat.coords[1] ) - ( coords[2] * quat.coords[2] );

    return out;
}

void Quaternion::Conjugate()
{
    coords[0] = -coords[0];
    coords[1] = -coords[1];
    coords[2] = -coords[2];
    coords[3] = coords[3];
}
