import time
import sys
 
def main() :

    def getMaxJolt(kJolts : list[int], nNumBatteries : int) -> int :

        nJolt = 0
        nPos  = 0

        # All this does is search for the highest jolt value in the list
        # being careful to leave enough batteries unsearches to satisfy
        # the required number of batteries.

        # So if we had 5 batteries and we needed to find 3.
        #
        # ABCDE
        #
        # Battery 1 - Search from A to C (leaving 2 batteries unsearched)
        # Battery 2 - Search from ? to D (leaving 1 battery unsearched
        # Battery 3 - Search from ? to E (leaving 0 batteries unsearched)
        #
        # But... since this could re-use batteries, so we need to shift the search
        # start position each time too.
        #
        # So for in the example above, if battery 1 found B, battery 2 would
        # need to search from C to D only.
        #
        # And if battery 2 found D, battery 3 would only be able to search E.

        for i in reversed(range(nNumBatteries)) :

            nJolt          *= 10
            nCurrentBattery = max(kJolts[nPos:-i if i != 0 else None])
            nPos            = kJolts.index(nCurrentBattery, nPos) + 1
            nJolt          += nCurrentBattery

        #end

        return nJolt

    with open("../input.txt", "r") as inputFile:

        kBanks = [[int(k) for k in l.strip()] for l in inputFile.readlines()]
        nJoltsPartOne = 0
        nJoltsPartTwo = 0

        for kJolts in kBanks:

            # Calculate the Max Jolts
            nJoltsPartOne += getMaxJolt(kJolts, 2)
            nJoltsPartTwo += getMaxJolt(kJolts, 12)

        #end

        print(f"Part 1: {nJoltsPartOne}")
        print(f"Part 2: {nJoltsPartTwo}")

    #end

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end