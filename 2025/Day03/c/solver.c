#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int64_t getMaxJolt(const char* const kJolts, const ssize_t nTotalBatteryCount, const ssize_t nNumBatteries)
{
    ssize_t nHead           = 0;
    ssize_t nTail           = nTotalBatteryCount - nNumBatteries;
    int64_t nJolt           = 0;
    char    cMaxJolt;

    while (nTail < nTotalBatteryCount)
    {
        /* Find the Max Digit between the Head and Tail */
        cMaxJolt = 0;
        for (ssize_t i = nHead; i <= nTail; ++i)
        {
            assert((kJolts[i] >= '0') && (kJolts[i] <= '9'));

            if (kJolts[i] > cMaxJolt)
            {
                cMaxJolt = kJolts[i];
                nHead    = i;
            }
        }

        /* Update the Jolts Total */
        nJolt = (nJolt * 10ll) + (int64_t)(cMaxJolt - '0');

        /* Move the Head/Tail Onwards */
        ++nHead;
        ++nTail;
    }

    return nJolt;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        int64_t     nJoltPartOne    = 0;
        int64_t     nJoltPartTwo    = 0;
        char*       kLine           = NULL;
        size_t      nLineBufferSize = 0;
        ssize_t     nLineLength; 

        while ((nLineLength = getline(&kLine, &nLineBufferSize, pData)) != -1)
        {
            /* Trim New Lines */
            if ((kLine[nLineLength - 1] < '0') || (kLine[nLineLength - 1] > '9'))
            {
                --nLineLength;
            }

            nJoltPartOne += getMaxJolt(kLine, nLineLength, 2);
            nJoltPartTwo += getMaxJolt(kLine, nLineLength, 12);
        }

        free(kLine);
        fclose(pData);

        printf("Part 1: %lld\n", nJoltPartOne);
        printf("Part 2: %lld\n", nJoltPartTwo);
    }
 
    return 0;
}