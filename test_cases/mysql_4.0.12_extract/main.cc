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
// Author - Pengfei Wang (wpengfeinudt@gmail.com)


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

#define LOG_CLOSED 0
#define LOG_TYPE_1 1

class MYSQL_LOG {
 public:
  MYSQL_LOG() {
    log_type = 0;
  }

  ~MYSQL_LOG() {}

  void new_file(unsigned int local_log_type) {

    //close old binlog
    log_type = LOG_CLOSED;


    //open new binlog
    log_type = local_log_type;
  }
/* sql_insert() reads an intermediate value produced by new_file(),
 and thinks the log file is already closed, as a result, 
 the action of sql_insert() is not recorded in log. */

  void sql_insert() {

  //do table update

    //log into bin_log_file
    if(log_type != LOG_CLOSED){
      //log into binlog.
      printf("log into binlog.\n");
    }
    else{
      //doing nothing
      printf("action is not recorded into binlog.\n");
      assert(0); // buggy path.
    }

}
  
  private:
  unsigned int log_type;

};


MYSQL_LOG mysql_bin_log;


void *t1_main(void *args) {

  mysql_bin_log.new_file(LOG_TYPE_1);
  printf("Thread 1: new_file().\n");

  return NULL;
}

void *t2_main(void *args) {

  mysql_bin_log.sql_insert();
  printf("Thread 2: sql_insert().\n");

  return NULL;
}

int main(int argc, char *argv[]) {

  pthread_t tid1;
  pthread_t tid2;

  pthread_create(&tid1, NULL, t1_main, NULL);
  pthread_create(&tid2, NULL, t2_main, NULL);

  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);


  printf("Program exit normally\n");

  return 0;
}



