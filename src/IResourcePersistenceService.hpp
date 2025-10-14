#pragma once

class IResourcePersistenceService
{
public:
    virtual void storeAll() = 0;
    virtual void restoreAll() = 0;
};