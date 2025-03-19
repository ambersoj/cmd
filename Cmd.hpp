#include <thread>
#include <atomic>

class Cmd {
private:
    std::thread cmdThread;

public:
    void run();
    void start();
    void stop();
};
