# wordcount
#Description
Use multiprocess to count word in a key file.   

1. Simply fork processes and assign each word to a different process    
2. Maintain fixed number of processes and use signals to synchronize between the work delegation of parents and worker processes

3. as for part2 the result is like below


```bash
Child 95541 will start to work
Child 95542 will start to work
Child 95544 will start to work
Child 95543 will start to work
Child 95545 will start to work
Child 95546 will start to work
Child 95547 will start to work
Child 95548 will start to work
Child 95549 will start to work
Child 95550 will start to work
Child process (95541) exited and it searched 5 words
Child process (95542) exited and it searched 6 words
Child process (95543) exited and it searched 5 words
Child process (95544) exited and it searched 5 words
Child process (95545) exited and it searched 4 words
Child process (95546) exited and it searched 5 words
Child process (95547) exited and it searched 5 words
Child process (95548) exited and it searched 5 words
Child process (95549) exited and it searched 5 words
Child process (95550) exited and it searched 5 words


bitmap : 106
segment : 140
PIECES : 329
semaphore : 51
lottery : 34
atomic : 29
synchronous : 0
hypervisor : 3
multiprogramming : 13
associative : 3
EASY : 359
reference : 77
parent : 75
frame : 37
signal : 46
miss : 118
round-robin : 7
lookaside : 5
directory : 204
livelock : 7
scheduling : 113
priority : 68
garbage : 18
working : 45
inode : 280
virtual : 290
mutex : 141
replacement : 45
thrashing : 4
least : 52
lock : 413
superblock : 15
kernel : 81
consumer : 71
thread : 359
locality : 27
preemptive : 6
translation : 74
multithreaded : 5
synchronization : 21
Linux : 57
journaling : 48
page : 793
deadlock : 58
MMU : 7
interrupt : 55
privilege : 9
fault : 68
trap : 99
zombie : 3

Total elapsed time: 175.95 ms

```
#Acknowledgement  

Starter code from OS project.   

Credit to Dr.Anthony Tam
