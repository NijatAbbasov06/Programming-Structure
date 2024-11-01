#define _GNU_SOURCE 1
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"







#define UNIT_ARRAY_SIZE 1024

struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  int purchase;              // purchase amount (> 0)
};

struct DB {
  struct UserInfo *pArray;   // pointer to the array
  int curArrSize;            // current array size (max # of elements)
  int numItems;              // # of stored items, needed to determine
	// # whether the array should be expanded
	//# or not
};


/*--------------------------------------------------------------------*/
DB_T
CreateCustomerDB(void)
{

  DB_T d;
  
  d = (DB_T) calloc(1, sizeof(struct DB));
  if (d == NULL) {
    fprintf(stderr, "Can't allocate a memory for DB_T\n");
    return NULL;
  }
  d->curArrSize = UNIT_ARRAY_SIZE; // start with 1024 elements
  d->numItems = 0;                 //start with 0 Users
  d->pArray = (struct UserInfo *)calloc(d->curArrSize, 
            sizeof(struct UserInfo));
  if (d->pArray == NULL) {
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	    d->curArrSize);   
    free(d);
    return NULL;
  }
  
  return d;

}
/*--------------------------------------------------------------------*/
void
DestroyCustomerDB(DB_T d)
{
  struct UserInfo * pUser = d->pArray;
  if (d){
    for (int i =0; i<d->curArrSize; i++){
      free(pUser->name);
      pUser->name = 0;
      free(pUser->id);
      pUser->id =0;
      pUser++;
    }
    free(d->pArray);
    free(d);
  }
}
/*--------------------------------------------------------------------*/
int
RegisterCustomer(DB_T d, const char *id,
		 const char *name, const int purchase)
{
  /* fill out this function */
  if (!d || !name || !id){
    return (-1);
  }
  if (purchase <= 0) {
    return (-1);
  }
  struct UserInfo * pUser = d->pArray;
  struct UserInfo * pUser_copy = d->pArray;
  for (int i = 0; i < d->curArrSize; i ++){
    if (!pUser->name || !pUser->id) continue;
    if(strcmp(pUser->name, name) == 0 || strcmp(pUser->id, id) ==0){
      return(-1);
    } 
    pUser ++;
  }
  if(d->numItems == d->curArrSize){
    int newArrSize = d->curArrSize + UNIT_ARRAY_SIZE;
    if(newArrSize<d->curArrSize){
      fprintf(stderr, "Maximum customer size is reached\n");
      return (-1);
    }
    d->curArrSize = newArrSize;
    printf("the value: %d\n",d->curArrSize);
    assert(d->pArray);
    struct UserInfo* pointer = (struct UserInfo*)realloc(d->pArray, 
                          d->curArrSize * sizeof(struct UserInfo));
    if (pointer == NULL){
      fprintf(stderr, "Realloc failed\n");
      return(-1);
    }
    else{
      d->pArray = pointer;
      memset(d->pArray + d->numItems, 0, 
                    UNIT_ARRAY_SIZE * sizeof(struct UserInfo));
    }
  }

  for (int i = 0; i< d-> curArrSize; i++){
    if (pUser_copy->name == 0){
      assert(name);
      assert(id);
      pUser_copy->name = strdup(name);   
      pUser_copy->id = strdup(id);
      pUser_copy->purchase = purchase; 
      d->numItems +=1;
      assert(d->numItems);
      return 0;
    }
    else pUser_copy++;
  }
  assert(0);
}
/*--------------------------------------------------------------------*/
int
UnregisterCustomerByID(DB_T d, const char *id)
{
  /* fill out this function */
  if (!d || !id){
    return (-1);
  }
  struct UserInfo * pUser = d->pArray;
  for (int i = 0; i<d->curArrSize; i++){
    if (!(pUser->id)) {
      continue;
    }
    int val = strcmp(pUser->id, id);
    if (val == 0){
      free(pUser->name);
      pUser->name = 0;
      free(pUser->id);
      d->numItems --;
      pUser->id = 0;
      pUser->purchase =0;
      return 0;
    }
    else pUser ++;
  }

  return (-1) ;
}

/*--------------------------------------------------------------------*/
int
UnregisterCustomerByName(DB_T d, const char *name)
{
  /* fill out this function */
  if (!d || !name){
    return (-1);
  }
  struct UserInfo * pUser = d->pArray;
  for (int i = 0; i<d->curArrSize; i++){
    if (!(pUser->name)) {
      continue;
    }
    int val = strcmp(pUser->name, name);
    if (val == 0){
      free(pUser->name);
      pUser->name = 0;
      free(pUser->id);
      d->numItems --;
      pUser->id = 0;
      pUser->purchase = 0;
      return 0;
    }
    else pUser++;
  }
  return (-1);
}
/*--------------------------------------------------------------------*/
int
GetPurchaseByID(DB_T d, const char* id)
{
  /* fill out this function */
  if (!d || !id){
    return (-1);
  }
  struct UserInfo * pUser = d->pArray;
  for (int i = 0; i<d->curArrSize; i++){
    if (!(pUser->id)) {
      continue;
    }
    int val = strcmp(pUser->id, id);
    if (val == 0){
      return pUser->purchase;
    }
    pUser++;
  }
  
  return (-1);
}
/*--------------------------------------------------------------------*/
int
GetPurchaseByName(DB_T d, const char* name)
{
  /* fill out this function */
  if (!d || !name){
    return (-1);
  }
  struct UserInfo * pUser = d->pArray;
  for (int i = 0; i<d->curArrSize; i++){
    if (!(pUser->name)) {
      continue;
    }
    int val = strcmp(pUser->name, name);
    if (val == 0){
      return pUser->purchase;
    }
    pUser ++;
  }
  return (-1);
}
/*--------------------------------------------------------------------*/
int
GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
{
  /* fill out this function */
  if (!d || !fp){
    return (-1);
  }
  int sum = 0;
  struct UserInfo * pUser = d->pArray;
  for (int i = 0; i<d->curArrSize; i++){
    if (!(pUser->name || pUser->id)) {
      continue;}
    int increment = (*fp)(pUser->id, pUser->name, pUser->purchase);
    sum += increment;
    pUser++;
  }

  return sum;
}

