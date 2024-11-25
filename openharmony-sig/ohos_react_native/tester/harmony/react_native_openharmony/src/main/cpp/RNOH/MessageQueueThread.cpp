#include "RNOH/MessageQueueThread.h"
#include <glog/logging.h>
#include <react/renderer/debug/SystraceSection.h>

using namespace rnoh;

MessageQueueThread::MessageQueueThread(
    std::shared_ptr<TaskExecutor> const& taskExecutor)
    : taskExecutor(taskExecutor) {}

MessageQueueThread::~MessageQueueThread() {
  DLOG(INFO) << "~MessageQueueThread";
};

void MessageQueueThread::runOnQueue(std::function<void()>&& func) {
  taskExecutor->runTask(TaskThread::JS, std::move(func));
}

void MessageQueueThread::runOnQueueSync(std::function<void()>&& func) {
  facebook::react::SystraceSection s(
      "#RNOH::MessageQueueThread::runOnQueueSync");
  // NOTE: we store the shared_ptr in a local variable to ensure that the
  // TaskExecutor is not destroyed before the task is executed.
  auto taskExecutor = this->taskExecutor;
  taskExecutor->runSyncTask(TaskThread::JS, std::move(func));
}

void MessageQueueThread::quitSynchronous() {
  // TODO!
}
