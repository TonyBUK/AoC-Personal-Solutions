#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define AOC_TRUE  (1)
#define AOC_FALSE (0)

#define ALLOW_VARIABLE_LENGTH_LINES (0)
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define TO_GRID(nOperation, nValue, nMaxValues) ((nOperation * (nMaxValues)) + (nValue))

typedef enum Operators_Type
{
    SUM     = '+',
    PRODUCT = '*'
} Operators_Type;

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

int64_t solveCephalopodsMaths(const int64_t* kValues, const size_t* kValueCount, const size_t nMaxValueCount, const Operators_Type* kOperators, const size_t nOperatorCount)
{
    int64_t nResult = 0;
    size_t  i;

    for (i = 0; i < nOperatorCount; ++i)
    {
        switch(kOperators[i])
        {
            case SUM:
            {
                size_t j;
                for (j = 0; j < kValueCount[i]; ++j)
                {
                    nResult += kValues[TO_GRID(i, j, nMaxValueCount)];
                }
            } break;

            case PRODUCT:
            {
                size_t j;
                int64_t nProduct = 1;
                for (j = 0; j < kValueCount[i]; ++j)
                {
                    nProduct *= kValues[TO_GRID(i, j, nMaxValueCount)];
                }
                nResult += nProduct;
            } break;
        }
    }

    return nResult;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        char*                       kInputBuffer;
        char**                      kLines;
        size_t                      nInputLineCount;
        size_t                      nMaxLineWidth;

        size_t                      nMaxOperators;

        Operators_Type*             kOperators;
        int64_t*                    kValuesPartOne;
        int64_t*                    kValuesPartTwo;

        size_t                      nOperatorsCount       = 0;
        size_t*                     kValueCountPartOne;
        size_t*                     kValueCountPartTwo;
        size_t                      nMaxValueCountPartTwo = 0;

        size_t                      i;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kInputBuffer, &kLines, &nInputLineCount, NULL, &nMaxLineWidth, 0);
        fclose(pData);

        /* Worst Case Operator Count is half the Max Line Width as the data set appears to
         * mandate a gap.
         */
        nMaxOperators      = nMaxLineWidth / 2;
        kOperators         = (Operators_Type*)malloc(nMaxOperators * sizeof(Operators_Type));
        kValueCountPartOne = (size_t*)        malloc(nMaxOperators * sizeof(size_t));
        kValueCountPartTwo = (size_t*)        malloc(nMaxOperators * sizeof(size_t));

        /* Parse the Operators */
        for (i = 0; i < nMaxLineWidth; ++i)
        {
            const char kOperator = kLines[nInputLineCount-1][i];
            if (kOperator != ' ')
            {
                /* Undo the blank column to the left */
                if (nOperatorsCount > 0)
                {
                    --kValueCountPartTwo[nOperatorsCount-1];
                }
                kValueCountPartTwo[nOperatorsCount] = 0;
                kOperators[nOperatorsCount++] = (Operators_Type)kOperator;
            }
#if ALLOW_VARIABLE_LENGTH_LINES
            /* Just incase not all lines are the same length... */
            else if (kOperator == '\0')
            {
                break;
            }
#else
            assert(kOperator != '\0');
#endif

            /* Use this as a change to calculate Part Two's Parameter Count */
            if ((++kValueCountPartTwo[nOperatorsCount-1]) > nMaxValueCountPartTwo)
            {
                nMaxValueCountPartTwo = kValueCountPartTwo[nOperatorsCount-1];
            }
        }

        /* Note: The extra effort to constraint the size of these saves a ton of execution time on
         * the constrained CALLOC's
         */
        kValuesPartOne     = (int64_t*)calloc((nInputLineCount - 1) * nOperatorsCount, sizeof(int64_t));
        kValuesPartTwo     = (int64_t*)calloc(nMaxValueCountPartTwo * nOperatorsCount, sizeof(int64_t));

        /* Construct the Values List for Both Parts */
        for (i = 0; i < (nInputLineCount-1); ++i)
        {
            ssize_t nExpression      = -1;
            size_t  nValuePosPartOne = i;
            size_t  nValuePosPartTwo = 0;
            size_t  j;

            for (j = 0; j < nMaxLineWidth; ++j)
            {
                const char kValue = kLines[i][j];
#if ALLOW_VARIABLE_LENGTH_LINES
                /* Just incase not all lines are the same length... */
                if (kValue == '\0')
                {
                    break;
                }
#else
                assert(kValue != '\0');
#endif
                ++nValuePosPartTwo;
                if (kLines[nInputLineCount-1][j] != ' ')
                {
                    ++nExpression;
                    nValuePosPartTwo = 0;

                    if (i == 0)
                    {
                        kValueCountPartOne[nExpression] = 1;
                    }
                    else
                    {
                        ++kValueCountPartOne[nExpression];
                    }
                }

                if (kValue != ' ')
                {
                    const int64_t nValue        = kValue - '0';
                    const size_t  nPartOnePos   = TO_GRID(nExpression, nValuePosPartOne, (nInputLineCount - 1));
                    const size_t  nPartTwoPos   = TO_GRID(nExpression, nValuePosPartTwo, nMaxValueCountPartTwo);

                    kValuesPartOne[nPartOnePos] = (kValuesPartOne[nPartOnePos] * 10) + nValue;
                    kValuesPartTwo[nPartTwoPos] = (kValuesPartTwo[nPartTwoPos] * 10) + nValue;
                }
            }
        }

        printf("Part 1: %lld\n", solveCephalopodsMaths(kValuesPartOne, kValueCountPartOne, (nInputLineCount - 1), kOperators, nOperatorsCount));
        printf("Part 2: %lld\n", solveCephalopodsMaths(kValuesPartTwo, kValueCountPartTwo, nMaxValueCountPartTwo, kOperators, nOperatorsCount));

        /* Cleanup */
        free(kLines);
        free(kInputBuffer);
        free(kOperators);
        free(kValuesPartOne);
        free(kValuesPartTwo);
        free(kValueCountPartOne);
        free(kValueCountPartTwo);
   }
 
    return 0;
}