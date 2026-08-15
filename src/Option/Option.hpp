#pragma once

// Option class storing either Some value or None
template <typename T> class Option {
private:
  T val;
  bool some;

public:
  static const Option<T> Some(const T &v) { return Option(v); }
  static const Option<T> None;

  Option() { some = false; }
  Option(const T &v) {
    val = v;
    some = true;
  }
  ~Option() {}
  Option(const Option<T> &other) { *this = other; }

  Option<T> &operator=(const Option<T> &other);
  Option<T> &operator=(const T &v);

  bool isSome() const { return some; }
  bool isNone() const { return !some; }

  // isSome not asserted for computation speed
  T &unpack() { return val; }
};

template <typename T> const Option<T> Option<T>::None = Option<T>();

template <typename T> Option<T> &Option<T>::operator=(const Option<T> &other) {
  this->some = other.some;
  if (some)
    this->val = other.val;
  return *this;
}

template <typename T> Option<T> &Option<T>::operator=(const T &v) {
  some = true;
  val = v;
  return *this;
}