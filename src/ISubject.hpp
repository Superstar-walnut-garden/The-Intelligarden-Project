#ifndef ISUBJECT_HPP
#define ISUBJECT_HPP
#include "IObserver.hpp"

template <class T>
class ISubject
{
  public:
    virtual ~ISubject() = default;
    virtual void attach(IObserver<T> *observer) = 0;
  protected:
    virtual void notify() = 0;
};

#endif