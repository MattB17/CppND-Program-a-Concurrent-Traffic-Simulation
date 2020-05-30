#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

template <class T>
class MessageQueue
{
public:
  MessageQueue() {};
  T receive();
  void send(T &&msg);
private:
  std::deque<T> _queue;
  std::condition_variable _cond;
  std::mutex _mutex;
};

enum TrafficLightPhase
{
  red,
  green
};

class TrafficLight : public TrafficObject
{
public:
    TrafficLight();
    void waitForGreen();
    void simulate();
    TrafficLightPhase getCurrentPhase();
private:
    void cycleThroughPhases();

    std::shared_ptr<MessageQueue<TrafficLightPhase>> _messages;
    TrafficLightPhase _currentPhase;
};

#endif
