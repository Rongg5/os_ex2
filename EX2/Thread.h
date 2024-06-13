#ifndef _THREAD_H_
#define _THREAD_H_

#include <list>
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdbool.h>

class Thread{
 private:
  int _tid;
  int _state; // 0 is running 1 is ready and 2 is blocked
  int _quantum_usecs;
  int _wake_up; // when to wake up if the tread is sleep.
  sigjmp_buf* _buffer;
 public:
  Thread();
  Thread(int tid, int state, int quantum_usecs);
  void set_state(int new_state);
  int get_state();
  int get_quantum_usecs();
  void set_wake_up(int _wake_up);
};

#endif //_THREAD_H_
