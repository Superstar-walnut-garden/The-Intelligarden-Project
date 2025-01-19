#ifndef SYSTEMMAINTAINER_HPP
#define SYSTEMMAINTAINER_HPP

#include <thread>
#include <mutex>
#include <chrono>

class SystemMaintainer {
public:
  static SystemMaintainer& getInstance();

  void start();
  void stop();
  void setAbnormalCondition(bool condition);
  void postponeRestart(int minutes);

private:
  SystemMaintainer();
  ~SystemMaintainer();

  SystemMaintainer(const SystemMaintainer&) = delete;
  SystemMaintainer& operator=(const SystemMaintainer&) = delete;

  void loop();

  std::thread loopThread;
  std::mutex mtx;
  bool running;
  bool abnormalCondition;
  std::chrono::steady_clock::time_point nextRestartTime;
};

#endif // SYSTEMMAINTAINER_HPP
