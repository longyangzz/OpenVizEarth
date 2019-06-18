#include "AppSettings.h"

AppSettings::AppSettings( )
    : QSettings (PACKAGE_ORGANIZATION, PACKAGE_NAME)
{}
