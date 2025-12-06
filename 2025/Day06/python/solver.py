import time
import sys
import math
 
def main() :

    def solveCephalopodsMaths(kValues : list[int], kOperators : list[str]) -> int :

        nResult = 0

        for n, kOperator in enumerate(kOperators) :
            if kOperator == "+":
                nResult += sum(kValues[n])
            elif kOperator == "*":
                nResult += math.prod(kValues[n])
            else:
                assert(False)
            #end
        #end

        return nResult

    #end

    with open("../input.txt", "r") as inputFile:

        kLines = [k.replace("\n", "") for k in inputFile.readlines()]

        # Part 1
        kRaw = [k.split() for k in kLines]
        kValuesPartOne = [ [int(k) for k in col] for col in zip(*kRaw[:-1]) ]

        print(f"Part 1: {solveCephalopodsMaths(kValuesPartOne, kRaw[-1])}")

        # Part 2
        # As the only operators are +/-, these are communicative, meaning we won't
        # have to worry about whether we parse left to right or right to left
        kValuesPartTwo = []
        for kLine in kLines[:-1] :

            nExpression = -1
            nSubPos     = -1

            for i, k in enumerate(kLine) :

                # Move onto the Next Column which we transpose to the Row
                nSubPos += 1

                # If the final line isn't blank, it means this is an Operator
                # meaning we're now handling a new expression
                if kLines[-1][i] != " ":
                    nExpression += 1
                    nSubPos      = 0
                #end

                # If the current entry isn't blank
                if k != " ":

                    # Initialise the Expression
                    if nExpression >= len(kValuesPartTwo) :
                        kValuesPartTwo.append([])
                    #end

                    # Pad the current Expression
                    while len(kValuesPartTwo[nExpression]) <= nSubPos:
                        kValuesPartTwo[nExpression].append(0)
                    #end

                    # Append the Decoded Digit shifting all prior digits by 10
                    kValuesPartTwo[nExpression][nSubPos] = kValuesPartTwo[nExpression][nSubPos] * 10 + int(k)

                #end

            #end

        #end

        print(f"Part 2: {solveCephalopodsMaths(kValuesPartTwo, kRaw[-1])}")

    #end

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end