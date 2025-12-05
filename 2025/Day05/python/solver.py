import time
import sys
 
def main() :

    with open("../input.txt", "r") as inputFile:

        KRaw                  = inputFile.read().split("\n\n")
        kFreshRanges          = [[int(f) for f in k.split("-")] for k in KRaw[0].split("\n")]
        kIngredients          = [int(k) for k in KRaw[1].split("\n")]

        # Sort the Ranges, this puts the mins in order, which greatly simplifies the merging process
        kFreshRanges.sort()

        # Forcibly add the First Element
        kMerged = [kFreshRanges[0]]

        for kRange in kFreshRanges[1:]:

            # If the min of the current range exceeds the bounds
            # of the last merged range
            if kMerged[-1][1] < kRange[0]:

                # It's not an overlap
                kMerged.append(kRange)

            else:

                # It's an overlap, merge the ranges
                kMerged[-1][1] = max(kMerged[-1][1], kRange[1])

            #end

        #end

        # Update the Fresh Ranges with the Merged List
        kFreshRanges = kMerged

        # Part One - Count Fresh Ingredients
        nNumFreshIngredientsPartOne = sum(any(nIngredient in range(kFreshRange[0], kFreshRange[1] + 1) for kFreshRange in kFreshRanges)
                                              for nIngredient in kIngredients)

        # Part Two - Count all Possible Fresh Ingredients
        nTotalFreshIntegredentsPartTwo = sum(kFreshRange[1] - kFreshRange[0] + 1 for kFreshRange in kFreshRanges)

        print(f"Part 1: {nNumFreshIngredientsPartOne}")
        print(f"Part 2: {nTotalFreshIntegredentsPartTwo}")

    #end

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end