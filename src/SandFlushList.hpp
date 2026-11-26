#pragma once

#include "BaseList.hpp"
#include "SandFlushItem.hpp"

class SandFlushList : public BaseList<SandFlushItem>
{
public:
    using BaseList::BaseList;
};
