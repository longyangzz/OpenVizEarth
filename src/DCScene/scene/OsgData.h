#ifndef _OSGDATA_H_
#define _OSGDATA_H_

#include <osg/Geometry>

// extract from osgWorks


/**
   \brief Contains several common geometry functions
 */
class OsgData
{
public:

    /** Default constructor */
    OsgData();

    /** Destructor */
    ~OsgData();

    /** Create a linear grid */
    static osg::Geometry* createLineGrid( const osg::Vec3& vCorner, const osg::Vec3& vWidth, const osg::Vec3& vHeight, float stepSize, const osg::Vec4 & color = osg::Vec4(1.,1.,1.,1.) );

    // Sphere generation routines
    // Creates a geodesic sphere starting from the 20 triangles of an icosehedron.
    // Each subdivision splits a triangle into four new triangles. The generated
    // Geometry uses a single DrawElementsUShort GL_TRIANGLES PrimitiveSet, and contains
    // vertex, normal, texture coordinate, and color data. The texture coordinates are
    // configured for unit 0 and suitable for cube mapping.
    static osg::Geometry* makeGeodesicSphere( const float radius = 1., const unsigned int subdivisions = 2, osg::Geometry* geometry = NULL );

    // Creates an alt-az sphere (also commonly called a lat-long sphere) using
    // GL_TRIANGLE_STRIP PrimitiveSets. subLat indicates the number of GL_TRIANGLE_STRIPS
    // wrapping the sphere in the xy plane. Each strip contains (subLong*2) triangles.
    // Normal and texture coordinate (unit 0) data are created. Texture coordinates are
    // suitable for applying a geographic projection texture. (t coordinate ranges from 0
    // at the (-z) sphere bottom to 1 at the (+z) sphere top, and s wraps around the sphere
    // with 0 at +x, to +y, to -x, to -y, and ending with 1.0 at +x.)
    static osg::Geometry* makeAltAzSphere( const float radius = 1., const unsigned int subLat = 8, const unsigned int subLong = 16, osg::Geometry* geometry = NULL );

    // Creates an alt-az sphere using GL_LINE_LOOP and GL_LINE_STRIP PrimitiveSets.
    // No normal or texture coordinate data. StateSet is configured to disable GL_LIGHTING
    // for OpenGL1/2 builds of OSG.
    static osg::Geometry* makeWireAltAzSphere( const float radius = 1., const unsigned int subLat = 8, const unsigned int subLong = 16, osg::Geometry* geometry = NULL );

    // Plane generation routines

    // Makes a plane using GL_TRIANGLE_STRIP PrimitiveSets. Dimensions are determined
    // by the u and v vectors. By default, the plane is made of two triangles. However,
    // this can be increased with the subdivisions parameter, which specifices how to
    // subdivide the plane in x, and y. Normal and texture coordinate (unit 0) data
    // are created, with the normal computed as (u^v(. The texture coordinate data
    // is suitable for applying an entire texture map to the plane.
    static osg::Geometry* makePlane( const osg::Vec3& corner, const osg::Vec3& u, const osg::Vec3& v, const osg::Vec2s& subdivisions = osg::Vec2s(1,1), osg::Geometry* geometry = NULL );

    static osg::Geometry* makeWirePlane( const osg::Vec3& corner, const osg::Vec3& u, const osg::Vec3& v, const osg::Vec2s& subdivisions = osg::Vec2s(1,1), osg::Geometry* geometry = NULL );

    // Box generation routines

    // Makes an axis-aligned box using GL_TRIANGLE_STRIP PrimitiveSets. Dimensions
    // are (halfExtents*2). By default, two triangles are on each side of the box, for
    // a total of 12 triangles. However, this can be increased with the subdivisions
    // parameter, which specifices how to subdivide the faces of the box in x, y, and z.
    // Normal and texture coordinate (unit 0) data are created. The texture coordinate data
    // is suitable for applying an entire texture map to each face of the box.
    static osg::Geometry* makeBox( const osg::Vec3& halfExtents, const osg::Vec3s& subdivisions = osg::Vec3s(1,1,1), osg::Geometry* geometry = NULL );

    // Creates a box using GL_LINE_LOOP and GL_LINES PrimitiveSets. No normal or
    // texture coordinate data. StateSet is configured to disable GL_LIGHTING for
    // OpenGL1/2 builds of OSG.
    static osg::Geometry* makeWireBox( const osg::Vec3& halfExtents, osg::Geometry* geometry = NULL );

    // Circle generation routines

    // Makes an axis-aligned circle (solid disk) using GL_TRIANGLE_FAN PrimitiveSets. Dimensions
    // are (halfExtents*2). By default, 32 segments are used to approximate the circle shape.
    // However, this can be increased with the subdivisions parameter.
    // Normal and texture coordinate (unit 0) data are created. The texture coordinate data
    // is suitable for applying an entire texture map to the face of the disk.
    static osg::Geometry* makeCircle( const float radius = 1., const unsigned int subdivisions = 32, const osg::Vec3& orientation = osg::Vec3( 0., 0., 1. ), osg::Geometry* geometry = NULL );

    // Makes an axis-aligned wireframe circle using GL_LINE_LOOP PrimitiveSets.
    static osg::Geometry* makeWireCircle( const float radius = 1., const unsigned int subdivisions = 32, const osg::Vec3& orientation = osg::Vec3( 0., 0., 1. ), osg::Geometry* geometry = NULL );

    // Arrow generation routines

    // Makes a unit-length arrow pointing towards +z with base at the origin.
    // Current implementation uses a minimal number of vertices. In the future,
    // this code could use cylinder/cone support routines to provide complete
    // control over approximation.
    // Adds vertex and normal data, with a single white color.
    static osg::Geometry* makeArrow( osg::Geometry* geometry = NULL );
};

#endif // _OSGDATA_H_
