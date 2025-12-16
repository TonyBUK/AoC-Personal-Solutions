#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define AOC_TRUE  (1)
#define AOC_FALSE (0)

typedef struct Button_Type
{
    int64_t   nButtonMask;
    uint16_t* kButton;
    size_t    nButtonCount;
} Button_Type;

typedef struct Machine_Type
{
    int64_t      nLight;

    size_t       nButtonsCount;
    Button_Type* kButtons;

    size_t       nJoltageCount;
    uint16_t*    kJoltage;

    size_t       nButtonCombinations;
    uint16_t*    kButtonCombinations;
    size_t*      kButtonCombinationCosts;
} Machine_Type;

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

int64_t parseLight(const char* kLight)
{
    int64_t nLight = 0;
    int64_t i;

    for (i = 0; i < strlen(kLight); ++i)
    {
        nLight <<= 1;
        if (kLight[i] == '#')
        {
            nLight |= 1;
        }
    }

    return nLight;
}

size_t parseDigits(const char* kInput, const char kStart, const char* kTerminator, uint16_t** kValues)
{
    const size_t nInputLength = strstr(kInput, kTerminator) - kInput;
    size_t       nSize = 0;
    size_t       i;
    size_t       j = 0;

    for (i = 0; i < nInputLength; ++i)
    {
        if ((kInput[i] == kStart) || (kInput[i] == ','))
        {
            ++nSize;
        }
    }

    *kValues = (uint16_t*)malloc(nSize * sizeof(uint16_t));

    for (i = 0; i < nInputLength; ++i)
    {
        if ((kInput[i] == kStart) || (kInput[i] == ','))
        {
            const int nValue = strtol(&kInput[i+1], NULL, 10);
            assert(nValue >= 0x0000);
            assert(nValue <= 0xFFFFu);
            (*kValues)[j++] = (uint16_t)nValue;
        }
    }

    return nSize;
}

void parseButtons(const char* kButtonsInput, const int nLightLength, Button_Type* kButtons)
{
    const size_t nButtonInputLength = strstr(kButtonsInput, ")") - kButtonsInput;
    size_t       nButtonCount       = 0;
    int64_t      nButtonMask        = 0;
    size_t       i;

    kButtons->nButtonCount = parseDigits(kButtonsInput, '(', ")", &kButtons->kButton);

    for (i = 0; i < kButtons->nButtonCount; ++i)
    {
        nButtonMask          |= (int64_t)1 << ((int64_t)nLightLength - (int64_t)kButtons->kButton[i] - (int64_t)1);
    }
    kButtons->nButtonMask = nButtonMask;
}

int64_t findFewestPressesPartOne(const Machine_Type* kMachine, const int64_t nLightState, const size_t nDepth, int64_t nBest, const size_t nStartIndex) //!kButtons : list[int], nLightState : int = 0, kLighstateCache : dict = None, nDepth : int = 0, nBest : int = math.inf)
{
    size_t i;
 
    if (nDepth >= nBest)
    {
        return nBest;
    }

    if (nLightState == kMachine->nLight)
    {
        return nDepth;
    }

    for (i = nStartIndex; i < kMachine->nButtonsCount; ++i)
    {
        nBest = findFewestPressesPartOne(kMachine, nLightState^kMachine->kButtons[i].nButtonMask, nDepth+1, nBest, i + 1);
    }

    return nBest;
}

void updateButtonCombination(uint16_t* kButtonCombination, const size_t nJoltageCount, const Button_Type* kButton)
{
    size_t i;
    
    for (i = 0; i < kButton->nButtonCount; ++i)
    {
        const size_t nJoltageIndex = kButton->kButton[i]; //!nJoltageCount -  - 1;
        ++kButtonCombination[nJoltageIndex];
    }
}

size_t getDuplicate(const uint16_t* kButtonCombinations, const uint16_t* kButtonCombination, const size_t nJoltageCount)
{
    const uint16_t* p = kButtonCombinations;
    size_t          n = 0;

    while (p != kButtonCombination)
    {
        size_t i;
        int    bAllMatch = AOC_TRUE;
        for (i = 0; i < nJoltageCount; ++i)
        {
            if (p[i] != kButtonCombination[i])
            {
                bAllMatch = AOC_FALSE;
                break;
            }
        }

        if (bAllMatch)
        {
            return n;
        }

        p += nJoltageCount;
        ++n;
    }

    return (size_t)-1;
}

void getButtonPermutationsAndCost(size_t* pCurrentCombination, uint16_t* kButtonCombinations, size_t* kButtonCombinationCosts, const size_t nMaxButonCombinations, const size_t nJoltageCount, const Button_Type* kButtons, const size_t nButtonsCount, const size_t nStartButtons, const uint16_t* kCurrentButtonCombinations, const size_t nCost)
{
    size_t i;

    assert(*pCurrentCombination < nMaxButonCombinations);

    if (NULL == kCurrentButtonCombinations)
    {
        const size_t nCurrentCombination      = *pCurrentCombination;
        const size_t nCurrentCombinationIndex = nCurrentCombination * nJoltageCount;
        memset(&kButtonCombinations[nCurrentCombinationIndex], 0, sizeof(uint16_t) * nJoltageCount);

        kCurrentButtonCombinations = &kButtonCombinations[nCurrentCombinationIndex];
        kButtonCombinationCosts[nCurrentCombination] = 0;

        *pCurrentCombination += 1;
    }

    for (i = nStartButtons; i < nButtonsCount; ++i)
    {
        const size_t nCurrentCombination      = *pCurrentCombination;
        const size_t nCurrentCombinationIndex = nCurrentCombination * nJoltageCount;
              size_t nNextCombinationIndex    = nCurrentCombinationIndex + nJoltageCount;
              size_t nDuplicateIndex;

        memcpy(&kButtonCombinations[nCurrentCombinationIndex], kCurrentButtonCombinations, nJoltageCount * sizeof(uint16_t));
        updateButtonCombination(&kButtonCombinations[nCurrentCombinationIndex], nJoltageCount, &kButtons[i]);

        /* Detect if this is a Duplicate */
        nDuplicateIndex = getDuplicate(kButtonCombinations, &kButtonCombinations[nCurrentCombinationIndex], nJoltageCount);

        /* If not, it's a unique item, add it */
        if ((size_t)-1 == nDuplicateIndex)
        {
            kButtonCombinationCosts[nCurrentCombination] = nCost;
            *pCurrentCombination += 1;
            getButtonPermutationsAndCost(pCurrentCombination, kButtonCombinations, kButtonCombinationCosts, nMaxButonCombinations, nJoltageCount, kButtons, nButtonsCount, i+1, &kButtonCombinations[nCurrentCombinationIndex], nCost + 1);
        }
        /* Even if it is a duplicate, we may have gotten to it in a unique fashion so continue parsing */
        else
        {
            if (nCost < kButtonCombinationCosts[nDuplicateIndex])
            {
                kButtonCombinationCosts[nDuplicateIndex] = nCost;
            }
            getButtonPermutationsAndCost(pCurrentCombination, kButtonCombinations, kButtonCombinationCosts, nMaxButonCombinations, nJoltageCount, kButtons, nButtonsCount, i+1, &kButtonCombinations[nDuplicateIndex*nJoltageCount], nCost + 1);
        }
    }
}

int reachedGoal(const uint16_t* kTargetJoltage, const size_t nJoltageCount)
{
    size_t i;

    for (i = 0; i < nJoltageCount; ++i)
    {
        if (kTargetJoltage[i])
        {
            return AOC_FALSE;
        }
    }

    return AOC_TRUE;
}

/* Adapted from here: https://www.reddit.com/r/adventofcode/comments/1pk87hl/2025_day_10_part_2_bifurcate_your_way_to_victory/ */
uint64_t findFewestPressesPartTwo(const uint16_t* kTargetJoltage, const uint16_t* kButtonCombinations, const size_t* kButtonCombinationCosts, const size_t nMaxButonCombinations, const size_t nJoltageCount, uint16_t* kJoltageTemp)
{
    /* Choose some very large value that won't overflow */
    uint64_t  nLocalMin = (uint32_t)-1;
    size_t    i;

    if (reachedGoal(kTargetJoltage, nJoltageCount))
    {
        return 0;
    }

    /* Decimate by the calculated Joltage Cost for the current Joltage Increment Permutation/Button Cost */
    for (i = 0; i < nMaxButonCombinations; ++i)
    {
        const uint16_t* kJoltageIncrement = &kButtonCombinations[i * nJoltageCount];
        const size_t    nButtonCost       = kButtonCombinationCosts[i];

        int             bValid            = AOC_TRUE;
        size_t          j;

        /*
         * To simplify the algorthm we only accept a value if two conditions are met:
         *
         * 1. All of the values of the current Joltage Increment that are Odd are also Odd for the same position in the Goal
         * 2. All of the values are within range
         *
         * Why do this? It means when we decimate our Joltage Goal by the Increment, we'll be subtracting Odds from Odds and
         * Evens from Evens... which means the goal is now exactly divisible by 2. This means we can set a new goal of half
         * the current one and just double the number of key presses we made this cycle when continuing the search.
         *
         * This *DRASTICALLY* lowers the Search Space
         */
        for (j = 0; j < nJoltageCount; ++j)
        {
            const uint16_t nJoltageFromButton = kJoltageIncrement[j];
            const uint16_t nTargetJoltage     = kTargetJoltage[j];

            if (nJoltageFromButton > nTargetJoltage)
            {
                bValid = AOC_FALSE;
                break;
            }
            else if ((nJoltageFromButton % 2) != (nTargetJoltage % 2))
            {
                bValid = AOC_FALSE;
                break;
            }

            kJoltageTemp[j] = (nTargetJoltage - nJoltageFromButton) / 2;
        }

        if (bValid)
        {
            const uint64_t nNewLocalMin = nButtonCost + 2 * findFewestPressesPartTwo(kJoltageTemp, kButtonCombinations, kButtonCombinationCosts, nMaxButonCombinations, nJoltageCount, &kJoltageTemp[nJoltageCount]);

            if (nNewLocalMin < nLocalMin)
            {
                nLocalMin = nNewLocalMin;
            }
        }
    }

    return nLocalMin;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        char*                       kInputBuffer;
        char**                      kInputLines;

        size_t                      nMachineCount;

        size_t                      i;

        int64_t                     nButtonPressCountPartOne = 0;
        int64_t                     nButtonPressCountPartTwo = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kInputBuffer, &kInputLines, &nMachineCount, NULL, NULL, 0);
        fclose(pData);

        for (i = 0; i < nMachineCount; ++i)
        {
            Machine_Type kMachine;

            char*        pCurr    = kInputLines[i] + 1;
            char*        pButtons = strstr(pCurr, " ");
            char*        pJoltage;
            size_t       nButtonCount = 0;
            size_t       nMaxButonCombinations;
            size_t       j;
            uint16_t*    kJoltageTemp;

            pButtons[-1] = '\0';

            /* Convert the Light Sequence into an Integer Value by treating the on/off states as binary */
            kMachine.nLight = parseLight(pCurr);

            /* Count the Buttons / Find the Joltage */
            pJoltage = pButtons + 1;
            while (pJoltage[1] != '{')
            {
                ++pJoltage;
                ++nButtonCount;
                pJoltage = strstr(pJoltage, " ");
            }
            ++pJoltage;
            ++pButtons;

            /* Decode the Joltage First as we need the Light Length */
            kMachine.nJoltageCount = parseDigits(pJoltage, '{', "}", &kMachine.kJoltage);

            /* Allocate the Buttons */
            kMachine.nButtonsCount = nButtonCount;
            kMachine.kButtons      = (Button_Type*)malloc(nButtonCount * sizeof(Button_Type));

            /* Parse the Buttons */
            for (j = 0; j < nButtonCount; ++j)
            {
                parseButtons(pButtons, kMachine.nJoltageCount, &kMachine.kButtons[j]);
                pButtons = strstr(pButtons, " ") + 1;
            }

            /* For Part Two we need to know the Permutations */
            nMaxButonCombinations            = (1 << kMachine.nButtonsCount) + 1;
            kMachine.kButtonCombinations     = (uint16_t*)malloc(nMaxButonCombinations * kMachine.nJoltageCount * sizeof(uint16_t));
            kJoltageTemp                     = (uint16_t*)malloc(nMaxButonCombinations * kMachine.nJoltageCount * sizeof(uint16_t));
            kMachine.kButtonCombinationCosts = (size_t*)  malloc(nMaxButonCombinations * sizeof(size_t));

            /* Solve Part One */
            nButtonPressCountPartOne += findFewestPressesPartOne(&kMachine, 0, 0, INT64_MAX, 0);

            /* Solve Part Two */

            /* First Calculate all of the Button Combinations / Associated Cost */
            j = 0;
            getButtonPermutationsAndCost(&j, kMachine.kButtonCombinations, kMachine.kButtonCombinationCosts, nMaxButonCombinations, kMachine.nJoltageCount, kMachine.kButtons, kMachine.nButtonsCount, 0, NULL, 1);

            nButtonPressCountPartTwo += findFewestPressesPartTwo(kMachine.kJoltage, kMachine.kButtonCombinations, kMachine.kButtonCombinationCosts, j, kMachine.nJoltageCount, kJoltageTemp);

            /* Cleanup */
            free(kMachine.kJoltage);
            for (j = 0; j < nButtonCount; ++j)
            {
                free(kMachine.kButtons[j].kButton);
            }
            free(kMachine.kButtons);
            free(kMachine.kButtonCombinations);
            free(kMachine.kButtonCombinationCosts);
            free(kJoltageTemp);
        }

        printf("Part 1: %lld\n", nButtonPressCountPartOne);
        printf("Part 2: %lld\n", nButtonPressCountPartTwo);

        /* Cleanup */
        free(kInputLines);
        free(kInputBuffer);
    }
 
    return 0;
}