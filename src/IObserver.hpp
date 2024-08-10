#ifndef IOBSERVER_HPP
#define IOBSERVER_HPP

template <class T>
class IObserver
{
 public:
  virtual ~IObserver() = default;
  virtual void update(T *subject) = 0;
};

#endif