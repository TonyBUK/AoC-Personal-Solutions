import time
import sys
import math

ITERATIONS = 1000

def main() :

    with open("../input.txt", "r") as inputFile:

        # Convert the Junction Boxes
        kJunctionBoxes = [tuple(int(k) for k in kLine.split(",")) for kLine in inputFile.readlines()]

        # Calculate all the possible distances between the Junction Boxes and sort
        kSortedDistances = sorted([
            (
                (x1 - x2)**2 +
                (y1 - y2)**2 +
                (z1 - z2)**2,
                kJunctionBox1,
                kJunctionBox2
            )
            for i, kJunctionBox1 in enumerate(kJunctionBoxes)
            for    kJunctionBox2 in           kJunctionBoxes[i+1:]
            for (x1, y1, z1), (x2, y2, z2) in [(kJunctionBox1, kJunctionBox2)]
        ])

#        Note: Equivalent Unravelled Code
#
#        kSortedDistances = []
#        for i, kJunctionBox1 in enumerate(kJunctionBoxes) :
#
#            for kJunctionBox2 in kJunctionBoxes[i+1:] :
#
#                # Calculate the Straight Line Distance (without the sqrt)
#                #
#                # Note: We don't actually care about the distances, just the weighting the distance provides
#                #       when sorting, meaning we can avoid a square root, and sorting will be unchanged.
#                nDistance = (((kJunctionBox1[0] - kJunctionBox2[0]) ** 2) +
#                             ((kJunctionBox1[1] - kJunctionBox2[1]) ** 2) +
#                             ((kJunctionBox1[2] - kJunctionBox2[2]) ** 2))
#
#                # Add the Distance/Key to a Data Set that will be sorted when complete
#                kSortedDistances.append((nDistance, kJunctionBox1, kJunctionBox2))
#
#            #end
#
#        #end
#
#        # Sort the Junction Pairs by Distance
#        kSortedDistances = sorted(kSortedDistances)

        # Part Two ends when:
        # 1. All Junction Boxes are in the Circuit
        # 2. We're now operating with a single circuit
        kJunctionBoxOutOfCircuilt = set(kJunctionBoxes)

        # 2D Array of all circuits and the junction boxes within
        kCircuits    = []

        # Map for each Junction Box to the Circuit
        kCircuitMap  = {}

        # Number of Circuits
        nNumCircuits = 0

        # Process Junction Boxes in order of distance from each other
        for i,kEntry in enumerate(kSortedDistances) :

            # Extract the Junction Boxes, and, if possible, the circuits they belong to
            kEntry1   = kEntry[1]
            kEntry2   = kEntry[2]
            kCircuit1 = kCircuitMap.get(kEntry1, None)
            kCircuit2 = kCircuitMap.get(kEntry2, None)

            # Remove the Junction Boxes from the list Junction Boxes yet to be processed
            if kEntry1 in kJunctionBoxOutOfCircuilt : kJunctionBoxOutOfCircuilt.remove(kEntry1)
            if kEntry2 in kJunctionBoxOutOfCircuilt : kJunctionBoxOutOfCircuilt.remove(kEntry2)

            # If neither circuits are known, we're creating a new circuit
            if (kCircuit1 == None) and (kCircuit2 == None) :

                # Create the New Circuit
                kCircuitMap[kEntry1] = len(kCircuits)
                kCircuitMap[kEntry2] = len(kCircuits)
                kCircuits.append([kEntry1, kEntry2])

                # Increment the Circuit Count
                nNumCircuits += 1

            else :

                # If both circuits are known, we're either getting a duplication on the
                # same circuit, or we're merging circuits
                if (kCircuit1 != None) and (kCircuit2 != None) :

                    # Check if we're merging Circuits
                    if kCircuit1 != kCircuit2 :

                        # Merge the Circuit
                        kCircuits[kCircuit1].extend(kCircuits[kCircuit2])

                        # Move the Circuit Ownership of the Unmerged Circuit
                        for kCircuitEntry in kCircuits[kCircuit2] :
                            kCircuitMap[kCircuitEntry] = kCircuit1
                        #end

                        # We can skip this once we complete Part One
                        if i < ITERATIONS :

                            # Remove the Unmerged Circuit
                            kCircuits[kCircuit2].clear()

                        #end

                        # Decrement the Circuit Count
                        nNumCircuits -= 1

                    #end

                else :

                    # We're just appending to an existing circuit

                    # Append to Circuit 2 if Junction Box 1 doesn't below to a Circuit
                    if kCircuit1 == None :
                        kCircuits[kCircuit2].append(kEntry1)
                        kCircuitMap[kEntry1] = kCircuit2
                    #end

                    # Append to Circuit 1 if Junction Box 2 doesn't below to a Circuit
                    if kCircuit2 == None :
                        kCircuits[kCircuit1].append(kEntry2)
                        kCircuitMap[kEntry2] = kCircuit1
                    #end

                #end

            #end

            # Part 1: 1000 Iterations, what are the three biggest circuits
            if i == ITERATIONS-1 :
                kLengths = sorted([len(k) for k in kCircuits], reverse=True)
                print(f"Part 1: {math.prod(kLengths[:3])}")
            #end

            # Part 2: What was the Junction Box Pair that created the final complete
            #         circuit
            if len(kJunctionBoxOutOfCircuilt) == 0 :
                if nNumCircuits == 1 :
                    print(f"Part 2: {kEntry1[0] * kEntry2[0]}")
                    break
                #end
            #end

        #end

    #end

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end