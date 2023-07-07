#ifndef CONSTRUCT_GLOBAL_H
#define CONSTRUCT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CONSTRUCT_LIBRARY)
#  define CONSTRUCT_EXPORT Q_DECL_EXPORT
#else
#  define CONSTRUCT_EXPORT Q_DECL_IMPORT
#endif

#endif // CONSTRUCT_GLOBAL_H
