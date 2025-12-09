#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define AOC_TRUE  (1)
#define AOC_FALSE (0)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef struct Point_Type
{
    int64_t X;
    int64_t Y;
} Point_Type;

typedef struct Edge_Type
{
    Point_Type kPoint1;
    Point_Type kPoint2;
} Edge_Type;

typedef struct Rectangle_Type
{
    Point_Type kCorners[4];
} Rectangle_Type;

typedef struct Rectangle_Edge_Type
{
    Edge_Type kEdges[4];
} Rectangle_Edge_Type;

/* Boilerplate code to read an entire file into a 1D buffer and give 2D entries per line.
 * This uses the EOL \n to act as a delimiter for each line.
 *
 * This will work for PC or Unix files, but not for blended (i.e. \n and \r\n)
 */
void readLines(FILE** pFile, char** pkFileBuffer, char*** pkLines, size_t* pnLineCount, size_t* pnFileLength, size_t* pnMaxLineLength, const size_t nPadLines)
{
    const unsigned long     nStartPos      = ftell(*pFile);
    const char*             kEOL           = "\n";
    unsigned long           nEndPos;
    unsigned long           nFileSize;
    unsigned                bProcessUnix   = 1;
    size_t                  nEOLLength     = 1;
    char*                   pLine;
    size_t                  nLineCount     = 0;
    size_t                  nMaxLineLength = 0;
    size_t                  nReadCount;
    size_t                  nPadLine;
 
    /* Read the File to a string buffer and append a NULL Terminator */
    fseek(*pFile, 0, SEEK_END);
    nEndPos = ftell(*pFile);
    assert(nStartPos != nEndPos);
    fseek(*pFile, nStartPos, SEEK_SET);
 
    nFileSize       = nEndPos - nStartPos;
 
    *pkFileBuffer   = (char*) malloc((nFileSize+3)         * sizeof(char));
    *pkLines        = (char**)malloc((nFileSize+nPadLines) * sizeof(char*));
 
    /* Handle weird EOL conventions whereby fread and fseek will handle EOL differently. */
    nReadCount = fread(*pkFileBuffer, nFileSize, 1, *pFile);
    assert(nReadCount == 1);
 
    /* Detect whether this has a \r\n or \n EOL convention */
    if (strstr(*pkFileBuffer, "\r\n"))
    {
        kEOL         = "\r\n";
        bProcessUnix = 0;
        nEOLLength   = 2;
    }
 
    /* Pad the File Buffer with a trailing new line (if needed) to simplify things later on */
    if (0 == bProcessUnix)
    {
        if ((*pkFileBuffer)[nFileSize] != '\n')
        {
            (*pkFileBuffer)[nFileSize]   = '\r';
            (*pkFileBuffer)[nFileSize+1] = '\n';
            (*pkFileBuffer)[nFileSize+2] = '\0';
        }
    }
    else
    {
        if ((*pkFileBuffer)[nFileSize] != '\n')
        {
            (*pkFileBuffer)[nFileSize]   = '\n';
            (*pkFileBuffer)[nFileSize+1] = '\0';
        }
    }
 
    /*
     * Convert the 1D string buffer into a 2D buffer delimited by EOL
     *
     * This effectively replaces all EOL's with NUL terminators.
     */
    pLine = *pkFileBuffer;
    while (1)
    {
        /* Find the next EOL */
        char* pEOL = strstr(pLine, kEOL);
 
        /* Check whether we've reached the EOF */
        if (pEOL)
        {
            const size_t nLineLength = pEOL - pLine;
            nMaxLineLength = (nLineLength > nMaxLineLength) ? nLineLength : nMaxLineLength;
 
            assert(pLine < &(*pkFileBuffer)[nFileSize]);
 
            (*pkLines)[nLineCount++] = pLine;
 
            /* Replace the EOL with a NUL terminator */
            *pEOL = '\0';
 
            /* Move to the start of the next line... */
            pLine = pEOL + nEOLLength;
        }
        else
        {
            break;
        }
    }
 
    for (nPadLine = 0; nPadLine < nPadLines; ++nPadLine)
    {
        (*pkLines)[nLineCount] = &(*pkFileBuffer)[nFileSize+1];
        ++nLineCount;
    }
 
    *pnLineCount  = nLineCount;
    if (NULL != pnFileLength)
    {
        *pnFileLength = nFileSize;
    }
    if (NULL != pnMaxLineLength)
    {
        *pnMaxLineLength = nMaxLineLength;
    }
}

int64_t rectangleArea(const Point_Type* kTile1, const Point_Type* kTile2)
{
    return (MAX(kTile1->X, kTile2->X) - MIN(kTile1->X, kTile2->X) + 1) *
           (MAX(kTile1->Y, kTile2->Y) - MIN(kTile1->Y, kTile2->Y) + 1);
}

void getRectangleEdges(const Edge_Type* kRectanglePoints, Rectangle_Edge_Type* kRectangleEdges)
{
    const int64_t nMinX = MIN(kRectanglePoints->kPoint1.X, kRectanglePoints->kPoint2.X);
    const int64_t nMaxX = MAX(kRectanglePoints->kPoint1.X, kRectanglePoints->kPoint2.X);
    const int64_t nMinY = MIN(kRectanglePoints->kPoint1.Y, kRectanglePoints->kPoint2.Y);
    const int64_t nMaxY = MAX(kRectanglePoints->kPoint1.Y, kRectanglePoints->kPoint2.Y);
    const Rectangle_Type kRectangle = {{
        {nMinX, nMinY}, /* Top Left     */
        {nMinX, nMaxY}, /* Bottom Left  */
        {nMaxX, nMaxY}, /* Bottom Right */
        {nMaxX, nMinY}  /* Top Right    */
    }};

    size_t i;

    for (i = 0; i < 4; ++i)
    {
        kRectangleEdges->kEdges[i].kPoint1 = kRectangle.kCorners[i];
        kRectangleEdges->kEdges[i].kPoint2 = kRectangle.kCorners[(i + 1) % 4];
    }
}

int pointInPolygon(const Point_Type* kPoint, const Point_Type* kPolygon, const size_t nPolygonSize)
{
    /* Point to cast from */
    const int64_t x  = kPoint->X;
    const int64_t y  = kPoint->Y;

    int           bInside = AOC_FALSE;
    int64_t       x1, y1;
    size_t        i;


    /* Ray Cast Crossing Test */

    /* First Point of the Polygon Edge */
    x1 = kPolygon[0].X;
    y1 = kPolygon[0].Y;

    for (i = 1; i < (nPolygonSize + 1); ++i)
    {
        /* Second Point of the Polyhon Edge */
        const int64_t x2 = kPolygon[i % nPolygonSize].X;
        const int64_t y2 = kPolygon[i % nPolygonSize].Y;

        /* Check for a Horizontal Crossing */
        if (((y1 > y) != (y2 > y)) &&
            (x < (x2 - x1) * (y - y1) / (y2 - y1) + x1))
        {
            bInside = (bInside == AOC_TRUE) ? AOC_FALSE : AOC_TRUE;
        }

        /* Move to the next Polygon Edge */
        x1 = x2;
        y1 = y2;
    }

    return bInside;
}

int segmentIntersect(const Point_Type* a1, const Point_Type* a2, const Point_Type* b1, const Point_Type* b2, Point_Type* kIntersect)
{
    const int64_t x1 = a1->X;
    const int64_t y1 = a1->Y;
    const int64_t x2 = a2->X;
    const int64_t y2 = a2->Y;

    const int64_t x3 = b1->X;
    const int64_t y3 = b1->Y;
    const int64_t x4 = b2->X;
    const int64_t y4 = b2->Y;

    const int     bAIsVertical   = (x1 == x2)                 ? AOC_TRUE  : AOC_FALSE;
    const int     bAIsHorizontal = (bAIsVertical == AOC_TRUE) ? AOC_FALSE : AOC_TRUE;
    const int     bBIsVertical   = (x3 == x4)                 ? AOC_TRUE  : AOC_FALSE;
    const int     bBIsHorizontal = (bBIsVertical == AOC_TRUE) ? AOC_FALSE : AOC_TRUE;

    if ((AOC_TRUE == bAIsVertical) && (AOC_TRUE == bBIsHorizontal))
    {
        const int64_t x = x1;
        const int64_t y = y3;

        if ((MIN(y1, y2) <= y) && (y <= MAX(y1, y2)) &&
            (MIN(x3, x4) <= x) && (x <= MAX(x3, x4)))
        {
            kIntersect->X = x;
            kIntersect->Y = y;
            return AOC_TRUE;
        }
    }
    else if ((AOC_TRUE == bAIsHorizontal) && (AOC_TRUE == bBIsVertical))
    {
        const int64_t x = x3;
        const int64_t y = y1;

        if ((MIN(x1, x2) <= x) && (x <= MAX(x1, x2)) &&
            (MIN(y3, y4) <= y) && (y <= MAX(y3, y4)))
        {
            kIntersect->X = x;
            kIntersect->Y = y;
            return AOC_TRUE;
        }
    }
    else if ((AOC_TRUE == bAIsVertical) && (AOC_TRUE == bBIsVertical) && (x1 == x3))
    {
        const int64_t yLow  = MAX(MIN(y1, y2), MIN(y3, y4));
        const int64_t yHigh = MIN(MAX(y1, y2), MAX(y3, y4));

        if (yLow <= yHigh)
        {
            kIntersect->X = x1;
            kIntersect->Y = yLow;
            return AOC_TRUE;
        }
    }
    else if ((AOC_TRUE == bAIsHorizontal) && (AOC_TRUE == bBIsHorizontal) && (y1 == y3))
    {
        const int64_t xLow  = MAX(MIN(x1, x2), MIN(x3, x4));
        const int64_t xHigh = MIN(MAX(x1, x2), MAX(x3, x4));

        if (xLow <= xHigh)
        {
            kIntersect->X = xLow;
            kIntersect->Y = y3;
            return AOC_TRUE;
        }
    }

    return AOC_FALSE;
}

int isInside(const Point_Type* kPolygonPoints, const Edge_Type* kPolygonEdges, const size_t nPolygonSize, const Edge_Type* kRectanglePoints)
{
    Rectangle_Edge_Type kRectangleEdges;
    size_t              i;

    getRectangleEdges(kRectanglePoints, &kRectangleEdges);

    /* Check the Trivial Case, one of the 4 corners is not inside the Polygon */
    for (i = 0; i < 4; ++i)
    {
        if (!pointInPolygon(&kRectangleEdges.kEdges[i].kPoint1, kPolygonPoints, nPolygonSize))
        {
            return AOC_FALSE;
        }
    }

    for (i = 0; i < 4; ++i)
    {
        const Point_Type ra = kRectangleEdges.kEdges[i].kPoint1;
        const Point_Type rb = kRectangleEdges.kEdges[i].kPoint2;

        /* Record the Orientation for Later */
        const int        bRectangleIsHorizontal = (ra.X != rb.X) ? AOC_TRUE : AOC_FALSE;

        size_t j;

        /* For each Polygon Edge */
        for (j = 0; j < nPolygonSize; ++j)
        {
            const Point_Type pa = kPolygonEdges[j].kPoint1;
            const Point_Type pb = kPolygonEdges[j].kPoint2;

            /* Get the Intersection with the Edge (if any) */
            Point_Type kIntersection;

            if (segmentIntersect(&ra, &rb, &pa, &pb, &kIntersection))
            {
                /*
                 * There's tons of nasty edge cases, so lets, in the direction of the Rectangle Edge, create 2 test points either side
                 * of the intersection
                 */
                const Point_Type kTestPoint1 =
                {
                    kIntersection.X - ((bRectangleIsHorizontal == AOC_TRUE)  ? 1 : 0),
                    kIntersection.Y - ((bRectangleIsHorizontal == AOC_FALSE) ? 1 : 0)
                };
                const Point_Type kTestPoint2 =
                {
                    kIntersection.X + ((bRectangleIsHorizontal == AOC_TRUE)  ? 1 : 0),
                    kIntersection.Y + ((bRectangleIsHorizontal == AOC_FALSE) ? 1 : 0)
                };

                /*
                 * Check the test points don't exceed the bounds of the Rectangle Edge
                 *
                 * Note: At most, only one of the test points could exceed the bounds
                 */
                int              bTestPoint1 = AOC_TRUE;
                int              bTestPoint2 = AOC_TRUE;

                if (kTestPoint1.X < MIN(ra.X, rb.X))
                {
                    bTestPoint1 = AOC_FALSE;
                }
                else if (kTestPoint2.X > MAX(ra.X, rb.X))
                {
                    bTestPoint2 = AOC_FALSE;
                }
                else if (kTestPoint1.Y < MIN(ra.Y, rb.Y))
                {
                    bTestPoint1 = AOC_FALSE;
                }
                else if (kTestPoint2.Y > MAX(ra.Y, rb.Y))
                {
                    bTestPoint2 = AOC_FALSE;
                }

                /* Both Points are Valid, so test both points are inside the Polygon */
                if (bTestPoint1 && bTestPoint2)
                {
                    if (pointInPolygon(&kTestPoint1, kPolygonPoints, nPolygonSize) &&
                        pointInPolygon(&kTestPoint2, kPolygonPoints, nPolygonSize))
                    {
                        continue;
                    }
                }
                /* Test Point 1 is inside the Polygon */
                else if (bTestPoint1)
                {
                    if (pointInPolygon(&kTestPoint1, kPolygonPoints, nPolygonSize))
                    {
                        continue;
                    }
                }
                /* Test Point 2 is inside the Polygon */
                else if (bTestPoint2)
                {
                    if (pointInPolygon(&kTestPoint2, kPolygonPoints, nPolygonSize))
                    {
                        continue;
                    }
                }

                /*
                 * If the test point(s) are all outside, then this rectangle
                 * is not fully enclosed
                 */
                return AOC_FALSE;
            }
        }
    }

    return AOC_TRUE;
}

int main(int argc, char** argv)
{
    FILE* pData = fopen("../input.txt", "r");
 
    if (pData)
    {
        char*                       kInputBuffer;
        char**                      kInputLines;

        size_t                      nTileCount;
        Point_Type*                 kTiles;
        Edge_Type*                  kPolygonEdges;

        size_t                      i;

        int64_t                     nMaxAreaPartOne = 0;
        int64_t                     nMaxAreaPartTwo = 0;

        /* Read the whole file into an easier to process 2D Buffer */
        readLines(&pData, &kInputBuffer, &kInputLines, &nTileCount, NULL, NULL, 0);
        fclose(pData);

        kTiles           = (Point_Type*)malloc(nTileCount * sizeof(Point_Type));
        kPolygonEdges    = (Edge_Type*) malloc(nTileCount * sizeof(Edge_Type));

        /* Decode the Tiles and Polygon Edges */
        for (i = 0; i <= nTileCount; ++i)
        {
            if (i < nTileCount)
            {
                char* p = kInputLines[i];
                kTiles[i].X = strtoll(p, NULL, 10);
                p = strstr(p, ",") + 1;
                kTiles[i].Y = strtoll(p, NULL, 10);
            }

            if (i > 0)
            {
                kPolygonEdges[i - 1].kPoint1 = kTiles[i - 1];
                kPolygonEdges[i - 1].kPoint2 = kTiles[i % nTileCount];
            }
        }

        for (i = 0; i < (nTileCount - 1); ++i)
        {
            size_t j;
            int64_t nMaxAreaForTile = 0;
            
            for (j = i + 1; j < nTileCount; ++j)
            {
                const int64_t nArea = rectangleArea(&kTiles[i], &kTiles[j]);
                nMaxAreaPartOne = MAX(nArea, nMaxAreaPartOne);

                /* Only bother testing collision if this has a larger area */
                if (nArea > nMaxAreaPartTwo)
                {
                    const Edge_Type kRectangle = {kTiles[i], kTiles[j]};

                    if (isInside(kTiles, kPolygonEdges, nTileCount, &kRectangle))
                    {
                        nMaxAreaPartTwo = nArea;
                    }
                }
            }
        }

        printf("Part 1: %lld\n", nMaxAreaPartOne);
        printf("Part 2: %lld\n", nMaxAreaPartTwo);

        /* Cleanup */
        free(kInputLines);
        free(kInputBuffer);
        free(kTiles);
        free(kPolygonEdges);
    }
 
    return 0;
}