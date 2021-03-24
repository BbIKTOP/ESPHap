#include "memdebug.h"

struct memChunk
{
    char file[64];
    int line;
    size_t size;
    void *ptr;
    unsigned long time;
} memChunks[DEBUG_MEM_TABLE_SIZE] = {{""}};

int memChunksSize = 0;

extern "C" int findChunk(void *ptr)
{
    int i;
    for (i = 0; i < memChunksSize && memChunks[i].ptr != ptr; i++)
        ;
    if (i >= memChunksSize)
    {
        // Not found, an attempt to free non allocated memory detected
        return (-1);
    }
    return (i);
}

extern "C" void *memDebugMalloc(const char *file, int line, size_t size)
{
    if (memChunksSize >= DEBUG_MEM_TABLE_SIZE - 1)
    {
        Serial.println("!!! Error !!! Memory debug: malloc: no more space in the memory chunks table left.\n");
        return (NULL);
    }

    void *ptr = malloc(size);
    if (ptr == NULL)
    {
        // Allocation error
        return (NULL);
    }
    strncpy(memChunks[memChunksSize].file, file, 63);
    memChunks[memChunksSize].line = line;
    memChunks[memChunksSize].size = size;
    memChunks[memChunksSize].ptr = ptr;
    memChunks[memChunksSize].time = millis();
    memChunksSize++;

    return (ptr);
}

extern "C" void *memDebugRealloc(const char *file, int line, void *ptr, size_t size)
{
    if (ptr == NULL)
    {
        return (memDebugMalloc(file, line, size));
    }

    int chunkNo = findChunk(ptr);
    if (chunkNo < 0)
    {
        Serial.printf("!!! Error !!! Memory debug: realloc: Chunk 0x%lX not found in the allocated chunks list\n",
                      (unsigned long)ptr);
        return (NULL);
    }
    ptr = realloc(ptr, size);
    if (ptr == NULL)
    {
        memDebugFree(file, line, ptr);
        return (NULL);
    }
    strncpy(memChunks[chunkNo].file, file, 63);
    memChunks[chunkNo].line = line;
    memChunks[chunkNo].size = size;
    memChunks[chunkNo].ptr = ptr;
    memChunks[chunkNo].time = millis();

    return (ptr);
}

extern "C" void memDebugFree(const char *file, int line, void *ptr)
{
    int chunkNo = findChunk(ptr);
    if (chunkNo < 0)
    {
        Serial.printf("!!! Error !!! Memory debug: free: [%s] [%d] Chunk 0x%lX not found in the allocated chunks list\n",
                      file,
                      line,
                      (unsigned long)ptr);
        free(ptr);
        return;
    }

    // Everything is ok, found allocated chunk
    // Remove freed entry
    if (memChunksSize > 0)
        memcpy(&memChunks[chunkNo], &memChunks[memChunksSize - 1], sizeof(memChunks[chunkNo]));
    memChunksSize--;
    free(ptr);
}

void memDebugPrintDump(void)
{
    Serial.printf("Allocated memory dump start:\n");
    for (int i = 0; i < memChunksSize; i++)
    {
        Serial.printf("%d) Allocated at %lu, file [%s], line [%d], size %d (0x%lX)\n",
                      i,
                      memChunks[i].time,
                      memChunks[i].file,
                      memChunks[i].line,
                      (int)memChunks[i].size,
                      (unsigned long)memChunks[i].ptr);
    }
    Serial.printf("Allocated memory dump end\n");
}

extern "C" void printError(const char *msg)
{
    Serial.print(msg);
}

extern "C" char *memDebugStrndup(const char *file, int line, const char *str, size_t size)
{
    char *res = strndup(str, size);
    if (res != NULL)
    {
        strncpy(memChunks[memChunksSize].file, file, 63);
        memChunks[memChunksSize].line = line;
        memChunks[memChunksSize].size = size;
        memChunks[memChunksSize].ptr = res;
        memChunks[memChunksSize].time = millis();
        memChunksSize++;
    }
    return (res);
}
extern "C" char *memDebugStrdup(const char *file, int line, const char *str)
{
    char *res = strdup(str);
    if (res != NULL)
    {
        strncpy(memChunks[memChunksSize].file, file, 63);
        memChunks[memChunksSize].line = line;
        memChunks[memChunksSize].size = strlen(str)+1;
        memChunks[memChunksSize].ptr = res;
        memChunks[memChunksSize].time = millis();
        memChunksSize++;
    }
    return (res);
}

