#ifndef SIGNALCOMPATIBLEBASEITEM_HPP
#define SIGNALCOMPATIBLEBASEITEM_HPP
#include "BaseItem.hpp"
#include "ISignalCompatibleItem.hpp"

class SignalCompatibleBaseItem: public BaseItem, public ISignalCompatibleItem
{
    public: 
    // Forward BaseItem Constructors
    using BaseItem::BaseItem;
    
    // Ambiguty Resolvation
    virtual std::string getName() const override
    {
        return BaseItem::getName();
    }
    virtual uint64_t getId() const override
    {
        return BaseItem::getId();
    }
};

#endif