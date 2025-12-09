import time
import sys

def main() :

    # Convert a pair of Corners to a complte rectangle
    def getRectangleEdges(kRectanglePoints : list) -> list[int, int, int, int] :

        kXPoints = sorted(k[0] for k in kRectanglePoints)
        kYPoints = sorted(k[1] for k in kRectanglePoints)

        kRectangle = ((kXPoints[0], kYPoints[0]), # Top Left
                      (kXPoints[0], kYPoints[1]), # Bottom Left
                      (kXPoints[1], kYPoints[1]), # Bottom Right
                      (kXPoints[1], kYPoints[0])) # Top Right

        return ((kRectangle[0], kRectangle[1]), # TL -> BL
                (kRectangle[1], kRectangle[2]), # BL -> BR
                (kRectangle[2], kRectangle[3]), # BR -> TR
                (kRectangle[3], kRectangle[0])) # TR -> TL

    #end

    kPointInPolygonCache = {}

    def pointInPolygon(kPoint, kPolygons):

        # Have we seen this point before
        nonlocal kPointInPolygonCache
        if kPoint in kPointInPolygonCache :
            return kPointInPolygonCache[kPoint]
        #end

        # Ray Cast Crossing Test

        # Flag to Toggle each time we cross
        bInside = False

        n = len(kPolygons)

        # Point to cast from
        x, y = kPoint

        # First Point of the Polygon Edge
        x1, y1 = kPolygons[0]

        for i in range(1, n + 1):

            # Second Point of the Polyhon Edge
            x2, y2 = kPolygons[i % n]

            # Check for a Horizontal Crossing
            if ((y1 > y) != (y2 > y)) and (x < (x2 - x1) * (y - y1) / (y2 - y1) + x1):
                bInside = not bInside
            #end

            # Move to the next Polygon Edge
            x1, y1 = x2, y2

        #end

        # Cache the Result
        kPointInPolygonCache[kPoint] = bInside

        return bInside

    #end

    def segmentIntersect(a1, a2, b1, b2) -> list[int, int]:

        (x1, y1), (x2, y2) = a1, a2
        (x3, y3), (x4, y4) = b1, b2

        bAIsVertical   = x1 == x2
        bAIsHorizontal = not bAIsVertical
        bBIsVertical   = x3 == x4
        bBIsHorizontal = not bBIsVertical

        if bAIsVertical and bBIsHorizontal :
            x = x1
            y = y3
            if min(y1, y2) <= y <= max(y1, y2) and min(x3, x4) <= x <= max(x3, x4) :
                return [x, y]
            #end
        elif bAIsHorizontal and bBIsVertical :
            x = x3
            y = y1
            if min(x1, x2) <= x <= max(x1, x2) and min(y3, y4) <= y <= max(y3, y4) :
                return [x, y]
            #end
        elif bAIsVertical and bBIsVertical and x1 == x3 :
            yLow  = max(min(y1, y2), min(y3, y4))
            yHigh = min(max(y1, y2), max(y3, y4))
            if yLow <= yHigh :
                return [x1, yLow]
            #end
        elif bAIsHorizontal and bBIsHorizontal and y1 == y3 :
            xLow  = max(min(x1, x2), min(x3, x4))
            xHigh = min(max(x1, x2), max(x3, x4))
            if xLow <= xHigh :
                return [xLow, y3]
            #end
        #end

        return None

    #end

    def isInside(kPolygonPoints : list, kPolygonEdges : list, kRectanglePoints) -> bool :

        kRectangleEdges = getRectangleEdges(kRectanglePoints)

        # Check the Trivial Case, one of the 4 corners is not inside the Polygon
        if not pointInPolygon(kRectangleEdges[0][0], kPolygonPoints) :
            return False
        if not pointInPolygon(kRectangleEdges[1][0], kPolygonPoints) :
            return False
        if not pointInPolygon(kRectangleEdges[2][0], kPolygonPoints) :
            return False
        if not pointInPolygon(kRectangleEdges[3][0], kPolygonPoints) :
            return False
        #end

        # For Each Edge
        for ra, rb in kRectangleEdges :

            # Record the Orientation for Later
            bRectangleIsHorizontal = ra[0] != rb[0]

            # For each Polygon Edge
            for pa, pb in kPolygonEdges :

                # Get the Intersection with the Edge (if any)
                kIntersect = segmentIntersect(ra, rb, pa, pb)

                # If no intersections, move to the next Edge
                if kIntersect == None :
                    continue
                #end

                # There's tons of nasty edge cases, so lets, in the direction of the Rectangle Edge, create 2 test points either side
                # of the intersection
                kTestPoint1 = (kIntersect[0] - (1 if bRectangleIsHorizontal else 0), kIntersect[1] - (1 if not bRectangleIsHorizontal else 0))
                kTestPoint2 = (kIntersect[0] + (1 if bRectangleIsHorizontal else 0), kIntersect[1] + (1 if not bRectangleIsHorizontal else 0))

                # Check the test points don't exceed the bounds of the Rectangle Edge
                #
                # Note: At most, only one of the test points could exceed the bounds
                bTestPoint1 = True
                bTestPoint2 = True
                if kTestPoint1[0] <   min(ra[0], rb[0]) :
                    bTestPoint1 = False
                elif kTestPoint2[0] > max(ra[0], rb[0]) :
                    bTestPoint2 = False
                elif kTestPoint1[1] < min(ra[1], rb[1]) :
                    bTestPoint1 = False
                elif kTestPoint2[1] > max(ra[1], rb[1]) :
                    bTestPoint2 = False
                #end

                # Both Points are Valid, so test both points are inside the Polygon
                if bTestPoint1 and bTestPoint2 :
                    if (pointInPolygon(kTestPoint1, kPolygonPoints) and
                        pointInPolygon(kTestPoint2, kPolygonPoints)) :
                        continue
                    #end
                # Test Point 1 is inside the Polygon
                elif bTestPoint1 :
                    if pointInPolygon(kTestPoint1, kPolygonPoints) :
                        continue
                    #end
                # Test Point 2 is inside the Polygon
                elif bTestPoint2 :
                    if pointInPolygon(kTestPoint2, kPolygonPoints) :
                        continue
                    #end
                #end

                # If the test point(s) are all outside, then this rectangle
                # is not fully enclosed
                return False

            #end

        #end

        # The Rectangle must be fully enclosed
        return True

    #end

    def area(k1 : list[int], k2 : list[int]) -> int :
        return (abs(k1[0] - k2[0]) + 1) * (abs(k1[1] - k2[1]) + 1)
    #end

    with open("../input.txt", "r") as inputFile:

        # Convert the Tiles
        kTiles = [tuple(int(k) for k in kLine.split(",")) for kLine in inputFile.readlines()]

        # Create the Polygon Edges
        kPolygonEdges = [(kTiles[i], kTiles[(i + 1) % len(kTiles)]) for i in range(len(kTiles))]

        # Find the Max Area (Both Parts)
        #
        # Note: We also cache the max possible areas for Part Two
        nMaxAreaPartOne = 0
        nMaxAreaPartTwo = 0
        for i, kTile1 in enumerate(kTiles) :

            for kTile2 in kTiles[i+1:] :

                nArea           = area(kTile1, kTile2)
                nMaxAreaPartOne = max(nArea, nMaxAreaPartOne)

                # Only bother testing collision if this has a larger area
                if nArea > nMaxAreaPartTwo :

                    if isInside(kTiles, kPolygonEdges, (kTile1, kTile2)) :

                        nMaxAreaPartTwo = nArea

                    #end

                #end

            #end

        #end

        print(f"Part 1: {nMaxAreaPartOne}")
        print(f"Part 2: {nMaxAreaPartTwo}")

    #end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end