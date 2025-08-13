#ifndef IMANAGER_HPP
#define IMANAGER_HPP

template <class T>
class IManager
{
public:
    virtual void create(T newItem) = 0;
    virtual void remove(uint64_t id) = 0;
    virtual void modify(uint64_t id, T newItem) = 0;
    virtual std::string getListJson() = 0;
    virtual void saveState() = 0;
    virtual void loadState() = 0;
};

#endif