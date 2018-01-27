#include <vector>
#include <map>
#include <assert.h>
#include <stdint.h>



#define STACK_LOWERBOUND 0x40000000  //base address to load shared libraries in Linux x86
//0xbfffffff 
#define USER_UPPERBOUND  0x15000000
#define USER_LOWERBOUND  0x08048000 //start address for Unix/Linux x86 program 

#define MAX_THREAD_NUM 10

#define DEBUG_TRACING 0  //debug mode for tracing run
#define DEBUG_REPLAY  1  //debug mode for replay run

#define TIMEOUT 300 //threshold for timeout

unsigned int timecounter = 0; // count time for timeout in controlled execution

FILE * replay_log;
FILE * file_mem;
FILE * file_lock;
FILE * file_sync;

PIN_LOCK lock;

typedef unsigned int ADDRESS;
typedef unsigned int TIME;
typedef unsigned int COUNT;


//global varibles
COUNT threadNum = 0; //number of running threads
COUNT threadExisted = 0; //used to mark how many threads existed once.
bool logging_start = false; //start logging the memory accessing when at least two threads exist.

/* A global timer to mark the ordering of the operations. 
*Use 0 to indicate a default value, which is meaningless.*/
TIME timestamp = 1; 


typedef vector<std::string> logString; //

logString mem_log;
logString lock_log;
logString sync_log;

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
