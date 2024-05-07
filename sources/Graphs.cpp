#include "Node.h"
#include "Utils.h"

int main()
{
    ifstream file("data1.txt");

    if (!file.is_open()) {
        cerr << "Unable to open file!" << endl;
        return 1;
    }

    // graph description
    vector<Node*> graph;
    string line;

    /* creating graph */
    while (getline(file, line)) {
        istringstream iss(line);
        int id;
        int connection;
        iss >> id >> connection;

        size_t i;
        size_t j;
        for (i = 0; i < graph.size(); ++i) {
            if ((*(graph[i])).getNodeId() == id) {
                /* node already exists, end searching */
                break;
            }
        }
        /* after the loop, i contains index of found node or size of graph (if node is not yet inserted) */

        if (i == graph.size()) {
            /* node not found in graph, add it */
            vector<Node*> connections; // life time extended, because Node will live outside if block
            graph.push_back(new Node(id, connections));
        }

        /* search for connection, if it already exists as node */
        for (j = 0; j < graph.size(); ++j) {
            if ((*(graph[j])).getNodeId() == connection) {
                /* node already exists, end searching */
                break;
            }
        }

        if (j == graph.size()) {
            /* node not found in graph, add it */
            vector<Node*> connections;
            graph.push_back(new Node(connection, connections));
        }

        (*(graph[i])).addConnection(graph[j]);
    }    

    file.close();

    /* find shortest path between node start and node end */
    int start = 1;
    int end = 16;

    auto startingMoment = chrono::high_resolution_clock::now();
    unordered_map<int, int> plannedBy = BFS(graph);
    auto endingMoment = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = endingMoment - startingMoment;

    // Print the duration in milliseconds
    cout << "Time taken: " << duration.count() * 1000 << " milliseconds" << endl;

    while (end != start) {
        cout << end << endl;
        end = plannedBy[end];
    }
    cout << start << endl;

    //printGraph(graph);
}