#ifndef _THREAD_H_
#define _THREAD_H_

#include <list>
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdbool.h>
#include <memory>

struct JmpBufWrapper {
    sigjmp_buf env;
};

class Thread{
 private:
  int _tid;
  int _state; // 0 is running 1 is ready and 2 is blocked
  int _quantum_usecs;
  int _wake_up; // when to wake up if the tread is sleep.
  bool _blocked;
  std::shared_ptr<JmpBufWrapper > _buffer;
  int _running_time;
 public:
  Thread();
  Thread(int tid, int state, int quantum_usecs); // constructor for the main
  Thread(int tid, int state, int quantum_usecs,
         std::shared_ptr<JmpBufWrapper > buffer); // constructor for new
         // threads.
  void set_state(int new_state);
  int get_state();
  int get_quantum_usecs();
  void set_wake_up(int _wake_up);
  int get_wake_up();
  void set_buffer(std::shared_ptr<JmpBufWrapper > buffer);
  int get_tid();
  std::shared_ptr<JmpBufWrapper> get_buffer();
  void set_blocked(bool blocked);
  bool get_blocked();
  int get_running_time();
  void inc_running_time();
};

#endif //_THREAD_H_
