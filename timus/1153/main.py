#!/usr/bin/env python3

import math
import sys


def count(s: int):
    up = -1 + math.sqrt(1 + 8 * s)
    down = 2
    return int(up / down)


def main():
    s = int(sys.stdin.readline())
    print(count(s))


if __name__ == "__main__":
    main()
