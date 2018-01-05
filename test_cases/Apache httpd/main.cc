
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Authors - Pengfei Wang (wpengfeinudt@gmail.com)

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "string.h"
#include "assert.h"

typedef struct {
  unsigned int len;
  char str[16];
} Args;


typedef struct {
  unsigned int outcnt;
  char output[128];
} Buffer;


Buffer b = {0, ""};
Buffer *buf = &b;


void *ap_buffered_log_writer(void *p) {

  Args *args = (Args*)p;
  unsigned int len = args->len;
  char *str = args->str;
  printf("len = %d, str = %s.\n", args->len, args->str);

  char *s;
  unsigned int idx;

  /* update log */
  idx = buf->outcnt;
  s = &buf->output[idx];
  memcpy(s, str, len);


  /* The update of buf->output and update of buf->outcnt should be atomic here,
  otherwise, it can be interleaved by another thread. The newly updated 
  output can be overwritten by the update from another thread because the 
  local update of buf->outcnt hasn't been finished yet. Thus, the buf->output 
  string mismatches the buf->outcnt.*/


  /* update outcnt */
  buf->outcnt = buf->outcnt + len;

  return NULL;
}



int main(int argc, char *argv[]) {

  pthread_t tid1;
  pthread_t tid2;


  Args arg1 = {5, "hello"};
  Args arg2 = {6, "world!"};

  pthread_create(&tid1, NULL, ap_buffered_log_writer, &arg1);
  pthread_create(&tid2, NULL, ap_buffered_log_writer, &arg2);

  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);
  
  //verify result
  if(strlen(buf->output) != buf->outcnt){
    printf("Buggy. len = %d, outcnt = %d.\n", strlen(buf->output),buf->outcnt);
    assert(0);
  }
  else{
    printf("Exits normally. len = %d, outcnt = %d.\n", strlen(buf->output), buf->outcnt);
  }

  

  return 0;
}

