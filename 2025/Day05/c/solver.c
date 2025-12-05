#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define AOC_TRUE  (1)
#define AOC_FALSE (0)

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef struct Range_Type
{
    int64_t nMin;
    int64_t nMax;
} Range_Type;

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

int compareRange(const void* pLeft, const void* pRight)
{
    Range_Type* kLeft  = (Range_Type*)pLeft;
    Range_Type* kRight = (Range_Type*)pRight;

    if (kLeft->nMin < kRight->nMin)
    {
        return -1;
    }
    else if (kLeft->nMin > kRight->nMin)
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
        char*                       kInputBuffer;
        char**                      kLines;
        size_t                      nInputLineCount;

        Range_Type*                 kFreshIngredients;
        size_t                      nFreshIngredientsCount;
        Range_Type*                 kFreshIngredientsUnmerged;
        size_t                      nFreshIngredientsUnmergedCount  = 0;
        int64_t*                    kIngredients;
        size_t                      nIngredientsCount               = 0;

        int                         bParsingFreshIngredients        = AOC_TRUE;

        int64_t                     nNumFreshIngredientsPartOne     = 0;
        int64_t                     nTotalFreshIntegredentsPartTwo  = 0;

        size_t                      i;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kInputBuffer, &kLines, &nInputLineCount, NULL, NULL, 0);
        fclose(pData);

        kFreshIngredients         = (Range_Type*)malloc(nInputLineCount * sizeof(Range_Type));
        kFreshIngredientsUnmerged = (Range_Type*)malloc(nInputLineCount * sizeof(Range_Type));
        kIngredients              = (int64_t*)   malloc(nInputLineCount * sizeof(int64_t));

        /* Parse the Input Data */
        for (i = 0; i < nInputLineCount; ++i)
        {
            if (bParsingFreshIngredients)
            {
                /* A blank line separates the Ranges from the Ingredients */
                if ('\0' == kLines[i][0])
                {
                    bParsingFreshIngredients = AOC_FALSE;
                }
                else
                {
                    /* Append the Fresh Ingredient Range */
                    kFreshIngredientsUnmerged[nFreshIngredientsUnmergedCount]  .nMin = strtoll(kLines[i],                  NULL, 10);
                    kFreshIngredientsUnmerged[nFreshIngredientsUnmergedCount++].nMax = strtoll(strstr(kLines[i], "-") + 1, NULL, 10);;
                }
            }
            else
            {
                /* Append the Fresh Integredient */
                kIngredients[nIngredientsCount++] = strtoll(kLines[i], NULL, 10);
            }
        }

        /* Sort the Ranges, this puts the mins in order, which greatly simplifies the merging process */
        qsort(kFreshIngredientsUnmerged, nFreshIngredientsUnmergedCount, sizeof(Range_Type), compareRange);

        /* Forcibly add the First Element */
        kFreshIngredients[0]   = kFreshIngredientsUnmerged[0];
        nFreshIngredientsCount = 1;

        for (i = 1; i < nFreshIngredientsUnmergedCount; ++i)
        {
            /* If the min of the current range exceeds the bounds of the last merged range */
            if (kFreshIngredients[nFreshIngredientsCount-1].nMax < kFreshIngredientsUnmerged[i].nMin - 1)
            {
                /* It's not an Overlap */
                kFreshIngredients[nFreshIngredientsCount++] = kFreshIngredientsUnmerged[i];
            }
            else
            {
                /* It's an overlap, merge the ranges */
                kFreshIngredients[nFreshIngredientsCount-1].nMax = MAX(kFreshIngredients[nFreshIngredientsCount-1].nMax,
                                                                       kFreshIngredientsUnmerged[i].nMax);
            }
        }

        /* Part One - Count Fresh Ingredients */
        for (i = 0; i < nIngredientsCount; ++i)
        {
            size_t j;

            for (j = 0; j < nFreshIngredientsCount; ++j)
            {
                if ((kIngredients[i] >= kFreshIngredients[j].nMin) &&
                    (kIngredients[i] <= kFreshIngredients[j].nMax))
                {
                    ++nNumFreshIngredientsPartOne;
                    break; /* Note: Due to the merge, we'll never duplicate count here, however this
                            *       is more efficient to cease comparing.
                            */
                }
            }
        }

        /* Part Two - Count Total Ingredients */
        for (i = 0; i < nFreshIngredientsCount; ++i)
        {
            nTotalFreshIntegredentsPartTwo += kFreshIngredients[i].nMax - kFreshIngredients[i].nMin + 1;
        }

        printf("Part 1: %lld\n", nNumFreshIngredientsPartOne);
        printf("Part 2: %lld\n", nTotalFreshIntegredentsPartTwo);

        /* Cleanup */
        free(kLines);
        free(kInputBuffer);
        free(kFreshIngredients);
        free(kFreshIngredientsUnmerged);
        free(kIngredients);
    }
 
    return 0;
}