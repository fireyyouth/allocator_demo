#include <vector>
#include <map>
#include <iostream>
#include <new>

template <class T>
struct custom_allocator {
  using value_type = T;

  uint8_t *addr_;
  size_t cap_;
  std::vector<bool> used_;

  custom_allocator(uint8_t *addr, size_t cap) noexcept : addr_(addr), cap_(cap), used_(cap, false) {}

  template <class U>
  custom_allocator (const custom_allocator<U>& other) noexcept : addr_(other.addr_), cap_(other.cap_), used_(other.used_) {}

  bool alloc_range(size_t start, size_t n) {
      for (size_t i = 0; i < n; ++i) {
          if (used_[start + i]) {
              return false;
          }
      }
      for (size_t i = 0; i < n; ++i) {
          used_[start + i] = true;
      }
      return true;
  }
      
  T* allocate (std::size_t n) {
      for (size_t i = 0; i < cap_ - n * sizeof(T); ++i) {
          if (alloc_range(i, n * sizeof(T))) {
              return (T*)(addr_ + i);
          }
      }
      throw std::bad_alloc();
  }
              
  void deallocate (T* p, std::size_t n) {
      for (size_t i = 0; i < n * sizeof(T); ++i) {
	  used_[(uint8_t *)p - addr_ + i] = false;
      }
  }

};

template <class T, class U>
constexpr bool operator== (const custom_allocator<T>& a, const custom_allocator<U>& b) noexcept {
    return &a == &b;
}

template <class T, class U>
constexpr bool operator!= (const custom_allocator<T>& a, const custom_allocator<U>& b) noexcept {
    return !(a == b);
}


uint8_t pool[1024];

int main() {
	std::map<char, char, std::less<char>, custom_allocator<std::pair<const char, char>>> m(custom_allocator<std::pair<const char, char>>(pool, sizeof(pool)));
	std::cout << "pool( " << sizeof(pool) << " ) = [ " << &pool << " , " << (void *)(pool + sizeof(pool)) << " ]" << std::endl;
	for (int i = 0; i < 5; i++) {
		m.emplace('a' + i, 'a' + i);
	}
	for (auto & kv : m) {
		std::cout << &kv << ":" << kv.first << ',' << kv.second << std::endl;
	}
		
}
