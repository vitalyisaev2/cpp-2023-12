#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class TNode {
public:
    TNode()
        : incoming(0)
        , outgoing({}) {
    }

    void AddChild(uint8_t childID) {
        outgoing.push_back(childID);
    }

    void AddParent() {
        incoming++;
    }

private:
    uint8_t incoming;
    std::vector<uint8_t> outgoing;
};

class TGraph {
public:
    void AddLink(uint8_t parentID, uint8_t childID) {
        auto parent = cache.insert(std::make_pair(parentID, TNode()));
        parent.first->second.AddChild(childID);

        auto child = cache.insert(std::make_pair(childID, TNode()));
        parent.first->second.AddParent();
    }

    void Dump() const {
        for (const auto& [id, node] : cache) {
        }
    }

private:
    std::unordered_map<uint8_t, TNode> cache;
};

int main() {
    uint8_t N;
    std::cin >> N;

    TGraph graph;

    std::string line;
    for (uint8_t parentID = 1; parentID <= N; parentID++) {
        std::getline(std::cin, line);
        std::istringstream ss(line);

        uint8_t childID;
        while (ss >> childID) {
            graph.AddLink(parentID, childID);
        }
    }

    return 0;
}
