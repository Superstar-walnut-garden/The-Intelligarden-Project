#ifndef SIGNALNAMERESOLVER_HPP
#define SIGNALNAMERESOLVER_HPP
#include <string>

class SignalNameResolver
{
public:
    enum class SignalType
    {
        Broadcaster,
        Listener
    };
    class SignalNameParameters
    {
        public:
        std::string subsystemName;
        int id;
        std::string localSignalName;
        SignalType type;
    };
    
    static std::string toString(SignalNameParameters p)
    {
        return p.subsystemName + std::to_string(p.id) + p.localSignalName + "_" + signalTypeToString(p.type);
    }
    static SignalNameParameters parse(std::string signalPath)
    {
        // not implemented yet
    }
    static SignalType parseType(std::string signalPath)
    {
        if(signalPath.at(signalPath.length() - 1) == 'B')
            return SignalType::Broadcaster;
        return SignalType::Listener;
    }
    static std::string generateLocalSignalName(std::string locallyUniqueName, SignalType type)
    {
        return locallyUniqueName + signalTypeToString(type);
    }
private:
    static std::string signalTypeToString(SignalType type)
    {
        if(type == SignalType::Broadcaster)
            return "_B";
        return "_L";
    }
};

#endif