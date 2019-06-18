#ifndef MANAGER_GLOBAL_H
#define MANAGER_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef MANAGER_LIB
# define MANAGER_EXPORT Q_DECL_EXPORT
#else
# define MANAGER_EXPORT Q_DECL_IMPORT
#endif

#endif // MANAGER_GLOBAL_H
