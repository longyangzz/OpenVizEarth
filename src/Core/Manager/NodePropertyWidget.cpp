#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtGui/QWheelEvent>
#include <QtWidgets/QScrollBar>
#include <QtCore/QDate>

//#include <osgwTools/CountsVisitor.h>

#include "../DCScene/scene/ExtentsVisitor.h"

#include <climits>

#include "NodePropertyWidget.h"
#include "propertybrowser\qtpropertymanager.h"

NodePropertyWidget::NodePropertyWidget( QWidget *parent ) :
    QtTreePropertyBrowser( parent )
{
    m_variantManager = new QtVariantPropertyManager(this);
    initDictionaries();
}

void NodePropertyWidget::initDictionaries()
{
    // Data variance
    m_listDataVariance << "DYNAMIC";
    m_listDataVariance << "STATIC";
    m_listDataVariance << "UNSPECIFIED";

    // LOD Center mode
    m_listCenterMode << tr("USE_BOUNDING_SPHERE_CENTER");
    m_listCenterMode << tr("USER_DEFINED_CENTER");

    // LOD Range mode
    m_listRangeMode << tr("DISTANCE_FROM_EYE_POINT");
    m_listRangeMode << tr("PIXEL_SIZE_ON_SCREEN");
}

void NodePropertyWidget::displayProperties(osg::Node *node)
{
    clear();

    if (!node)
        return;

    displayNodeProperties(node);
    displayLODProperties( dynamic_cast<osg::LOD *>(node) );
    displaySwitchProperties( dynamic_cast<osg::Switch *>(node) );
    displayGeodeProperties( dynamic_cast<osg::Geode *>(node) );
    displayBaseStats(node);
}

void NodePropertyWidget::displayNodeProperties(osg::Node *node)
{
    if (!node)
        return;

    QtVariantProperty *property;
    QtVariantProperty *parent;

    // Name
    if ( !node->getName().empty() )
    {
        parent = m_variantManager->addProperty( QVariant::String, tr("Name") );
        parent->setValue(m_listDataVariance[node->getDataVariance()]);
        addProperty(parent);
    }

    // data variance
    parent = m_variantManager->addProperty( QVariant::String, tr("Data Variance") );
    parent->setValue( node->getName().c_str() );
    addProperty(parent);

    // Node Mask
    parent = m_variantManager->addProperty( QVariant::String, tr("Node Mask") );
    parent->setValue( QString( "0x%1" ).arg( QString().setNum( (unsigned int)node->getNodeMask(), 16 ) ) );
    addProperty(parent);
}

void NodePropertyWidget::displayLODProperties(osg::LOD *node)
{
    if (!node)
        return;

    QtVariantProperty *property;
    QtVariantProperty *parent;

    // Center Mode
    parent = m_variantManager->addProperty( QVariant::String, tr("Center Mode") );
    parent->setValue(m_listCenterMode[node->getCenterMode()]);
    addProperty(parent);

    // Range Mode
    parent = m_variantManager->addProperty( QVariant::String, tr("Center Mode") );
    parent->setValue(m_listRangeMode[node->getRangeMode()]);
    addProperty(parent);

    // Center
    parent = m_variantManager->addProperty( QVariant::String, tr("Center") );
    parent->setValue("");
    addProperty(parent);

    property = m_variantManager->addProperty( QVariant::Double, tr("X") );
    property->setValue( node->getCenter().x() );
    parent->addSubProperty(property);

    property = m_variantManager->addProperty( QVariant::Double, tr("Y") );
    property->setValue( node->getCenter().y() );
    parent->addSubProperty(property);

    property = m_variantManager->addProperty( QVariant::Double, tr("Z") );
    property->setValue( node->getCenter().z() );
    parent->addSubProperty(property);

    parent = m_variantManager->addProperty( QVariant::Double, tr("Radius") );
    parent->setValue( node->getRadius() );
    addProperty(parent);

    // Range list
    parent = m_variantManager->addProperty( QVariant::String, tr("Range list") );
    parent->setValue("");
    addProperty(parent);

    osg::LOD::RangeList rangeList = node->getRangeList();
    QList<QVariant> ranges;
    for (unsigned int i = 0; i<rangeList.size(); i++)
    {
        ranges.push_back(rangeList[i].second);
        property = m_variantManager->addProperty( QVariant::Double, tr("Range") );
        property->setValue(rangeList[i].second);
        parent->addSubProperty(property);
    }
}

void NodePropertyWidget::displaySwitchProperties(osg::Switch *node)
{
    if (!node)
        return;

    QtVariantProperty *property;
    QtVariantProperty *parent;

    // Range list
    parent = m_variantManager->addProperty( QVariant::String, tr("Value list") );
    parent->setValue("");
    addProperty(parent);

    osg::Switch::ValueList valueList = node->getValueList();
    for (unsigned int i = 0; i<valueList.size(); i++)
    {
        property = m_variantManager->addProperty( QVariant::Bool, tr("Value") );
        property->setValue( (bool)valueList[i] );
        parent->addSubProperty(property);
    }
}

void NodePropertyWidget::displayGeodeProperties(osg::Geode *node)
{
    if (!node)
        return;

    QtVariantProperty *property;
    QtVariantProperty *parent;

    // bounding box

    const osg::BoundingBox& bBox = node->getBoundingBox();

    // Size
    parent = m_variantManager->addProperty( QVariant::String, tr("Size") );
    parent->setValue("");
    addProperty(parent);

    property = m_variantManager->addProperty( QVariant::Double, tr("Height") );
    property->setValue( bBox.zMin() - bBox.zMin() );
    parent->addSubProperty(property);

    property = m_variantManager->addProperty( QVariant::Double, tr("Width") );
    property->setValue( bBox.yMin() - bBox.yMin() );
    parent->addSubProperty(property);

    property = m_variantManager->addProperty( QVariant::Double, tr("Length") );
    property->setValue( bBox.xMin() - bBox.xMin() );
    parent->addSubProperty(property);
}

void NodePropertyWidget::displayBaseStats(osg::Node *node)
{
    //// setup global stats
    //osgwTools::CountsVisitor cv;
    //node->accept( cv );

    //// translate all the underlay layers to bottom
    //ExtentsVisitor ext;
    //node->accept(ext);

    //QtVariantProperty *property;
    //QtVariantProperty *parent;

    //const osg::BoundingBox bBox = ext.GetBound();

    //// Size
    //parent = m_variantManager->addProperty( QVariant::String, tr("Size") );
    //parent->setValue("");
    //addProperty(parent);

    //property = m_variantManager->addProperty( QVariant::Double, tr("Height") );
    //property->setValue( bBox.zMin() - bBox.zMin() );
    //parent->addSubProperty(property);

    //property = m_variantManager->addProperty( QVariant::Double, tr("Width") );
    //property->setValue( bBox.yMin() - bBox.yMin() );
    //parent->addSubProperty(property);

    //property = m_variantManager->addProperty( QVariant::Double, tr("Length") );
    //property->setValue( bBox.xMin() - bBox.xMin() );
    //parent->addSubProperty(property);

    //// Center
    //parent = m_variantManager->addProperty( QVariant::String, tr("Center Point") );
    //parent->setValue("");
    //addProperty(parent);

    //property = m_variantManager->addProperty( QVariant::Double, tr("X") );
    //property->setValue( bBox.center().x() );
    //parent->addSubProperty(property);

    //property = m_variantManager->addProperty( QVariant::Double, tr("Y") );
    //property->setValue( bBox.center().y() );
    //parent->addSubProperty(property);

    //property = m_variantManager->addProperty( QVariant::Double, tr("Z") );
    //property->setValue( bBox.center().z() );
    //parent->addSubProperty(property);

    //// Stats
    //parent = m_variantManager->addProperty( QVariant::String, tr("Total vertices") );
    //parent->setValue( (unsigned int)cv._vertices );
    //addProperty(parent);

    //parent = m_variantManager->addProperty( QVariant::String, tr("Max depth") );
    //parent->setValue( (unsigned int)cv._maxDepth );
    //addProperty(parent);

    //if (cv._slowPathGeometries)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Slow path Geometries") );
    //    parent->setValue( (unsigned int)cv._slowPathGeometries );
    //    addProperty(parent);
    //}

    //// ostr << "             Groups \t" << _groups << "\t" << _uGroups.size() << std::endl;
    //parent = m_variantManager->addProperty( QVariant::String, tr("Groups") );
    //parent->setValue( (unsigned int)cv._groups );
    //addProperty(parent);

    //// ostr << "               LODs \t" << _lods << "\t" << _uLods.size() << std::endl;
    //if (cv._lods)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Groups") );
    //    parent->setValue( (unsigned int)cv._lods );
    //    addProperty(parent);
    //}

    ////ostr << "          PagedLODs \t" << _pagedLods << "\t" << _uPagedLods.size() << std::endl;
    //if (cv._pagedLods)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("PagedLODs") );
    //    parent->setValue( (unsigned int)cv._pagedLods );
    //    addProperty(parent);
    //}

    ////ostr << "           Switches \t" << _switches << "\t" << _uSwitches.size() << std::endl;
    //if (cv._switches)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Switches") );
    //    parent->setValue( (unsigned int)cv._switches );
    //    addProperty(parent);
    //}

    ////ostr << "          Sequences \t" << _sequences << "\t" << _uSequences.size() << std::endl;
    //if (cv._sequences)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Sequences") );
    //    parent->setValue( (unsigned int)cv._sequences );
    //    addProperty(parent);
    //}

    ////ostr << "   MatrixTransforms \t" << _matrixTransforms << "\t" << _uMatrixTransforms.size() << std::endl;
    //if (cv._matrixTransforms)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("MatrixTransforms") );
    //    parent->setValue( (unsigned int)cv._matrixTransforms );
    //    addProperty(parent);
    //}

    ////ostr << "      DOFTransforms \t" << _dofTransforms << "\t" << _uDofTransforms.size() << std::endl;
    //if (cv._dofTransforms)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("DOFTransforms") );
    //    parent->setValue( (unsigned int)cv._dofTransforms );
    //    addProperty(parent);
    //}

    ////ostr << "   Other Transforms \t" << _transforms << "\t" << _uTransforms.size() << std::endl;
    //if (cv._transforms)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Other Transforms") );
    //    parent->setValue( (unsigned int)cv._transforms );
    //    addProperty(parent);
    //}

    ////ostr << "             Geodes \t" << _geodes << "\t" << _uGeodes.size() << std::endl;
    //if (cv._geodes)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Geodes") );
    //    parent->setValue( (unsigned int)cv._geodes );
    //    addProperty(parent);
    //}

    ////ostr << "        Other Nodes \t" << _nodes << "\t" << _uNodes.size() << std::endl;
    //if (cv._nodes)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Other Nodes") );
    //    parent->setValue( (unsigned int)cv._nodes );
    //    addProperty(parent);
    //}

    ////ostr << "    Empty StateSets \t" << _emptyStateSets << std::endl;
    //if (cv._emptyStateSets)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Empty StateSets") );
    //    parent->setValue( (unsigned int)cv._emptyStateSets );
    //    addProperty(parent);
    //}

    ////ostr << "    Total StateSets \t" << _stateSets << "\t" << _uStateSets.size() << std::endl;
    //if (cv._stateSets)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Total StateSets") );
    //    parent->setValue( (unsigned int)cv._stateSets );
    //    addProperty(parent);
    //}

    ////ostr << "           Programs \t" << _programs << "\t" << _uPrograms.size() << std::endl;
    //if (cv._programs)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Programs") );
    //    parent->setValue( (unsigned int)cv._programs );
    //    addProperty(parent);
    //}

    ////ostr << "           Uniforms \t" << _uniforms << "\t" << _uUniforms.size() << std::endl;
    //if (cv._uniforms)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Uniforms") );
    //    parent->setValue( (unsigned int)cv._uniforms );
    //    addProperty(parent);
    //}

    ////ostr << "   Total Attributes \t" << _attributes << "\t" << _uAttributes.size() << std::endl;
    //if (cv._attributes)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Total Attributes") );
    //    parent->setValue( (unsigned int)cv._attributes );
    //    addProperty(parent);
    //}

    ////ostr << "        Total Modes \t" << _modes << std::endl;
    //if (cv._modes)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Total Modes") );
    //    parent->setValue( (unsigned int)cv._modes );
    //    addProperty(parent);
    //}

    ////ostr << "           Textures \t" << _textures << "\t" << _uTextures.size() << std::endl;
    //if (cv._textures)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Textures") );
    //    parent->setValue( (unsigned int)cv._textures );
    //    addProperty(parent);
    //}

    ////ostr << "Total TexAttributes \t" << _texAttributes << "\t" << _uTexAttributes.size() << std::endl;
    //if (cv._texAttributes)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Total TexAttributes") );
    //    parent->setValue( (unsigned int)cv._texAttributes );
    //    addProperty(parent);
    //}

    ////ostr << "     Total TexModes \t" << _texModes << std::endl;
    //if (cv._texModes)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Total TexModes") );
    //    parent->setValue( (unsigned int)cv._texModes );
    //    addProperty(parent);
    //}

    ////ostr << "    NULL Geometries \t" << _nullGeometries << std::endl;
    //if (cv._nullGeometries)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("NULL Geometries") );
    //    parent->setValue( (unsigned int)cv._nullGeometries );
    //    addProperty(parent);
    //}

    ////ostr << "   Total Geometries \t" << _geometries << "\t" << _uGeometries.size() << std::endl;
    //if (cv._geometries)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Total Geometries") );
    //    parent->setValue( (unsigned int)cv._geometries );
    //    addProperty(parent);
    //}

    ////ostr << "              Texts \t" << _texts << "\t" << _uTexts.size() << std::endl;
    //if (cv._texts)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Texts") );
    //    parent->setValue( (unsigned int)cv._texts );
    //    addProperty(parent);
    //}

    ////ostr << "    Other Drawables \t" << _drawables << "\t" << _uDrawables.size() << std::endl;
    //if (cv._drawables)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Other Drawables") );
    //    parent->setValue( (unsigned int)cv._drawables );
    //    addProperty(parent);
    //}

    ////ostr << "    Total Drawables \t" << _totalDrawables << std::endl;
    //if (cv._totalDrawables)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Total Drawables") );
    //    parent->setValue( (unsigned int)cv._totalDrawables );
    //    addProperty(parent);
    //}

    ////ostr << "         DrawArrays \t" << _drawArrays << "\t" << _uDrawArrays.size() << std::endl;
    //if (cv._drawArrays)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("DrawArrays") );
    //    parent->setValue( (unsigned int)cv._drawArrays );
    //    addProperty(parent);
    //}

    ////ostr << "Total PrimitiveSets \t" << _primitiveSets << "\t" << _uPrimitiveSets.size() << std::endl;
    //if (cv._primitiveSets)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Total PrimitiveSets") );
    //    parent->setValue( (unsigned int)cv._primitiveSets );
    //    addProperty(parent);
    //}

    ////ostr << "Total vertices: " << _vertices << std::endl;
    //if (cv._vertices)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Total vertices") );
    //    parent->setValue( (unsigned int)cv._vertices );
    //    addProperty(parent);
    //}

    ////ostr << "Max depth: " << _maxDepth << std::endl;
    //if (cv._maxDepth)
    //{
    //    parent = m_variantManager->addProperty( QVariant::String, tr("Max depth") );
    //    parent->setValue( (unsigned int)cv._maxDepth );
    //    addProperty(parent);
    //}
}
