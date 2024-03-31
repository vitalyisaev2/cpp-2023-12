#!/usr/bin/env python3

import sys
import dataclasses

from typing import Sequence, Final, Tuple

seats_per_carriage: Final = 36


@dataclasses.dataclass
class Interval:
    first: int
    last: int
    tickets: int


@dataclasses.dataclass
class Route:
    intervals: Sequence[Interval]

    @classmethod
    def build(cls, stations: int):
        intervals = [
            Interval(first=i, last=i + 1, tickets=0) for i in range(1, stations)
        ]
        return cls(intervals=intervals)

    def register(self, first: int, last: int, tickets: int):
        start = min(first, last)
        end = max(first, last)
        for i in range(start, end):
            # print(i, first, last, tickets)
            self.intervals[i].tickets += tickets
        # print(self.intervals)

    @property
    def max_tickets_on_interval(self) -> int:
        return max((i.tickets for i in self.intervals))


@dataclasses.dataclass
class TicketMatrix:
    rows: Sequence[Sequence[int]]

    def add_row(self, line: str):
        row = [int(w) for w in line.split()]
        self.rows.append(row)

    def to_routes(self) -> Tuple[Route, Route]:
        size = len(self.rows)
        route_forward = Route.build(size)
        route_backward = Route.build(size)

        for i in range(size):
            for j in range(size):
                tickets = self.rows[i][j]
                if i < j:
                    # print(">> FORWARD", i, j, tickets)
                    route_forward.register(i, j, tickets)
                elif i > j:
                    # print(">> BACKWARD", i, j, tickets)
                    route_backward.register(i, j, tickets)

        return (route_forward, route_backward)


def main():
    _ = int(sys.stdin.readline())

    ticket_matrix = TicketMatrix([])

    for line in sys.stdin.readlines():
        ticket_matrix.add_row(line)

    (route_forward, route_backward) = ticket_matrix.to_routes()

    # print(route_forward)
    # print(route_backward)

    max_tickets = max(
        route_forward.max_tickets_on_interval,
        route_backward.max_tickets_on_interval,
    )

    if max_tickets % seats_per_carriage == 0:
        print(int(max_tickets / seats_per_carriage))
    else:
        print(int(max_tickets / seats_per_carriage) + 1)


if __name__ == "__main__":
    main()
