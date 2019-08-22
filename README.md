# TaskGraphScheduler
 
Some windows desktop C++ code based on Izadkhah, H. Learning Based Genetic Algorithm for Task Graph Scheduling https://www.hindawi.com/journals/acisc/2019/6543957/

The application can determine a feasible order of tasks without communications costs to be performed on multiple homogeneous processor cores.  Running on the task graph of figure 1 in the Izadkhah paper with two processor cores, optimizing with a random search

```
tgs ..\data\IzadkhahFig1.dat

Best 110

Core 0:
 1 1 1 1 1 1 1 1 1 1 5 5 5 5 5 5 5 5 5 5 4 4
 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4
 4 4 4 4 4 4 9 9 9 9 9 9 9 9 9 9101010101010
10101010..........11111111111111111111111111
11111212121212121212121212121212121212121212


Core 1:
 0 0 0 0 0 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2
 2 2 2 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3
 3 6 6 6 6 6 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 8
 8 8 8 8 8 8 8 8 8
```

Note that this is a better schedule than in figure 2 of the paper.  The task graph is completed in 110 time units, rather than 140.

The program can also read files in standard task graph format as described http://www.kasahara.cs.waseda.ac.jp/schedule/format_e.html#nocomm
