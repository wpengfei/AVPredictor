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
// Modified by Pengfei Wang (wpengfeinudt@gmail.com)

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>

// define the circular list data structure
typedef struct list_node_st list_node;
struct list_node_st {
  void *data;
  list_node *next;
};

typedef struct {
  list_node *head;
  list_node *tail;
  int size;
  pthread_mutex_t lock;
} circular_list;

typedef struct {
  int num;
} data_type;

pthread_mutex_t mem_lock;

list_node *new_list_node() {
  pthread_mutex_lock(&mem_lock);
  list_node *node = new list_node;
  pthread_mutex_unlock(&mem_lock);
  return node;
}

void free_list_node(list_node *node) {
  pthread_mutex_lock(&mem_lock);
  delete node;
  pthread_mutex_unlock(&mem_lock);
}

void list_push_back(circular_list *list, void *data) {
  list_node *node = new_list_node();
  node->data = data;
  node->next = NULL;

  if (list->tail) {
    list->tail->next = node;
    list->tail = node;
  } else {
    list->head = node;
    list->tail = node;
  }
  list->size++;

}

void *list_pop_front(circular_list *list) {
  void *ret_val;
  list_node *node = NULL;

  if (list->head) {
    ret_val = list->head->data;
    node = list->head;
    if (list->head == list->tail) {
      list->head = list->head->next;
      list->tail = list->head;
    } else {
      list->head = list->head->next;
    }
    list->size--;
  } else {
    ret_val = NULL;
  }

  if (node) {
    free_list_node(node);
  }
  return ret_val;
}

void list_init(circular_list *list) {
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
  pthread_mutex_init(&list->lock, NULL);
}

/* Here the pop and push should be atomic in both t1 and t2, 
  otherwise, problematci order as follows can be caused:
  we suppose the original list is (a,b,c,d)

       T1         T2
     pop a,
                pop b,
                push b,
     push a

    then we got (c,d,b,a), while the right result should be (c,d,a,b)

*/

/* pop and push in t1 are not atomic due to improper used locks*/
void *t1_main(void *args) {
  circular_list* list = (circular_list *)args;

  pthread_mutex_lock(&list->lock);
  data_type *data = (data_type *)list_pop_front(list);
  printf("t1 poped\n");
  pthread_mutex_unlock(&list->lock);

  data->num += 10;

  pthread_mutex_lock(&list->lock);
  list_push_back(list, data);
  pthread_mutex_unlock(&list->lock);
  printf("t1 pushed\n");
  return NULL;
}

/* pop and push in t2 are atomic*/
void *t2_main(void *args) {
  circular_list* list = (circular_list *)args;

  pthread_mutex_lock(&list->lock);
  data_type *data = (data_type *)list_pop_front(list);
  printf("t2 poped\n");

  data->num += 10;

  list_push_back(list, data);
  printf("t2 pushed\n");
  pthread_mutex_unlock(&list->lock);
  
  return NULL;
}

int main(int argc, char *argv[]) {
  pthread_mutex_init(&mem_lock, NULL);
  circular_list *work_list = new circular_list;
  list_init(work_list);
  for (int i = 0; i < 10; i++) {
    data_type *data = new data_type;
    data->num = i;
    list_push_back(work_list, data);
  }

  pthread_t *tids = new pthread_t[2];
  pthread_create(&tids[0], NULL, t1_main, work_list);
  pthread_create(&tids[1], NULL, t2_main, work_list);
  pthread_join(tids[0], NULL);
  pthread_join(tids[1], NULL);

  // print and verify results
  list_node *iterator = work_list->head;
  int prev_num = -1;
  while (iterator != NULL) {
    data_type *data = (data_type *)iterator->data;
    printf("%d ", data->num);
    if (prev_num != -1) {
      assert(data->num > prev_num); // program aborts when this failed
    }
    prev_num = data->num;
    iterator = iterator->next;
  }
  printf("\n");

  return 0;
}

