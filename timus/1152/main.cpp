#include <algorithm>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <numeric>
#include <sstream>
#include <vector>

class TGroup {
public:
    using TData = std::vector<std::size_t>;

    TGroup() = delete;

    explicit TGroup(TData data_)
        : data(std::move(data_)){};

    using TVec = std::vector<TGroup>;

    TVec Shoot(std::size_t position) const {
        TData g1, g2;
        std::copy(data.cbegin(), data.cbegin() + position, std::back_inserter(g1));
        std::copy(data.cbegin() + position + 3, data.cend(), std::back_inserter(g2));

        TVec outgoing;
        std::size_t cost = 0;

        if (g1.size() > 0) {
            outgoing.emplace_back(TGroup(std::move(g1)));
        }

        if (g2.size() > 0) {
            outgoing.emplace_back(TGroup(std::move(g2)));
        }

        return outgoing;
    };

    std::size_t Size() const {
        return data.size();
    }

    bool IsOneShot() const {
        return data.size() <= 3;
    }

    std::size_t Fireback() const {
        return std::accumulate(data.cbegin(), data.cend(), 0);
    }

    std::string Dump() const {
        std::stringstream ss;

        ss << "[";
        for (const auto n : data) {
            ss << n << " ";
        }
        ss << "]";

        return ss.str();
    }

private:
    TData data;
};

class TDisposition {
public:
    TDisposition() = delete;

    TDisposition(TGroup::TVec groups_)
        : groups(std::move(groups_)) {
    }

    std::string Dump() const {
        std::stringstream ss;

        for (const auto g : groups) {
            ss << g.Dump();
            ss << " ";
        }

        return ss.str();
    }

    void MakeStep(std::size_t cumulativeCost, std::size_t& optimum) const {
        std::cout << "Groups: " << Dump() << " Cost: " << cumulativeCost << std::endl;

        if (groups.size() == 1 && groups[0].IsOneShot()) {
            if (cumulativeCost < optimum) {
                optimum = cumulativeCost;
            }

            std::cout << "Update and return" << std::endl;

            return;
        }

        for (std::size_t i = 0; i < groups.size(); i++) {
            for (int j = 0; j < int(groups[i].Size()) - 2; j++) {
                std::cout << "i " << i << " j " << j << " groups.size() " << groups.size() << " groups[i].Size() "
                          << groups[i].Size() << std::endl;
                TGroup::TVec outgoing;

                // copy head
                std::copy(groups.cbegin(), groups.cbegin() + i, std::back_inserter(outgoing));

                // perform shooting
                auto result = groups[i].Shoot(j);

                // copy middle
                std::copy(result.cbegin(), result.cend(), std::back_inserter(outgoing));

                // copy tail
                std::copy(groups.cbegin() + i + 1, groups.cend(), std::back_inserter(outgoing));

                TDisposition newDisposition(std::move(outgoing));

                std::cout << "i: " << i << " j: " << j << " old: " << Dump() << " new: " << newDisposition.Dump()
                          << std::endl;
                newDisposition.MakeStep(cumulativeCost + newDisposition.Fireback(), optimum);
            }
        }
    }

    std::size_t Fireback() const {
        std::size_t cost = 0;
        for (const auto& g : groups) {
            cost += g.Fireback();
        }
        return cost;
    }

private:
    TGroup::TVec groups;
};

int main() {
    std::size_t N;
    std::cin >> N;

    TGroup::TData numbers;
    std::size_t number;
    for (; N > 0; N--) {
        std::cin >> number;
        numbers.push_back(number);
    }
    // print_vector(numbers);

    std::size_t optimum = std::accumulate(numbers.cbegin(), numbers.cend(), std::size_t(0));

    TDisposition disposition(TGroup::TVec{TGroup(std::move(numbers))});

    disposition.MakeStep(0, optimum);

    std::cout << optimum << std::endl;
}
