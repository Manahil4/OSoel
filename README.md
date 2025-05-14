
###**Introduction** 
The simulation demonstrates how operating systems allocate, manage, and swap
memory pages between RAM and Virtual Memory (VM) for multiple processes. The system
ensures optimized page allocation, avoids fragmentation, and facilitates smooth swapping of
leftover pages.

<hr/>
###**Features and Challenges**
The system minimizes fragmentation through smart limit calculations during page allocation
and supports dynamic, efficient swapping of pending pages. Real-time memory visualization
after each operation enhances transparency. Key challenges included avoiding fragmentation,
addressed by adjusting limits based on allocation gaps, and managing leftover pages without
disturbing existing frames, solved through circular replacement and careful tracking via
framesAllocated[]. Realistic arrival and paging behavior were achieved by structured
file-based data loading
