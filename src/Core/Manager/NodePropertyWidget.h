#ifndef NODE_PROPERTYWIDGET_H_
#define NODE_PROPERTYWIDGET_H_

#include "manager_global.h"

//Qt
#include <QtCore/QString>
#include <QtWidgets/QScrollArea>
#include <QtCore/QMap>

//osg
#include <osg/Node>
#include <osg/Switch>
#include <osg/LOD>

//propertybrowser
#include "propertybrowser\qttreepropertybrowser.h"
#include "propertybrowser\qtvariantproperty.h"

class MANAGER_EXPORT NodePropertyWidget :
    public QtTreePropertyBrowser
{
	Q_OBJECT
public:

    NodePropertyWidget( QWidget *parent = 0 );
    ~NodePropertyWidget() {}

    void displayProperties(osg::Node *);
    void displayNodeProperties(osg::Node *node);
    void displayLODProperties(osg::LOD *);
    void displaySwitchProperties(osg::Switch *);
    void displayGeodeProperties(osg::Geode *node);
    void displayBaseStats(osg::Node *);

public slots:

signals:

protected:

private:

    void initDictionaries();

    QString m_file;

    QtVariantPropertyManager *m_variantManager;

    QList<QString> m_listDataVariance;
    QList<QString> m_listCenterMode;
    QList<QString> m_listRangeMode;
};

#endif // _PROPERTYWIDGET_H_
