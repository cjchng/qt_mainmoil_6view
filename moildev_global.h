#ifndef MOILDEV_GLOBAL_H
#define MOILDEV_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MOILDEV_LIBRARY)
#  define MOILDEVSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MOILDEVSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MOILDEV_GLOBAL_H
