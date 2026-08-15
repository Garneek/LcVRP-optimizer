#pragma once

#include <atomic>

template <typename T> class SmartPointer {
private:
  std::atomic_int *rc;
  T *ptr;

  void inner_free();

public:
  SmartPointer() = delete;
  SmartPointer(T *ptr);
  SmartPointer(const SmartPointer<T> &other);

  ~SmartPointer() { inner_free(); }

  SmartPointer<T> &operator=(const SmartPointer<T> &other);

  T &operator*() { return *ptr; }
  T *operator->() { return ptr; }

  bool operator==(const SmartPointer<T> &other) const;
  bool operator!=(const SmartPointer<T> &other) const;
};

template <typename T> void SmartPointer<T>::inner_free() {
  if (--(*rc) == 0) {
    delete rc;
    delete ptr;
  }
}

template <typename T> SmartPointer<T>::SmartPointer(T *ptr) {
  rc = new std::atomic_int(1);
  this->ptr = ptr;
}

template <typename T>
SmartPointer<T>::SmartPointer(const SmartPointer<T> &other) {
  ptr = other.ptr;
  rc = other.rc;
  (*rc)++;
}

template <typename T>
SmartPointer<T> &SmartPointer<T>::operator=(const SmartPointer<T> &other) {
  if (ptr != other.ptr) {
    inner_free();

    ptr = other.ptr;
    rc = other.rc;
    (*rc)++;
  }
  return *this;
}

template <typename T>
bool SmartPointer<T>::operator==(const SmartPointer<T> &other) const {
  return this->ptr == other.ptr;
}

template <typename T>
bool SmartPointer<T>::operator!=(const SmartPointer<T> &other) const {
  return this->ptr != other.ptr;
}
