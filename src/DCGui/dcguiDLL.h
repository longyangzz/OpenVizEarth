#ifndef DCGUI_GLOBAL_H
#define DCGUI_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef DCGUI_LIB
# define DCGUI_EXPORT Q_DECL_EXPORT
#else
# define DCGUI_EXPORT Q_DECL_IMPORT
#endif

#endif // DCGUI_GLOBAL_H
