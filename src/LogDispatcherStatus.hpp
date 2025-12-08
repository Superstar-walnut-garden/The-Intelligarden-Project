/**
 * @file LogDispatcherStatus.hpp
 * @brief Defines the LogDispatcherStatus enum class for representing various status codes.
 */

#pragma once

enum class LogDispatcherStatus 
{
    Idle,
    Running,
    StorageFullError,
    StorageNotReadyError
};