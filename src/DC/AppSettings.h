#ifndef _APPETTINGS_H_
#define _APPETTINGS_H_

#include "dc_global.h"

#include <QtCore/QSettings>

#define PACKAGE_ORGANIZATION "DCLW"
#define PACKAGE_NAME "DC-Points"
#define PACKAGE_VERSION "4.1.0"

class DC_EXPORT AppSettings :
    public QSettings
{
    Q_OBJECT

public:

    AppSettings( );

protected:
};

#endif // _APPETTINGS_H_
