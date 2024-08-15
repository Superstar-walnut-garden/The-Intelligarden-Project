#include "TempSensorNode.hpp"

// Constructor implementation
TempSensorNode::TempSensorNode(uint64_t address, std::string name, bool isConnected)
    : address(address), name(name), connectionState(isConnected) {}

// Operator overloading implementation
bool TempSensorNode::operator == (TempSensorNode &obj)
{
    return this->address == obj.address;
}

// Static methods implementation
bool TempSensorNode::doesExist(std::vector<TempSensorNode> &list, TempSensorNode &node)
{
    for (auto &element : list)
        if (element == node)
            return true;
    return false;
}

void TempSensorNode::mergeAndCopy(std::vector<TempSensorNode> &primary, std::vector<TempSensorNode> secondary)
{
    bool alreadyExist = false;
    for (auto sDev : secondary)
    {
        alreadyExist = false;
        for (auto &pDev : primary) // Search and compare
        {
            if (pDev == sDev)
            {
                alreadyExist = true;
                pDev.setName(sDev.getName()); // Have the same address but different name
            }
        }
        if (!alreadyExist)
            primary.push_back(sDev);
    }
}

TempSensorNode TempSensorNode::findByName(std::vector<TempSensorNode> &list, std::string name)
{
    for (auto &node : list)
        if (node.getName() == name)
            return node;
    return TempSensorNode(0); // Dummy node
}

TempSensorNode TempSensorNode::findByAddress(std::vector<TempSensorNode> &list, uint64_t address)
{
    for (auto &node : list)
        if (node.getAddress() == address)
            return node;
    return TempSensorNode(0); // Dummy node (assuming it should return something)
}

// Member methods implementation
void TempSensorNode::setAddress(uint64_t address)
{
    this->address = address;
}

void TempSensorNode::setName(std::string name)
{
    this->name = name;
}

void TempSensorNode::setConnectionState(bool state)
{
    this->connectionState = state;
}

uint64_t TempSensorNode::getAddress()
{
    return address;
}

std::string TempSensorNode::getName()
{
    return name;
}

bool TempSensorNode::isConnected()
{
    return connectionState;
}
