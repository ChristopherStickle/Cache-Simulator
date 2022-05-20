A small program to demonstrate the inner workings of a cache 
As partly described in CSAPP 3rd edition by Bryant and O'Hallaron

To make things simple, this only deals with reads.

The input, from stdin, is as follows:

The first four lines contain the fundamental parameters of the cache, one per line. 
All four are integers: S E B m 
The next line contains the replacement policy. The only policies implemented are LFU and LRU.

The next two lines contains the fundamental parameters of the hardware: 
the hit time (h) and the miss penalty (p). 
Both are integers: h p 

Next, the runtime information. 
All further lines contain integer addresses (in hex) that the simulation attempts to "read" from "main memory" through the cache, one per line. 
The simulation stops when the "address" read is -1.

All information is then output to stdout in the following format:

For each address read (except the final -1), a line containing the address itself (in hex), a single character: H for cache hit or M for cache miss is output.

After -1 is read, the miss rate of the cache followed by the total cycles spent during the simulation are output.
