#pragma once
#include "SharedPointer.h"

template <class T> class SharedPointer;
struct Count;

template <class T>
class WeakPointer {
  template <class T> friend class SharedPointer;
  template <class T> friend class WeakPointer;

  T* p_;
  Count* count_;

public:
  // --- Constructors
  WeakPointer(): p_(nullptr) { }
  template <class U>
  WeakPointer(WeakPointer<U>& wp): p_(wp.p_), count_(wp.count_) { if (p_) ++count_->weak_count; }
  template <class U>
  WeakPointer(SharedPointer<U>& sp): p_(sp.p_), count_(sp.count_) { if (p_) ++count_->weak_count; }

  ~WeakPointer() { Decrease(); }

  // --- Assignment
  WeakPointer& operator=(WeakPointer& other) { return Set(other.p_, other.count_); }
  WeakPointer& operator=(SharedPointer<T>& sp) { return Set(sp.p_, sp.count_); }

  // --- bool
  operator bool() const { return !expired(); }

  SharedPointer<T> lock() const {
    //return expired() ? SharedPointer<T>() : SharedPointer<T>(p_, count_);
    if (expired()) return SharedPointer<T>(); 
    else return SharedPointer<T>(p_, count_);
  }

  bool expired() const { return !p_ || count_->expired(); }

private:
  void Decrease() {
    if (!p_) return;
    --count_->weak_count;
    if (count_->DeleteIfPossible()) {
      p_ = nullptr;
      count_ = nullptr;
    }
  }
  WeakPointer& Set(T* p, Count* count) {
    if (p != p_) {
      Decrease();
      p_ = p;
      count_ = count;
      if (p_) ++count_->weak_count;
    }
    return *this;
  }
};
