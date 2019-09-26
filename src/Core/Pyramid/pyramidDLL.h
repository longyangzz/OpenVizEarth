#ifndef PYRAMID_GLOBAL_H
#define PYRAMID_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef PYRAMID_EXPORT
# define PYRAMID_API Q_DECL_EXPORT
#else
# define PYRAMID_API Q_DECL_IMPORT
#endif

#endif // PYRAMID_GLOBAL_H
