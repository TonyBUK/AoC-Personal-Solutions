#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int64_t getDirection(FILE** pData)
{
    while (!feof(*pData))
    {
        const char cDirection = fgetc(*pData);
        if (cDirection == 'L')
        {
            return -1;
        }
        else if (cDirection == 'R')
        {
            return 1;
        }
    }

    return 0;
}

int64_t getMagnitude(FILE** pData)
{
    int64_t nMagnitude = 0;

    while (!feof(*pData))
    {
        const char cDigit = fgetc(*pData);
        if (cDigit >= '0' && cDigit <= '9')
        {
            nMagnitude = (nMagnitude * 10) + (cDigit - '0');
        }
        else
        {
            break;
        }
    }

    return nMagnitude;
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
            int64_t nPrevValue = nValue;

            /* Offset/Direction Calculations. */
            const int64_t nDirection = getDirection(&pData);
            if (feof(pData)) break;
            const int64_t nOffset    = (nDirection == 1) ? 0 : -1;

            const int64_t nMagnitude = getMagnitude(&pData);

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