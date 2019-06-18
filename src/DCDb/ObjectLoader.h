#ifndef _SCREENSHOT_HANDLER_H_
#define _SCREENSHOT_HANDLER_H_

#include "dcdb_global.h"

#include <QtCore/QObject>

#include <osg/Node>

class DCDB_EXPORT ObjectLoader :
    public QObject
{
    Q_OBJECT

public:

    ObjectLoader(QObject *parent = 0);
    virtual ~ObjectLoader(){}

public slots:

    void newObjectToLoad(const QString &, QString);
    void setOptimization(bool val) {m_optimize = val; }

signals:

    void newObjectToView(osg::Node *, QString);

private:

    QString m_file;
    osg::ref_ptr<osg::Node> m_loadedModel;
    bool m_optimize;
};

#endif // _SCREENSHOT_HANDLER_H_
