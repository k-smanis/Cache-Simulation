# Cache Simulator Overview
This project is a simulation of the operation of a direct mapped cache controller on an embedded system.
The simulation program analyses the impact of cache memory size and cache memory block size on the performance of the embedded system when executing a bubble sort algorithm. The analysis considers a range of cache performance metrics from hit/miss rates through to 
speed-up factors. For the purpose of calculating execution times, the assumption is that the main 
memory has access time of 80 ns, and the cache memory has an access time of 8 ns.


# Embedded system memory architecture
The embedded system is interfaced to a 128 Ki x 16-bit external data memory using a 20-bit 
address bus and a 16-bit data bus. The embedded system contains a direct mapped cache 
controller for data accesses, and the controller can be configured in one of the 12 modes listed in 
the following table.
The cache controller uses a write-back, write-allocate policy for cache write misses.

| Mode ID | Cache block size (16-bit words) | Number of cache blocks | Cache size (16-bit words) |
| --- | --- | --- | --- |
| 1 | 4 | 128 | 512 |
| 2 | 8 | 64 | 512 |
| 3 | 16 | 32 | 512 |
| 4 | 32 | 16 | 512 |
| 5 | 64 | 8 | 512 |
| 6 | 128 | 4 | 512 |
| 7 | 16 | 4 | 64 |
| 8 | 16 | 8 | 128 |
| 9 | 16 | 16 | 256 |
| 10 | 16 | 64 | 1024 |
| 11 | 16 |128 | 2048 |
| 12 | 16 |256 | 4096 |

# Credit
This project was part of my BEng studies.
The memory access data were also provided as part of this class.
However, as they are not part of my work, they will not be provided in this repo.
