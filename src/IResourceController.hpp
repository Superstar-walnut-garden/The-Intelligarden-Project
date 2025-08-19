#pragma once
#include <vector>
#include <cstdint>
#include "IService.hpp"

/** * @brief Interface for resource controllers that manage a collection of items.
 * 
 * @tparam T Type of the items managed by the controller.
 */
template <class T>
class IResourceController : public IService {
public:
    virtual ~IResourceController() = default;

    virtual std::string getAll() = 0;
    virtual std::string get(uint64_t id) = 0;
    virtual void create(T item) = 0;
    virtual void update(uint64_t id, T item) = 0;
    virtual void remove(uint64_t id) = 0;
};
