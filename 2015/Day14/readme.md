# Day 14: Reindeer Olympics

https://adventofcode.com/2015/day/14

This introduces:
- Simple state machines

So this is one where in some ways I wish the puzzle had a longer duration for the time input.  Currently, it's solveable in a relatively simple way, with Part 1 working out how far a Reindeer will have travelled in the allotted time without needing to process each individual second:

    WHILE Time > 0
    
      Move the Reindeer forward by: speed per second * seconds allotted for movement
        Where: seconds allottedd for movement is either the full time duration for movement or the remaing time, whichever is smaller
      
      Decrease Time by seconds allotted for movement
      
      Decrease Time by seconds allotted for rest

    WEND

Whereas Part 2 really needs to keep track of how far each Reindeer has moved on a per second basis, which is more like:

    FOR each second...
    
      IF the Reindeer is racing

        Move the Reindeer by 1 seconds worth of distance
        
        Decrease the remaining movement time by 1 second
        
        IF the remaining movement time has expired
        
          Indicate the Reindeer is resting

          Store the remaining rest time

        END

      ELSE
      
        Decrease the remaining rest time by 1 second
        
        IF the remaining rest time has expired
        
          Indicate the Reindeer is Racing

          Store the remaining movement time

        END
        
      END
    
    NEXT

This way we should, on a per second basis, know exactly how far each Reindeer has travelled, and we'll then be able to award points to the lead Reindeer(s).  Were the time longer, we'd start looking for loop repetitions (Lowest Common Multiples of the rest/flight durations), and whether we can start processing data without resorting to a per-second loop.  But as it stands, utterly not worth the effort.

**Part 1**

As described above, output the full distance travelled for the lead Reindeer after 2503 seconds.

**Part 2**

As described above, output the points total for the Reindeer that has been in the lead the most times within the 2503 second period.

**Python vs C++ vs C**

**Python**

Split the data more than I would like.  Probably should have kept it bundled and used a lambda expression to help determine the max distance/points without having to copy the data, however laziness prevailed here.

**C++**

A bit more verbose but essentially the Python solution.  The code could be a tad faster for calculating points if I passed by reference rather than const reference, as this would prevent me having to create a copy within the winningPoints function for manipulation, but I typically favour functions that can use const inputs where possible as it typically increases the versatility of the usage (i.e. better to be non-destructive where possible).  Plus it's not a race!

The cast from std::size_t to int64_t incidently is to allow the while loop to go negative, otherwise I'd need guards on ensuring I don't underflow an unsigned type when subtracting duration/rest time.

**C**

Follows the C++ solution closely.  This time the calls are destructive to the data being processed, as copying the data in C is more of a pain compared to C++, as I'd need to allocate the space, perform the copy, and de-allocate.  Plus since some of the data is actually a pointer to another structure anyway, whilst safe in this instance, it could be more of a pain if that too needed to be destructive.

One of the weaknesses of printf incidently is that it's really beholden to the size of the type being processed, making the format strings something of a pain for portability sake.  C99 added %zu for size_t types but I'm determined to go OG C here, so I've instead cast to uint32_t which should force the type to be compatible with %u across compilers.

As an aside, if anyone is curious why this is an issue, it's actually all because printf is an example of a variadic function, which is just a way of saying it takes a variable number of arguments.

https://en.cppreference.com/w/c/variadic

The problem is, for better or worse, the function that's called actually has no idea what the types of the parameters were, instead it has to figure out at runtime how to traverse the data passed in, which is why the format string type defintions such as %i / %llu etc. are so important in the first place, as these not only tell printf how to present the data once printed, but also how to move from one argument to the next (i.e. the size of each data type).

Decent compilers understand this, and typically have specialised warnings if you ever mismatch the format string (if known at compilation time) with the arguments supplied, but ultimately printf is still going to have to re-do all this at runtime.  One of the many reasons never to write variadic functions unless you've a very *very* good reason to.
