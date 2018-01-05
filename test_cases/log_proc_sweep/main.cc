// Copyright 2011 The University of Michigan
//
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
// Authors - Jie Yu (jieyu@umich.edu)
// Modified by Pengfei Wang(wpengfeinudt@gmail.com)

#include <cstdio>
#include <cstdlib>
#include <pthread.h>

typedef struct list_node_st list_node;
struct list_node_st {
  const char *log_entry;
  list_node *next;
};

typedef struct {
  list_node log_entry_list;
  int num_entries;
  pthread_mutex_t lock;
} log_type;

log_type *shared_log = NULL;
pthread_mutex_t mem_lock;

list_node *new_log_entry() {
  pthread_mutex_lock(&mem_lock);
  list_node *node = new list_node;
  pthread_mutex_unlock(&mem_lock);
  return node;
}

void add_log_entry(log_type *log, const char *entry) {
  
  list_node *node = new_log_entry();
  node->log_entry = entry;
  pthread_mutex_lock(&log->lock);
  node->next = log->log_entry_list.next;
  log->log_entry_list.next = node;
  log->num_entries++;
  pthread_mutex_unlock(&log->lock);
  
  printf("Add log entry done\n");
}

void init_log(log_type *log) {
  printf("init log\n");
  log->log_entry_list.log_entry = "HEAD";
  log->log_entry_list.next = NULL;
  log->num_entries = 0;
  pthread_mutex_init(&log->lock, NULL);
}

/* atomicity violation here,
 shared_log can be set to NULL by t2, between two reads of it in t1 */

void *t1_main(void *args) {
  printf("t1 is logging\n");

  const char * str = "NewLog";
  if (shared_log) {  //   <--  buggy here when shard_log is set to NULL by t2.
    printf("Passed NULL check, enter add_log_entry()\n");
    add_log_entry(shared_log, str);

  }
  else{
    /*If the execution goes to this path, the AV bug cannot be exposed, 
    we have to run it again.*/
    printf(">>>>>>>>>>>>>>>>>>>Non-buggy execution path, run it again!<<<<<<<<<<<<<<<<\n");
  }

  printf("logging done\n");
  return NULL;
}

void *t2_main(void *args) {
  printf("t2 is resetting the log\n");
  shared_log = NULL;
  shared_log = new log_type;
  printf("resetting done\n");
  return NULL;
}

int main(int argc, char *argv[]) {
  pthread_mutex_init(&mem_lock, NULL);
  shared_log = new log_type;
  init_log(shared_log);

  pthread_t *tids = new pthread_t[2];
  pthread_create(&tids[0], NULL, t1_main, NULL);
  pthread_create(&tids[1], NULL, t2_main, NULL);
  pthread_join(tids[0], NULL);
  pthread_join(tids[1], NULL);

  return 0;
}

