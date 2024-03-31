#!/usr/bin/python3

import sys


# Helper function
# -----------------
def f(x, n):
    r = (x**2 + 1) % n
    return r


# Brute force GCD
# ---------------
def gcd(a, b):
    while b != 0:
        a, b = b, a % b
    return a


# Main function
# ---------------
def pollard_rho(n):
    # print("Finding GCD of " + str(n))
    x = 2
    y = 2
    d = 1

    while d == 1:

        x = f(x, n)
        y = f(f(y, n), n)
        d = gcd(abs(x - y), n)

    if d == n:
        return None
    return d, n // d


def factorize1(value):
    out = []
    residual = None
    while value != 1:
        result = pollard_rho(value)
        if result is None:
            break

        (divisor, residual) = result
        out.append(divisor)
        value = residual

    if residual:
        out.append(residual)

    return out


def primes_sieve2(limit):
    a = [True] * limit  # Initialize the primality list
    a[0] = a[1] = False

    for i, isprime in enumerate(a):
        if isprime:
            yield i
            for n in range(i * i, limit, i):  # Mark factors non-prime
                a[n] = False


primes = list(primes_sieve2(10000))


def factorize2(n):
    factorization = []
    for prime in primes:
        if prime * prime > n:
            break
        while n % prime == 0:
            factorization.append(prime)
            n /= prime

    if n > 1:
        factorization.append(n)

    return factorization


def factorize(value):
    # out = factorize1(value)
    # if out:
    #    # print("F1", out)
    #    return out

    out = factorize2(value)
    # print("F2", out)
    return out


def parse_line(line: str) -> int:
    (first, last) = [int(val) for val in line.split(" ")]
    if last % first != 0:
        return 0

    return len(factorize(int(last / first))) + 1


def main():
    lines = sys.stdin.readlines()[1:]
    for line in lines:
        print(parse_line(line))


if __name__ == "__main__":
    main()
