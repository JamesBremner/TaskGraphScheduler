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
  0  0 12  1  1  1  1  4  1  2  7  0 15  5  0 12  0 19  8 18  1 10
 18  5 18 10 38 15 17  5 10  8 17 15 10 18 17 17 11 11 11 12 15 38
  9 18 38 17 19 15  9 15 20 31 13 15 14 22 34 16 14 22 34 23 26 34
 23 21 27 27 31 37 21 31 31 25 21 31 32 28 23 24 30 25 37 26 46 28
 30 32 30 21 26 28 28 29 28 30 32 25 28 25 24 28 33 29 28 46 29 33
 46 42 46 42 36 29 41 42 29 36 42 46 43 36 40 41 39 40 41 43 45 47
 44 45 39 39 51 36 40 39 45 51 44 44 43 62 62 47 47 48 50 47 45 50
 50 57 55 62 60 60 50 53 54 62 49 57 61 55 59 59 49 61 61 70 55 55
 56 58 56 56 54 70 58 58 58 70 59 58 58 61 64 64 65 69 63 71 63 65
 63 77 77 63 71 65 66 67 77 66 95 71 72 72 95 66 68 74 68 75 85 73
 75 79 72 68 85 75 75 74 78 75 78 78 78 76 79 95 82 95 78 82 87 82
 84 82 93 86 93 87 86 83 86 93 86 83 80 88 83 90 83 93 87 86 84 90
 89 86 89 86 88 90 86 90 89 91 90 92 92 89 92 91 92 97 92 99 94 96
 98 99 96 99 94 94 97 96 98100100100 94 97 98100100 96 98100 98100
100100100100100

Core 1:
  2  2  7  1  1  2  2  4  1 18  3  2  4  4  4  8  7  3  0  8 12 18
  4  6 18  6 19  0 38 10 10  6 17 19 10  8 18  9 19  9 10  9 13  9
 35 13 17 11 11 35 14 16 20 37 38 16 20 37 35 16 21 16 20 34 20 27
 31  9 32 24 24 37 21 34  9 34 24 22 34 23 24 25 25 26 22 24 32 27
 26 32 24 31 28 26 32 23 35 26 26 30 32 34 24 46 33 33 29 30 37 29
 29 41 33 41 36 26 40 42 40 42 41 42 36 41 43 40 40 40 44 40 41 39
 44 45 40 43 48 43 52 51 47 52 48 60 47 51 51 57 52 51 45 54 62 48
 60 55 49 48 55 50 54 57 49 57 49 53 55 53 53 59 55 50 56 70 53 57
 61 59 70 70 54 56 60 58 58 58 59 63 58 71 63 69 77 64 71 67 67 71
 66 67 77 67 77 63 72 73 71 65 72 68 85 66 72 72 85 85 76 72 85 73
 78 74 79 74 74 67 79 75 75 76 78 77 76 75 72 75 83 80 76 79 81 79
 80 78 82 78 83 79 81 87 81 93 87 87 86 82 82 83 88 93 90 90 91 88
 86 84 88 84 89 89 89 89 91 91 93 90 89 92 89 89 90 91 91 94 97 97
 94 94 94 98 96100 98 98 99 94 94100 96 97 98 96 98100100 99100100
100100100
```

Note that the optimal schedule, complete in 288, has been found!
