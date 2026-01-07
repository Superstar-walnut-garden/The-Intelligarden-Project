#pragma once
#include <vector>
#include <cstdint>
#include "IService.hpp"

/** * @brief Interface for resource controllers that manage a collection of items.
 * 
 * @tparam T Type of the items managed by the controller.
 */
template <class T = void>
class IResourceController : public IService {
public:
    virtual ~IResourceController() = default;

    virtual std::string getAll() const = 0;
    virtual std::string get(uint64_t id) const = 0;
    virtual void create(std::string json) = 0;
    virtual void update(uint64_t id, std::string json) = 0;
    virtual void remove(uint64_t id) = 0;
};
