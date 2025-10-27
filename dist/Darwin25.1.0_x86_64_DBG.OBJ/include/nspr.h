#include <stdint.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef int8_t         PRInt8;
typedef uint8_t        PRUint8;
typedef int16_t        PRInt16;
typedef uint16_t       PRUint16;
typedef int32_t        PRInt32;
typedef uint32_t       PRUint32;
typedef int64_t        PRInt64;
typedef uint64_t       PRUint64;
typedef float          PRFloat32;
typedef double         PRFloat64;
typedef int            PRStatus;
typedef size_t         PRSize;
typedef ptrdiff_t      PRPtrdiff;
typedef int            PRBool;
typedef struct PRMonitor  PRMonitor;
typedef struct PRThread   PRThread;

#define PR_TRUE         1
#define PR_FALSE        0
#define PR_SUCCESS      0
#define PR_FAILURE      -1
