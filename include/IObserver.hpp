#ifndef IOBSERVER_HPP
#define IOBSERVER_HPP

#include <vector>
#include <memory>
#include <iostream>
#include <queue>
#include <mutex>

class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void update(const std::vector<uint8_t>& packet) = 0;
};

#endif