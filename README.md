# Feeder-Consumers-Problem
I create a programm that produces processes in order to create this architecture of interprocessing communication:

![Screenshot from 2020-09-17 13-41-09](https://user-images.githubusercontent.com/60033683/93460228-93dda180-f8eb-11ea-87a2-28748bc3c0c6.png)

The Feeder process creates an array of size M (M> 3000) with random integers. This array should be reproduced in exactly the same sequence in processes Ci (C1 ... Cn). The shared memory, through which the processes communicate with each other, allows the storage of only one integer as well as the time stamp that accompanies it. The time stamp that accompanies each integer is initialized when it is stored in shared memory by the Feeder process. The Ci process, when it recovered the contents of the shared memory, then it will compare this time stamp with the current time and will determine the time difference (delay). Each process will maintain a moving average of running time (running average) in which time differences (delays) will accumulate. Whe one processe complete its M iterations, then it print in a file the sequence of integers, the PID as well as the specified average (which is also printed in the terminal). The Feeder process completes after all Ci are terminated by deallocate the shared memory and the memory of all synchronization structures.

**You can run multiple program executions for different values of n (num_of_processes) and M (num_of_integers_in_array) and observe and compare the results with each other**

## Execution

1) **make**

2) **./feeder num_of_integers_in_array num_of_processes**

3) **make clean**
