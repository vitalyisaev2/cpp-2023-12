#include <map>

#include <gtest/gtest.h>

#include "allocator.hpp"
#include "container.hpp"
#include "factorial.hpp"

TEST(Allocator, StdMap_HeapAllocator) {
    std::map<int, int, std::less<int>, NAllocator::THeapAllocator<std::pair<const int, int>, 3>> map;

    map[1] = 1;
    map[2] = 2;
    map[3] = 3;
    ASSERT_EQ(map[1], 1);
    ASSERT_EQ(map[2], 2);
    ASSERT_EQ(map[3], 3);

    map[1] = 100;
    ASSERT_EQ(map[1], 100);
    ASSERT_EQ(map[2], 2);
    ASSERT_EQ(map[3], 3);
}

TEST(Allocator, Factorial) {
    ASSERT_EQ(NAllocator::Factorial(0), 1);
    ASSERT_EQ(NAllocator::Factorial(1), 1);
    ASSERT_EQ(NAllocator::Factorial(2), 2);
    ASSERT_EQ(NAllocator::Factorial(3), 6);
}

TEST(Allocator, Container_StdAllocator) {
    NAllocator::TContainer<uint64_t> c;
    c.push(0);
    ASSERT_EQ(c.size(), 1);
    ASSERT_EQ(c[0], 0);

    c.push(1);
    ASSERT_EQ(c.size(), 2);
    ASSERT_EQ(c[0], 0);
    ASSERT_EQ(c[1], 1);

    c.push(2);
    ASSERT_EQ(c.size(), 3);
    ASSERT_EQ(c[0], 0);
    ASSERT_EQ(c[1], 1);
    ASSERT_EQ(c[2], 2);

    for (uint64_t v = 3; v < 10; v++) {
        c.push(v);
    }

    ASSERT_EQ(c.size(), 10);
    ASSERT_TRUE(c.capacity() >= c.size());

    for (uint64_t i = 0; i < c.size(); i++) {
        ASSERT_EQ(c[i], i);
    }
}

TEST(Allocator, Container_HeapAllocator) {
    NAllocator::TContainer<uint64_t, NAllocator::THeapAllocator<uint64_t, 3>> c;

    for (uint64_t v = 0; v < 10; v++) {
        c.push(v);
    }

    ASSERT_EQ(c.size(), 10);
    ASSERT_TRUE(c.capacity() >= c.size());

    for (uint64_t i = 0; i < c.size(); i++) {
        ASSERT_EQ(c[i], i);
    }
}