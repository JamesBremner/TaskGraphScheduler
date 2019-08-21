# TaskGraphScheduler
 
Some windows desktop C++ code based on Izadkhah, H. Learning Based Genetic Algorithm for Task Graph Scheduling https://www.hindawi.com/journals/acisc/2019/6543957/

The application can determine a feasible order of tasks to be performed on a single processor.  Running on the task graph of figure 1 in the Izadkhah paper, the output is

```
tgs ..\data\IzadkhahFig1.dat
T0( 5 ) -> T2( 20 ) costs 3
T0( 5 ) -> T3( 20 ) costs 8
T1( 10 ) -> T3( 20 ) costs 3
T1( 10 ) -> T4( 30 ) costs 10
T1( 10 ) -> T5( 10 ) costs 5
T2( 20 ) -> T6( 5 ) costs 3
T2( 20 ) -> T7( 15 ) costs 8
T3( 20 ) -> T7( 15 ) costs 10
T4( 30 ) -> T7( 15 ) costs 1
T4( 30 ) -> T9( 10 ) costs 10
T5( 10 ) -> T7( 15 ) costs 8
T5( 10 ) -> T9( 10 ) costs 6
T6( 5 ) -> T8( 10 ) costs 5
T7( 15 ) -> T8( 10 ) costs 2
T8( 10 ) -> T11( 15 ) costs 5
T10( 10 ) -> T11( 15 ) costs 8
T10( 10 ) -> T12( 20 ) costs 10
T11( 15 ) -> T12( 20 ) costs 15
Task T0
Task T1
Task T2
Task T3
Task T4
Task T5
Task T6
Task T7
Task T8
Task T9
Task T10
Task T11
Task T12
```
