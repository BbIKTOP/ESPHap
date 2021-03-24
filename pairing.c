#include "pairing.h"
#include <string.h>

// Defined in the arduino_homekit_server.cpp
#define PAIRINGS_SIZE (10 * 4)

pairing_t pairings[PAIRINGS_SIZE] = {{0}};
int pairingsSize = 0;

pairing_t *pairing_new()
{
    if (pairingsSize >= PAIRINGS_SIZE - 1)
    {
        return (NULL);
    }

    pairings[pairingsSize].id = -1;
    pairings[pairingsSize].device_id = NULL;
    pairings[pairingsSize].device_key = NULL;
    pairings[pairingsSize].permissions = 0;
    pairingsSize++;

    return (&pairings[pairingsSize]);
}

void pairing_free(pairing_t *pairing)
{
    if (pairing->device_id)
        free(pairing->device_id);

    if (pairing->device_key)
        crypto_ed25519_free(pairing->device_key);

    int i;
    for (i = 0; i < pairingsSize; i++)
    {
        if (pairing == &pairings[i])
        {
            break;
        }
    }

    if (i >= pairingsSize) // Not found
    {
        // handle error here
        return;
    }
    if (pairingsSize > 0)
    {
        memcpy(&pairings[i], &pairings[pairingsSize - 1], sizeof(pairings[i]));
    }
    pairingsSize--;
}
