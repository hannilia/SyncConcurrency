# CS441/541 Synchronization Project

## Author(s):

Liam Hannigan

## Date:

05/05/23


## Description:

This is a solution to the finicky voters semaphore problem, when based on a number of voters from various parties and a number of booths but parties can only line up with their party to vote. See pdf for full project description.


## How to build the software

Run make, then ./finicky-voters followed by the number of booths, democrats, republicans, then independents. All optional.


## How to use the software

Number of booths defaults to 10, all party voters default to 5 if not supplied to command line. Run ./finicky-voters #booths #dems #reps #inds #libs.


## How the software was tested

  For testing exclusion from different parties while voting, the print_state demonstrated if party voters were voting with each other by displaying the status of the booths. Testing was largely based on print statements and following the input to see if it matched the hypothetical structure that should result from running an input.
  
  Starvation was prevented by enforcing a queue that keep track of the total arrival of all threads; voters of the voting party would be released from the queue equal the number of booths. Then, the next party waiting in line would be next to be released, therefore enabling a booth number depedendent turnstile, ensuring no voters are skipped.
  
   Deadlock was prevented by using a single queue semaphore to control all access to queue variables. A couple global ints kept track of the status of the queue (first arrival, empty queue) and a central driver thread called each voter's unique semaphore to ensure the queue was fair and to enforce voting exclusion. A semaphore with value equal to the number of booths kept two voters from overlaping in a booth. 
   
   Busy waiting was prevented by using global semaphores to signal everything, with threads having multiple critical sections. Printing, changing the queue, and changing booths could only happen by one thread at a time. 
   
   Independents can line up behind any party, but to keep things fair, once a member of a non-voting party is infront of an independent they can't skip the line.
   If their is a long line of democrats and a republican arrives, the republican won't be able to cut infront like a traditional turnstile. The democrats would be allowed to vote, repeatedly free'd from the queue based on the number of open booths, until the republican was next in line. (Or for any party). This is to ensure both fairness and throughput, because all booths can be filled for throughput but ultimately first in line in the queue will always be the party that votes.


## Known bugs and problem areas

Not every function is error checked to see if it returns the correct value(such as pthread create), additionally, memory leaks because memory is not free'd.

## Special Section

For the special solution, I added another command input for Libertarians, and because of the way the voter threads are set up, I just treated them as another party, since they don't follow special rules like indepdendents. I didn't have to change any code besides creating and joining the libertarians threads, and adjusting the print statements.
