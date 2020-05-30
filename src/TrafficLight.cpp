#include <iostream>
#include <random>
#include <mutex>
#include <thread>
#include <chrono>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> uLock(_mutex);
    _cond.wait(uLock, [this] { return !_queue.empty(); });
    T msg = std::move(_queue.back());
    _queue.pop_back();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> uLock(_mutex);
    _queue.push_back(std::move(msg));
    _cond.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
    _messages = std::make_shared<MessageQueue<TrafficLightPhase>>();
}

void TrafficLight::waitForGreen()
{
    while (true) {
      TrafficLightPhase phase = _messages->receive();
      if (phase == TrafficLightPhase::green) {
        return;
      }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    _threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

void TrafficLight::cycleThroughPhases() {
  // start stopwatch.
  auto prevUpdate = std::chrono::system_clock::now();

  // generate a random number to determine when the traffic light should change.
  std::random_device rd;
  std::mt19937 eng(rd());
  std::uniform_int_distribution<> distr(4000.0, 6000.0);
  double ms = distr(eng);

  while(true) {
    // put thread to sleep
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    // compute the time since last change and update if it is above threshold.
    double elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now() - prevUpdate).count();
    if (elapsedTime >= ms) {
      if (_currentPhase == TrafficLightPhase::red) {
        _currentPhase = TrafficLightPhase::green;
      } else {
        _currentPhase = TrafficLightPhase::red;
      }

      // move the phase into the queue and restart the clock.
      TrafficLightPhase phase = _currentPhase;
      _messages->send(std::move(phase));
      prevUpdate = std::chrono::system_clock::now();

      std::random_device rd;
      std::mt19937 eng(rd());
      double ms = distr(eng);
    }
  }
}
