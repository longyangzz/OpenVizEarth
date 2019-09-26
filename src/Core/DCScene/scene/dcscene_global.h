#ifndef DCSCENE_GLOBAL_H
#define DCSCENE_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef DCSCENE_LIB
# define DCSCENE_EXPORT Q_DECL_EXPORT
#else
# define DCSCENE_EXPORT Q_DECL_IMPORT
#endif

#endif // DCSCENE_GLOBAL_H
