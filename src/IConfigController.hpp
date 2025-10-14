#pragma once
#include <string>
#include "IService.hpp"

/** * @brief Interface for configuration controllers that manage a single configuration item.
 * 
 * @tparam T Type of the configuration item managed by the controller.
 */
template <class T>
class IConfigController : public IService 
{
public:
    virtual ~IConfigController() = default;

    virtual std::string getConfig() = 0;
    virtual void updateConfig(T newConfig) = 0;
};