#!/usr/bin/env python3

from dataclasses import dataclass
from typing import List, Set
import copy
import fileinput


@dataclass
class Result:
    min_damage: int

    solution_cache: Set[str]

    def maybe_update(self, damage: int):
        self.min_damage = min(self.min_damage, damage)


@dataclass
class Balcony:
    monsters: int
    destroyed: bool


@dataclass
class Disposition:
    cum_damage: int
    balconies: List[Balcony]

    def __str__(self) -> str:
        return self.__dump()

    def __dump(self) -> str:
        s = "["
        for b in self.balconies:
            if b.destroyed:
                s += " - "
            else:
                s += f" {b.monsters} "
        s += "]"
        return s

    def non_destroyed_balcony_ixs(self) -> List[int]:
        return [
            ix for ix in range(len(self.balconies)) if not self.balconies[ix].destroyed
        ]

    def step(self, result: Result):
        # print("START ", self, "->", self.cum_damage)

        if str(self) in result.solution_cache:
            return

        starting_points = self.non_destroyed_balcony_ixs()

        if len(starting_points) == 0:
            result.maybe_update(self.cum_damage)
            return

        for start in starting_points:
            disp = self.shoot(start=start)
            disp.step(result=result)

        # marking this branch as resolved
        result.solution_cache.add(str(self))

    def shoot(self, start: int) -> "Disposition":
        balconies = copy.deepcopy(self.balconies)

        i = 0
        while i < 3:
            balconies[self._ix(start + i)].destroyed = True
            i += 1

        added_damage = 0
        for balcony in balconies:
            if not balcony.destroyed:
                added_damage += balcony.monsters

        return Disposition(
            balconies=balconies, cum_damage=self.cum_damage + added_damage
        )

    def _ix(self, i: int) -> int:
        return i % len(self.balconies)


if __name__ == "__main__":
    lines = list(fileinput.input())
    N = int(lines[0])
    monsters = list(map(lambda x: int(x), lines[1].split()))
    balconies = [Balcony(monsters=m, destroyed=False) for m in monsters]
    disp = Disposition(cum_damage=0, balconies=balconies)
    result = Result(min_damage=sum(b.monsters for b in balconies), solution_cache=set())
    disp.step(result=result)
    print(result.min_damage)
