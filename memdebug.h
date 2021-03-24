#ifndef _MEMDEBUG_H_
#define _MEMDEBUG_H_

#include <Arduino.h>

#define MEM__DEBUG

#ifdef __cplusplus
extern "C"
{
#endif
    void *memDebugMalloc(const char *file, int line, size_t size);
    void *memDebugRealloc(const char *file, int line, void *ptr, size_t size);
    void memDebugFree(const char *file, int line, void *ptr);
    void printError(const char *msg);
    char *memDebugStrndup(const char *file, int line, const char *str, size_t size);
    char *memDebugStrdup(const char *file, int line, const char *str);

#ifdef __cplusplus
}
#endif

void memDebugPrintDump(void);

#ifdef MEM__DEBUG

#define DBG_MALLOC(size) memDebugMalloc(__FILE__, __LINE__, size)
#define DBG_CALLOC(count, size) memDebugMalloc(__FILE__, __LINE__, ((count) * (size)))
#define DBG_REALLOC(ptr, size) memDebugRealloc(__FILE__, __LINE__, ptr, size)
#define DBG_FREE(ptr) memDebugFree(__FILE__, __LINE__, ptr)
#define DBG_STRDUP(s) memDebugStrdup(__FILE__, __LINE__, s)
#define DBG_STRNDUP(s, n) memDebugStrndup(__FILE__, __LINE__, s, n)
#define DBG_MEMDUMP() memDebugPrintDump()

#define DEBUG_MEM_TABLE_SIZE 50

#else // MEM__DEBUG

#define DBG_MALLOC(size) malloc(size)
#define DBG_CALLOC(count, size) malloc(count, size)
#define DBG_REALLOC(ptr, size) realloc(ptr, size)
#define DBG_FREE(ptr) free(ptr)

#define DBG_STRDUP(s) strdup(s)
#define DBG_STRNDUP(s, n) strndup(s, n)

#define DBG_MEMDUMP

#endif // MEMDEBUG

#endif // _MEMDEBUG_H_
