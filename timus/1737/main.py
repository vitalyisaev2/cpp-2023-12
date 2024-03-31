#!/usr/bin/env python3
import sys

from typing import Sequence
import dataclasses


@dataclasses.dataclass
class Result:
    lines: Sequence[str]
    total_length: int

    def add_line(self, line: str):
        self.lines.append(line)
        self.total_length += len(line)
        if self.total_length > 100000:
            raise ValueError("TOO LONG")


def is_palyndrome(data: str) -> bool:
    if len(data) <= 1:
        return False

    for i in range(0, int(len(data) / 2)):
        if data[i] != data[len(data) - 1 - i]:
            return False

    return True


chars = ["a", "b", "c"]


def build_string(n: int, prefix: str, result: Result):
    # print(n, prefix, result.lines)
    if n == 0:
        if not is_palyndrome(prefix):
            result.add_line(prefix)
        return

    for char in chars:
        build_string(n - 1, prefix + char, result)


def main():
    arg = int(sys.stdin.readline())

    result = Result([], 0)

    try:
        build_string(arg, "", result)
    except Exception as e:
        print(e)
        return
    else:
        for l in result.lines:
            print(l)


if __name__ == "__main__":
    main()
