#include <cassert>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <algorithm>

#include <utility>
#include <vector>

constexpr std::size_t seatsPerCarriage = 36;

struct TInterval {
    std::size_t first;
    std::size_t last;
    std::size_t tickets;
};

class TRoute {
public:
    TRoute(std::size_t stations) {
        intervals.reserve(stations);
        for (std::size_t i = 1; i <= stations; i++) {
            intervals.emplace_back(TInterval{i, i + 1, 0});
        }
    }

    void Register(std::size_t first, std::size_t last, std::size_t tickets) {
        std::size_t start = std::min(first, last);
        std::size_t end = std::max(first, last);
        for (std::size_t i = start; i < end; i++) {
            intervals[i].tickets += tickets;
        }
    }

    std::size_t MaxTicketsPerInterval() const {
        auto iter = std::max_element(intervals.cbegin(), intervals.cend(),
                                     [](const TInterval& a, const TInterval& b) { return a.tickets < b.tickets; });
        return iter->tickets;
    }

private:
    std::vector<TInterval> intervals;
};

class TTicketMatrix {
public:
    TTicketMatrix(std::size_t stations) :Stations(stations) {
        Rows.reserve(stations);
    }

    void AddRow(const std::string& line) {
        std::string target;
        std::stringstream ss(line);
        std::vector<std::size_t> row;
        row.reserve(Stations);

        while (std::getline(ss, target, ' ')) {
            std::size_t value = std::stoi(target);
            row.push_back(value);
        }

        Rows.emplace_back(std::move(row));
    }

    std::pair<TRoute, TRoute> ToRoutes() const {
        TRoute routeForward(Stations);
        TRoute routeBackward(Stations);

        assert(Rows.size() == Stations);
        for (std::size_t i = 0; i < Stations; i++) {
            assert(Rows[i].size() == Stations);
            for (std::size_t j = 0; j < Stations; j++) {
                auto tickets = Rows[i][j];
                if (i < j) {
                    routeForward.Register(i, j, tickets);
                } else if (i > j) {
                    routeBackward.Register(i, j, tickets);
                }
            }
        }

        return std::make_pair(std::move(routeForward), std::move(routeBackward));
    }

private:
    std::size_t Stations;
    std::vector<std::vector<std::size_t>> Rows;
};

int main() {
    std::size_t stations;
    std::cin >> stations;

    TTicketMatrix ticketMatrix(stations);

    std::string target;
    while (std::getline(std::cin, target)) {
        if (target.size()) {
            ticketMatrix.AddRow(target);
        }
    }

    auto result = ticketMatrix.ToRoutes();
    auto maxTickets = std::max(result.first.MaxTicketsPerInterval(), result.second.MaxTicketsPerInterval());

    if (maxTickets % seatsPerCarriage == 0) {
        std::cout << maxTickets / seatsPerCarriage << std::endl;
    } else {
        std::cout << maxTickets / seatsPerCarriage + 1 << std::endl;
    }
}
