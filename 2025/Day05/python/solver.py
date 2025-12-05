import time
import sys
 
def main() :

    with open("../input.txt", "r") as inputFile:

        KRaw                  = inputFile.read().split("\n\n")
        kFreshRangesUnmerged  = [[int(f) for f in k.split("-")] for k in KRaw[0].split("\n")]
        kIngredients          = [int(k) for k in KRaw[1].split("\n")]

        # Sort the Ranges, this puts the mins in order, which greatly simplifies the merging process
        kFreshRangesUnmerged.sort()

        # Note: For the following code we can abuse an interesting/annoying property of the range
        #       class.

        # Forcibly add the First Element
        kFreshRanges = [range(kFreshRangesUnmerged[0][0], kFreshRangesUnmerged[0][1] + 1)]

        for kRange in kFreshRangesUnmerged[1:]:

            # If the min of the current range exceeds the bounds
            # of the last merged range
            #
            # Note: We subtract 1 as we want to merge the hypothetical ranges:
            #       1..4
            #       5..8
            #
            # Into:
            #       1..8
            #
            # Hence we would compare 4 to (5-1)
            #
            # Because we're using an abused range however, .stop is already made to be 1 higher
            # for the "in" operator to work as intended, meaning we effectively naturally
            # re-express this as comparing (4+1) to 5
            if kFreshRanges[-1].stop < kRange[0]:

                # It's not an overlap, create a new range
                kFreshRanges.append(range(kRange[0], kRange[1] + 1))

            else:

                # It's an overlap, merge the ranges
                #
                # Note: The existing stop will already have been pushed on by 1.
                kFreshRanges[-1] = range(kFreshRanges[-1].start, max(kFreshRanges[-1].stop, kRange[1] + 1))

            #end

        #end

        # Part One - Count Fresh Ingredients
        #
        # Note: The ranges already incorporate pushing the stop on by 1 so that the limits are
        #       inclusive.
        nNumFreshIngredientsPartOne = sum(any(nIngredient in kFreshRange for kFreshRange in kFreshRanges)
                                              for nIngredient in kIngredients)

        # Part Two - Count all Possible Fresh Ingredients
        #
        # Note: The ranges already incorporate pushing the stop on by 1 so that the range is
        #       correct.
        nTotalFreshIntegredentsPartTwo = sum(kFreshRange.stop - kFreshRange.start for kFreshRange in kFreshRanges)

        print(f"Part 1: {nNumFreshIngredientsPartOne}")
        print(f"Part 2: {nTotalFreshIntegredentsPartTwo}")

    #end

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end