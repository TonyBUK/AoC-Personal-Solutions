import time
import sys
import math
 
def main() :
 
    with open("../input.txt", "r") as inputFile:

        kRanges     = [[int(k) for k in kRange.split("-")] for kRange in inputFile.read().strip().split(",")]
        nSumPartOne = 0
        nSumPartTwo = 0

        for kRange in kRanges:

            # The basic algorithm here is rather than testing each number for repeatitions,
            # we'll instead generate all the possible Invalid IDs that numbers in that range
            # could have.
            #
            # The idea is that if the range is 11-31
            #
            # We'll do the following:
            #
            # 1. Take the Starting Number of the Range (11)
            # 2. Determine the Length of that Number (2)
            # 3. Decimate this down to the first possible slice (1)
            # 4. Use that as a means of iterating, so we'll generate 11, 22, 33, etc
            # 5. Stop when we exceed the Max of the Range (31) OR
            #    When we exceed the possible slices (in this case 9) which we can compare
            #    against the divisor the length would require (10 for length 2)
            #
            # Where this gets tricky is when the range has multiple lengths, e.g., 95-105
            #
            # In this case we'll iterate through the lengths, 95 will be handled as above.
            # 100 we can cheat and just use the divisor/10 as the starting point.
            #
            # The one remaining gotcha now is that we can have repetitions for Part Two.
            # Consider 2222, this is both 22,22 and 2,2,2,2, so we can simply record invalid
            # ids for a given length in a set to discard duplicates.

            # Determine Length Bounds
            nLenMin = math.floor(math.log10(kRange[0])) + 1
            nLenMax = math.floor(math.log10(kRange[1])) + 1

            # Iterate through the possible lengths
            for nLen in range(nLenMin, nLenMax + 1):

                # Part Two requirement to track duplicates
                kInvaldIds = set()

                # Iterate through possible slice counts, starting at 2
                # to ensure Part One gets priority
                for nSliceCount in range(2, nLen + 1):

                    # Ignore if not divisible
                    if (nLen % nSliceCount) != 0:
                        continue
                    #end

                    # Determine the Divisor for this Length/Slice Count
                    nDivisor = 10 ** (nLen // nSliceCount)

                    # Take the First Slice
                    if nLen == nLenMin:
                        # For Min, we want the first group of digits
                        nFirstSlice = kRange[0]
                        for _ in range(nSliceCount - 1):
                            nFirstSlice //= nDivisor
                        #end
                    else:
                        # Anything else, we can basically cheat here and just start at the lowest possible slice
                        nFirstSlice = nDivisor // 10
                    #end

                    # Iterate through all possible invalid IDs for this Length/Slice Count
                    while True :

                        # Repeat the digit(s) to form the Invalid ID
                        nInvalidID = nFirstSlice
                        for _ in range(nSliceCount - 1):
                            nInvalidID = (nInvalidID * nDivisor) + nFirstSlice
                        #end

                        # Stop if we exceed the Max of the Range
                        if nInvalidID > kRange[1]:
                            break
                        #end

                        # Record if within Range
                        if nInvalidID in range(kRange[0], kRange[1] + 1):

                            # Avoid Duplicates for Part Two
                            if not nInvalidID in kInvaldIds:

                                kInvaldIds.add(nInvalidID)

                                # Part One only cares about 2 slices
                                if nSliceCount == 2:
                                    nSumPartOne += nInvalidID
                                #end

                                # Part Two records all
                                nSumPartTwo += nInvalidID

                            #end

                        #end

                        # Increment the first slice
                        nFirstSlice += 1

                        # Stop if we exceed the possible slices
                        if nFirstSlice >= nDivisor:
                            break
                        #end
                        
                    #end

                #end

            #end

        #end

        print(f"Part 1: {nSumPartOne}")
        print(f"Part 2: {nSumPartTwo}")

    #end

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end