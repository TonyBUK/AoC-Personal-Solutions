import time
import sys
import math

def main() :

    with open("../input.txt", "r") as inputFile:

        kShapes  = []
        kRegions = []

        for kLine in inputFile:

            if "x" in kLine :
                kGridSize = [int(k) for k in kLine.split(":")[0].split("x")]
                kShapeCount = [int(k) for k in kLine.split(": ")[1].split(" ")]
                kRegions.append([kGridSize, kShapeCount])

            elif ":" in kLine :

                kShapes.append([])
            
            elif "." in kLine or "#" in kLine :

                kShapes[-1].append(kLine.strip())

            #end

        #end

        # Shape Dimensions + Fill Counts for the Simple Discard Cases
        kShapeDimensions = [[max(map(len, s)), len(s)] for s in kShapes]
        kShapeCount      = [sum(r.count("#") for r in s) for s in kShapes]

        nPossible    = 0
        nNotPossible = 0

        for kRegion in kRegions :

            nArea                         = math.prod(kRegion[0])
            nNonTesselatedAreaRequired    = 0
            nTotalTesselationAreaRequired = 0

            for i, nCount in enumerate(kRegion[1]) :
                nNonTesselatedAreaRequired    += nCount * kShapeDimensions[i][0] * kShapeDimensions[i][1]
                nTotalTesselationAreaRequired += nCount * kShapeCount[i]
            #end

            # Yes
            if nArea >= nNonTesselatedAreaRequired :
                nPossible += 1
            # No
            elif nArea < nTotalTesselationAreaRequired :
                nNotPossible += 1
            # Maybe
            else :
                # Can you repeat the question?
                assert(False)
            #end

        #end

        print(f"Part 1: {nPossible}")

    #end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end