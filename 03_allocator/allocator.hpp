#include <array>
#include <cstddef>
#include <iostream>
#include <memory>
#include <new>
#include <vector>

namespace NAllocator {

    struct deleter {
        void operator()(void* ptr) {
            ::operator delete(ptr);
        }
    };

    template <class T, std::size_t N>
    struct THeapAllocator {
        using value_type = T;

        THeapAllocator() noexcept {
            // initially only one pool for N elements of type T is created,
            // but the vector of pools can be extended later
            pools.emplace_back(std::make_unique<TPool>(N));
        }

        template <class U>
        struct rebind {
            typedef THeapAllocator<U, N> other;
        };

        T* allocate(std::size_t n) {
            if (!pools.back()->hasSpaceFor(n)) {
                addNewPool(n);
            }
            return pools.back()->allocate(n);
        }

        void deallocate([[maybe_unused]] T* p, [[maybe_unused]] std::size_t n) {
            // Not required in this homework
        }

    private:
        class TPool {
        public:
            explicit TPool(std::size_t size)
                : capacity(size * sizeof(T))
                , ptr(new char[capacity])
                , position(ptr){};

            T* allocate(std::size_t n) {
                auto result = reinterpret_cast<T*>(position);
                position += n * sizeof(T);
                return result;
            }

            bool hasSpaceFor(std::size_t n) const {
                return capacity >= (position - ptr) + n * sizeof(T);
            }

            ~TPool() {
                delete[] ptr;
            }

        private:
            std::size_t capacity; // memory slice capacity
            char* ptr;            // the contigious slice of memory
            char* position;       // the position for the next write
        };

        void addNewPool(std::size_t n) {
            // allocate space required by client or a little bit more
            auto new_pool_size = std::max(n, N) * sizeof(T);
            pools.emplace_back(std::make_unique<TPool>(new_pool_size));
        }

        std::vector<std::unique_ptr<TPool>> pools; // vector of contiguous memory areas (grown by demand)
    };

    template <class T, size_t TN, class U, size_t UN>
    constexpr bool operator==(const THeapAllocator<T, TN>& lhs, const THeapAllocator<U, UN>& rhs) noexcept {
        return lhs.pools == rhs.pools;
    }

    template <class T, size_t TN, class U, size_t UN>
    constexpr bool operator!=(const THeapAllocator<T, TN>& lhs, const THeapAllocator<U, UN>& rhs) noexcept {
        return lhs.pools != rhs.pools;
    };

} //namespace NAllocator