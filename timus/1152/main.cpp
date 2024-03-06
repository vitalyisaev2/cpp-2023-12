#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <vector>

class TResult {
public:
    TResult()
        : MinimalDamage(UINT64_MAX) {
    }

    void MaybeUpdate(std::size_t CurrentDamage) {
        MinimalDamage = std::min(MinimalDamage, CurrentDamage);
    }

    bool PossibleImrovement(std::size_t CurrentDamage) const {
        return CurrentDamage < MinimalDamage;
    }

    std::size_t GetValue() const {
        return MinimalDamage;
    }

private:
    std::size_t MinimalDamage;
};

struct TBalcony {
    std::size_t Monsters;
    bool Destroyed;
};

class TDisposition {
public:
    TDisposition(std::vector<TBalcony> balconies)
        : Balconies(std::move(balconies))
        , CumulativeDamage(0){};

private:
    std::size_t CumulativeDamage;
    std::vector<TBalcony> Balconies;

    std::string Dump() const {
        std::stringstream ss;
        ss << "[";
        for (const auto& b : Balconies) {
            if (b.Destroyed) {
                ss << " - ";
            } else {
                ss << " " << b.Monsters << " ";
            }
        }
        ss << "]";
        return ss.str();
    }

    void GetNonDestroyedBalconyIxs(std::vector<std::size_t>& ixs) const {
        ixs.clear();
        ixs.reserve(Balconies.size());
        for (std::size_t ix = 0; ix < Balconies.size(); ix++) {
            if (!Balconies[ix].Destroyed) {
                ixs.push_back(ix);
            }
        }
    }

    TDisposition Shoot(std::size_t start) const {
        auto newDisposition = *this;

        for (std::size_t i = 0; i < 3; i++) {
            newDisposition.Balconies[(start + i) % Balconies.size()].Destroyed = true;
        }

        for (const auto& b : newDisposition.Balconies) {
            if (!b.Destroyed) {
                newDisposition.CumulativeDamage += b.Monsters;
            }
        }

        return newDisposition;
    }

public:
    void Step(TResult& result) {
        // std::cout << "STEP A" << Dump() << " " << CumulativeDamage << std::endl;

        std::vector<std::size_t> starting_points;
        GetNonDestroyedBalconyIxs(starting_points);

        if (starting_points.size() == 0) {
            result.MaybeUpdate(CumulativeDamage);
            return;
        }

        for (auto start : starting_points) {
            TDisposition newDisposition = Shoot(start);
            // std::cout << "STEP B " << start << " " << newDisposition.Dump() << " " << newDisposition.CumulativeDamage
            //           << std::endl;
            if (result.PossibleImrovement(CumulativeDamage)) {
                // std::cout << "STEP C" << std::endl;
                newDisposition.Step(result);
            }
        }
    }
};

int main() {
    std::size_t N;
    std::cin >> N;

    std::vector<TBalcony> balconies;
    std::size_t monsters;
    for (; N > 0; N--) {
        std::cin >> monsters;
        balconies.push_back(TBalcony{.Monsters = monsters, .Destroyed = false});
    }

    TResult result;

    TDisposition(balconies).Step(result);

    std::cout << result.GetValue() << std::endl;
}
