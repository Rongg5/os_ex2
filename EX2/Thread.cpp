//
// Created by Ron on 09/06/2024.
//

#include "Thread.h"

// Default constructor for the Thread class.
// Initializes member variables _tid, _state, and _quantum_usecs to zero.
Thread::Thread() : _tid(0), _state(0), _quantum_usecs(0), _wake_up(0),
_blocked(false), _running_time(0) {}

// Parameterized constructor for the Thread class.
// Initializes member variables _tid, _state, and _quantum_usecs with
// the provided values.
Thread::Thread(int tid, int state, int quantum_usecs) : _tid(tid), _state
(state), _quantum_usecs(quantum_usecs), _wake_up(0), _blocked(false),
_buffer(nullptr), _running_time(0) {}

Thread::Thread(int tid, int state, int quantum_usecs,
       std::shared_ptr<JmpBufWrapper > buffer) : _tid(tid), _state
    (state), _quantum_usecs(quantum_usecs), _wake_up(0), _blocked(false),
    _buffer(buffer), _running_time(0) {}

// Setter method to update the state of the thread.
void Thread::set_state(int new_state)
{
  _state = new_state;
}

// Getter method to retrieve the current state of the thread.
int Thread::get_state()
{
  return _state;
}

// Getter method to retrieve the quantum usecs of the thread.
int Thread::get_quantum_usecs()
{
  return _quantum_usecs;
}

void Thread::set_wake_up(int wake_up)
{
  _wake_up = wake_up;
}

void Thread::set_buffer (std::shared_ptr<JmpBufWrapper> buffer)
{
  _buffer = buffer;
}

int Thread::get_tid(){
  return _tid;
}

std::shared_ptr<JmpBufWrapper> Thread::get_buffer()
{
  return _buffer;
}

void Thread::set_blocked(bool blocked)
{
  _blocked = blocked;
}

int Thread::get_running_time(){
  return _running_time;
}
void Thread::inc_running_time(){
  _running_time++;
}