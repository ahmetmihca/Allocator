# Allocator #


I'have implemented a simple allocator library. I kept a linked list as well. However, my linked list will not occupy space inside the heap. I can safely assume that it is disjoint with the heap. Moreover, my linked list will track free chunks and occupied spaces inside the heap. 

Operations of my allocator library will be called concurrently by different threads. Therefore, I need to be careful about data races. I implemented a sequential heap manager first, i.e., it works when processes have a single thread. Then, I add a synchronization mechanism to prevent data races. 

# Sample Outputs #

   # Sample Run 1 #

  This is an example of the very basic case. A thread is allocating 40 Bytes of memory and the execution is finished without using the free function. 

    [-1][100][0]
    Allocated for thread 0
    [0][40][0]---[-1][60][40]
    Execution is done
    [0][40][0]---[-1][60][40]
     
   # Sample Run 2 #
  Addition to the first example, now thread uses a freeMemory function. 

    Memory initialized
    [-1][100][0]
    Allocated for thread 0
    [0][40][0]---[-1][60][40]
    Freed for thread 0
    [-1][100][0]
    Execution is done
    [-1][100][0]
    
   # Sample Run 3 #

  Now there are two threads, first allocating 40 Bytes of memory and uses free. Then allocates 20 Bytes of memory and uses free. 

    [-1][100][0]
    Allocated for thread 0
    [0][40][0]---[-1][60][40]
    Freed for thread 0
    [-1][100][0]
    Allocated for thread 0
    [0][20][0]---[-1][80][20]
    Freed for thread 0
    [-1][100][0]
    Allocated for thread 1
    [1][40][0]---[-1][60][40]
    Freed for thread 1
    [-1][100][0]
    Allocated for thread 1
    [1][20][0]---[-1][80][20]
    Freed for thread 1
    [-1][100][0]
    Execution is done
    [-1][100][0]


  # Sample Run 4 #
Let’s consider more realistic case. There are 5 threads running at the same time. All of them will try to request a memory between 1 and 30 Bytes. If the   allocation is successfully done, thread will sleep random amount of time. When the memory allocated for a thread, It should spend some time to finish the desired job, we can use sleep function to simulate such behaviour. After passing the above allocation request, each thread will create another request for requesting a memory between 1 and 30 Bytes. After the allocation and memory usage is done, successfully allocated threads will free the memory. 

Note that, thread with ID 3 creates a request with 11 Bytes size. However, 4 Bytes available and the allocation is not successful. And the state of the memory is not changed. Also consider the corner cases for the hole management. Check how the holes are merged after the free operations of threads. 


    Memory initialized
    [-1][100][0]
    Allocated for thread 0
    [0][14][0]---[-1][86][14]
    Allocated for thread 1
    [0][14][0]---[1][28][14]---[-1][58][42]
    Allocated for thread 4
    [0][14][0]---[1][28][14]---[4][24][42]---[-1][34][66]
    Allocated for thread 2
    [0][14][0]---[1][28][14]---[4][24][42]---[2][17][66]---[-1][17][83]
    Allocated for thread 3
    [0][14][0]---[1][28][14]---[4][24][42]---[2][17][66]---[3][10][83]---[-1][7][93]
    Allocated for thread 2
    [0][14][0]---[1][28][14]---[4][24][42]---[2][17][66]---[3][10][83]---[2][3][93]
    ---[-1][4][96]
    Can not allocate, requested size 11 for thread 3 is bigger than remaining size
    [0][14][0]---[1][28][14]---[4][24][42]---[2][17][66]---[3][10][83]---[2][3][93]
    ---[-1][4][96]
    Freed for thread 3
    [0][14][0]---[1][28][14]---[4][24][42]---[2][17][66]---[-1][10][83]---[2][3][93]
    ---[-1][4][96]
    Allocated for thread 0
    [0][14][0]---[1][28][14]---[4][24][42]---[2][17][66]---[0][4][83]---[-1][6][87]
    ---[2][3][93]---[-1][4][96]
    Freed for thread 2
    [0][14][0]---[1][28][14]---[4][24][42]---[-1][17][66]---[0][4][83]---[-1][6][87]
    ---[2][3][93]---[-1][4][96]
    Freed for thread 2
    [0][14][0]---[1][28][14]---[4][24][42]---[-1][17][66]---[0][4][83]---[-1][13][87]
    Allocated for thread 1
    [0][14][0]---[1][28][14]---[4][24][42]---[1][1][66]---[-1][16][67]---[0][4][83]
    ---[-1][13][87]
    Allocated for thread 4
    [0][14][0]---[1][28][14]---[4][24][42]---[1][1][66]---[4][13][67]---[-1][3][80]
    ---[0][4][83]---[-1][13][87]
    Freed for thread 0
    [-1][14][0]---[1][28][14]---[4][24][42]---[1][1][66]---[4][13][67]---[-1][3][80]
    ---[0][4][83]---[-1][13][87]
    Freed for thread 0
    [-1][14][0]---[1][28][14]---[4][24][42]---[1][1][66]---[4][13][67]---[-1][20][80]
    Freed for thread 1
    [-1][42][0]---[4][24][42]---[1][1][66]---[4][13][67]---[-1][20][80]
    Freed for thread 1
    [-1][42][0]---[4][24][42]---[-1][1][66]---[4][13][67]---[-1][20][80]
    Freed for thread 4
    [-1][67][0]---[4][13][67]---[-1][20][80]
    Freed for thread 4
    [-1][100][0]
    Execution is done
    [-1][100][0]





