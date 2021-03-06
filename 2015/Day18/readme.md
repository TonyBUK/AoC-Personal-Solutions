# Day 18: Like a GIF For Your Yard

https://adventofcode.com/2015/day/18

This is the classic Conway's Game of Life, something that is another staple of the Advent of Code series.

**Part 1**

The key to solving this one is relatively straight forwards...

    Ensure that the grid you're counting is *not* being updated at the same time.

Everything from that point onwards falls into the category of optimisation.  Let's consider just how far down the rabbits hole we can go...  So the simplest algorithm here is:

    Copy the Conway Grid to a Target Grid
    
    FOR each point in the Conway Grid
    
      IF the Light is off, count the neighbours, and if the count is exactly 3, turn the light on
      
      IF the Light is on, count the neighbours, and if the count is not 2 or 3, turn the light off

    NEXT

The problem is that there are 1000 points in the Conway Grid, and we want to re-run that loop 100 times, and counting neighbours is quite expensive, and will invariably result in reading each location around 9 times.

So maybe instead of doing this, we pre-calculate the Neighbour Count for all positions, knowing that this can only occur around On Lights.

So that would be something like:

    Copy the Conway Grid to a Target Grid
    
    Calculate the Neighbour Count for all Grid Elements using a list of On Light locations
    
    FOR each point in the Conway Grid
    
      IF the Light is off, if the pre-calculated neighbour count is exactly 3, turn the light on and add to the list of On Light locations
      
      IF the Light is on, if the pre-calculated neighbour count is not 2 or 3, turn the light off and remove from the list of On Light locations
      
    NEXT

This removes the counting from the loop, which for my input resulted in about a 3-6x speedup from counting Neighbours as we go.  But of course looking at this, you can't help but think that it's still going over *every* point in the Conway Grid, except we know interesting things only happen around On Lights anyway...

So just iterate over the On Lights and their neighbouring points.  Plus now if we're pre-calculating the Neighbour Count, we actually don't need to copy the On Light locations, since the whole reason we worked on a copy in the first place was the volatility of the Neighbour Count.

So this brings the whole thing down to:

    Pre-calculate the Positions and Neighbour Counts for all On Light Locations and their Neighbours
    
    FOR each of the Pre-calculated positions
    
      IF the Light is off, if the pre-calculated neighbour count is exactly 3, add to the list of On Light locations

      IF the Light is off, if the pre-calculated neighbour count is not 2 or 3, remove from the list of On Light locations

    NEXT

This is also the first time I've purposely left debug functionality in the code.  A good thing to do with these sorts of puzzles is spend the time to output the data in a fashion to allow yourself to follow what's happened between iterations of the puzzle.  It'll often illuminate where mistakes were made in a way that isn't always obvious when looking in the debugger.  Plus the puzzle for the day gives some good examples of what should happen each cycle, which is definately worth implementing.

In-fact as the puzzles get more complex, unless you're really confident, or going for the leaderboard position, the workflow is normally:

1. Use the examples from the puzzle as the input before even touching the real input.
2. And once that works, now move to the real input.

Not saying it's fool proof, as often the real input exposes some nuance/scalability that the example didn't, but it's certainly a good way of gaining confidence the core algorithm is basically doing the right thing.

**Part 2**

Same as Part 1, but the corners now have to forcibly remain on, even if the rules say to switch them off.

**Python vs C++ vs C**

**Python**

Sets and Dictionaries pay dividends here, albeit, whilst I've stuck with the set/dictionary solution as it's more in keeping with the Python approach, it isn't actually faster than the previous solution that used the grid with cached neighbour counts.  However it would be weird in Python to go with a half way measure between the grid based solution and hash map/set solution, so I've gone with the latter.

**C++**

Not going to lie, the C++ attempts to be a close port of the Python, and it's noticable slower, around 2X slower to be precise... and that's after I try and come up with a faster indexing method for std map (fun fact, indexing with std::array is 8X slower that the Python code).  I guess baking the data into a primitive type is pretty much what a tuple is, but still.

Depends how much this irritates me as to whether I'll revisit this...

**C**

And as usual, the C version went in a different direction.  Instead of adding/removing from a hash map the On Lights, this works on a double buffer principle where it's no longer interested in whether a light changes state or not, merely whether it will be on.  So the implementation ends up being:

    Calculate the Neighbour Count for all Grid Elements using a list of On Light locations for the Current Buffer
    
    FOR each point in the Conway Grid
    
      IF the Light is off, if the pre-calculated neighbour count is exactly 3, add to the list of On Lights for the Other Buffer
      
      IF the Light is on, if the pre-calculated neighbour count is 2 or 3, add to the list of On Lights for the Other Buffer
      
    NEXT
    
This neatly means it never has to worry about insertion/deletion/searching.  This also operates on a 1D Grid rather than 2D (typically yields faster results) and encodes the X/Y position in the form ((Y * WIDTH) + X) to provide a 1D lookup index (i.e. for a 100x100 grid, this algorithm will count from 0 .. 9999 when given X = 0..99 and Y = 0..99.

To say this is faster than both the Python/C++ versions is an understatement.  And again, it's absolutely possible to implement this method in both languages, it would just be a weird choice (especially for Python) to do so, whereas C's lack of trivial provided hash map capability means it's more likely a solution like this would be implemented.
