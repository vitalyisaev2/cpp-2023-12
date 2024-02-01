#include <memory>
#include <sstream>
#include <cstring>

namespace NAllocator {

    template <class T, class Allocator = std::allocator<T>>
    class TContainer {
    public:
        TContainer()
            : allocator(Allocator())
            , storage_(nullptr)
            , position_(nullptr)
            , size_(0)
            , capacity_(0) {
        }

        explicit TContainer(const Allocator& allocator)
            : allocator(allocator)
            , storage_(nullptr)
            , position_(nullptr)
            , size_(0)
            , capacity_(0) {
        }

        void push(const T& value) {
            maybeGrow();
            *position_ = value;
            position_++;
            size_++;
        }

        T& operator[](std::size_t ix) {
            if (ix >= capacity_) {
                std::stringstream ss;
                ss << "Index " << ix << " is greater than the capacity of the container " << size_;
                throw ss.str();
            };

            T& out = *static_cast<T*>(storage_ + ix);
            return out;
        }

        const T& operator[](std::size_t ix) const {
            if (ix >= size_) {
                std::stringstream ss;
                ss << "Index " << ix << " is greater than the size of the container " << size_;
                throw ss.str();
            };

            const T& out = *static_cast<T*>(storage_ + ix);
            return out;
        }

        std::size_t size() const noexcept {
            return size_;
        }

        std::size_t capacity() const noexcept {
            return capacity_;
        }

        ~TContainer() {
            allocator.deallocate(storage_, capacity_);
        }

    private:
        void maybeGrow() {
            // no growth needed
            if (size_ < capacity_) {
                return;
            }

            // lazy initialization of the storage
            if (capacity_ == 0) {
                storage_ = allocator.allocate(1);
                position_ = storage_;
                capacity_ = 1;
                return;
            }

            // grow when space is not enough
            auto new_capacity = capacity_ * 2;
            auto new_storage = allocator.allocate(new_capacity);

            // std::memcpy(new_storage, storage, size_);
            for (std::size_t i = 0; i < size_; i++) {
                new_storage[i] = storage_[i];
            }

            allocator.deallocate(storage_, size_);
            storage_ = new_storage;
            position_ = new_storage + size_;
            capacity_ = new_capacity;
        }

        Allocator allocator;
        T* storage_;           // array where data is stored
        T* position_;          // position for the next write
        std::size_t size_;     // actual size of the container
        std::size_t capacity_; // potential size of the container
    };
} //namespace NAllocator