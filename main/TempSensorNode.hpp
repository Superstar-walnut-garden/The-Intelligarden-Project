#ifndef TEMPSENSORNODE_HPP
#define TEMPSENSORNODE_HPP
#include <vector>

class TempSensorNode
{
    public:
    TempSensorNode(uint64_t address, std::string name = "", bool isConnected = true)
        : address(address), name(name), connectionState(isConnected){}
    auto operator == (TempSensorNode &obj)
    {
        return this->address == obj.address;
    }
    static bool doesExist(std::vector<TempSensorNode> &list, TempSensorNode &node)
    {
        for(auto &element : list)
            if(element == node)
                return true;
        return false;
    }
    static void mergeAndCopy(std::vector<TempSensorNode> &primary, std::vector<TempSensorNode> secondary)
    {
        auto alreadyExist = false;
        for(auto sDev : secondary)
        {
            for(auto &pDev : primary) //search and compare
                if(pDev == sDev)
                {
                    alreadyExist = true;
                    pDev.setName(sDev.getName()); // have the same address but not name and ...
                }
            if(!alreadyExist)
                primary.push_back(sDev);
        }
    }
    static TempSensorNode findByName(std::vector<TempSensorNode> &list, std::string name)
    {
        for(auto &node : list)
            if(node.getName() == name)
                return node;
    }
    static TempSensorNode findByAddress(std::vector<TempSensorNode> &list, uint64_t address)
    {
        for(auto &node : list)
            if(node.getAddress() == address)
                return node;
    }
    void setAddress(uint64_t address)
    {
        this->address = address;
    }
    void setName(std::string name)
    {
        this->name = name;
    }
    void setConnectionState(bool state)
    {
        this->connectionState = state;
    }
    uint64_t getAddress()
    {
        return address;
    }
    std::string getName()
    {
        return name;
    }
    bool isConnected()
    {
        return connectionState;
    }

    private:
    uint64_t address;
    std::string name;
    bool connectionState;
};

#endif