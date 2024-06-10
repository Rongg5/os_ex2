#ifndef _THREAD_H_
#define _THREAD_H_

#include <list>


class Thread{
 private:
  int _tid;
  int _state; // 0 is running 1 is ready and 2 is blocked
  int _quantum_usecs;
 public:
  Thread();
  Thread(int tid, int state, int quantum_usecs);
  void set_state(int new_state);
  int get_state();
  int get_quantum_usecs();
};

#endif //_THREAD_H_
