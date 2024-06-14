#include "uthreads.h"


#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define JB_SP 6
#define JB_PC 7
#define MICRO_TO_SECOND 1000000

static std::shared_ptr<Thread> main_thread; // Global main thread
static bool initialized = false; // Flag to check if the library is initialized
static std::deque<std::shared_ptr<Thread>> ready;
static std::deque<std::shared_ptr<Thread>> blocked;
static std::shared_ptr<Thread> running;
static int count_treads = 0;
static std::set<int> free_tid;
static int global_quantum_usecs;
static int quantums_parts;

int create_tid (){
  int new_tid = *(free_tid.begin());
  free_tid.erase (new_tid);
  return new_tid;
}


address_t translate_address(address_t addr)
{
  address_t ret;
  asm volatile("xor    %%fs:0x30,%0\n"
               "rol    $0x11,%0\n"
      : "=g" (ret)
      : "0" (addr));
  return ret;
}
 // to search and del in the queue the relvet theard.
 std::shared_ptr<Thread> search_deque(std::deque<std::shared_ptr<Thread>> q,
                                     int tid)
 {
   std::deque<std::shared_ptr<Thread>> temp = q; // Make a copy of the
   // original queue

   while (!temp.empty ())
   {
     if ((*temp.front ().get ()).get_tid () == tid)
     {

       std::shared_ptr<Thread> return_theard = temp.front (); // Return a
       // reference to the found element
       auto it = std::remove (q.begin (), q.end (), return_theard);
       q.erase (it, q.end ());
       return return_theard;
     }
   }
   // If the element is not found, throw an exception
   return nullptr;
 }


std::shared_ptr<JmpBufWrapper > setup_thread(std::shared_ptr<char[]> stack, thread_entry_point entry_point)
{
  // initializes env[tid] to use the right stack, and to run from the
  // function 'entry_point', when we'll use
  // siglongjmp to jump into the thread.
  std::shared_ptr<JmpBufWrapper > new_buff = std::make_shared<JmpBufWrapper>();
  address_t sp = (address_t) stack.get() + STACK_SIZE - sizeof(address_t);
  address_t pc = (address_t) entry_point;
  sigsetjmp(new_buff->env, 1);
  ((new_buff->env)->__jmpbuf)[JB_SP] = translate_address(sp);
  ((new_buff->env)->__jmpbuf)[JB_PC] = translate_address(pc);
  sigemptyset(&(new_buff->env)->__saved_mask);
  return new_buff;
}

void scheduler(int sig){
  quantums_parts++;
  if (sig == SIGVTALRM){
    if (ready.empty()){
      (*running.get()).inc_running_time();
      return;
    }
    (*running.get()).set_state (READY);
    yield (ready, 0);
  }

}

void start_timer()
{
  struct sigaction sa = {0};
  struct itimerval timer;

  // Install timer_handler as the signal handler for SIGVTALRM.
  sa.sa_handler = &scheduler;
  if (sigaction(SIGVTALRM, &sa, NULL) < 0)
  {
    std::cerr << "This is an error message: fail in start timer.\n" <<
              std::endl;
  }
  try
  {  // Configure the timer to expire after quantum_usecs... */
    timer.it_value.tv_sec = global_quantum_usecs / MICRO_TO_SECOND;        //
    // first time interval, seconds part
    timer.it_value.tv_usec = global_quantum_usecs % MICRO_TO_SECOND;        //
    // first time interval, microseconds part

    // configure the timer to expire every quantum_usecs after that.
    timer.it_interval.tv_sec = global_quantum_usecs / MICRO_TO_SECOND;    //
    // following time intervals, seconds part
    timer.it_interval.tv_usec = global_quantum_usecs % MICRO_TO_SECOND;    //
    // following time intervals, microseconds part
  }
  catch(...) {
    std::cerr << "This is an error message: fail in start timer.\n" <<
              std::endl;
  }
  // Start a virtual timer. It counts down whenever this process is executing.
  if (setitimer(ITIMER_VIRTUAL, &timer, NULL))
  {
    std::cerr << "This is an error message: fail in start timer.\n" <<
              std::endl;
  }

}
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
    std::cerr << "This is an error message: fail in the init.\n" <<
              std::endl;
    return -1;
  }
  try
  {
    global_quantum_usecs = quantum_usecs;
    main_thread = std::make_shared<Thread> (tid, RUNNING, quantum_usecs);
    running = main_thread;
    initialized = true;
    count_treads++;
    quantums_parts++;
    for (int i = 1; i < MAX_THREAD_NUM; ++i)
    {
      free_tid.insert (i);
    }
    start_timer ();
    return 0;
  }
  catch (...){
    std::cerr << "This is an error message: fail in the init.\n" <<
              std::endl;
    return -1;
  }
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
  std::shared_ptr<JmpBufWrapper > new_buffer;
  std::shared_ptr<Thread> new_thread;
  try {
    std::shared_ptr<char[]> stack(new char[STACK_SIZE](), std::default_delete<char[]>());
    new_buffer = setup_thread(stack, entry_point);
    int tid = create_tid ();
    new_thread = std::make_shared<Thread>(tid, READY, global_quantum_usecs,
                                          new_buffer);
  }
  catch (...){
    std::cerr << "This is an error message: error with the create thread.\n" <<
              std::endl;
    return -1;
  }
  ready.push_back(new_thread);
  count_treads++;
}

// this  function swith the running. remeber that the function only change
// the new running state and not the old running.
void yield(std::deque<std::shared_ptr<Thread>> deque_future, int del)
{
  int ret_val = sigsetjmp((*running.get()).get_buffer()->env, 1);
  bool did_just_save_bookmark = ret_val == 0;
//    bool did_jump_from_another_thread = ret_val != 0;
  if (did_just_save_bookmark)
  {
    if (!ready.empty()){
      running = ready.front ();
      ready.pop_front ();
      (*running.get()).set_state (RUNNING);
      if(del == 0)
      {
        deque_future.push_back (running);
      }
      (*running.get()).inc_running_time();
      siglongjmp (running->get_buffer ()->env, 1);
    }
  }
}

/**
 * @brief Terminates the thread with ID tid and deletes it from all relevant control structures.
 *
 * All the resources allocated by the library for this thread should be released. If no thread with ID tid exists it
 * is considered an error. Terminating the main thread (tid == 0) will result in the termination of the entire
 * process using exit(0) (after releasing the assigned library memory).
 *
 * @return The function returns 0 if the thread was successfully terminated and -1 otherwise. If a thread terminates
 * itself or the main thread is terminated, the function does not return.
*/
int uthread_terminate(int tid){
  std::shared_ptr<Thread> del_thread;
  bool is_running = false;
  if (tid==0){
    exit(0);
  }
  del_thread = search_deque(ready, tid);
  if (del_thread==nullptr){
    del_thread = search_deque(blocked, tid);
  }
  else if(del_thread == nullptr){
    if ((*(running.get())).get_tid()!=tid){
      std::cerr << "This is an error message: try to terminates a "
                   "not existing thread.\n" << std::endl;
      return -1;
    }
    del_thread = running;
    is_running = true;
  }
  if (is_running){
//    yield(ready, 1);
    scheduler(-1);
  }
}

/**
 * @brief Blocks the thread with ID tid. The thread may be resumed later using uthread_resume.
 *
 * If no thread with ID tid exists it is considered as an error. In addition, it is an error to try blocking the
 * main thread (tid == 0). If a thread blocks itself, a scheduling decision should be made. Blocking a thread in
 * BLOCKED state has no effect and is not considered an error.
 *
 * @return On success, return 0. On failure, return -1.
*/
int uthread_block(int tid){
  // Check if the thread ID is valid and not free
  auto it = free_tid.find(tid);
  if (it != free_tid.end() || tid >=MAX_THREAD_NUM){
    std::cerr << "This is an error message: try to resume not existing "
                 "thread.\n" << std::endl;
    return -1;
  }
  if (tid == 0)
  {
    std::cerr << "This is an error message: try to resume the main.\n" <<
    std::endl;
    return -1;
  }
  try{
    if ((*running.get()).get_tid()==tid){
      (*running.get()).set_state (BLOCKED);
      (*running.get()).set_blocked (true);
      blocked.push_back (running);
      scheduler (-1);
      return 0;
    }
    std::shared_ptr<Thread> block_thread = search_deque (ready, tid);
    if(block_thread== nullptr){
      block_thread = search_deque (blocked, tid);
    }
    if (block_thread){
      (*block_thread.get()).set_state (BLOCKED);
      (*block_thread.get()).set_blocked (true);
      blocked.push_back (block_thread);
    }
    return 0;
  }
  catch (...){
    std::cerr << "This is an error message: error in the block thread.\n" <<
    std::endl;
    return -1;
  }
}

/**
 * @brief Resumes a blocked thread with ID tid and moves it to the READY state.
 *
 * Resuming a thread in a RUNNING or READY state has no effect and is not
 *  considered as an error. If no thread with
 * ID tid exists it is considered an error.
 *
 * @return On success, return 0. On failure, return -1.
*/
int uthread_resume(int tid)
{
  // Check if the thread ID is valid and not free
  auto it = free_tid.find(tid);
  if (it != free_tid.end() || tid >=MAX_THREAD_NUM){
    std::cerr << "This is an error message: try to resume not existing "
                  "thread.\n" << std::endl;
    return -1;
  }
  try
  {// Check if the thread is in the blocked deque
    std::shared_ptr<Thread> resume_thread = search_deque (blocked, tid);
    if (resume_thread)
    {
      resume_thread.get ()->set_state (READY);
      resume_thread.get ()->set_blocked (false);
      ready.push_back (resume_thread);
    }
    return 0;
  }
  catch (...){
    std::cerr << "This is an error message: error with the uthread_resume "
                 "function.\n" << std::endl;
    return -1;
  }
}

/**
 * @brief Blocks the RUNNING thread for num_quantums quantums.
 *
 * Immediately after the RUNNING thread transitions to the BLOCKED state a scheduling decision should be made.
 * After the sleeping time is over, the thread should go back to the end of the READY queue.
 * If the thread which was just RUNNING should also be added to the READY queue, or if multiple threads wake up
 * at the same time, the order in which they're added to the end of the READY queue doesn't matter.
 * The number of quantums refers to the number of times a new quantum starts, regardless of the reason. Specifically,
 * the quantum of the thread which has made the call to uthread_sleep isn’t counted.
 * It is considered an error if the main thread (tid == 0) calls this function.
 *
 * @return On success, return 0. On failure, return -1.
*/
int uthread_sleep(int num_quantums){
  Thread* running_thread = running.get();
  if ((*running_thread).get_tid()==0){
    std::cerr << "This is an error message: try to sleep the main, not cool "
                 "bro.\n"
    << std::endl;
  }
  try
  {
    (*running_thread).set_state (BLOCKED);
    (*running_thread).set_wake_up (quantums_parts + num_quantums);
    scheduler (-1);
  }
  catch (...){
    std::cerr << "This is an error message: error in the sleep function.\n"
              << std::endl;
    return -1;
  }
  return 0;
}


