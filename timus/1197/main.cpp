#include <iostream>
#include <array>
#include <vector>

struct TCell {
    TCell() = delete;

    TCell(int x, int y)
        : x(x)
        , y(y) {
    }

    explicit TCell(const std::string& position) {
        char vertical = position[0];
        this->x = vertical - 'a';
        char horisontal = position[1];
        this->y = horisontal - '0' - 1;
    }

    bool IsValid() const {
        return ((x >= 0 && x < 8) && (y >= 0 && y < 8));
    }

    int x, y;
};

struct THorse {
    explicit THorse(const std::string& position)
        : own(TCell(position)) {
    }

    std::size_t UnderAttack() const {
        std::array<TCell, 8> attacked = {
            TCell(own.x + 1, own.y + 2), TCell(own.x + 2, own.y + 1), TCell(own.x + 2, own.y - 1),
            TCell(own.x + 1, own.y - 2), TCell(own.x - 1, own.y - 2), TCell(own.x - 2, own.y - 1),
            TCell(own.x - 2, own.y + 1), TCell(own.x - 1, own.y + 2),
        };
        std::size_t result = 0;
        for (const auto& cell : attacked) {
            if (cell.IsValid()) {
                result++;
            }
        }

        return result;
    }

private:
    TCell own;
};

int main() {
    std::size_t N;
    std::cin >> N;

    std::vector<THorse> horses;
    horses.reserve(N);

    std::string position;
    for (std::size_t i = 0; i < N; i++) {
        std::cin >> position;
        horses.push_back(THorse(position));
    }

    for (const auto& horse: horses) {
        std::cout << horse.UnderAttack() << std::endl;
    }
}
