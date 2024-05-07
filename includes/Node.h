#pragma once

#include "Top.h"

class Node {
    private:
        int id;
        vector<Node*> connections;
    
    public:
        Node(int nodeId, const vector<Node*>& nodeConnections);
        ~Node();
        void addNode(int id);
        void addConnection(Node* connection);
        int getNodeId() const;
        vector<Node*> getConnections() const;
};
