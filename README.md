# TaskGraphScheduler
 
The application searches for an optimal feasible order of tasks without communications costs to be performed on multiple homogeneous processor cores.  

The application reads files in standard task graph format as described http://www.kasahara.cs.waseda.ac.jp/schedule/index.html

Sample run of 100 task sample #2 from 

```
C:\Users\James\code\TaskGraphScheduler\bin>tgs.exe ..\data\stg100\rand0002.stg


Processor with 2 cores
8 tasks are ready to start initially
Searching schedules with first task 0
Searching schedules with first task 1
Searching schedules with first task 2
Searching schedules with first task 3
Searching schedules with first task 4
Searching schedules with first task 7
Searching schedules with first task 8
Searching schedules with first task 12

========================
Best Complete in 288

Core 0:
  7  7  7  4  4  4  4  4  4  4  4  4  4 15 15 15 15 15 15  2  2  2
  2  2  2  2  2  2  2  6  6  6  6  6  6 18 18 18 18 18 18 18 18 18
 18  9  9  9  9  9  9  9  9  9  9 13 13 14 35 35 35 35 35 35 27 27
 27 27 27 27 27 27 27 37 37 37 26 26 26 31 31 31 31 31 31 31 31 31
 31 28 28 28 28 28 28 28 28 28 25 25 25 25 25 25 25 33 33 33 33 33
 33 33 33 33 33 42 42 42 42 42 42 42 42 42 42 36 36 43 43 43 43 47
 47 47 44 44 44 44 44 44 44 44 44 44 62 62 62 62 62 57 57 57 57 54
 54 54 54 54 54 60 60 60 60 60 60 60 49 49 49 55 55 55 61 61 61 61
 61 61 61 58 58 58 58 58 58 58 58 69 64 67 67 67 67 67 67 67 67 67
 66 66 66 66 66 66 66 66 66 66 95 95 95 95 74 74 74 74 76 76 76 76
 71 71 78 78 78 78 78 75 75 75 75 75 75 75 75 75 75 80 80 80 80 80
 80 86 86 86 86 86 86 86 86 86 86 90 90 72 72 88 88 89 89 89 89 89
 89 89 89 89 92 92 92 92 92 92 92 92 94 94 94 94 94 97 97 97 97 97
 97100

Core 1:
  8  8  0  0  0  0  0  0  0  0  0 12 12 12 12 12 10 10  1  1  1  1
  1  1  1  1  1  1  5  5 19 19 19 19 19 19 17 17 17 17 17 17 17 17
 17 17 38 11 11 11 11 11 11 11 16 22 22 22 22 22 22 20 23 23 23 23
 23 23 23 23 23 23 24 24 24 34 34 34 34 34 34 34 34 34 21 21 21 21
 21 21 21 21 32 32 32 32 32 32 32 32 32 32 46 46 46 46 46 46 29 29
 41 41 41 41 41 41 41 41 41 41 40 40 40 40 40 40 45 45 45 45 45 45
 45 45 52 52 48 48 48 48 48 51 51 51 51 51 51 51 51 30 30 30 30 30
 30 39 39 39 39 39 39 59 59 50 50 50 53 53 53 53 53 53 53 56 56 56
 56 70 70 70 70 70 70 70 70 70 70 63 63 63 73 73 73 73 73 77 77 77
 77 77 65 65 65 65 65 65 65 65 65 68 68 68 68 68 68 68 79 79 79 79
 79 79 79 79 79 82 82 82 82 82 82 82 82 82 81 81 81 87 87 87 87 87
 87 93 93 93 93 93 93 93 83 83 84 84 84 84 84 84 84 84 84 91 91 91
 91 91 85 85 85 85 85 85 85 85 85... 96 99 99 99 99 99 99 99 98 98
 98 98
```

Note that the optimal schedule, complete in 288, has been found!
