#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int getDirectionAndMagnitude(FILE* pData, int64_t* pDirection, int64_t* pMagnitude)
{
    /* Read the Direction */
    while (1)
    {
        const char cDirection = fgetc(pData);
        if (cDirection == 'L')
        {
            *pDirection = -1;
            break;
        }
        else if (cDirection == 'R')
        {
            *pDirection = 1;
            break;
        }
        else if (cDirection == EOF)
        {
            return 0;
        }
    }

    /* Read the Magnitude */
    uint64_t nMagnitude = 0;
    while (1)
    {
        const char cDigit = fgetc(pData);
        if (cDigit >= '0' && cDigit <= '9')
        {
            nMagnitude = (nMagnitude * 10) + (cDigit - '0');
        }
        else
        {
            *pMagnitude = nMagnitude;
            break;
        }
    }
    
    return 1;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        int64_t     nTimesPartOne = 0;
        int64_t     nTimesPartTwo = 0;
        int64_t     nValue        = 50;

        while(1)
        {
            /* Offset/Direction Calculations. */
            int64_t nDirection, nMagnitude;
            if (!getDirectionAndMagnitude(pData, &nDirection, &nMagnitude))
            {
                break;
            }

            const int64_t nOffset    = (nDirection == 1) ? 0 : -1;
                  int64_t nPrevValue = nValue;

            /* Move the Dial */
            nValue += (nDirection * nMagnitude);

            /* Small C Hack as Python division/floor behaves differently to C for negative
             * values, therefore we'll just never go negative rather than replicate.
             *
             * Python rounds to -infinity, C rounds to zero.  Therefore positive values
             * behave the same, so we do some minor adjustments to keep them positive.
             */
            while (nValue <= 0)
            {
                nValue     += 100000ll;
                nPrevValue += 100000ll;
            }

            /* Part One */
            nTimesPartOne += ((nValue % 100) == 0) ? 1 : 0;

            /* Part Two - Detailed notes in Python Code */
            nTimesPartTwo += nDirection * (((nValue + nOffset) / 100) - ((nPrevValue + nOffset) / 100));
        }

        fclose(pData);

        printf("Part 1: %lld\n", nTimesPartOne);
        printf("Part 2: %lld\n", nTimesPartTwo);
    }
 
    return 0;
}