#!/usr/bin/python3

import sys
import math


def primes_sieve2(limit):
    a = [True] * limit  # Initialize the primality list
    a[0] = a[1] = False

    for i, isprime in enumerate(a):
        if isprime:
            yield i
            for n in range(i * i, limit, i):  # Mark factors non-prime
                a[n] = False


primes = list(primes_sieve2(100000))


def prime_divisors(value: int) -> int:
    out = 0
    while value > 1:
        i = 0
        while int(value % primes[i]) != 0:
            i += 1
        while int(value % primes[i]) == 0:
            out += 1
            value = int(value / primes[i])

    return out


def parse_line(line: str) -> int:
    (first, last) = [int(val) for val in line.split(" ")]
    if last % first != 0:
        return 0

    return prime_divisors(int(last / first)) + 1


def main():
    lines = sys.stdin.readlines()[1:]
    for line in lines:
        print(parse_line(line))


if __name__ == "__main__":
    main()
