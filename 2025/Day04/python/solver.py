import time
import sys
 
def main() :

    with open("../input.txt", "r") as inputFile:

        # All 8 directions to traverse
        DELTAS = [(-1, -1), (-1, 0), (-1, 1),
                  ( 0, -1),          ( 0, 1),
                  ( 1, -1), ( 1, 0), ( 1, 1)]

        # Store the Rolls in a set for simplified lookups
        # "@" indicates a roll is present
        #
        # Note: Whilst a grid is more intuitive, I would
        #       either have to pad it to avoid boundary
        #       checks or add boundary checks to each lookup.
        #       Using a set avoids both issues.
        kRolls = {(nRow, nCol)
                  for nRow, kRow in enumerate(inputFile)
                  for nCol, kCell in enumerate(kRow.strip())
                  if kCell == "@"}

        nAccessibleRollsPartOne = 0
        nAccessibleRollsPartTwo = 0

        bFirstPass              = True
        while True :

            # Find all the rolls that are accessible this pass
            kAccessibleRollsPartTwo = set()
            for kRoll in kRolls :

                # Count the number of connected rolls
                #
                # Note: If 4 or more are found, we can stop,
                #       as it won't get any more accessible
                #       this pass.
                nConnectedRolls = 0
                for kDelta in DELTAS :
                    nRow = kRoll[0] + kDelta[0]
                    nCol = kRoll[1] + kDelta[1]
                    if (nRow, nCol) in kRolls:
                        nConnectedRolls += 1
                        if nConnectedRolls >= 4 :
                            break
                        #end
                    #end
                #end

                # If less than 4 connected rolls, it's accessible
                if nConnectedRolls < 4 :
                    kAccessibleRollsPartTwo.add(kRoll)
                #end

            #end

            # If no more accessible rolls, we're done
            if len(kAccessibleRollsPartTwo) == 0 :
                break
            #end

            # Update Part Two
            nAccessibleRollsPartTwo += len(kAccessibleRollsPartTwo)

            # Handle Part One, accessible rolls before we start
            # removing
            if bFirstPass :
                nAccessibleRollsPartOne = nAccessibleRollsPartTwo
                bFirstPass = False
            #end

            # Remove the accessible rolls from the set
            kRolls -= kAccessibleRollsPartTwo

        #end

        print(f"Part 1: {nAccessibleRollsPartOne}")
        print(f"Part 2: {nAccessibleRollsPartTwo}")

    #end

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end