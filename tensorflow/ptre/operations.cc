#include <thread>
#include <atomic>

namespace ptre {

struct PtreGlobalState {
  // Background thread running communication.
  std::thread comm_thread;
  std::atmoic_bool initialization_done{false};
  // Whether the communication thread should shutdown.
  std::atomic_bool shut_down{false};

  ~PtreGlobalState() {
    if (comm_thread.joinable()) {
      shut_down = true;
      comm_thread.join();
    }
  }
};

void BackgroundThreadLoop(PtreGlobalState& state) {
  while (RunLoopOnce(state));
}

void InitializePtreOnce() {
}

}  // namespace ptre
