#ifndef _FINDNAMELISTVISITOR_H_
#define _FINDNAMELISTVISITOR_H_

#include <osg/NodeVisitor>
#include <osg/Node>

#include <QtCore/QStringList>

class FindNameListVisitor :
    public osg::NodeVisitor
{
public:

    FindNameListVisitor();

    virtual void apply(osg::Node &searchNode);

    const QStringList& getNameList() const { return m_nameList; }

private:

    QStringList m_nameList;
};

#endif // _FINDNAMELISTVISITOR_H_
