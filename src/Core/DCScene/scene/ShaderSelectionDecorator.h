#ifndef _SHADERSELECTIONDECORATOR_H_
#define _SHADERSELECTIONDECORATOR_H_

#include <osg/Group>
#include <osg/StateSet>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osg/Material>

#include "SelectionDecorator.h"

/*!
    @brief Selection decorator class for selection highlighting bu shader.
 */

class ShaderSelectionDecorator :
    public ISelectionDecorator
{
public:

    // Initialization
    ShaderSelectionDecorator();           //!< Constructor
    virtual ~ShaderSelectionDecorator();  //!< Destructor

    virtual void traverse( osg::NodeVisitor& nv );

protected:

private:

    bool loadShader();

    osg::ref_ptr<osg::PolygonOffset> m_pPolyOffset; //!< PolygonOffset attribute for highlighting.
    osg::ref_ptr<osg::Program> m_program;
    osg::ref_ptr<osg::Uniform> m_glowFactor;
};

#endif // _SHADERSELECTIONDECORATOR_H_
