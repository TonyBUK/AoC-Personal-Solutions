#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define AOC_TRUE  (1)
#define AOC_FALSE (0)

#define SQUARE(x) ((x) * (x))
#define TO_CIRCUIT_ARRAY(nCurrentCircuit, nMaxCircuitSize, nCurrentCircuitSize) (((nCurrentCircuit) * (nMaxCircuitSize)) + (nCurrentCircuitSize))

#define ITERATIONS (1000)

typedef struct Junction_Box_Type
{
    int64_t X;
    int64_t Y;
    int64_t Z;
} Junction_Box_Type;

typedef struct Distance_Type
{
    int64_t           nDistance;
    size_t            nJunctionBox1Index;
    size_t            nJunctionBox2Index;
} Distance_Type;

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

int compareDistances(const void* pLeft, const void* pRight)
{
    Distance_Type* kLeft  = (Distance_Type*)pLeft;
    Distance_Type* kRight = (Distance_Type*)pRight;

    if (kLeft->nDistance < kRight->nDistance)
    {
        return -1;
    }
    else if (kLeft->nDistance > kRight->nDistance)
    {
        return 1;
    }

    return 0;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        const size_t                INVALID_MAP                = (size_t)-1;

        char*                       kInputBuffer;
        char**                      kInputLines;

        /* Raw Junction Boxes */
        Junction_Box_Type*          kJunctionBoxes;
        size_t                      nJunctionBoxCount;

        Distance_Type*              kDistances;
        size_t                      nDistanceMaxCount;
        size_t                      nDistanceCount             = 0;

        /* 2D Array of all circuits and the junction boxes within */
        size_t*                     kCircuits;
        size_t*                     kCircuitSizes;

        /* Note: As we lack true array insertion/popping, this is used to skip gaps that will inevitably
         *       be created as we merge circuits.
         */
        size_t                      nCircuitHighWaterMark      = 0;

        /* Map for each Junction Box to the Circuit */
        size_t*                     kCircuitMap;

        /* Buffer indicating whether a Junction is in a Circuit or Not */
        uint8_t*                    kJunctionBoxInCircuit;
        size_t                      nJunctionBoxInCircuitCount = 0;

        size_t                      nNumCircuits               = 0;

        size_t                      i;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kInputBuffer, &kInputLines, &nJunctionBoxCount, NULL, NULL, 0);
        fclose(pData);

        /* The Number of Distances is effectively the triangle number of the final Junction Box count.
         *
         * i.e. 999 + 998 + ... 2 + 1 = 449,500
         */
        nDistanceMaxCount = (nJunctionBoxCount * (nJunctionBoxCount - 1)) / 2;

        kJunctionBoxes        = (Junction_Box_Type*)malloc(nJunctionBoxCount *                     sizeof(Junction_Box_Type));
        kDistances            = (Distance_Type*)    malloc(nDistanceMaxCount *                     sizeof(Distance_Type));        
        kCircuits             = (size_t*)           malloc(nJunctionBoxCount * nJunctionBoxCount * sizeof(size_t));
        kCircuitSizes         = (size_t*)           malloc(nJunctionBoxCount                     * sizeof(size_t));
        kCircuitMap           = (size_t*)           malloc(nJunctionBoxCount                     * sizeof(size_t));
        kJunctionBoxInCircuit = (uint8_t*)          calloc(nJunctionBoxCount,                      sizeof(uint8_t));

        /* Calculate all the possible distances between the Junction Boxes and sort */
        for (i = 0; i < nJunctionBoxCount; ++i)
        {
            char* p;
            kJunctionBoxes[i].X = strtoll(kInputLines[i], NULL, 10);
            p = strstr(kInputLines[i], ",") + 1;
            kJunctionBoxes[i].Y = strtoll(p, NULL, 10);
            p = strstr(p, ",") + 1;
            kJunctionBoxes[i].Z = strtoll(p, NULL, 10);

            /* Initialise the Circuit Map to a Deterministic Value */
            kCircuitMap[i] = INVALID_MAP;

            if (i > 0)
            {
                size_t j;
                for (j = 0; j < i; ++j)
                {
                    kDistances[nDistanceCount  ].nDistance          = SQUARE(kJunctionBoxes[i].X - kJunctionBoxes[j].X) +
                                                                      SQUARE(kJunctionBoxes[i].Y - kJunctionBoxes[j].Y) +
                                                                      SQUARE(kJunctionBoxes[i].Z - kJunctionBoxes[j].Z);
                    kDistances[nDistanceCount  ].nJunctionBox1Index = i;
                    kDistances[nDistanceCount++].nJunctionBox2Index = j;
                }
            }
        }

        /* Sort the Distances */
        qsort(kDistances, nDistanceCount, sizeof(Distance_Type), compareDistances);

        /*
         * Part Two ends when:
         * 1. All Junction Boxes are in the Circuit
         * 2. We're now operating with a single circuit
         */

        for (i =  0; i < nDistanceCount; ++i)
        {
            /* Extract the circuits the Junction Boxes belong to */
            const size_t nJunctionBox1Index = kDistances[i].nJunctionBox1Index;
            const size_t nJunctionBox2Index = kDistances[i].nJunctionBox2Index;
            const size_t nCircuit1          = kCircuitMap[nJunctionBox1Index];
            const size_t nCircuit2          = kCircuitMap[nJunctionBox2Index];

            /* Remove the Junction Boxes from the list Junction Boxes yet to be processed */
            if (AOC_FALSE == kJunctionBoxInCircuit[nJunctionBox1Index])
            {
                kJunctionBoxInCircuit[nJunctionBox1Index] = AOC_TRUE;
                ++nJunctionBoxInCircuitCount;
            }

            if (AOC_FALSE == kJunctionBoxInCircuit[nJunctionBox2Index])
            {
                kJunctionBoxInCircuit[nJunctionBox2Index] = AOC_TRUE;
                ++nJunctionBoxInCircuitCount;
            }

            /* If neither circuits are known, we're creating a new circuit */
            if ((INVALID_MAP == nCircuit1) && (INVALID_MAP == nCircuit2))
            {
                /* Note: We use the High Watermark as the Number of Circuits would only work if we constantly
                 *       shuffled the data arround when popping an array entry, which would be very slow.
                 */
                const size_t nCircuitIndex = TO_CIRCUIT_ARRAY(nCircuitHighWaterMark, nJunctionBoxCount, 0);

                /* Create the New Circuit */
                kCircuitSizes[nCircuitHighWaterMark] = 2;
                kCircuits[nCircuitIndex]             = nJunctionBox1Index;
                kCircuits[nCircuitIndex+1]           = nJunctionBox2Index;
                kCircuitMap[nJunctionBox1Index]      = nCircuitHighWaterMark;
                kCircuitMap[nJunctionBox2Index]      = nCircuitHighWaterMark;

                /* Increment the Circuit Count */
                ++nNumCircuits;
                ++nCircuitHighWaterMark;
            }
            else
            {
                /*
                 * If both circuits are known, we're either getting a duplication on the
                 * same circuit, or we're merging circuits
                 */
                if ((INVALID_MAP != nCircuit1) && (INVALID_MAP != nCircuit2))
                {
                    /* Check if we're merging Circuits */
                    if (nCircuit1 != nCircuit2)
                    {
                        const size_t nCircuit1Size  = kCircuitSizes[nCircuit1];
                        const size_t nCircuit2Size  = kCircuitSizes[nCircuit2];
                        const size_t nCircuit1Index = TO_CIRCUIT_ARRAY(nCircuit1, nJunctionBoxCount, nCircuit1Size);
                        const size_t nCircuit2Index = TO_CIRCUIT_ARRAY(nCircuit2, nJunctionBoxCount, 0);
                        
                        size_t j;

                        /* Merge the Circuit */
                        kCircuitSizes[nCircuit1] += nCircuit2Size;
                        for (j = 0; j < nCircuit2Size; ++j)
                        {
                            const size_t nJunctionIndex   = kCircuits[nCircuit2Index + j];

                            /* Sanity Check */
                            assert(kCircuitMap[nJunctionIndex] == nCircuit2);

                            kCircuits[nCircuit1Index + j] = nJunctionIndex;
                            kCircuitMap[nJunctionIndex]   = nCircuit1;
                        }

                        /* Clear the old Circuit */
                        kCircuitSizes[nCircuit2] = 0;

                        /* Sanity Check */
                        assert(nNumCircuits != 0);

                        /* Decrement the Circuit Count */
                        --nNumCircuits;
                    }
                }
                else
                {
                    /* We're just appending to an existing circuit */

                    /* Append to Circuit 2 if Junction Box 1 doesn't below to a Circuit */
                    if (INVALID_MAP == nCircuit1)
                    {
                        const size_t nCircuit2Size       = kCircuitSizes[nCircuit2];
                        const size_t nCircuit2Index      = TO_CIRCUIT_ARRAY(nCircuit2, nJunctionBoxCount, nCircuit2Size);

                        kCircuits[nCircuit2Index]        = nJunctionBox1Index;
                        kCircuitMap[nJunctionBox1Index]  = nCircuit2;
                        kCircuitSizes[nCircuit2]        += 1;
                    }

                    /* Append to Circuit 1 if Junction Box 2 doesn't below to a Circuit */
                    if (INVALID_MAP == nCircuit2)
                    {
                        const size_t nCircuit1Size       = kCircuitSizes[nCircuit1];
                        const size_t nCircuit1Index      = TO_CIRCUIT_ARRAY(nCircuit1, nJunctionBoxCount, nCircuit1Size);

                        kCircuits[nCircuit1Index]        = nJunctionBox2Index;
                        kCircuitMap[nJunctionBox2Index]  = nCircuit1;
                        kCircuitSizes[nCircuit1]        += 1;
                    }
                }
            }

            /* Part 1: 1000 Iterations, what are the three biggest circuits */
            if (i == (ITERATIONS-1))
            {
                int64_t a = 0;
                int64_t b = 0;
                int64_t c = 0;
                size_t j;

                for (j = 0; j < nCircuitHighWaterMark; ++j)
                {
                    const size_t nCircuitSize = kCircuitSizes[j];
                    
                    if (nCircuitSize > a)
                    {
                        c = b;
                        b = a;
                        a = nCircuitSize;
                    }
                    else if (nCircuitSize > b)
                    {
                        c = b;
                        b = nCircuitSize;
                    }
                    else if (nCircuitSize > c)
                    {
                        c = nCircuitSize;
                    }
                }

                printf("Part 1: %lld\n", a*b*c);
            }

            /*
             * Part 2: What was the Junction Box Pair that created the final complete
             *        circuit
             */
            if (nJunctionBoxInCircuitCount == nJunctionBoxCount)
            {
                if (nNumCircuits == 1)
                {
                    printf("Part 2: %lld\n", kJunctionBoxes[nJunctionBox1Index].X * kJunctionBoxes[nJunctionBox2Index].X);
                    break;
                }
            }
        }

        /* Cleanup */
        free(kInputLines);
        free(kInputBuffer);
        free(kJunctionBoxes);
        free(kDistances);
        free(kCircuits);
        free(kCircuitSizes);
        free(kCircuitMap);
        free(kJunctionBoxInCircuit);
    }
 
    return 0;
}