#ifndef DC_GLOBAL_H
#define DC_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef DC_LIB
# define DC_EXPORT Q_DECL_EXPORT
#else
# define DC_EXPORT Q_DECL_IMPORT
#endif

#endif // DC_GLOBAL_H
