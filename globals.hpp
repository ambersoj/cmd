#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <atomic>

extern std::atomic<bool> running;
extern std::atomic<bool> hud_running;
extern int listener_port;

#endif // GLOBALS_HPP
