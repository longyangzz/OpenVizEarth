#include "SelectionDecorator.h"

#include <osgDB/Registry>

#include <osgDB/FileUtils>

/*!
    Constructor
 */

ISelectionDecorator::ISelectionDecorator() :
    m_bEnable(false)
{}

/*!
    Destructor
 */
ISelectionDecorator::~ISelectionDecorator()
{
}

/*!
    Enable or disable selection highlighting.
    @param[in] enable Enable state.
 */
void ISelectionDecorator::setEnable(bool enable)
{
    m_bEnable = enable;

    if (m_bEnable)
        setNodeMask(-1);
    else
        setNodeMask(0);
}

/*!
    Return 'true' if selection highlighting is enabled.
    @return Enable state.
 */
bool ISelectionDecorator::getEnable() const
{
    return m_bEnable;
}

void ISelectionDecorator::traverse( osg::NodeVisitor& nv )
{
    osg::Group::traverse( nv );
}

/*!
    Constructor
 */
DefaultSelectionDecorator::DefaultSelectionDecorator() :
    ISelectionDecorator()
{
    // create highlighting state
    m_pStateSet = new osg::StateSet;
    m_pPolyOffset = new osg::PolygonOffset;
    m_pPolyOffset->setFactor(-1.0f);
    m_pPolyOffset->setUnits(-1.0f);
    m_pPolyMode = new osg::PolygonMode;
    m_pPolyMode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
    m_pStateSet->setAttributeAndModes(m_pPolyOffset.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    m_pStateSet->setAttributeAndModes(m_pPolyMode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    m_pLineWidth = new osg::LineWidth;
    m_pLineWidth->setWidth(4.0f);
    m_pStateSet->setAttributeAndModes(m_pLineWidth.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    m_pMaterial = new osg::Material;
    m_pMaterial->setDiffuse( osg::Material::FRONT_AND_BACK,osg::Vec4(1.0, 0.0, 0.0, 1.0) );
    m_pMaterial->setColorMode(osg::Material::DIFFUSE);
    m_pStateSet->setAttributeAndModes(m_pMaterial.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    m_pStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
    m_pStateSet->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
    setStateSet( m_pStateSet.get() );

    setEnable(true);
}

/*!
    Destructor
 */
DefaultSelectionDecorator::~DefaultSelectionDecorator()
{
}

/*!
    Set offset factor of polygon offset attribute.
    @param[in] factor Offset factor of polygon offset attribute.
 */
void DefaultSelectionDecorator::setOffsetFactor(float factor)
{
    m_pPolyOffset->setFactor(factor);
}

/*!
    Return offset factor of polygon offset attribute.
    @return Offset factor of polygon offset attribute.
 */
float DefaultSelectionDecorator::getOffsetFactor() const
{
    return m_pPolyOffset->getFactor();
}

/*!
    Set offset units of polygon offset attribute.
    @param[in] units Offset units of polygon offset attribute.
 */
void DefaultSelectionDecorator::setOffsetUnits(float units)
{
    m_pPolyOffset->setUnits(units);
}

/*!
    Return offset units of polygon offset attribute.
    @return Offset units of polygon offset attribute.
 */
float DefaultSelectionDecorator::getOffsetUnits() const
{
    return m_pPolyOffset->getUnits();
}

/*!
    Set polygon mode attribute for highlight rendering.
    @param[in] mode Polygon mode.
 */
void DefaultSelectionDecorator::setPolygonMode(osg::PolygonMode::Mode mode)
{
    m_pPolyMode->setMode(osg::PolygonMode::FRONT_AND_BACK, mode);
}

/*!
    Return polygon mode attribute for highlight rendering.
    @return Polygon mode.
 */
osg::PolygonMode::Mode DefaultSelectionDecorator::getPolygonMode() const
{
    return m_pPolyMode->getMode(osg::PolygonMode::FRONT_AND_BACK);
}

/*!
    Set line width attribute for highlight rendering.
    @param[in] width Line width.
 */
void DefaultSelectionDecorator::setLineWidth(float width)
{
    m_pLineWidth->setWidth(width);
}

/*!
    Return line width attribute for highlight rendering.
    @return Line width.
 */
float DefaultSelectionDecorator::getLineWidth() const
{
    return m_pLineWidth->getWidth();
}

/*!
    Set highlight color.
    @param[in] color Highlight color.
 */
void DefaultSelectionDecorator::setEmissionColor(osg::Vec4 color)
{
    m_pMaterial->setEmission(osg::Material::FRONT_AND_BACK, color);
}

/*!
    Return highlight color.
    @return Highlight color.
 */
osg::Vec4 DefaultSelectionDecorator::getEmissionColor() const
{
    return m_pMaterial->getEmission(osg::Material::FRONT_AND_BACK);
}

/*!
    Enable or disable textureing for highlight rendering.
    @param[in] texturing Texturing state.
 */
void DefaultSelectionDecorator::setTexturing(bool texturing)
{
    m_pStateSet->setTextureMode( 0, GL_TEXTURE_2D, osg::StateAttribute::OVERRIDE | ( texturing ? (osg::StateAttribute::ON): (osg::StateAttribute::OFF) ) );
}

/*!
    Return textureing state for highlight rendering.
    @return Texturing state.
 */
bool DefaultSelectionDecorator::getTexturing() const
{
    return m_pStateSet->getTextureMode(0, GL_TEXTURE_2D) & osg::StateAttribute::ON;
}

/*!
    Enable or disable lighting for highlight rendering.
    @param[in] lighting Lighting state.
 */
void DefaultSelectionDecorator::setLighting(bool lighting)
{
    m_pStateSet->setMode( GL_LIGHTING, osg::StateAttribute::OVERRIDE | ( lighting ? (osg::StateAttribute::ON): (osg::StateAttribute::OFF) ) );
}

/*!
    Return lighting state for highlight rendering.
    @return Lighting state.
 */
bool DefaultSelectionDecorator::getLighting() const
{
    return m_pStateSet->getMode(GL_LIGHTING) & osg::StateAttribute::ON;
}
