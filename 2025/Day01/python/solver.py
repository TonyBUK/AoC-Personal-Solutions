import time
import sys
 
def main() :
 
    with open("../input.txt", "r") as inputFile:

        OFFSET_AND_DIRECTION = {"R": (0, 1), "L": (-1, -1)}

        # Starting States - Dial is 50
        nValue        = 50
        nTimesPartOne = 0
        nTimesPartTwo = 0

        for kLine in inputFile:

            # Offset/Direction Calculations.
            #
            # Direction is for both parts: +1 for R, -1 for L
            # Offset is only for part 2
            nOffset, nDirection = OFFSET_AND_DIRECTION[kLine[0]]

            # Buffer the Previous State for Part 2
            nPrevValue = nValue

            # Calculate the new Dial Position
            #
            # Note: For Part 2, this is simplified if we don't actually constrain the dial
            #       in the range 0 .. 99, but instead let it go negative or above 99.
            nValue += nDirection * int(kLine[1:])

            # Part 1: If number is divisible by 100, then the dial is at 0.
            nTimesPartOne += 1 if (nValue % 100) == 0 else 0

            # Part 2: Count the number of times we cross a multiple of 100
            #
            #         The thing we need to be super careful about here is that we never
            #         use the starting point of the dial as a crossing, as this will
            #         be counted twice every time we land exactly on a multiple of 100.
            #
            #         What this tries to do is reduce where we are by a factor of 100.
            #
            #         i.e. if previously we were at position 50, and we're not at 200
            #
            #         Then we can reduce this to 0 and 2 respectively.  The difference of
            #         these two values is how many times we've crossed a multiple of 100.
            #
            #         Negative is where this gets interesting as increasing values move us
            #         to the next multiple of 100, meaning the reduction works as expected.
            #         But decreasing values only move us to the previous multiple of 100
            #         when we pass it, not when we land on it.
            #
            #         Consider 250 to 200.  These would reduce to 2 and 2 respectively, which
            #         would indicate no crossing occured.  So instead we'll reduce both values
            #         by 1 before the division, meaning we're now reducing 249 and 199
            #         respectively, being 2 and 1, indicating a crossing did occur.
            #
            #         What about 300 to 200, wouldn't this be broken as well?  No, because
            #         300 is where we started, we already don't count that as a crossing,
            #         meaning the reduction by 1 helps us here as well.
            nTimesPartTwo += nDirection * (((nValue + nOffset) // 100) - ((nPrevValue + nOffset) // 100))

        #end

    #end

    print(f"Part 1: {nTimesPartOne}")
    print(f"Part 2: {nTimesPartTwo}")

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end