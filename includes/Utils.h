#pragma once

#include "Top.h"
#include "Node.h"

void printGraph(const vector<Node*>& graph);
unordered_map<int, int> BFS(const vector<Node*>& graph);
void signalingThread(int id, unordered_map<int, int>& plannedBy, deque<Node>& toBeVisited, vector<Node>& visited);
void waitingThread(int id, unordered_map<int, int>& plannedBy, deque<Node>& toBeVisited, vector<Node>& visited);