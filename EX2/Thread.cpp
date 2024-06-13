//
// Created by Ron on 09/06/2024.
//

#include "Thread.h"

// Default constructor for the Thread class.
// Initializes member variables _tid, _state, and _quantum_usecs to zero.
Thread::Thread() : _tid(0), _state(0), _quantum_usecs(0), _wake_up(0) {}

// Parameterized constructor for the Thread class.
// Initializes member variables _tid, _state, and _quantum_usecs with
// the provided values.
Thread::Thread(int tid, int state, int quantum_usecs) : _tid(tid), _state
(state), _quantum_usecs(quantum_usecs), _wake_up(0), _buffer(nullptr) {}

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
