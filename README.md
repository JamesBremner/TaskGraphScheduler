# TaskGraphScheduler
 
Some windows desktop C++ code based on Izadkhah, H. Learning Based Genetic Algorithm for Task Graph Scheduling https://www.hindawi.com/journals/acisc/2019/6543957/

The application can determine a feasible order of tasks to be performed on multiple processor cores.  Running on the task graph of figure 1 in the Izadkhah paper, the output is for a single processor core

```
tgs ..\data\IzadkhahFig1.dat

Task T0 started at 0 on core 0
Task T0 completed at 5
Task T1 started at 5 on core 0
Task T1 completed at 15
Task T2 started at 15 on core 0
Task T2 completed at 35
Task T3 started at 35 on core 0
Task T3 completed at 55
Task T4 started at 55 on core 0
Task T4 completed at 85
Task T5 started at 85 on core 0
Task T5 completed at 95
Task T6 started at 95 on core 0
Task T6 completed at 100
Task T7 started at 100 on core 0
Task T7 completed at 115
Task T8 started at 115 on core 0
Task T8 completed at 125
Task T9 started at 125 on core 0
Task T9 completed at 135
Task T10 started at 135 on core 0
Task T10 completed at 145
Task T11 started at 145 on core 0
Task T11 completed at 160
Task T12 started at 160 on core 0
Task T12 completed at 180
all tasks complete at 180
all tasks complete at 180
0:  0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 5 5 5 5 5 5 5 5 5 5 6 6 6 6 6 7 7 7 7 7 7 7 7 7 7 7 7 7 7 7 8 8 8 8 8 8 8 8 8 8 9 9 9 9 9 9 9 9 9 9101010101010101010101111111111111111111111111111111212121212121212121212121212121212121212

```

for two processor cores, optimizing with a random search

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
