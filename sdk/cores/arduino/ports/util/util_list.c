/********************************************************************************************************
 * @file    util_list.c
 *
 * @brief   This is the source file for Arduino SDK
 *
 * @author  Platform Group
 * @date    2026
 *
 * @par     Copyright (c) 2026, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#include "util_common.h"
#include "port_interface.h"

struct _list_node_t
{
    void *data;
    struct _list_node_t *next;
};
typedef struct _list_node_t list_node_t;


typedef struct {
    list_node_t *head;
    void *mutex;
    size_t size;
}list_handle_block_t, *list_handle_prv_t;

static bool list_compare(const void *data1, const void *data2) {
    return data1 == data2;
}

static inline void list_lock(list_handle_t handle) {
    list_handle_prv_t listHandle = (list_handle_prv_t)handle;
    if(listHandle) {
        util_lock_mutex(listHandle->mutex);
    }
}

static inline void list_unlock(list_handle_t handle) {
       list_handle_prv_t listHandle = (list_handle_prv_t)handle;
    if(listHandle) {
        util_unlock_mutex(listHandle->mutex);
    } 
}

static list_node_t * list_create_node(void *data) {
    list_node_t *node = (list_node_t *)util_malloc(sizeof(list_node_t));
    if(node) {
        node->data = data;
        node->next = NULL;
    }
    return node;
}


list_handle_t list_create(void) {
    list_handle_prv_t listHandle;
    list_handle_t handle = NULL;
    do {
        listHandle = (list_handle_prv_t)util_malloc(sizeof(list_handle_block_t));
        if(!listHandle) break;
        listHandle->head = NULL;
        listHandle->size = 0;
        listHandle->mutex = util_create_mutex();
        if(!listHandle->mutex) break;
        handle = listHandle;
    } while(0);

    return handle;
}

bool list_destroy(list_handle_t handle, listFreeFunc func) {
    bool ret = false;
    do {
        if(!handle) break;
        list_handle_prv_t listHandle = (list_handle_prv_t)handle;
        list_node_t *cNode = listHandle->head;
        while(cNode) {
            list_node_t *temp = cNode->next;
            if(func && cNode->data) {
                func(cNode->data);
            }
            util_free(cNode);
            cNode = temp;
        }
        ret = true;
    } while(0);

    return ret;
}

bool list_push_front(list_handle_t handle, void *data) {
    bool ret = false;
    list_lock(handle);
    do {
        if(!handle) break;
        list_handle_prv_t listHandle = (list_handle_prv_t)handle;
        list_node_t *tempNode = list_create_node(data);
        if(!tempNode) break;
        tempNode->next = listHandle->head;
        listHandle->head = tempNode;
        listHandle->size++;
        ret = true;
    } while(0);
    list_unlock(handle);
    return ret;
}

bool list_push_back(list_handle_t handle, void *data) {
    bool ret = false;
    list_lock(handle);
    do {
        if(!handle) break;
        list_handle_prv_t listHandle = (list_handle_prv_t)handle;
        list_node_t *tempNode = list_create_node(data);
        if(!tempNode) break;
        if(listHandle->head == NULL) {
            listHandle->head = tempNode;
        } else {
            list_node_t *tail = listHandle->head;
            while(tail->next) {
                tail = tail->next;
            }
            tail->next = tempNode;
        }

        listHandle->size++;
        ret = true;
    } while(0);
    list_unlock(handle);
    return ret;
}

bool list_insert(list_handle_t handle, size_t index, void *data) {
    bool ret = false;
    list_lock(handle);
    do {
        if(!handle) break;
        list_handle_prv_t listHandle = (list_handle_prv_t)handle;
        if(index > listHandle->size) break;
        
        if(index == 0) {
            list_node_t *tempNode = list_create_node(data);
            if(!tempNode) break;
            tempNode->next = listHandle->head;
            listHandle->head = tempNode;
        } else {
            size_t curIndex = 0;
            list_node_t *prvNode = listHandle->head;
            if(!prvNode) break;
            while(curIndex != (index - 1)) {
                prvNode = prvNode->next;
                if(!prvNode) break;
                curIndex++;
            }

            if(curIndex != (index - 1)) break;
            list_node_t *tempNode = list_create_node(data);
            if(!tempNode) break;

            tempNode->next = prvNode->next;
            prvNode->next = tempNode; 
        }
        
        listHandle->size++;
        ret = true;
    } while(0);
    list_unlock(handle);
    return ret;
}

bool list_remove(list_handle_t handle, void *data, listCompareFunc compareFunc, listFreeFunc freeFunc) {
    bool ret = false;
    list_lock(handle);
    do {
        if(!handle) break;
        if(!compareFunc) compareFunc = list_compare;
        list_handle_prv_t listHandle = (list_handle_prv_t)handle;
        list_node_t *prvNode = NULL;
        list_node_t *curNode = listHandle->head;
        while(curNode) {
            if(compareFunc(curNode->data, data)) break;
            prvNode = curNode;
            curNode = curNode->next;
        }

        if(!curNode) break;

        if(prvNode == NULL) {
            listHandle->head = curNode->next;
        } else {
            prvNode->next = curNode->next;
        }

        if(freeFunc) {
            freeFunc(curNode->data);
        }


        util_free(curNode);
        listHandle->size--;
        ret = true;
    } while(0);
    list_unlock(handle);
    return ret;
}

bool list_remove_at(list_handle_t handle, size_t index, listFreeFunc freeFunc) {
    bool ret = false;
    list_lock(handle);
    do {
        if(!handle) break;
        list_handle_prv_t listHandle = (list_handle_prv_t)handle;
        if(index >= listHandle->size) break;

        list_node_t *prvNode = NULL;
        list_node_t *curNode = listHandle->head;
        size_t curIndex = 0;
        while(curNode) {
            if(curIndex == index) break;
            prvNode = curNode;
            curNode = curNode->next;
            curIndex++;
        }

        if(!curNode) break;

        if(prvNode == NULL) {
            listHandle->head = curNode->next;
        } else {
            prvNode->next = curNode->next;
        }

        if(freeFunc) {
            freeFunc(curNode->data);
        }

        util_free(curNode);
        listHandle->size--;
        ret = true;
    } while(0);
    list_unlock(handle);
    return ret;
}

int list_find(list_handle_t handle, void *data, listCompareFunc func) {
    int ret = -1;
    list_lock(handle);
    do {
        if(!handle) break;
        if(!func) func = list_compare;
        list_handle_prv_t listHandle = (list_handle_prv_t)handle;

        list_node_t *curNode = listHandle->head;
        size_t curIndex = 0;
        while(curNode) {
            if(func(data, curNode->data)) break;
            curNode = curNode->next;
            curIndex++;
        }

        if(!curNode) break;

        ret = curIndex;
    } while(0);
    list_unlock(handle);
    return ret;
}

size_t list_size(list_handle_t handle) {
    list_lock(handle);
    list_handle_prv_t listHandle = (list_handle_prv_t)handle;
    size_t ret = ((!listHandle)? 0: listHandle->size);
    list_unlock(handle);
    return ret;
}

void *list_get(list_handle_t handle, size_t index) {
    void *ret = NULL;

    do {
        if(!handle) break;
        list_handle_prv_t listHandle = (list_handle_prv_t)handle;
        if(index >= listHandle->size) break;

        list_node_t *curNode = listHandle->head;
        size_t curIndex = 0;
        while(curNode) {
            if(curIndex == index) break;
            curNode = curNode->next;
            curIndex++;
        }

        if(!curNode) break;
        ret = curNode->data;

    } while(0);

    return ret;
}

bool list_clear(list_handle_t handle, listFreeFunc func) {
    bool ret = false;
    list_lock(handle);
    do {
        if(!handle) break;
        list_handle_prv_t listHandle = (list_handle_prv_t)handle;
        list_node_t *cNode = listHandle->head;
        while(cNode) {
            list_node_t *temp = cNode->next;
            if(func && cNode->data) {
                func(cNode->data);
            }
            util_free(cNode);
            cNode = temp;
        }
        ret = true;
    } while(0);
    list_unlock(handle);

    return ret;
}

bool list_is_empty_(list_handle_t handle) {
    list_lock(handle);
    list_handle_prv_t listHandle = (list_handle_prv_t)handle;
    bool ret = ((!listHandle)? false: (listHandle->size == 0));
    list_unlock(handle);
    return ret;
}