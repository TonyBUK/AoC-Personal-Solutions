#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int getRange(FILE* pData, int64_t* pMin, int64_t* pMax, int64_t* pMinLength, int64_t* pMaxLength)
{
    /* Read the Magnitude */
    int64_t nValue  = 0;
    int64_t nLength = 0;
    int64_t nStage  = 0;

    while (1)
    {
        const char cDigit = fgetc(pData);
        if (cDigit >= '0' && cDigit <= '9')
        {
            nValue = (nValue * 10) + (cDigit - '0');
            ++nLength;
        }
        else if ((EOF == cDigit) && (0 == nStage))
        {
            return 0;
        }
        else
        {
            switch (nStage)
            {
                case 0:
                {
                    *pMin       = nValue;
                    *pMinLength = nLength;
                    nValue      = 0;
                    nLength     = 0;
                } break;

                case 1:
                {
                    *pMax       = nValue;
                    *pMaxLength = nLength;
                    return 1;
                }

                default:
                {
                    assert(0);
                }
            }

            ++nStage;
        }
    }
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        int64_t     nSumPartOne = 0;
        int64_t     nSumPartTwo = 0;

        int64_t*    kSeen;
        int64_t     nSeenSize   = 256;
        int64_t     nSeenCount;

        kSeen = (int64_t*)malloc(sizeof(int64_t) * nSeenSize);

        while(1)
        {
            /* Offset/Direction Calculations. */
            int64_t nMin, nMax, nMinLength, nMaxLength;
            if (!getRange(pData, &nMin, &nMax, &nMinLength, &nMaxLength))
            {
                break;
            }

            int64_t nLength;

            for (nLength = nMinLength; nLength <= nMaxLength; ++nLength)
            {
                int64_t nSliceCount;

                /* Part Two requirement to track duplicates */
                nSeenCount = 0;

                for (nSliceCount = 2; nSliceCount <= nLength; ++nSliceCount)
                {
                    int64_t i;

                    if (nLength % nSliceCount != 0)
                    {
                        continue;
                    }

                    /* Determine the Divisor for this Length/Slice Count */
                    int64_t nDivisor = 10;
                    for (i = 1; i < (nLength / nSliceCount); ++i)
                    {
                        nDivisor *= 10;
                    }

                    /* Take the First Slice */
                    int64_t nFirstSlice;
                    if (nLength == nMinLength)
                    {
                        /* For Min, we want the first group of digits */
                        nFirstSlice = nMin;
                        for (i = 0; i < (nSliceCount - 1); ++i)
                        {
                            nFirstSlice /= nDivisor;
                        }
                    }
                    else
                    {
                        /* Anything else, we can basically cheat here and just start at the lowest possible slice */
                        nFirstSlice = nDivisor / 10;
                    }

                    /* Iterate through all possible invalid IDs for this Length/Slice Count */
                    while (1)
                    {
                        /* Repeat the digit(s) to form the Invalid ID */
                        int64_t nInvalidID = nFirstSlice;
                        for (i = 1; i < nSliceCount; ++i)
                        {
                            nInvalidID = (nInvalidID * nDivisor) + nFirstSlice;
                        }

                        /* Stop if we exceed the Max of the Range */
                        if (nInvalidID > nMax)
                        {
                            break;
                        }

                        /* Record if within Range */
                        if (nInvalidID >= nMin && nInvalidID <= nMax)
                        {
                            /* Avoid Duplicates for Part Two */
                            int bFound = 0;

                            for (i = 0; i < nSeenCount; ++i)
                            {
                                if (kSeen[i] == nInvalidID)
                                {
                                    bFound = 1;
                                    break;
                                }
                            }

                            if (!bFound)
                            {
                                /* Just in-case... */
                                if (nSeenCount >= nSeenSize)
                                {
                                    nSeenSize *= 2;
                                    kSeen = (int64_t*)realloc(kSeen, sizeof(int64_t) * nSeenSize);
                                }
                                kSeen[nSeenCount++] = nInvalidID;

                                /* Part One only cares about 2 slices */
                                if (nSliceCount == 2)
                                {
                                    nSumPartOne += nInvalidID;
                                }

                                /* Part Two records all */
                                nSumPartTwo += nInvalidID;
                            }
                        }

                        /* Increment the first slice */
                        ++nFirstSlice;

                        /* Stop if we exceed the possible slices */
                        if (nFirstSlice >= nDivisor)
                        {
                            break;
                        }
                    }
                }
            }
        }

        free(kSeen);

        fclose(pData);

        printf("Part 1: %lld\n", nSumPartOne);
        printf("Part 2: %lld\n", nSumPartTwo);
    }
 
    return 0;
}