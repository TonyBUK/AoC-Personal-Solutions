#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define AOC_TRUE  (1)
#define AOC_FALSE (0)

#define TO_1D(s) (((uint16_t)((s)[0]-'a') << 10)|((uint16_t)((s)[1]-'a') << 5)|((uint16_t)((s)[2]-'a') << 0))
#define TO_CACHE_KEY(a,b,n) ((n) | ((a) << 15) | ((b) << 16))

typedef struct Device_Type
{
    size_t       nConnectedDeviceCount;
    uint16_t*    kConnectedDevices;
} Device_Type;

/* Boilerplate code to read an entire file into a 1D buffer and give 2D entries per line.
 * This uses the EOL \n to act as a delimiter for each line.
 *
 * This will work for PC or Unix files, but not for blended (i.e. \n and \r\n)
 */
void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount, size_t* pnFileLength, size_t* pnMaxLineLength, const size_t nPadLines)
{
    const unsigned long     nStartPos      = ftell(*pFile);
    const char*             kEOL           = "\n";
    unsigned long           nEndPos;
    unsigned long           nFileSize;
    unsigned                bProcessUnix   = 1;
    size_t                  nEOLLength     = 1;
    char*                   pLine;
    size_t                  nLineCount     = 0;
    size_t                  nMaxLineLength = 0;
    size_t                  nReadCount;
    size_t                  nPadLine;
 
    /* Read the File to a string buffer and append a NULL Terminator */
    fseek(*pFile, 0, SEEK_END);
    nEndPos = ftell(*pFile);
    assert(nStartPos != nEndPos);
    fseek(*pFile, nStartPos, SEEK_SET);
 
    nFileSize       = nEndPos - nStartPos;
 
    *pkFileBuffer   = (char*) malloc((nFileSize+3)         * sizeof(char));
    *pkLines        = (char**)malloc((nFileSize+nPadLines) * sizeof(char*));
 
    /* Handle weird EOL conventions whereby fread and fseek will handle EOL differently. */
    nReadCount = fread(*pkFileBuffer, nFileSize, 1, *pFile);
    assert(nReadCount == 1);
 
    /* Detect whether this has a \r\n or \n EOL convention */
    if (strstr(*pkFileBuffer, "\r\n"))
    {
        kEOL         = "\r\n";
        bProcessUnix = 0;
        nEOLLength   = 2;
    }
 
    /* Pad the File Buffer with a trailing new line (if needed) to simplify things later on */
    if (0 == bProcessUnix)
    {
        if ((*pkFileBuffer)[nFileSize] != '\n')
        {
            (*pkFileBuffer)[nFileSize]   = '\r';
            (*pkFileBuffer)[nFileSize+1] = '\n';
            (*pkFileBuffer)[nFileSize+2] = '\0';
        }
    }
    else
    {
        if ((*pkFileBuffer)[nFileSize] != '\n')
        {
            (*pkFileBuffer)[nFileSize]   = '\n';
            (*pkFileBuffer)[nFileSize+1] = '\0';
        }
    }
 
    /*
     * Convert the 1D string buffer into a 2D buffer delimited by EOL
     *
     * This effectively replaces all EOL's with NUL terminators.
     */
    pLine = *pkFileBuffer;
    while (1)
    {
        /* Find the next EOL */
        char* pEOL = strstr(pLine, kEOL);
 
        /* Check whether we've reached the EOF */
        if (pEOL)
        {
            const size_t nLineLength = pEOL - pLine;
            nMaxLineLength = (nLineLength > nMaxLineLength) ? nLineLength : nMaxLineLength;
 
            assert(pLine < &(*pkFileBuffer)[nFileSize]);
 
            (*pkLines)[nLineCount++] = pLine;
 
            /* Replace the EOL with a NUL terminator */
            *pEOL = '\0';
 
            /* Move to the start of the next line... */
            pLine = pEOL + nEOLLength;
        }
        else
        {
            break;
        }
    }
 
    for (nPadLine = 0; nPadLine < nPadLines; ++nPadLine)
    {
        (*pkLines)[nLineCount] = &(*pkFileBuffer)[nFileSize+1];
        ++nLineCount;
    }
 
    *pnLineCount  = nLineCount;
    if (NULL != pnFileLength)
    {
        *pnFileLength = nFileSize;
    }
    if (NULL != pnMaxLineLength)
    {
        *pnMaxLineLength = nMaxLineLength;
    }
}

int64_t parseLight(const char* kLight)
{
    int64_t nLight = 0;
    int64_t i;

    for (i = 0; i < strlen(kLight); ++i)
    {
        nLight <<= 1;
        if (kLight[i] == '#')
        {
            nLight |= 1;
        }
    }

    return nLight;
}

int64_t countPaths(const uint16_t nNode, const uint16_t nTargetNode, const Device_Type* kPaths, const uint16_t* kCriticalNodes, uint16_t* kFoundCriticalNodes, int64_t* kCache)
{
    /*
     * The Cache indicates the current state as follows:
     *
     * 1. Which Critical Nodes have been found (fft/dac)
     * 2. What Node are we on
     *
     * This allows caching of repeats
     */
    const uint32_t nCacheKey = TO_CACHE_KEY((NULL == kCriticalNodes) ? 1 : ((kFoundCriticalNodes[0] == kCriticalNodes[0]) ? 1 : 0),
                                            (NULL == kCriticalNodes) ? 1 : ((kFoundCriticalNodes[1] == kCriticalNodes[1]) ? 1 : 0),
                                            nNode);
    int64_t        nRoutes   = 0;
    size_t         nNextNode;
    int            bResetCriticalNodes[2] = {AOC_FALSE, AOC_FALSE};

    if (kCache[nCacheKey] >= 0)
    {
        return kCache[nCacheKey];
    }

    if (NULL != kCriticalNodes)
    {
        assert(kFoundCriticalNodes);
        if (nNode == kCriticalNodes[0])
        {
            kFoundCriticalNodes[0] = nNode;
            bResetCriticalNodes[0] = AOC_TRUE;
        }
        else if (nNode == kCriticalNodes[1])
        {
            kFoundCriticalNodes[1] = nNode;
            bResetCriticalNodes[1] = AOC_TRUE;
        }
    }

    /* If this is our target node, indicate it is valid if we found all our critical nodes */
    if (nNode == nTargetNode)
    {
        if (NULL == kCriticalNodes)
        {
            return 1;
        }
        else if ((kCriticalNodes[0] == kFoundCriticalNodes[0]) &&
                 (kCriticalNodes[1] == kFoundCriticalNodes[1]))
        {
            return 1;
        }
    }

    for (nNextNode = 0; nNextNode < kPaths[nNode].nConnectedDeviceCount; ++nNextNode)
    {
        nRoutes += countPaths(kPaths[nNode].kConnectedDevices[nNextNode], nTargetNode, kPaths, kCriticalNodes, kFoundCriticalNodes, kCache);
    }

    /* Cache the Number of Routes for any repetitions */
    kCache[nCacheKey] = nRoutes;

    if (bResetCriticalNodes[0])
    {
        kFoundCriticalNodes[0] = 0;
    }
    else if (bResetCriticalNodes[1])
    {
        kFoundCriticalNodes[1] = 0;
    }

    return nRoutes;
}

/*
        def countPaths(kNode, kTargetNode, kPaths, kCriticalNodes, kFoundCriticalNodes = None, kCache = None) :
            # Calculate the Number of Routes for our current path
            nRoutes = sum([countPaths(kNextNode, kTargetNode, kPaths, kCriticalNodes, kFoundCriticalNodes, kCache) for kNextNode in kPaths[kNode]])

            # Cache the Number of Routes for any repetitions
            kCache[kCacheKey] = nRoutes

            return nRoutes

        #end
*/

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        char*                       kInputBuffer;
        char**                      kInputLines;

        size_t                      nDeviceCount;
        Device_Type*                kDevices;
        int64_t*                    kCache;

        size_t                      i;

        const uint16_t              kCriticalNodes[2]      = {TO_1D("dac"), TO_1D("fft")};
        uint16_t                    kFoundCriticalNodes[2] = {0,0};


        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kInputBuffer, &kInputLines, &nDeviceCount, NULL, NULL, 0);
        fclose(pData);

        /* Allocate the Connected Devices */
        /* Note: This will be a fixed size, because we want to index by the name of the device as a 15 bit
         *       number, where each value will be expected to be in the range 'a' .. 'z', where a = 0, z = 25,
         *       which neatly means we can express abc as a 15 bit number.  This allows a very fast 1D lookup
         *       rather than a hash map.
         *
         *       This does mean however because for Part Two, we need 2 extra bits for a cache, being 2 bits
         *       indicating whether fft/dac were found, this will need to be a 17 bit number.
         */
        kDevices = (Device_Type*)malloc(0x08000u * sizeof(Device_Type));
        kCache   = (int64_t*)    malloc(0x20000u * sizeof(int64_t));

        for (i = 0; i < nDeviceCount; ++i)
        {
            const uint16_t nDeviceId             = TO_1D(kInputLines[i]);
            const size_t   nConnectedDeviceCount = (strlen(kInputLines[i]) - 4) / 4;

            size_t         j;

            /* Add the Connected Devices */
            kDevices[nDeviceId].nConnectedDeviceCount = nConnectedDeviceCount;
            kDevices[nDeviceId].kConnectedDevices     = (uint16_t*)malloc(nConnectedDeviceCount * sizeof(uint16_t));
            for (j = 0; j < nConnectedDeviceCount; ++j)
            {
                kDevices[nDeviceId].kConnectedDevices[j] = TO_1D(&kInputLines[i][5 + (j * 4)]);
            }
        }

        memset(kCache, 0xFF, 0x20000u * sizeof(int64_t));
        printf("Part 1: %lld\n", countPaths(TO_1D("you"), TO_1D("out"), kDevices, NULL, NULL, kCache));

        /* Reset the Cache */
        memset(kCache, 0xFF, 0x20000u * sizeof(int64_t));
        printf("Part 2: %lld\n", countPaths(TO_1D("svr"), TO_1D("out"), kDevices, kCriticalNodes, kFoundCriticalNodes, kCache));

        /* Cleanup */
        for (i = 0; i < nDeviceCount; ++i)
        {
            free(kDevices[TO_1D(kInputLines[i])].kConnectedDevices);
        }
        free(kInputLines);
        free(kInputBuffer);
        free(kDevices);
        free(kCache);
    }
 
    return 0;
}