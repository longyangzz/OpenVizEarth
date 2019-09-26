#ifndef DCDB_GLOBAL_H
#define DCDB_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef DCDB_LIB
# define DCDB_EXPORT Q_DECL_EXPORT
#else
# define DCDB_EXPORT Q_DECL_IMPORT
#endif

#endif // DCDB_GLOBAL_H
