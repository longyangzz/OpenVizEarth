#ifndef FILTERS_GLOBAL_H
#define FILTERS_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef FILTERS_EXPORT
# define FILTERS_API Q_DECL_EXPORT
#else
# define FILTERS_API Q_DECL_IMPORT
#endif

#endif // PYRAMID_GLOBAL_H
