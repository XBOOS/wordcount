# wordcount
Use multiprocess to count word in a key file.   

1. Simply fork processes and assign each word to a different process    
2. Maintain fixed number of processes and use signals to synchronize between the work delegation of parents and worker processes
