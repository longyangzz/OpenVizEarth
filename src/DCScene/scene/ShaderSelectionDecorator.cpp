#include "ShaderSelectionDecorator.h"

#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include <iostream>

class SineAnimation :
    public osg::Uniform::Callback
{
public:

    SineAnimation( float rate = 1.0f, float scale = 1.0f, float offset = 0.0f ) :
        _rate(rate), _scale(scale), _offset(offset)
    {}

    void operator()( osg::Uniform* uniform, osg::NodeVisitor* nv )
    {
        float angle = _rate * nv->getFrameStamp()->getSimulationTime();
        float value = sinf( angle ) * _scale + _offset;
        uniform->set( value );
    }

private:
    const float _rate;
    const float _scale;
    const float _offset;
};

// callback for trackball manipulator
class GlowUpdateCallback :
    public osg::NodeCallback
{
public:

    GlowUpdateCallback(osg::Uniform *uniform, size_t speed = 1) :
        m_uniform(uniform),
        m_pos(0),
        m_speed(speed),
        m_increment(speed)
    {}

    virtual ~GlowUpdateCallback() {}

    virtual void operator() ( osg::Node* node, osg::NodeVisitor* nv )
    {
        float alpha = 1.0 * ( ( 50.0 + (m_pos) ) / 255.0 );
        m_uniform->set(alpha);
        m_pos += m_increment;

        if (m_pos > 150)
            m_increment = -1 * m_speed;
        else if (m_pos <= 0)
            m_increment = 1 * m_speed;
    }

private:

    size_t m_pos;
    size_t m_speed;
    int m_increment;
    osg::Uniform *m_uniform;
};

/*!
    Constructor
 */
ShaderSelectionDecorator::ShaderSelectionDecorator() :
    ISelectionDecorator()
{
    // create highlighting state
    m_pStateSet = getOrCreateStateSet();
    m_pPolyOffset = new osg::PolygonOffset;
    m_pPolyOffset->setFactor(-1.0f);
    m_pPolyOffset->setUnits(-1.0f);
    m_pStateSet->setAttributeAndModes(m_pPolyOffset.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

    setEnable(true);

    if ( loadShader() )
    {
        m_pStateSet->setAttributeAndModes( m_program.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );

        m_glowFactor = new osg::Uniform("glowFactor",0.1f );
        m_glowFactor->setUpdateCallback( new SineAnimation( 4, 0.5, 0.5 ) );
        m_pStateSet->addUniform( m_glowFactor );
    }
}

/*!
    Destructor
 */
ShaderSelectionDecorator::~ShaderSelectionDecorator()
{
}

void ShaderSelectionDecorator::traverse( osg::NodeVisitor& nv )
{
    const osg::FrameStamp *fs = nv.getFrameStamp();
    if (fs)
    {
	    float rate = 1.0f;
		float scale = 0.2f;
		float offset = 0.5f; // in order to change from 0.0 to 1.0
        float angle = rate * fs->getSimulationTime();
        float value = sinf( angle ) * scale + offset;
        m_glowFactor->set( value );
    }

    ISelectionDecorator::traverse( nv );
}

bool ShaderSelectionDecorator::loadShader()
{
    bool success = true;

    osg::Shader *vertShader = new osg::Shader(osg::Shader::VERTEX);
    std::string vertShaderSource = osgDB::findDataFile("highlight.vert");

    success = vertShader->loadShaderSourceFromFile(vertShaderSource);
    if ( !success  )
    {
        std::cout << "Couldn't load file: " << vertShaderSource << std::endl;
        return success;
    }

    osg::Shader *fragShader = new osg::Shader(osg::Shader::FRAGMENT);
    std::string fragShaderSource = osgDB::findDataFile("highlight.frag");
    success = fragShader->loadShaderSourceFromFile(fragShaderSource);
    if ( !success  )
    {
        std::cout << "Couldn't load file: " << fragShaderSource << std::endl;
        return success;
    }

    m_program = new osg::Program;
    m_program->setName( "highlightshading" );

    m_program->addShader( vertShader );
    m_program->addShader( fragShader );

    return success;
}