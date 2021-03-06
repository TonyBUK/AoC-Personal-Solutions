# Day 10: Balance Bots

https://adventofcode.com/2016/day/10

This problem is alot like 2015 Day 7: Some Assembly Required.  This looks like it should just be processing a series of commands in order, but in reality, the order of the commands is actually irrelevant.  Once again, if you're familiar with VHDL and sensitivities, that's really the crux of how this challenge works.

**Part 1**

So once you realise that this task is really about determining what command is performed next once the current command(s) have been performed, it starts to give a better indication as to how the data should be split.

The first stab at this might be:

- Store a separate list of commands
- Store a separate list of Input Bins to store which chips they're holding
- Store a separate list of bots to store which chips they're holding
- Store a separate list of Output Bins to store which chips they're holding

The problem here is that you end up with an algorithm something like:

1: Initialise

    FOR each Command
    
      IF it's a VALUE command
      
        Create an Input Bin associated with the Command and store the Chip
        
        Create, if required, the Bot associated with the Command
        
      ELSE (Bot Command)
      
        Create, if required, the Bot(s) associated with the Command
        
        Create, if required, the Output Bin(s) associated with the Command
        
      END
      
    END

2: Process the Commands that have no dependencies

    FOR each Command
    
      IF it's a VALUE command
      
        Move the chip from the Input Bin to the Bot
        
      END
      
    END

3: Keep processing until not Bots can process any commands

    WHILE any commands can be performed
    
      FOR each Command
      
        IF the Bot referenced by the Command is holding two chips

          IF the two chips being held by the Bot referenced by the Command match those specified by the Part 1 Puzzle
          
            Store the ID of the Bot referenced by the Command as the Part 1 Solution
            
          END

          Move the High/Low Chip to the target Bot/Output Bin as required
          
        END
        
      END
      
    END

As you can see, it's basically going over the entire command list several times, and in reality, at any given point, only one or two of the commands are actually valid.

What might be better then is:

- Store a separate list of bots to store which chips they're holding, and what commands should be performed if they're holding two chips
- Store a separate list of Output Bins to store which chips they're holding
- Store a Queue of Bots that can be processed (holding two chips).

So what happens to the input, well, let's see...


1: Initialise

    FOR each Command
    
      IF it's a VALUE command
      
        Create, if required, the Bot associated with the Command and store the Chip
        
        IF the Bot is now holding two chips
        
          Add the Bot to the queue of Bots that can be processed
          
        END
        
      ELSE (Bot Command)
      
        Create, if required, the Bot(s) associated with the Command
        
        Create, if required, the Output Bin(s) associated with the Command
        
        Store the Command with the primary Bot for the Command
        
      END
      
    END

2: Keep processing until not Bots can process any commands

    WHILE the Bot Queue isn't Empty
    
      Pop the Last Command from the Bot Queue for processing
          
      IF the Bot referenced by the Queue Entry is holding two chips

        IF the two chips being held by the Bot referenced by the Queue Entry match those specified by the Part 1 Puzzle
          
          Store the ID of the Bot referenced by the Queue Entry as the Part 1 Solution
            
        END

        Move the High/Low Chip to the target Bot/Output Bin as required
        
        IF any of the Bots that were given chips are now holding two chips
        
          Add the Bot to the queue of Bots that can be processed
          
        END
        
      END
      
    END

Hopefully you can see how much less processing this solution requires


There are a couple of gotchas with this challenge that we need to consider, but not necessarily do anything with.

1. What if after processing all the Input Bins, no Bot is holding two chips?
2. What if after processing all the Input Bins, one or more Bots needs to hold more than two chips?
3. What if after processing a Command to move chips from Bot to Bot, a Bot now holds more than two chips?
4. What if after processing a Command, an Output Bin needs to hold more than 2 chips?
5. What if there's always a Command that can be processed (i.e. there's always at least one Bot holding two chips)?

Now my solution addresses none of these, as I suspect the puzzle inputs make each of these points moot, however to go through these in order.

1. This means the puzzle is unsolveable, so ignore.
2. This would mean the puzzle description is incomplete as it doesn't define what to do in this instance, though I would suspect we'd process the Input Bins in the order we find them in the puzzle input, meaning we'd have to keep the Input Bins as separate items and keep processing (interleaved) with the Bot Commands until all Input Bins are empty.
3. Ignore the command, and re-add the Bot to the queue.
4. This would mean the puzzle description is incomplete as it doesn't define what to do in this instance, though I suspect we'd only care about the first.
5. Since Part 1 just cares about which bot is holding two specific chips, we just need to keep going until that occurs.


**Part 2**

So once again, this is a Puzzle that's solved in Part 1, in that we change nothing, and all we care about now is when the bots have finished, what chips are in Output Bins 0, 1 and 2.  The only thing this might influence is the gotcha's, specifically:

Q. What if after processing a Command, an Output Bin needs to hold more than 2 chips?

A. This would mean the puzzle description is incomplete as it doesn't define what to do in this instance, though I suspect we'd only care about the first.

Q. What if there's always a Command that can be processed (i.e. there's always at least one Bot holding two chips)?

A. Since Part 2 just cares about when Output Bins 0, 1 and 2 are populated, we could use these three bins holding chips as a means of halting any processing.  However, all the wording of the puzzle strongly implies that the bots will *always* complete.


**Python vs C++ vs C**

**Python**

Pretty much implemented as described making Python a very good fit for this puzzle.


**C++**

This basically partitions ownership / uniqueness of processing within the Output Bin and Bot classes.  I've also derived these from a parent class in order to force the one method any of the derived classes would need, the ability to receive chips, even if they don't necessarily do the same thing when received.  Now arguably, the task of self identifying whether a bot should add itself to the queue or not should lie with the bot receiving the chip, not the one giving the chip, but honestly, it doesn't get you much for this puzzle, and would convolute things since I'd now need to make the queue available to the bot receiving the chip as well.  I feel what's done gets the point across just fine...


**C**

This is a faster, but lazier hybrid of the Python/C++ solution.  The primary source of laziness is this makes the 100% dodgy assumption that the bot id's and output bin id's will just be integer array indexes starting from 0, meaning we can just pre-allocate enough bots/bins based on the number of lines of text in the input file.  This means that whereas Python/C++ will gracefully handle arbitrary bot id's should they be provided, the C solution will not (it'll just bomb out with an assertion failure when testing the indexes are valid for my assumption).

This does however mean that unlike the Python/C++ solutions, this lacks any dynamic array resizing, which gives it a fractional speed advantage over the C++ solution, where most of time, after rounding to the nearest ms, they take roughly the same amount of time to execute.
