#pragma once

template <class T>
class IObserver
{
 public:
  virtual ~IObserver() = default;
  virtual void update(T *subject) = 0;
};
