Main purpose of this project was practicing of C++ skills.

HOW TO RUN?

Make empty Visual Studio project and place files from includes folder to Header Files dir in Visual Studio (Add Existing Item) and place files from sources folder to Source Files dir in Visual Studio (Add Existing Item). Also, place data1.txt on location of Graphs.cpp 
Then build solution and run.
If you want to disable debug messages, comment out #define DEBUG_ENABLED in Top.h. This will speed up the execution.
If you want to disable multithreading and run algorithm sequentially, comment out #define THREADS_ENABLED
If you want to find the shortest path from starting node to any graph node, change value of int end variable from Graphs.cpp file to any desired node id and path to it will be printed. 
Graph is represented in data1.txt as list of connections between nodes (i.e. startingNodeId endingNodeId). You can put your own data instead of data1.txt (remark: data1.txt needs to be placed where Graphs.cpp is placed)

MAIN IDEA
Visiting of nodes on same layer can be parallelized, since vising of one node is mostly independent of visiting another node on the same layer (visiting means examination of node neighbours and planning of visiting them) 
Point of conflict: deque of nodes which are planned to be visited, since both nodes that are being visited currently can plan visiting of the same node from the next layer 

RESULTS
Because of fact that BFS heavily relies on shared data structures (like queue of already visited nodes and deque of nodes which are planned to be visited), parallelization didn't speed up the execution. 
In order to work properly, threads are constantly syncing by using mutexes, which is causing slowed down execution 

To be done:
try this implementation on huge graph (in which overhead of constant syncing becomes negligible in comparison with time needed to visit all nodes with only one working thread)