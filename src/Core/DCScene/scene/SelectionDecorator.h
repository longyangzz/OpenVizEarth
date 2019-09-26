#ifndef _SELECTIONDECORATOR_H_
#define _SELECTIONDECORATOR_H_

#include <osg/Group>
#include <osg/StateSet>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osg/Material>

/*!
    @brief Selection decorator class for selection highlighting.
 */
class ISelectionDecorator :
    public osg::Group
{
public:

    // Initialization
    ISelectionDecorator();           //!< Constructor
    virtual ~ISelectionDecorator();  //!< Destructor

    // Highlighting states
    virtual void setEnable(bool enable);    //!< Enable or disable selection highlighting.
    virtual bool getEnable() const;         //!< Return 'true' if selection highlighting is enabled.

    virtual void traverse( osg::NodeVisitor& nv );

protected:

    bool m_bEnable;         //!< Enable state
    osg::ref_ptr<osg::StateSet> m_pStateSet;        //!< StateSet for highlight rendering.
};

class DefaultSelectionDecorator :
    public ISelectionDecorator
{
public:

    // Initialization
    DefaultSelectionDecorator();           //!< Constructor
    virtual ~DefaultSelectionDecorator();  //!< Destructor

    void setOffsetFactor(float factor); //!< Set offset factor of polygon offset attribute.
    float getOffsetFactor() const;      //!< Return offset factor of polygon offset attribute.

    void setOffsetUnits(float units);   //!< Set offset units of polygon offset attribute.
    float getOffsetUnits() const;       //!< Return offset units of polygon offset attribute.

    void setPolygonMode(osg::PolygonMode::Mode mode);   //!< Set polygon mode attribute for highlight rendering.
    osg::PolygonMode::Mode getPolygonMode() const;      //!< Return polygon mode attribute for highlight rendering.

    void setLineWidth(float width);     //!< Set line width attribute for highlight rendering.
    float getLineWidth() const;         //!< Return line width attribute for highlight rendering.

    void setEmissionColor(osg::Vec4 color); //!< Set highlight color.
    osg::Vec4 getEmissionColor() const;     //!< Return highlight color.

    void setTexturing(bool texturing);  //!< Enable or disable textureing for highlight rendering.
    bool getTexturing() const;          //!< Return textureing state for highlight rendering.

    void setLighting(bool lighting);    //!< Enable or disable lighting for highlight rendering.
    bool getLighting() const;           //!< Return lighting state for highlight rendering.

protected:

    osg::ref_ptr<osg::PolygonOffset> m_pPolyOffset; //!< PolygonOffset attribute for highlighting.
    osg::ref_ptr<osg::PolygonMode> m_pPolyMode;     //!< PolygonMode attribute for highlighting.
    osg::ref_ptr<osg::LineWidth> m_pLineWidth;      //!< LineWidth attribute for highlighting.
    osg::ref_ptr<osg::Material> m_pMaterial;        //!< Material attribute for highlighting.
};

#endif // _SELECTIONDECORATOR_H_
