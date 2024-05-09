#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

namespace sjtu
{
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
auto resize_func = [] (size_t s) { return s / 2 * 3 + 1; };
template<typename T>
class vector
{
private:
  size_t capacity, siz;
  T* data;

public:
  class const_iterator;
  class iterator
  {
    friend class vector;
  // The following code is written for the C++ type_traits library.
  // Type traits is a C++ feature for describing certain properties of a type.
  // For instance, for an iterator, iterator::value_type is the type that the 
  // iterator points to. 
  // STL algorithms and containers may use these type_traits (e.g. the following 
  // typedef) to work properly. In particular, without the following code, 
  // @code{std::sort(iter, iter1);} would not compile.
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::output_iterator_tag;

  private:
    vector* origin;
    int pos;
  public:
    iterator(vector* _origin = nullptr, int _pos = 0)
            : origin (_origin), pos (_pos) {}
    ~iterator() = default;
    iterator(iterator &other) {
      origin = other.origin;
      pos = other.pos;
    }
    /**
     * return a new iterator which pointer n-next elements
     * as well as operator-
     */
    iterator operator+(const int &n) const 
    {
      return iterator(origin, pos + n);
    }
    iterator operator-(const int &n) const 
    {
      return iterator(origin, pos - n);
    }
    // return the distance between two iterators,
    // if these two iterators point to different vectors, throw invaild_iterator.
    int operator-(const iterator &rhs) const 
    {
      if(origin != rhs.origin) throw invalid_iterator();
      else return pos - rhs.pos;
    }
    iterator& operator+=(const int &n) 
    {
      pos += n;
      return *this;
    }
    iterator& operator-=(const int &n) 
    {
      pos -= n;
      return *this;
    }
    iterator operator++(int) {
      iterator tmp(*this);
      ++pos;
      return tmp;
    }
    iterator& operator++() {
      ++pos;
      return *this;
    }
    iterator operator--(int) {
      iterator tmp(*this);
      --pos;
      return tmp;
    }
    iterator& operator--() {
      --pos;
      return *this;
    }
    T& operator*() const{
      return origin->data[pos];
    }
    /**
     * a operator to check whether two iterators are same (pointing to the same memory address).
     */
    bool operator==(const iterator &rhs) const {
      return (origin == rhs.origin) && (pos == rhs.pos);
    }
    bool operator==(const const_iterator &rhs) const {
      return (origin == rhs.origin) && (pos == rhs.pos);
    }
    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
      return !(*this == rhs);
    }
    bool operator!=(const const_iterator &rhs) const {
      return !(*this == rhs);
    }
  };

  class const_iterator 
  {
    friend class vector;

  public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::output_iterator_tag;

  private:
    const vector* origin;
    int pos;

  public:
    const_iterator(const vector* _origin = nullptr, int _pos = 0)
            : origin(_origin), pos(_pos) {}
    ~const_iterator() = default;
    const_iterator(const const_iterator &other) {
      origin = other.origin;
      pos = other.pos;
    }
    const_iterator operator+(const int &n) const 
    {
      return const_iterator(origin, pos + n);
    }
    const_iterator operator-(const int &n) const 
    {
      return const_iterator(origin, pos - n);
    }
    int operator-(const const_iterator &rhs) const 
    {
      if(origin != rhs.origin) throw invalid_iterator();
      else return pos - rhs.pos;
    }
    const_iterator& operator+=(const int &n) 
    {
      pos += n;
      return *this;
    }
    const_iterator& operator-=(const int &n) 
    {
      pos -= n;
      return *this;
    }
    const_iterator operator++(int) {
      const_iterator tmp(*this);
      ++pos;
      return tmp;
    }
    const_iterator& operator++() {
      ++pos;
      return *this;
    }
    const_iterator operator--(int) {
      const_iterator tmp(*this);
      --pos;
      return tmp;
    }
    const_iterator& operator--() {
      --pos;
      return *this;
    }
    const T& operator*() const {
      return origin->data[pos];
    }
    bool operator==(const const_iterator &rhs) const {
      return (origin == rhs.origin) && (pos == rhs.pos);
    }
    bool operator==(const iterator &rhs) const {
      return (origin == rhs.origin) && (pos == rhs.pos);
    }
    bool operator!=(const const_iterator &rhs) const {
      return !(*this == rhs);
    }
    bool operator!=(const iterator &rhs) const {
      return !(*this == rhs);
    }
  };

  vector() : capacity(16), siz(0){
    data = (T*) malloc(capacity * sizeof(T));
  }
  vector(const vector &other) : capacity(other.siz), siz(other.siz) {
    data = (T*) malloc(capacity * sizeof(T));
    for(int i = 0; i < siz; ++i) {
      new(data + i) T(other[i]); // Placement new
    }
  }

  ~vector() {
    destroy();
  }
  void destroy() {
    for(int i = 0; i < siz; ++i) {
      data[i].~T();
    }
    free(data);
  }

  vector &operator=(const vector &other) {
    if(this != &other) {
      destroy();
      capacity = other.capacity, siz = other.siz;
      data = (T*) malloc(capacity * sizeof(T));
      for(int i = 0; i < siz; ++i) {
        new(data + i) T(other[i]);
      }
    }
    return *this;
  }
  // Move the whole chunk to a bigger space
  void expand() {
    capacity = resize_func(capacity); // siz maintained
    T* n_data = (T*) malloc(capacity * sizeof(T));
    for(int i = 0; i < siz; ++i) {
      new(n_data + i) T(data[i]);
    }
    destroy();
    data = n_data;
  }

  T & at(const size_t &pos) {
    if(pos >= siz) {
      throw index_out_of_bound();
    }
    return data[pos];
  }
  const T & at(const size_t &pos) const {
    if(pos >= siz) {
      throw index_out_of_bound();
    }
    return data[pos];
  }
  /**
   * assigns specified element with bounds checking
   * throw index_out_of_bound if pos is not in [0, size)
   */
  T & operator[](const size_t &pos) {
    return at(pos);
  }
  const T & operator[](const size_t &pos) const {
    return at(pos);
  }
  /**
   * access the first element.
   * throw container_is_empty if size == 0
   */
  const T & front() const {
    if(siz == 0) throw container_is_empty();
    return data[0];
  }
  const T & back() const {
    if(siz == 0) throw container_is_empty();
    return data[siz - 1];
  }
  /**
   * returns an iterator to the beginning.
   */
  iterator begin() {
    return iterator(this, 0);
  }
  const_iterator cbegin() const {
    return const_iterator(this, 0);
  }
  /**
   * returns an iterator to the end.
   */
  iterator end() {
    return iterator(this, siz);
  }
  const_iterator cend() const {
    return const_iterator(this, siz);
  }
  /**
   *
   */
  bool empty() const {
    return siz == 0;
  }
  size_t size() const {
    return siz;
  }
  void clear() {
    destroy();
    siz = 0; capacity = 16;
    data = (T*) malloc(capacity * sizeof(T));
  }
  /**
   * inserts value before pos
   * returns an iterator pointing to the inserted value.
   */
  iterator insert(iterator pos, const T &value) {
    if(siz == capacity) expand();
    new(data + siz) T(data[siz - 1]);
    for(int i = siz - 1; i > pos.pos; --i) {
      data[i] = data[i - 1];
    }
    data[pos.pos] = value;
    ++siz;
    return pos;
  }
  /**
   * inserts value at index ind.
   * after inserting, this->at(ind) == value
   * returns an iterator pointing to the inserted value.
   * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
   */
  iterator insert(const size_t &ind, const T &value) {
    if(ind > siz) throw index_out_of_bound();
    if(siz == capacity) expand();
    new(data + siz) T(data[siz - 1]);
    for(int i = siz - 1; i > ind; --i) {
      data[i] = data[i - 1];
    }
    data[ind] = value;
    ++siz;
    return iterator(this, ind);
  }
  /**
   * removes the element at pos.
   * return an iterator pointing to the following element.
   * If the iterator pos refers the last element, the end() iterator is returned.
   */
  iterator erase(iterator pos) {
    for(int i = pos.pos; i < siz - 1; ++i) {
      data[i] = data[i + 1];
    }
    data[siz - 1].~T();
    --siz;
    return pos;
  }
  /**
   * removes the element with index ind.
   * return an iterator pointing to the following element.
   * throw index_out_of_bound if ind >= size
   */
  iterator erase(const size_t &ind) {
    if(ind >= siz) throw index_out_of_bound();
    for(int i = ind; i < siz - 1; ++i) {
      data[i] = data[i + 1];
    }
    data[siz - 1].~T();
    --siz;
    return iterator(this, ind);
  }
  /**
   * adds an element to the end.
   */
  void push_back(const T &value) {
    if(siz == capacity) expand();
    new(data + siz) T(value);
    ++siz;
  }
  /**
   * remove the last element from the end.
   * throw container_is_empty if size() == 0
   */
  void pop_back() {
    if(siz == 0) throw container_is_empty();
    data[siz - 1].~T();
    --siz;
  }
};
}

#endif
