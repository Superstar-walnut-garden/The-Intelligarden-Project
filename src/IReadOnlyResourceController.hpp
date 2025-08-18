#pragma once
#include <vector>
#include <cstdint>
#include "IService.hpp"

/** * @brief Interface for read-only resource controllers that manage a collection of items.
 * 
 * @tparam T Type of the items managed by the controller.
 */
template <class T>
class IReadOnlyResourceController : public IService 
{
public:
    virtual ~IReadOnlyResourceController() = default;

    virtual std::string getAll() = 0;
    virtual std::string get(uint64_t id) = 0;
};
