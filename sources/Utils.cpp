#include "Utils.h"

void printGraph(const vector<Node*>& graph) {
	int prev = 0;
	for (int i = 0; i < graph.size(); i++) {
		if ((*(graph[i])).getNodeId() != prev) {
			prev = (*(graph[i])).getNodeId();
			cout << (*(graph[i])).getNodeId() << " (";
		}
		for (int j = 0; j < (*(graph[i])).getConnections().size(); j++) {
			cout << (*((*(graph[i])).getConnections()[j])).getNodeId() << " ";
		}
		cout << ")" << endl;
	}
}

mutex mtx;
mutex mtx_ready;
condition_variable cv;
bool ready = false;

void waitingThread(int id, unordered_map<int, int>& plannedBy, deque<Node>& toBeVisited, vector<Node>& visited) {

	DWORD_PTR mask;
	DWORD_PTR systemMask;
	GetProcessAffinityMask(GetCurrentProcess(), &mask, &systemMask);
	DWORD_PTR threadMask = GetCurrentProcessorNumber();

	{
		unique_lock<mutex> lock(mtx_ready);
		cv.wait(lock, [] { return ready; }); // waiting here until receive signal
	}
	
	/* processing section - mtx locked from previous section */
	
	mtx.lock();
	DEBUG_MSG("Waiting thread: signal received");
	DEBUG_MSG(YELLOW << "Waiting thread: running on core " << threadMask << RESET);
	while (!toBeVisited.empty()) {
		Node currentlyVisiting = toBeVisited.front();
		toBeVisited.pop_front();
		visited.push_back(currentlyVisiting);
		DEBUG_MSG("Thread " << id << ": currently visiting: " << currentlyVisiting.getNodeId());
		
		deque<Node> toBeVisitedLocal(toBeVisited);
		vector<Node> visitedLocal(visited);
		mtx.unlock();

		/* plan visiting each connection */
		for (int i = 0; i < currentlyVisiting.getConnections().size(); i++) {
			/* node found, check if it is already visited */
			int j = 0;
			for (j = 0; j < visitedLocal.size(); j++) {
				if ((*(currentlyVisiting.getConnections()[i])).getNodeId() == visitedLocal[j].getNodeId()) {
					/* node was already visited, skip it */
					break;
				}
			}
			if (j == visitedLocal.size()) {
				/* node wasn't visited, check if already planned */
				int k = 0;
				for (k = 0; k < toBeVisitedLocal.size(); k++) {
					if ((*(currentlyVisiting.getConnections()[i])).getNodeId() == toBeVisitedLocal[k].getNodeId()) {
						/* visiting was already planned in some moment (case when some node is connected with two nodes on previous layer) */
						break;
					}
				}
				if (k == toBeVisitedLocal.size()) {
					mtx.lock();					
					/* here, toBeVisited might be already updated with Node that we want to add by another thread, check needed (we will check last 3 elements, because we keep things synchronized) */
					bool found = false;
					int h = toBeVisited.size() - 1;
					int count = 0;
					while ((h >= 0) && (count < 3)) {
						count++;
						if (toBeVisited[h].getNodeId() == (*(currentlyVisiting.getConnections()[i])).getNodeId()) {
							found = true;
							break;
						}
						h--;
					}
					if (!found) {
						DEBUG_MSG("Thread " << id << ": updating toBeVisited with: " << (*(currentlyVisiting.getConnections()[i])).getNodeId());
						toBeVisited.push_back(*(currentlyVisiting.getConnections()[i]));
						plannedBy[(*(currentlyVisiting.getConnections()[i])).getNodeId()] = currentlyVisiting.getNodeId();
					}
					mtx.unlock();
				}
			}
		}
		mtx.lock(); // we will check toBeVisited shared variable in while condition
	}
	mtx.unlock();
}

void signalingThread(int id, unordered_map<int, int>& plannedBy, deque<Node>& toBeVisited, vector<Node>& visited) {

	DWORD_PTR mask;
	DWORD_PTR systemMask;
	GetProcessAffinityMask(GetCurrentProcess(), &mask, &systemMask);
	DWORD_PTR threadMask = GetCurrentProcessorNumber();
	
	/* processing section - mtx locked from previous section */
	mtx.lock(); // waiting here negligible
	DEBUG_MSG(YELLOW << "Signaling thread: running on core " << threadMask << RESET);
	while (!toBeVisited.empty()) {
		Node currentlyVisiting = toBeVisited.front();
		toBeVisited.pop_front();
		visited.push_back(currentlyVisiting);
		
		// time measuring of cout  
		auto start = chrono::high_resolution_clock::now();
		DEBUG_MSG("Thread " << id << ": currently visiting: " << currentlyVisiting.getNodeId());
		auto end = chrono::high_resolution_clock::now();
		chrono::duration<double> duration = end - start;
		DEBUG_MSG(GREEN << "Signaling thread: waited " << duration.count() * 1000 << " miliseconds" << RESET);
	
		// copying to local variables - up to 0.05 milliseconds for current graph
		deque<Node> toBeVisitedLocal(toBeVisited);
		vector<Node> visitedLocal(visited);		
		mtx.unlock();

		/* plan visiting each connection */
		for (int i = 0; i < currentlyVisiting.getConnections().size(); i++) {
			/* node found, check if it is already visited */
			int j = 0;
			for (j = 0; j < visitedLocal.size(); j++) {
				if ((*(currentlyVisiting.getConnections()[i])).getNodeId() == visitedLocal[j].getNodeId()) {
					/* node was already visited, skip it */
					break;
				}
			}
			if (j == visitedLocal.size()) {
				/* node wasn't visited, check if already planned */
				int k = 0;
				for (k = 0; k < toBeVisitedLocal.size(); k++) {
					if ((*(currentlyVisiting.getConnections()[i])).getNodeId() == toBeVisitedLocal[k].getNodeId()) {
						/* visiting was already planned in some moment (case when some node is connected with two nodes on previous layer) */
						break;
					}
				}
				if (k == toBeVisitedLocal.size()) {					
					mtx.lock(); // waiting here around 0.5 - 1 millisec!
					/* here, toBeVisited might be already updated with Node that we want to add by another thread, check needed (we will check last 3 elements, because we keep things synchronized) */
					bool found = false;
					int h = toBeVisited.size() -1;
					int count = 0;
					while ((h >= 0) && (count < 3)) {
						count++;
						if (toBeVisited[h].getNodeId() == (*(currentlyVisiting.getConnections()[i])).getNodeId()) {
							found = true;
							break;
						}
						h--;
					}
					if (!found) {
						DEBUG_MSG("Thread " << id << ": updating toBeVisited with: " << (*(currentlyVisiting.getConnections()[i])).getNodeId());
						toBeVisited.push_back(*(currentlyVisiting.getConnections()[i]));
						plannedBy[(*(currentlyVisiting.getConnections()[i])).getNodeId()] = currentlyVisiting.getNodeId();
					}					
					mtx.unlock();
				}
			}
		}
		
		mtx.lock(); // we will check toBeVisited shared variable in while condition - waiting here can go up to 2 millisec!
		{
			lock_guard<mutex> lock(mtx_ready);
			if (!ready && toBeVisited.size() > 2) {				
				ready = true;
				DEBUG_MSG("Signaling thread: Signal sent!");
				cv.notify_all(); // Notify all waiting threads
			}
		}
	}
	mtx.unlock();
}

unordered_map<int, int> BFS(const vector<Node*>& graph) {
	
	unordered_map<int, int> plannedBy;
	deque<Node> toBeVisited;
	vector<Node> visited;
	
	toBeVisited.push_back((*(graph[0])));

#ifdef THREADS_ENABLED
	thread thread1(signalingThread, 0, ref(plannedBy), ref(toBeVisited), ref(visited));
	thread thread2(waitingThread, 1, ref(plannedBy), ref(toBeVisited), ref(visited));

	// Wait for threads to finish
	thread1.join();
	thread2.join();
#else
	while (!toBeVisited.empty()) {
		Node currentlyVisiting = toBeVisited.front();
		toBeVisited.pop_front();
		visited.push_back(currentlyVisiting);

		/* plan visiting each connection */
		for (int i = 0; i < currentlyVisiting.getConnections().size(); i++) {
			/* node found, check if it is already visited */
			int j = 0;
			for (j = 0; j < visited.size(); j++) {
				if ((*(currentlyVisiting.getConnections()[i])).getNodeId() == visited[j].getNodeId()) {
					/* node was already visited, skip it */
					break;
				}
			}
			if (j == visited.size()) {
				/* node wasn't visited, check if already planned */
				int k = 0;
				for (k = 0; k < toBeVisited.size(); k++) {
					if ((*(currentlyVisiting.getConnections()[i])).getNodeId() == toBeVisited[k].getNodeId()) {
						/* visiting was already planned in some moment (case when some node is connected with two nodes on previous layer) */
						break;
					}
				}
				if (k == toBeVisited.size()) {
					toBeVisited.push_back(*(currentlyVisiting.getConnections()[i]));
					plannedBy[(*(currentlyVisiting.getConnections()[i])).getNodeId()] = currentlyVisiting.getNodeId();
				}						
			}
		}
	}
#endif

	return plannedBy;
}