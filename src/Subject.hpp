#ifndef SUBJECT_HPP
#define SUBJECT_HPP
#include "IObserver.hpp"
#include "ISubject.hpp"
#include <iterator>
#include <vector>

template <class T>
class Subject: public ISubject<T>
{
  public:
    virtual ~Subject() = default;
    void attach (IObserver<T> *observer)
    {
      m_observers.push_back(observer);
    }
  protected:
    void notify ()
    {
      typename std::vector<IObserver<T> *>::iterator it;
      for (it=m_observers.begin();it!=m_observers.end();it++) 
        (*it)->update(static_cast<T *>(this));
    }
  private:
      std::vector<IObserver<T> *> m_observers;
};

#endif