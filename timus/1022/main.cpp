#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class TNode {
public:
    TNode(uint32_t id)
        : id(id)
        , incoming(0)
        , outgoing({}) {
    }

    void AddChild(uint32_t childID) {
        outgoing.push_back(childID);
    }

    void AddParent() {
        incoming++;
    }

    void DropParent() {
        incoming--;
    }

    const std::vector<uint32_t>& GetChildren() const {
        return outgoing;
    }

    bool operator<(const TNode& other) const {
        return this->incoming > other.incoming;
    }

    uint32_t GetID() const {
        return id;
    }

private:
    uint32_t id;
    uint32_t incoming;
    std::vector<uint32_t> outgoing;
};

class TGraph {
public:
    void AddParent(uint32_t parentID) {
        cache.insert(std::make_pair(parentID, TNode(parentID)));
    }

    void AddChild(uint32_t parentID, uint32_t childID) {
        auto parent = cache.insert(std::make_pair(parentID, TNode(parentID)));
        parent.first->second.AddChild(childID);

        auto child = cache.insert(std::make_pair(childID, TNode(childID)));
        child.first->second.AddParent();
    }

    void DropParent(uint32_t parentID) {
        const auto parent = cache.find(parentID);
        for (const auto childID : parent->second.GetChildren()) {
            auto child = cache.find(childID);
            std::cout << "Dropping parent " << parentID << " for child " << childID << std::endl;
            child->second.DropParent();
        }
    }

    std::vector<uint32_t> TopologicalSort() {
        std::vector<uint32_t> results;
        std::vector<TNode> nodes_heap;

        std::for_each(cache.begin(), cache.end(),
                      [&](const std::pair<uint32_t, TNode>& pair) { nodes_heap.push_back(pair.second); });

        std::make_heap(nodes_heap.begin(), nodes_heap.end());

        while (nodes_heap.size() > 0) {
            std::pop_heap(nodes_heap.begin(), nodes_heap.end());
            auto& minNode = nodes_heap.back();
            for (auto targetChildID: minNode.GetChildren()) {
                for (auto& child: nodes_heap) {
                    if (child.GetID() == targetChildID) {
                        child.DropParent();
                    }
                }
            }
            nodes_heap.pop_back();
            results.push_back(minNode.GetID());
            std::make_heap(nodes_heap.begin(), nodes_heap.end());
        }

        return results;
    };

private:
    std::unordered_map<uint32_t, TNode> cache;
};

int main() {
    uint32_t N;
    std::cin >> N;

    std::string line;
    std::getline(std::cin, line);

    TGraph graph;

    for (uint32_t parentID = 1; parentID <= N; parentID++) {
        std::getline(std::cin, line);
        std::istringstream ss(line);

        graph.AddParent(parentID);

        uint32_t childID;
        while (ss >> childID) {
            if (childID == 0) {
                break;
            }

            graph.AddChild(parentID, childID);
        }
    }

    const auto& topSort = graph.TopologicalSort();

    for (std::size_t i = 0; i < topSort.size(); i++) {
        std::cout << topSort[i];
        if (i != topSort.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;

    return 0;
}
