#ifndef TEMPSENSORNODE_HPP
#define TEMPSENSORNODE_HPP

#include <vector>
#include <string>

class TempSensorNode
{
public:
    TempSensorNode(uint64_t address, std::string name = "", bool isConnected = true);

    bool operator == (TempSensorNode &obj);

    static bool doesExist(std::vector<TempSensorNode> &list, TempSensorNode &node);
    static void mergeAndCopy(std::vector<TempSensorNode> &primary, std::vector<TempSensorNode> secondary);
    static TempSensorNode findByName(std::vector<TempSensorNode> &list, std::string name);
    static TempSensorNode findByAddress(std::vector<TempSensorNode> &list, uint64_t address);

    void setAddress(uint64_t address);
    void setName(std::string name);
    void setConnectionState(bool state);

    uint64_t getAddress();
    std::string getName();
    bool isConnected();

private:
    uint64_t address;
    std::string name;
    bool connectionState;
};

#endif // TEMPSENSORNODE_HPP
