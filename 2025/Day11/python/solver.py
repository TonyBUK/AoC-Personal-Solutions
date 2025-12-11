import time
import sys

def main() :

    with open("../input.txt", "r") as inputFile:

        def countPaths(kNode, kTargetNode, kPaths, kCriticalNodes, kFoundCriticalNodes = None, kCache = None) :

            if kCache == None :
                kCache = {}
            #end

            if kFoundCriticalNodes == None :
                kFoundCriticalNodes = ()
            #end

            # We keep a running list of the found critical nodes
            if kNode in kCriticalNodes :
                kFoundCriticalNodes = list(kFoundCriticalNodes) + [kNode]
            #end

            # The Cache indicates the current state as follows:
            #
            # 1. Which Critical Nodes have been found (fft/dac)
            # 2. What Node are we on
            #
            # This allows caching of repeats
            kCacheKey = (tuple([k in kFoundCriticalNodes for k in kCriticalNodes]), kNode)
            if kCacheKey in kCache :
                return kCache[kCacheKey]
            #end

            # If this is our target node, indicate it is valid if we found all our critical nodes
            if kNode == kTargetNode :
                return 1 if len(kCriticalNodes) == len(kFoundCriticalNodes) else 0
            #end

            # Calculate the Number of Routes for our current path
            nRoutes = sum([countPaths(kNextNode, kTargetNode, kPaths, kCriticalNodes, kFoundCriticalNodes, kCache) for kNextNode in kPaths[kNode]])

            # Cache the Number of Routes for any repetitions
            kCache[kCacheKey] = nRoutes

            return nRoutes

        #end

        # Create our Paths Map
        kPaths = {j[0] : j[1:] for j in [k.split(" ") for k in inputFile.read().replace(":", "").split("\n")]}

        # Solve
        print(f"Part 1: {countPaths("you", "out", kPaths, ())}")
        print(f"Part 2: {countPaths("svr", "out", kPaths, ["dac", "fft"])}")

    #end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end