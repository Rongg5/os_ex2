#include "uthreads.h"
#include "Thread.h"
#include <list>
#include <queue>
#include <memory>
#include <iostream>


#define RUNNING 0
#define READY 1
#define BLOCKED 2

static Thread* main_thread; // Global main thread
static bool initialized = false; // Flag to check if the library is initialized
static std::queue<Thread*> ready;
static std::queue<Thread*> blocked;
static Thread* running;
static int count_treads = 0;
/**
 * @brief initializes the thread library.
 *
 * Once this function returns, the main thread (tid == 0) will be set as RUNNING. There is no need to
 * provide an entry_point or to create a stack for the main thread - it will be using the "regular" stack and PC.
 * You may assume that this function is called before any other thread library function, and that it is called
 * exactly once.
 * The input to the function is the length of a quantum in micro-seconds.
 * It is an error to call this function with non-positive quantum_usecs.
 *
 * @return On success, return 0. On failure, return -1.
*/
int uthread_init(int quantum_usecs)
{
  if((quantum_usecs <= 0) or (initialized)){
    return -1;
  }
  new (main_thread) Thread(1, RUNNING, quantum_usecs);
  running = main_thread;
  initialized = true;
  count_treads++;
  return 0;
}

/**
 * @brief Creates a new thread, whose entry point is the function entry_point with the signature
 * void entry_point(void).
 *
 * The thread is added to the end of the READY threads list.
 * The uthread_spawn function should fail if it would cause the number of concurrent threads to exceed the
 * limit (MAX_THREAD_NUM).
 * Each thread should be allocated with a stack of size STACK_SIZE bytes.
 * It is an error to call this function with a null entry_point.
 *
 * @return On success, return the ID of the created thread. On failure, return -1.
*/
int uthread_spawn(thread_entry_point entry_point){
  if (!entry_point){
    std::cerr << "This is an error message: the entry_point in null.\n" <<
    std::endl;
    return -1;
  }
  if (count_treads>=MAX_THREAD_NUM){
    std::cerr << "This is an error message: you make too much treads.\n" <<
              std::endl;
    return -1;
  }

  count_treads++;
}