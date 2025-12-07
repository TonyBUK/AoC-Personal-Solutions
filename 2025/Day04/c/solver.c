#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define AOC_TRUE  (1)
#define AOC_FALSE (0)

typedef struct Position_Type
{
    ssize_t nRow;
    ssize_t nCol;
} Position_Type;

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

int connectedRoll(const ssize_t nRow, const ssize_t nCol, char** kGrid, const size_t nGridHeight, const size_t nGridWidth)
{
    if (nRow < 0 || nCol < 0 || nRow >= nGridHeight || nCol >= nGridWidth)
    {
        return AOC_FALSE;
    }
    return (kGrid[nRow][nCol] == '@') ? AOC_TRUE : AOC_FALSE;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        char*                       kInputBuffer;
        char**                      kGrid;
        size_t                      nGridHeight;
        size_t                      nGridWidth;

        Position_Type*              kRolls;
        size_t                      nRollCount  = 0;

        Position_Type*              kConnectedRolls;
        size_t                      nConnectedRollCount;

        int64_t                     nAccessibleRollsPartOne = 0;
        int64_t                     nAccessibleRollsPartTwo = 0;
        uint8_t*                    kGridMap;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kInputBuffer, &kGrid, &nGridHeight, NULL, &nGridWidth, 0);
        fclose(pData);

        /* Extract all rolls into a 1D array */
        kRolls          = (Position_Type*)malloc(nGridHeight * nGridWidth * sizeof(Position_Type));
        kConnectedRolls = (Position_Type*)malloc(nGridHeight * nGridWidth * sizeof(Position_Type));
        kGridMap        = (uint8_t*)      calloc(nGridHeight * nGridWidth,  sizeof(uint8_t));

        {
            ssize_t row, col;
            for (row = 0; row < (ssize_t)nGridHeight; ++row)
            {
                for (col = 0; col < (ssize_t)nGridWidth; ++col)
                {
                    if (kGrid[row][col] == '@')
                    {
                        kRolls[nRollCount].nRow = row;
                        kRolls[nRollCount].nCol = col;
                        ++nRollCount;
                    }
                }
            }
        }

        /* All 8 directions to traverse */
        const Position_Type DELTAS[] = {
            {-1, -1}, {-1, 0}, {-1, 1},
            { 0, -1},          { 0, 1},
            { 1, -1}, { 1, 0}, { 1, 1}
        };
        int bFirstPass = AOC_TRUE;

        while (1)
        {
            size_t nRoll;

            /* Find all the rolls that are accessible this pass */
            nConnectedRollCount = 0;

            for (nRoll = 0; nRoll < nRollCount; ++nRoll)
            {
                const ssize_t nRow = kRolls[nRoll].nRow;
                const ssize_t nCol = kRolls[nRoll].nCol;

                /* Undo the Map for the next Iteration */
                const size_t  nGridPosition = (nRow * nGridWidth) + nCol;
                kGridMap[nGridPosition]     = 0;

                /*
                 * Count the number of connected rolls
                 *
                 * Note: If 4 or more are found, we can stop,
                 *       as it won't get any more accessible
                 *       this pass.
                 */

                size_t        nDeltas;
                size_t        nConnectedRolls = 0;

                for (nDeltas = 0; nDeltas < (sizeof(DELTAS)/sizeof(DELTAS[0])); ++nDeltas)
                {
                    const ssize_t nDeltaRow = DELTAS[nDeltas].nRow;
                    const ssize_t nDeltaCol = DELTAS[nDeltas].nCol;

                    if (connectedRoll(nRow + nDeltaRow, nCol + nDeltaCol, kGrid, nGridHeight, nGridWidth))
                    {
                        ++nConnectedRolls;

                        /* Early Termination Case, we don't care once we have 4 connected points */
                        if (nConnectedRolls == 4)
                        {
                            break;
                        }
                    }
                }

                /* If less than 4 connected rolls, it's accessible */
                if (nConnectedRolls < 4)
                {
                    kConnectedRolls[nConnectedRollCount++] = kRolls[nRoll];
                }
            }

            /* If no more accessible rolls, we're done */
            if (nConnectedRollCount == 0)
            {
                break;
            }
            else
            {
                /*
                * Handle Part One, accessible rolls before we start
                * removing
                */
                if (bFirstPass)
                {
                    nAccessibleRollsPartOne = nConnectedRollCount;
                    bFirstPass              = AOC_FALSE;
                }

                /* Update Part Two */
                nAccessibleRollsPartTwo += nConnectedRollCount;

                /* Remove the accessible rolls */
                for (nRoll = 0; nRoll < nConnectedRollCount; ++nRoll)
                {
                    kGrid[kConnectedRolls[nRoll].nRow][kConnectedRolls[nRoll].nCol] = '.';
                }

                /* When iterating rolls, we really only need to concern ourselves with rolls that were connected to
                 * a removed roll, as this is the only subset of rolls that *could* be removed the next cycle around.
                 * This constructs that list, but removes any duplicates that may arise.
                 *
                 * This drastically reduces the subsequent number of iterations.
                 */
                nRollCount = 0;
                for (nRoll = 0; nRoll < nConnectedRollCount; ++nRoll)
                {
                    const ssize_t nRow = kConnectedRolls[nRoll].nRow;
                    const ssize_t nCol = kConnectedRolls[nRoll].nCol;
                    size_t        nDeltas;

                    for (nDeltas = 0; nDeltas < (sizeof(DELTAS)/sizeof(DELTAS[0])); ++nDeltas)
                    {
                        const ssize_t nDeltaRow = nRow + DELTAS[nDeltas].nRow;
                        const ssize_t nDeltaCol = nCol + DELTAS[nDeltas].nCol;

                        /* Make sure the Grid Position is Valid */
                        if ((nDeltaRow < 0) || (nDeltaRow >= nGridHeight) || (nDeltaCol < 0) || (nDeltaCol >= nGridWidth))
                        {
                            continue;
                        }

                        /* Add the Roll Position if it's actually a roll, and not one we've added already. */
                        if (kGrid[nDeltaRow][nDeltaCol] == '@')
                        {
                            const size_t nGridPosition = (nDeltaRow * nGridWidth) + nDeltaCol;

                            if (kGridMap[nGridPosition] == AOC_FALSE)
                            {
                                kRolls[nRollCount  ].nRow = nDeltaRow;
                                kRolls[nRollCount++].nCol = nDeltaCol;
                                kGridMap[nGridPosition]   = AOC_TRUE;
                            }
                        }
                    }
                }
            }
        }

        printf("Part 1: %lld\n", nAccessibleRollsPartOne);
        printf("Part 2: %lld\n", nAccessibleRollsPartTwo);

        /* Cleanup */
        free(kGrid);
        free(kInputBuffer);
        free(kRolls);
        free(kConnectedRolls);
    }
 
    return 0;
}