#include <iostream>
#include <memory>
#include "../04_error_handling/ap_error.h"

template <typename num>
class Vector {
  std::size_t _size;
  std::unique_ptr<num[]> elem;

 public:
  // custom ctor
  explicit Vector(const std::size_t length)
      : _size{length}, elem{new num[length]{}} {
    std::cout << "custom ctor\n";
  }

  // default ctor
  Vector() : _size{}, elem{} { std::cout << "default ctor\n"; }

  // copy semantics
  // copy ctor -- deep copy
  Vector(const Vector& v);
  // copy assignment -- deep copy
  Vector& operator=(const Vector& v);

  // move semantics
  // move ctor
  Vector(Vector&& v) : _size{std::move(v._size)}, elem{std::move(v.elem)} {
    std::cout << "move ctor\n";
  }
  Vector& operator=(Vector&& v) {
    std::cout << "move assignment\n";
    _size = std::move(v._size);
    elem = std::move(v.elem);
    return *this;
  }

  const num& operator[](const std::size_t& i) const noexcept { return elem[i]; }
  num& operator[](const std::size_t& i) noexcept { return elem[i]; }

  std::size_t size() const noexcept { return _size; }

  // range-for
  const num* begin() const noexcept { return &elem[0]; }
  num* begin() noexcept { return &elem[0]; }

  const num* end() const noexcept { return &elem[0] + _size; }
  num* end() noexcept { return &elem[0] + _size; }
};

// copy ctor
template <typename num>
Vector<num>::Vector(const Vector& v) : _size{v._size}, elem{new num[_size]} {
  std::cout << "copy ctor\n";
  for (std::size_t i = 0; i < _size; ++i)
    elem[i] = v[i];
}

// copy assignment
template <typename num>
Vector<num>& Vector<num>::operator=(const Vector& v) {
  std::cout << "copy assignment\n";
  _size = v._size;
  elem.reset(new num[_size]);
  for (std::size_t i = 0; i < _size; ++i)
    elem[i] = v[i];
  return *this;
}

// define sum between two arrays
template <typename num>
Vector<num> operator+(const Vector<num>& lhs, const Vector<num>& rhs) {
  const auto size = lhs.size();
  AP_assert(size == rhs.size(), "Vector lenght mismatch:", size, "!=",
            rhs.size(), "\nlhs:", lhs, "\nrhs:", rhs);
  Vector<num> res{size};
  for (std::size_t i = 0; i < size; ++i)
    res[i] = lhs[i] + rhs[i];
  return std::move(res);
}

template <typename num>
std::ostream& operator<<(std::ostream& os, const Vector<num>& v) {
  for (const auto& x : v)
    os << x << " ";
  os << std::endl;
  return os;
}

int main() {
  try {
    std::cout << "Vector<int> v0; calls\n";
    Vector<int> v0;
    std::cout << "\nVector<double> v1{5}; calls\n";
    Vector<double> v1{5};

    std::cout << "\nVector<double> v2 = v1; calls\n";
    Vector<double> v2 = v1;
    std::cout << "\nv2 = Vector<double>{7}; calls\n";
    v2 = Vector<double>{7};
    std::cout << "\nVector<double> v3{std::move(v1)}; calls\n";
    Vector<double> v3{std::move(v1)};  // now v1 should not be used
    std::cout << "\nv3 = v2; calls\n";
    v3 = v2;

    {
      // auto i = v3.size();
      // while (i--)
      //   v3[i] = i;

      int c = 0;
      for (auto& x : v3)
        x = c++;
    }

    std::cout << "\nv2 = " << v2;
    std::cout << "v3 = " << v3;

    std::cout << "\nVector<double> v4{v3 + v2}; calls\n";
    Vector<double> v4{v3 + v2};

    std::cout << "v4 = " << v4;

    std::cout << "\nv4 = v3 + v3 + v2 + v3; calls\n";
    v4 = v3 + v3 + v2 + v3;
    std::cout << v4;

    std::cout << "\nLet's test our assert in operator+\n";
    Vector<int> a{5};
    Vector<int> b{7};
    Vector<int> c{a + b};
    return 0;
  } catch (std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
