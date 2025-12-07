import time
import sys
 
def main() :

    with open("../input.txt", "r") as inputFile:

        kGrid                   = [kLine.strip() for kLine in inputFile.readlines()]
        kTachyonBeamsInTimeLine = {}
        nSplitCount             = 0

        # Note: Because this solution will effectively always move all beams 1 row down at the
        #       same time, we never need to store the rows per beam point, only the columns.

        # Find our Start Point
        for y, kRow in enumerate(kGrid) :
            if "S" in kRow :
                nPosition = kRow.index("S")
                kTachyonBeamsInTimeLine[nPosition] = 1
                nRow = y+1
                break
            #end
        #end

        # Move the Tachyon Beams
        #
        # The basic premise is we never want to actually duplicate the Tachyon Beams to anything
        # other than unique points, for which we can use a set... or more precisely, the keys of
        # a dictionary to unify part two.
        #
        # Using a list would get us the timeline expansion for part two, but it would mean repeating
        # the process far more times than required, problem there is you'll eventually be processing
        # more than > 10,000,000,000,000 on the real puzzle input which is unsustainable.
        #
        # Instead we can just keep a tally on each Tachyon Beam as to how many ways it reached its
        # point.
        while True :

            # Next Round of Movements...
            kNextTachyonBeamsInTimeLine = {}

            # For each unique Tachyon Beam
            for nTachyonBeam, nTimeLines in kTachyonBeamsInTimeLine.items() :

                # Is it going to move freely?
                if kGrid[nRow][nTachyonBeam] == "." :

                    # Update the Number of Timelines
                    #
                    # Note: We may end up needing to incorporate other timelines as well
                    kNextTachyonBeamsInTimeLine[nTachyonBeam] = kNextTachyonBeamsInTimeLine.get(nTachyonBeam, 0) + nTimeLines

                else :

                    # Update Split Count
                    nSplitCount += 1

                    # Update the Positions
                    nLeft  = nTachyonBeam - 1
                    nRight = nTachyonBeam + 1

                    # Update the Number of Timelines
                    #
                    # Note: We may end up needing to incorporate other timelines as well
                    kNextTachyonBeamsInTimeLine[nLeft]  = kNextTachyonBeamsInTimeLine.get(nLeft,  0) + nTimeLines
                    kNextTachyonBeamsInTimeLine[nRight] = kNextTachyonBeamsInTimeLine.get(nRight, 0) + nTimeLines

                    # Sanity check to make sure Eric isn't being evil!
                    assert(kGrid[nRow + 1][nLeft]  != "^")
                    assert(kGrid[nRow + 1][nRight] != "^")

                #end

            #end

            # Refresh the list of Tachyon Beams/Timelines
            kTachyonBeamsInTimeLine = kNextTachyonBeamsInTimeLine

            # Check whether we're done
            nRow += 1
            if nRow >= len(kGrid) :
                break
            #end

        #end

        print(f"Part 1: {nSplitCount}")
        print(f"Part 2: {sum(kTachyonBeamsInTimeLine.values())}")

    #end

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end