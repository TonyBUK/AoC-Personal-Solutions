import sys
import time
import math

def main() :

    def findFewestPressesPartOne(nRequiredLightSequence : int, kButtons : list[int], nLightState : int = 0, nStartIndex = 0, kLighstateCache : dict = None, nDepth : int = 0, nBest : int = math.inf) :

        # Initialise the Cache
        if kLighstateCache == None :
            kLighstateCache = {}
        #end

        # Terminate Processing if we're worse than the best solution
        if nDepth >= nBest :
            return nBest
        #end

        # Terminate Processing if we're worse than a prior path to get here
        if nDepth >= kLighstateCache.get(nLightState, math.inf) :
            return nBest
        #end

        # This now must be the best way to have gotten here
        kLighstateCache[nLightState] = nDepth

        # If this has achieved our goal
        if nLightState == nRequiredLightSequence :
            return nDepth
        #end

        # Note: At most we will only press each button once, as two presses will be equivalent to not
        #       pressing it, so we can skip any we've previously seen.  Whilst the cache would catch
        #       loops anyway, this saves around 2s on total execution time, so is a very significant
        #       optimisation.
        for i, nButton in enumerate(kButtons[nStartIndex:]) :

            # Toggle the Button (which is neatly just an XOR)
            nBest = findFewestPressesPartOne(nRequiredLightSequence, kButtons, nLightState^nButton, nStartIndex + i + 1, kLighstateCache, nDepth + 1, nBest)

        #end

        # Return the Lowest Seen Button Press Count
        return nBest

    #end

    def getButtonPermutationsAndCost(kButtonGroupsJoltageImpact, nDepth = 0, nStartIndex = 0, kCost = None, kPermutations = None) :

        if None == kCost :
            kCost = tuple([0 for _ in range(len(kButtonGroupsJoltageImpact[0]))])
        #end

        if None == kPermutations :
            kPermutations = {kCost : 0}
        #end

        for i, kButtonGroupJoltageImpact in enumerate(kButtonGroupsJoltageImpact[nStartIndex:]) :

            kNewCost = tuple([a+b for a,b in zip(kButtonGroupJoltageImpact, kCost)])

            kPermutations[kNewCost] = min(kPermutations.get(kNewCost, math.inf), nDepth + 1)

            if i + 1 < len(kButtonGroupsJoltageImpact) :
                getButtonPermutationsAndCost(kButtonGroupsJoltageImpact, nDepth+1, nStartIndex + i + 1, kNewCost, kPermutations)
            #end

        #end

        return kPermutations

    #end

    # Adapted from here: https://www.reddit.com/r/adventofcode/comments/1pk87hl/2025_day_10_part_2_bifurcate_your_way_to_victory/
    def findFewestPressesPartTwo(kGoal: tuple, kButtonPermutationsAndCosts : dict, kCache : dict) -> int:

        # Have we Seen this Before?
        if kGoal in kCache :
            return kCache[kGoal]
        #end

        # Have we Reached our Goal
        if max(kGoal) == 0:
            return 0
        #end

        nLocalMin = math.inf

        # Decimate by the calculated Joltage Cost for the current Joltage Increment Permutation/Button Cost
        for kJoltageIncrement, nButtonCost in kButtonPermutationsAndCosts.items():

            # To simplify the algorthm we only accept a value if two conditions are met:
            #
            # 1. All of the values of the current Joltage Increment that are Odd are also Odd for the same position in the Goal
            # 2. All of the values are within range
            #
            # Why do this? It means when we decimate our Joltage Goal by the Increment, we'll be subtracting Odds from Odds and
            # Evens from Evens... which means the goal is now exactly divisible by 2. This means we can set a new goal of half
            # the current one and just double the number of key presses we made this cycle when continuing the search.
            #
            # This *DRASTICALLY* lowers the Search Space
            if all(nJoltageFromButton <= nTargetJoltage and (nJoltageFromButton % 2) == (nTargetJoltage % 2) for nJoltageFromButton, nTargetJoltage in zip(kJoltageIncrement, kGoal)):
                nLocalMin = min(nLocalMin, nButtonCost + 2 * findFewestPressesPartTwo(tuple((nTargetJoltage - nJoltageFromButton) // 2 for nJoltageFromButton, nTargetJoltage in zip(kJoltageIncrement, kGoal)), kButtonPermutationsAndCosts, kCache))
            #end

        #end

        # Update the Cache with the Calculated Local Minimum
        kCache[kGoal] = nLocalMin

        return nLocalMin

    #end

    with open("../input.txt") as inputFile:

        LIGHTTOBIN = {'#' : 1, '.' : 0}

        kRaw              = inputFile.readlines()

        # Part One Data

        # Light Values are stored as a series of bitmasks rather than as strings
        kLights  = [
            sum(LIGHTTOBIN[ch] << (len(k.split(" ")[0][1:-1]) - idx - 1) 
                for idx, ch in enumerate(k.split(" ")[0][1:-1]))
            for k in kRaw
        ]

        # Buttons is Cached and used for Part Two
        kButtons = [[[int(c) for c in b[1:-1].split(",")] for b in k.split(" ")[1:-1]] for k in kRaw]

        # Buttons are also converted to the Bit Mask applicable to Light Values
        kButtonMasks = [
            [
                sum(1 << (len(k.split(" ")[0][1:-1]) - n - 1) for n in button_group)
                for button_group in kButtonGroup
            ]
            for kButtonGroup, k in zip(kButtons, kRaw)
        ]

        # Solve Part One
        print(f"Part 1: {sum(findFewestPressesPartOne(nLight, kButtonGroup) for nLight, kButtonGroup in zip(kLights, kButtonMasks))}")

        # Joltages
        kJoltages           = [[int(c) for c in k.split(" ")[-1].strip()[1:-1].split(",")] for k in kRaw]

        # Button Influence on Joltage
        kButtonsJoltageImpact = []
        for kButtonGroups, kJoltage in zip(kButtons, kJoltages) :

            kButtonGroupsJoltageImpact = []
            for kButtonGroup in kButtonGroups :
                kButtonGroupJoltageImpact = [0 for _ in range(len(kJoltage))]
                for nButton in kButtonGroup :
                    kButtonGroupJoltageImpact[nButton] += 1
                #end
                kButtonGroupsJoltageImpact.append(kButtonGroupJoltageImpact)
            #end
            kButtonsJoltageImpact.append(kButtonGroupsJoltageImpact)
        #end

        print(f"Part 2: {sum(findFewestPressesPartTwo(tuple(kJoltage), getButtonPermutationsAndCost(kButtonGroupsJoltageImpact), {}) for kJoltage,kButtonGroupsJoltageImpact in zip(kJoltages, kButtonsJoltageImpact))}")

    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
