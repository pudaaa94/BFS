#include "Node.h"

// Constructor definition with initializer list
Node::Node(int nodeId, const vector<Node*>& nodeConnections) : id(nodeId), connections(nodeConnections) {
    // Constructor body (if needed)
}

Node::~Node() {
    // Destructor body (if needed)
}

void Node::addNode(int nodeId) {
    id = nodeId;
}

void Node::addConnection(Node* connection) {
    connections.push_back(connection);
}

int Node::getNodeId() const {
    return id;
}

vector<Node*> Node::getConnections() const {
    return connections;
}