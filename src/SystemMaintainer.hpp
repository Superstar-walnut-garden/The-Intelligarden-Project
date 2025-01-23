#ifndef SYSTEMMAINTAINER_HPP
#define SYSTEMMAINTAINER_HPP

#include <thread>
#include <mutex>
#include <chrono>
#include <string>
#include <map>

class SystemMaintainer {
public:
    static SystemMaintainer& getInstance();

    void start();
    void stop();
    void setGeneralAbnormalCondition(bool condition);
    int createTrackableAbnormality(const std::string& description, int restartTimeMinutes);
    void resolveAbnormality(int &id);
    void postponeRestart(int minutes);
    void refreshCycleTime();

private:
    SystemMaintainer();
    ~SystemMaintainer();

    SystemMaintainer(const SystemMaintainer&) = delete;
    SystemMaintainer& operator=(const SystemMaintainer&) = delete;

    struct Abnormality {
        std::string description;
        std::chrono::steady_clock::time_point timestamp;
        int restartTimeMinutes;
        bool resolved;
    };

    void loop();

    std::thread loopThread;
    std::mutex mtx;
    bool running;
    bool generalAbnormalCondition;
    std::chrono::steady_clock::time_point nextRestartTime;
    std::chrono::steady_clock::time_point lastCycleTime;
    bool cycleTimeInitialized;
    int abnormalityCounter;
    std::map<int, Abnormality> abnormalities;
    bool restartDelayed;
    std::chrono::steady_clock::time_point disableRestartUntil;
};

#endif // SYSTEMMAINTAINER_HPP
