#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define AOC_TRUE  (1)
#define AOC_FALSE (0)

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

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        char*                       kInputBuffer;
        char**                      kGrid;
        size_t                      nGridHeight;
        size_t                      nGridWidth;
        size_t*                     kTachyonBeams;
        size_t                      nTachyonBeamCount;
        size_t*                     kNextTachyonBeams;
        size_t                      nNextTachyonBeamCount       = 0;
        int64_t*                    kTachyonBeamsInTimeLine;
        int64_t*                    kNextTachyonBeamsInTimeLine;

        size_t                      nRow;
        size_t                      y;

        int64_t                     nSplitCount                 = 0;
        int64_t                     nTotalTimeLines             = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kInputBuffer, &kGrid, &nGridHeight, NULL, &nGridWidth, 0);
        fclose(pData);

        kTachyonBeams               = (size_t*) malloc(nGridWidth * sizeof(size_t));
        kNextTachyonBeams           = (size_t*) malloc(nGridWidth * sizeof(size_t));
        kTachyonBeamsInTimeLine     = (int64_t*)calloc(nGridWidth,  sizeof(int64_t));
        kNextTachyonBeamsInTimeLine = (int64_t*)calloc(nGridWidth,  sizeof(int64_t));

        /*
         * Note: Because this solution will effectively always move all beams 1 row down at the
         *       same time, we never need to store the rows per beam point, only the columns.
         */

        /* Find our Start Point */
        for (y = 0; y < nGridHeight; ++y)
        {
            const char* p = strstr(kGrid[y], "S");
            if (p)
            {
                kTachyonBeams[0]                          = p - kGrid[y];
                nTachyonBeamCount                         = 1;
                kTachyonBeamsInTimeLine[kTachyonBeams[0]] = 1;
                nRow                                      = y + 1;
                break;
            }
        }

        /*
         * Move the Tachyon Beams
         *
         * The basic premise is we never want to actually duplicate the Tachyon Beams to anything
         * other than unique points.
         *
         * Using non-unique would get us the timeline expansion for part two, but it would mean repeating
         * the process far more times than required, problem there is you'll eventually be processing
         * more than > 10,000,000,000,000 on the real puzzle input which is unsustainable.
         *
         * Instead we can just keep a tally on each Tachyon Beam as to how many ways it reached its
         * point.
        */
        while (1)
        {
            size_t nTachyonBeamIndex;

            for (nTachyonBeamIndex = 0; nTachyonBeamIndex < nTachyonBeamCount; ++nTachyonBeamIndex)
            {
                const size_t  nTachyonBeam = kTachyonBeams[nTachyonBeamIndex];
                const int64_t nTimeLines   = kTachyonBeamsInTimeLine[nTachyonBeam];

                /* Is it going to move freely? */
                if (kGrid[nRow][nTachyonBeam] == '.')
                {
                    /* Update, if needed, the next unique set of Tachyon Beams */
                    if (kNextTachyonBeamsInTimeLine[nTachyonBeam] == 0)
                    {
                        kNextTachyonBeams[nNextTachyonBeamCount++] = nTachyonBeam;
                    }

                    /*
                     * Update the Number of Timelines
                     *
                     * Note: We may end up needing to incorporate other timelines as well
                     */
                    kNextTachyonBeamsInTimeLine[nTachyonBeam] += nTimeLines;
                }
                else
                {
                    /* Update the Positions */
                    const size_t nLeft  = nTachyonBeam - 1;
                    const size_t nRight = nTachyonBeam + 1;

                    /* Check for Underflow/Overflow */
                    assert(nLeft < nRight);

                    /* Update, if needed, the next unique set of Tachyon Beams */
                    if (kNextTachyonBeamsInTimeLine[nLeft]  == 0) kNextTachyonBeams[nNextTachyonBeamCount++] = nLeft;
                    if (kNextTachyonBeamsInTimeLine[nRight] == 0) kNextTachyonBeams[nNextTachyonBeamCount++] = nRight;

                    /* Update Split Count */
                    ++nSplitCount;

                    /*
                     * Update the Number of Timelines
                     *
                     * Note: We may end up needing to incorporate other timelines as well
                     */
                    kNextTachyonBeamsInTimeLine[nLeft]  += nTimeLines;
                    kNextTachyonBeamsInTimeLine[nRight] += nTimeLines;

                    /* Sanity check to make sure Eric isn't being evil! */
                    assert(kGrid[nRow + 1][nLeft]  != '^');
                    assert(kGrid[nRow + 1][nRight] != '^');
                }
            }

            /* Check whether we're done */
            if (++nRow >= nGridHeight)
            {
                break;
            }
            else /* Refresh the list of Tachyon Beams/Timelines */
            {
                /* Swap the Buffers */
                size_t*  p                  = kTachyonBeams;
                int64_t* q                  = kTachyonBeamsInTimeLine;

                kTachyonBeams               = kNextTachyonBeams;
                nTachyonBeamCount           = nNextTachyonBeamCount;
                kNextTachyonBeams           = p;

                kTachyonBeamsInTimeLine     = kNextTachyonBeamsInTimeLine;
                kNextTachyonBeamsInTimeLine = q;

                /* Clear the Next Beam Positions */
                nNextTachyonBeamCount = 0;
                memset(kNextTachyonBeamsInTimeLine, 0, nGridWidth * sizeof(int64_t));
            }
        }

        /* Sum the Final Timeline Tally */
        for (y = 0; y < nNextTachyonBeamCount; ++y)
        {
            const size_t nTachyonBeam = kNextTachyonBeams[y];
            nTotalTimeLines += kNextTachyonBeamsInTimeLine[nTachyonBeam];
        }

        printf("Part 1: %lld\n", nSplitCount);
        printf("Part 2: %lld\n", nTotalTimeLines);

        /* Cleanup */
        free(kGrid);
        free(kInputBuffer);
        free(kTachyonBeams);
        free(kNextTachyonBeams);
        free(kTachyonBeamsInTimeLine);
        free(kNextTachyonBeamsInTimeLine);
   }
 
    return 0;
}