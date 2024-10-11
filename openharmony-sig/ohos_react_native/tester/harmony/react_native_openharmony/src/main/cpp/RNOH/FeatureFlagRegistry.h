#pragma once
#include <mutex>
#include <string>
#include <unordered_map>

class FeatureFlagRegistry {
 private:
  std::unordered_map<std::string, bool> flagStatusByName;
  mutable std::mutex mtx;

 public:
  using Shared = std::shared_ptr<FeatureFlagRegistry>;

  void setFeatureFlagStatus(const std::string& name, bool status) {
    std::lock_guard<std::mutex> lock(mtx);
    flagStatusByName[name] = status;
  }

  bool getFeatureFlagStatus(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = flagStatusByName.find(name);
    if (it != flagStatusByName.end()) {
      return it->second;
    }
    return false;
  }
    
    bool isFeatureFlagOn(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mtx);
        auto it = flagStatusByName.find(name);
        if (it != flagStatusByName.end()) {
        return it->second;
     }
    return false;
  }

};