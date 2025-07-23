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
        SignalNameParameters()
            : subsystemName("Untitled"), id(-1), localSignalName("Signal"), type(SignalType::Broadcaster) {}

        SignalNameParameters(std::string subsystemName, int id, std::string localSignalName, SignalType type = SignalType::Broadcaster)
            : subsystemName(subsystemName), id(id), localSignalName(localSignalName), type(type) {}
        std::string subsystemName;
        int id;
        std::string localSignalName;
        SignalType type;
    };
    
    static std::string toString(SignalNameParameters p)
    {
        return p.subsystemName + "_" + std::to_string(p.id) + "_" + p.localSignalName; // localSignalName already includes type (e.g. "_B")
    }
    static SignalNameParameters parse(std::string signalPath)
    {
        std::string subsystemName, id, localSignalName, type;
        auto firstDelim = signalPath.find('_');
        auto secondDelim = signalPath.find('_', firstDelim + 1);
        auto thirdDelim = signalPath.find('_', secondDelim + 1);

        subsystemName = signalPath.substr(0, firstDelim);
        id = signalPath.substr(firstDelim + 1, secondDelim - firstDelim -1);
        localSignalName = signalPath.substr(secondDelim + 1, thirdDelim - secondDelim -1);
        type = signalPath.substr(thirdDelim + 1);

        SignalNameParameters snp;
        snp.subsystemName = subsystemName;
        snp.id = std::stoi(id);
        snp.localSignalName = localSignalName;
        snp.type = parseType(type);
        return snp;
    }
    static SignalType parseType(std::string signalPath)
    {
        if(signalPath.at(signalPath.length() - 1) == 'B')
            return SignalType::Broadcaster;
        return SignalType::Listener;
    }
    // underscore, hyphen or space ain't allowed in the name
    static std::string generateLocalSignalName(std::string locallyUniqueName, SignalType type)
    {
        return locallyUniqueName + "_" + signalTypeToString(type);
    }
private:
    static std::string signalTypeToString(SignalType type)
    {
        if(type == SignalType::Broadcaster)
            return "B";
        return "L";
    }
    static SignalType parseSignalType(std::string type)
    {
        if(type == "B")
            return SignalType::Broadcaster;
        return SignalType::Listener;
    }
};

#endif