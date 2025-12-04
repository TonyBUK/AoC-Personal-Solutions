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

typedef struct Roll_Type
{
    int           bRemoved;
    Position_Type kPosition;
} Roll_Type;

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

        Roll_Type*                  kRolls;
        size_t                      nRollCount  = 0;

        Roll_Type**                 kConnectedRolls;
        size_t                      nConnectedRollCount;

        int64_t                     nAccessibleRollsPartOne = 0;
        int64_t                     nAccessibleRollsPartTwo = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kInputBuffer, &kGrid, &nGridHeight, NULL, &nGridWidth, 0);
        fclose(pData);

        /* Extract all rolls into a 1D array */
        kRolls          = (Roll_Type*) malloc(nGridHeight * nGridWidth * sizeof(Roll_Type));
        kConnectedRolls = (Roll_Type**)malloc(nGridHeight * nGridWidth * sizeof(Roll_Type*));

        {
            ssize_t row, col;
            for (row = 0; row < (ssize_t)nGridHeight; ++row)
            {
                for (col = 0; col < (ssize_t)nGridWidth; ++col)
                {
                    if (kGrid[row][col] == '@')
                    {
                        kRolls[nRollCount].bRemoved       = AOC_FALSE;
                        kRolls[nRollCount].kPosition.nRow = row;
                        kRolls[nRollCount].kPosition.nCol = col;
                        ++nRollCount;
                    }
                }
            }
        }

        int bFirstPass = AOC_TRUE;
        while (1)
        {
            size_t nRoll;

            /* Find all the rolls that are accessible this pass */
            nConnectedRollCount = 0;

            for (nRoll = 0; nRoll < nRollCount; ++nRoll)
            {
                if (kRolls[nRoll].bRemoved)
                {
                    continue;
                }

                /* All 8 directions to traverse */
                const Position_Type DELTAS[] = {
                    {-1, -1}, {-1, 0}, {-1, 1},
                    { 0, -1},          { 0, 1},
                    { 1, -1}, { 1, 0}, { 1, 1}
                };

                const ssize_t nRow = kRolls[nRoll].kPosition.nRow;
                const ssize_t nCol = kRolls[nRoll].kPosition.nCol;

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
                        if (nConnectedRolls >= 4)
                        {
                            break;
                        }
                    }
                }

                /* If less than 4 connected rolls, it's accessible */
                if (nConnectedRolls < 4)
                {
                    kConnectedRolls[nConnectedRollCount] = &kRolls[nRoll];
                    ++nConnectedRollCount;
                }
            }

            /* If no more accessible rolls, we're done */
            if (nConnectedRollCount == 0)
            {
                break;
            }

            /*
             * Handle Part One, accessible rolls before we start
             * removing
             */
            if (bFirstPass)
            {
                nAccessibleRollsPartOne = nConnectedRollCount;
                bFirstPass = AOC_FALSE;
            }

            /* Update Part Two */
            nAccessibleRollsPartTwo += nConnectedRollCount;

            /* Remove the accessible rolls */
            for (nRoll = 0; nRoll < nConnectedRollCount; ++nRoll)
            {
                Roll_Type* pRoll = kConnectedRolls[nRoll];
                pRoll->bRemoved = AOC_TRUE;
                kGrid[pRoll->kPosition.nRow][pRoll->kPosition.nCol] = '.';
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