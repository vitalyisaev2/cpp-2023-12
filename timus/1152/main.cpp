#include <algorithm>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <numeric>
#include <sstream>
#include <vector>

using group = std::vector<std::size_t>;

std::string dump_group(const group& g) {
    std::stringstream ss;
    for (const auto n : g) {
        ss << n << " ";
    }

    return ss.str();
}

std::string dump_groups(const std::vector<group>& gs) {
    std::stringstream ss;
    for (const auto g : gs) {
        ss << "[ ";
        ss << dump_group(g);
        ss << " ]";
    }

    return ss.str();
}

void makeStep(const std::vector<group>& incoming, std::size_t cumulativeCost, std::size_t& optimum) {
    std::cout << "incoming " << dump_groups(incoming) << " cumulativeCost " << cumulativeCost << std::endl; 

    // std::vector<group> transformed;

    // for (const auto& numbers : incoming) {
    //     // if (numbers.size() >= 3) {
    //         transformed.push_back(numbers);
    //     // }
    // }

    if (incoming.size() == 1 && incoming[0].size() <= 3 ) {
        std::cout << "cost " << cumulativeCost << std::endl;
        if (cumulativeCost < optimum) {
            optimum = cumulativeCost;
        }
        return;
    }

    // for (const auto& numbers : transformed) {
    for (std::size_t j = 0; j < incoming.size(); j++) {
        // std::cout << "transformed " << dump_groups(incoming) << std::endl; 

        const auto& numbers = incoming[j];

        for (std::size_t i = 0; i < numbers.size() - 2; i++) {
            std::vector<group> truncated;
            std::size_t addedCost = 0;

            group g1;
            std::copy(numbers.cbegin(), numbers.cbegin() + i, std::back_inserter(g1));
            if (g1.size() > 0) {
                addedCost += std::accumulate(g1.cbegin(), g1.cend(), 0);
                truncated.emplace_back(std::move(g1));
            }

            group g2;
            std::copy(numbers.cbegin() + i + 3, numbers.cend(), std::back_inserter(g2));
            if (g2.size() > 0) {
                addedCost += std::accumulate(g2.cbegin(), g2.cend(), 0);
                truncated.emplace_back(std::move(g2));
            }

            std::vector<group> outgoing;
            for (std::size_t ix = 0; ix < j; ix++) {
                outgoing.push_back(incoming[ix]);
            }
            for (const auto& tr: truncated) {
                outgoing.push_back(tr);
            }
            for (std::size_t ix = j+1; ix < incoming.size(); ix++) {
                outgoing.push_back(incoming[ix]);
            }

            // print_vector(truncated);
            makeStep(outgoing, cumulativeCost + addedCost, optimum);
        }
    }
}

int main() {
    std::size_t N;
    std::cin >> N;

    std::vector<std::size_t> numbers;
    std::size_t number;
    for (; N > 0; N--) {
        std::cin >> number;
        numbers.push_back(number);
    }
    // print_vector(numbers);

    std::size_t optimum = std::accumulate(numbers.cbegin(), numbers.cend(), std::size_t(0));

    makeStep(std::vector<group>{numbers}, 0, optimum);

    std::cout << optimum << std::endl;
}
