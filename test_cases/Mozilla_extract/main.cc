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

#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <limits.h>
#include <pthread.h>

class nsSpt {
 public:
  nsSpt() {
    value = new char[4];
    value[0] = 't';
    value[1] = 'e';
    value[2] = 's';
    value[3] = 't';
    length = 4;
  }

  ~nsSpt() {
    delete[] value;
    length = 0;
  }

  void compile() {
    for (int i = 0; i < length; i++) {
      printf("%c", *(value + i));
    }
    printf("\n");
  }

 private:
  char *value;
  int length;

};


pthread_mutex_t mutex_lock;
nsSpt *gCurrentScript = new nsSpt();

/* gCurrentScript is set to NULL by remote thread between local RAW */

void* LoadScript(void* aspt){
  pthread_mutex_lock(&mutex_lock);
  gCurrentScript = (nsSpt *)aspt;
  /* Call LaunchLoad(aspt) */
  printf("Call LaunchLoad().\n");
  pthread_mutex_unlock(&mutex_lock);


  /* OnLoadComplete(), callback function of LaunchLoad() */
  pthread_mutex_lock(&mutex_lock);
  gCurrentScript->compile();
  printf("OnLoadComplete().\n");
  pthread_mutex_unlock(&mutex_lock);

  return NULL;
}


void* sFree(void* args) {
  pthread_mutex_lock(&mutex_lock);
  gCurrentScript = NULL;
  printf("gCurrentScript is set to NULL.\n");
  pthread_mutex_unlock(&mutex_lock);

  return NULL;
}

int main(int argc, char *argv[]) {

  pthread_t tid_1;
  pthread_t tid_2;
  
  pthread_mutex_init(&mutex_lock, NULL);

  nsSpt *aspt = new nsSpt();

  pthread_create(&tid_1, NULL, LoadScript, (void*)aspt);
  pthread_create(&tid_2, NULL, sFree, NULL);


  pthread_join(tid_1, NULL);
  pthread_join(tid_2, NULL);

  printf("Exits normally\n");

  return 0;
}

