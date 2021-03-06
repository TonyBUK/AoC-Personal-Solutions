# Day 7: Internet Protocol Version 7

https://adventofcode.com/2016/day/7

In a nutshell, we're looking for 3 and 4 letter palendromes inside and outside of the square brackets and applying some rules for acceptance.  Definately one that can be trivialised with sufficient knowledge of regex.  Personally, I've never really needed to go too deep into that rabits hole, so my solution is more verbose.

**Part 1**

So the basic algorithm here really starts with the data parsing itself.  What we're trying to do is create two separate data sets, one with all the data that appears outside of the brackets, and one with all the data that appears inside of the brackets, bearing in mind there can be multiple brackets.

There's a few different ways of achieving this, the solution I've gone with is array slicing.  Let's assume we have a data set as follows:

    AAA[BBB]CCC[DDD]EEE
    
This basic approach is I'll keep tabs on where the open/close brackets are at any given time as follows with a basic algorithm as follows:

    Initialise the close bracket index to a value of -1 (this will make more sense in a bit)
    
    Set the open bracket index to the location of the first open bracket
    
    WHILE the index of the open bracket is valid
    
      Copy the text between the close bracket index + 1, and the open bracket index -1 to the list of addresses
      
      Set the close bracket index to the next close bracket location
      
      Copy the text between the open bracket index + 1, and the close bracket index -1 to the list of hypernets
      
      Set the open bracket index to the next open bracket location

    END
      
    Copy the text from the last close bracket to the end of the string to the list of addresses

All the +1 / -1 are because we don't want to store the text associated with the brackets themselves.  Once we have this, we're ready for the actual algorithm.

    FOR each IP Address (address/hypernet)
    
      IF any of the Hypernet Addresses contains an ABBA sequence, skip this IP Address
      
      IF any of the Regular Addresses contains an ABBA sequence, increment the number of TLS Addresses by 1
      
    END

Because we took the effort to split this all out, the main algorithm itself ends up being rather trivial.

**Part 2**

So this keeps the parsing step, but the main algorithm is a bit more complex.

    FOR each IP Address (address/hypernet)
    
      FOR each ABA sequence in the Regular Addresses
      
        Calculate the inverse of the ABA sequence (BAB)
        
        IF any of the Hypernet Addresses contains the BAB sequence, increment the number of SSL Addresses by 1,
        then move onto the next IP Address

      END
      
    END

The most important thing to avoid is double counting SSL Addresses if a single IP Address contains multiple ABA/BAB pairs.

**Python vs C++ vs C**

**Python**

Python is pretty well suited to this.  One thing some might have noticed is I tend to use dictionaries like I would a C struct.  It's mostly because Data Classes, which would be the more obvious choice, and whilst I'm sure pretty much everyone is on at least Python 3.7, I'm trying not to overly limit the compatability of the solutions for now.  If it gets silly, I may reverse my stance here.

**C++**

The IP7 functionality tends to be my preferred use case for C++ in general, in that it makes alot of sense to wrap all the functionality associated with this in a class.  Otherwise this is basically the Python solution, just much faster.

**C**

Pretty much the C++ code, but does everything in-place rather than using a class.  This does introduce a few... quirks, such as extractIPAddress allocating data, but expecting the function that called it to know it has to de-allocate it afterwards.  C++ does a much better job of encapsulating this and giving actual ownership of data, whereas in C it's more ambiguous.  Typically most C libraries would also have a clean function to handle this part, but it is still a manual process, whereas with C++, it's all automatic thanks to the fact it's able to automatically trigger clean-up behaviour the moment something goes out of scope.
