#include <assert.h>
#include <fstream>


//#define DEBUG_MONITOR   //debug mode for replay run
//#define DEBUG_REPLAY    //debug mode for replay run

#define STACK_LOWERBOUND 0x40000000  //base address to load shared libraries in Linux x86
//
#define USER_UPPERBOUND  0x15000000 
#define USER_LOWERBOUND  0x08048000 //start address for Unix/Linux x86 program 

#define MAX_THREAD_NUM 10

#define TIMEOUT 300 //threshold for timeout
#define LOCKBUFSIZE 100000
#define SYNCBUFSIZE 100000

#define NUM_BUF_PAGES 4096

//global varibles
UINT32 countTimeout = 0; // count time for timeout in controlled execution
UINT32 countLockRef = 0; // count time for
UINT32 countSyncRef = 0;
UINT32 countMemRef = 0;

BOOL logEnable = false; //start logging the memory accessing when at least two threads exist.
UINT32 threadNum = 0; //number of running threads
UINT32 threadExisted = 0; //used to mark how many threads existed once.
UINT32 timestamp = 1;  // timer to mark the ordering of the operations, 0 is default and meaningless.*/

FILE * file_mem0;
FILE * file_mem;
FILE * file_lock;
FILE * replay_log;

FILE * file_sync;

PIN_LOCK lock;

BUFFER_ID bufId; // The ID of the buffer

struct MEMREF
{
    UINT32     pc;
    UINT32     addr;
    UINT64     ts; //timestamp
    UINT32     tid;
    UINT32     read; // read: 1, write: 0
};

struct LOCKREF{
     UINT32 tid;
     UINT64 ts; //timestamp
     UINT32 callv; //callsite value
     UINT32 entryv; //entrypoint value
     UINT32 lock; // lock: 1, unlock: 0
} lockBuf[LOCKBUFSIZE];

struct SYNCREF{
    UINT32 tid;
    UINT64 ts; //timestamp
    UINT32 type; //1 Barrier, 2 CondWait, 3CondiTimeWait, 4 Sleep
} syncBuf[SYNCBUFSIZE];

// for controller
#define SLEEP_TIME 5

#define max_size 32 // default max size for each group

UINT32 ci_count = 0;
UINT32 cur_ci = 0; //current CI that being processed

UINT32 type[max_size] = {0}; // 0: to move instructions; 1: to move critical sections.

UINT32 first_id[max_size] = {0};
UINT32 first_arg1[max_size] = {0};
UINT32 first_arg2[max_size] = {0};

UINT32 second_id[max_size] = {0};
UINT32 second_arg1[max_size] = {0};
UINT32 second_arg2[max_size] = {0};

UINT32 inter_id[max_size] = {0};
UINT32 inter_arg1[max_size] = {0};
UINT32 inter_arg2[max_size] = {0};

UINT32 first_tid = 0;
UINT32 first_inst = 0; //use for non-critical section situations
UINT32 first_addr = 0;
UINT32 first_lock = 0; // use for critical section situations
UINT32 first_unlock = 0;

UINT32 second_tid = 0;
UINT32 second_inst = 0;
UINT32 second_addr = 0;
UINT32 second_lock = 0;
UINT32 second_unlock = 0;

UINT32 inter_tid = 0;
UINT32 inter_inst = 0;
UINT32 inter_addr = 0;
UINT32 inter_lock = 0;
UINT32 inter_unlock = 0;

bool is_locked_type = false;
bool first_ready = false;
bool inter_ready = false;
bool second_ready = false;


#define LIB_RTN_NAME_SIZE 15
string LIB_RTN_NAME[] = {
  "_start",
  ".plt",
  "__libc_csu_init",
  "__i686.get_pc_thunk.bx",
  "_init",
  "frame_dummy",
  "__do_global_ctors_aux",
  "_fini",
  "__do_global_dtors_aux",
  ".text",
  "malloc",
  "calloc",
  "__libc_memalign",
  "bsearch",
  "cfree"
}; //routines from the lib, we skip memory accesses from these routines.



bool isLibRtnName(string name){
  for(int i=0;i<LIB_RTN_NAME_SIZE;++i){
    if(name==LIB_RTN_NAME[i])
      return true;
  }
  return false;
}