#pragma once
#include <mutex>
#include <condition_variable>
#include <memory>
#include <set>
#include "glog/logging.h"

namespace PS {

class TaskTracker {
 public:
  TaskTracker() { }
  ~TaskTracker() { }

  // wait until task k has been finished
  void wait(int k) {
    ULock l(mu_);
    cond_.wait(l, [this, k]{return (task_.count(k) && task_[k] == true); });
  }

  // non-blocking wait
  bool tryWait(int k) {
    Lock l(mu_);
    return (task_.count(k) && task_[k] == true);
  }

  // start task k, do nothing if it has been started or finished
  void start(int k) {
    Lock l(mu_);
    if (!task_.count(k)) task_[k] = false;
  }

  // whether of not task k has been finished
  bool hasFinished(int k) {
    Lock l(mu_);
    return (task_.count(k) && task_[k] == true);
  }

  // finish k, no warning if it has not been started or has been finished
  void finish(int k) {
    Lock l(mu_);
    task_[k] = true;
    cond_.notify_all();
  }

 private:
  // typedef std::lock_guard<std::recursive_mutex> Lock;
  // std::recursive_mutex mu_;

  typedef std::lock_guard<std::mutex> Lock;
  typedef std::unique_lock<std::mutex> ULock;

  std::mutex mu_;
  std::condition_variable cond_;

  std::map<int, bool> task_;
};

} // namespace PS
