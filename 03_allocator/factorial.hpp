namespace NAllocator {
    template <class T>
    T Factorial(T n) {
        if (n == 0) {
            return 1;
        }
        return n * Factorial(n - 1);
    };
} //namespace NAllocator