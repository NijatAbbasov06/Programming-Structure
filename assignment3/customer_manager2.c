#define _GNU_SOURCE 1
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

enum {HASH_MULTIPLIER = 65599};

static int hash_function(const char *pcKey, int iBucketCount)
/* Return a hash code for pcKey that is between 0 and iBucketCount-1,
  inclusive. Adapted from the EE209 lecture notes. */
{
  int i;
  unsigned int uiHash = 0U;
  for (i = 0; pcKey[i] != '\0'; i++)
    uiHash = uiHash * (unsigned int)HASH_MULTIPLIER
          + (unsigned int)pcKey[i];
  return (int)(uiHash % (unsigned int)iBucketCount);
}



#define UNIT_ARRAY_SIZE 4



struct UserInfo {
  char *name;                // customer name
  char *id;                  // customer id
  int purchase;              // purchase amount >0
  struct UserInfo* next_ID;  
  struct UserInfo* next_Name;      
};

struct DB {
  struct UserInfo **pArray_Name;
  struct UserInfo **pArray_ID;  // pointer to the array
  int curArrSize;            // current array size (max # of elements)
  int numItems;              // # of stored items, needed to determine
	// # whether the array should be expanded
	//# or not
};

void resize_and_rehash(DB_T d) {
    int oldSize = d->curArrSize;
    int newSize = d->curArrSize * 2;
    struct UserInfo **oldArray_Name = d->pArray_Name;
    struct UserInfo **oldArray_ID = d->pArray_ID;

    d->pArray_Name = (struct UserInfo **)calloc(newSize, sizeof(struct UserInfo *));
    d->pArray_ID = (struct UserInfo **)calloc(newSize, sizeof(struct UserInfo *));
    d->curArrSize = newSize;

    for (int i = 0; i < oldSize; i++) {
      struct UserInfo * pUser = oldArray_Name[i];
      while (pUser) {
        int newIndex_Name = hash_function(pUser->name, newSize);
        pUser->next_Name = d->pArray_Name[newIndex_Name];
        d->pArray_Name[newIndex_Name] = pUser;
        pUser = pUser->next_Name;
      }

      pUser = oldArray_ID[i];
      while (pUser) {
        struct UserInfo *next = pUser->next_ID;
        int newIndex_ID = hash_function(pUser->id, newSize);
          pUser->next_ID = d->pArray_ID[newIndex_ID];
          d->pArray_ID[newIndex_ID] = pUser;
          pUser = next;
      }
    }
    free(oldArray_Name);
    free(oldArray_ID);

}


/*--------------------------------------------------------------------*/

struct UserInfo* Table_search_Name(DB_T d, const char *name,
                                       int index_Name)
{
  
  struct UserInfo* pUser_Name = *(d->pArray_Name + index_Name);
  for (; pUser_Name != NULL; pUser_Name = pUser_Name->next_Name){
    if (strcmp(pUser_Name->name, name) == 0) {
      return pUser_Name;
    }
  }

  return NULL;
}
struct UserInfo* Table_search_ID(DB_T d, const char *id,
                                 int index_ID){

  struct UserInfo* pUser_ID = *(d->pArray_ID + index_ID);
  for (; pUser_ID != NULL; pUser_ID = pUser_ID->next_ID){
    if (strcmp(pUser_ID->id, id) == 0) {
      return pUser_ID;
    }
    
  }
  return NULL;
}


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
  d->numItems = 0;                 // start with 0 Users  
  d->pArray_Name = (struct UserInfo **)calloc(d->curArrSize, 
            sizeof(struct UserInfo*));
  d->pArray_ID = (struct UserInfo **)calloc(d->curArrSize, 
            sizeof(struct UserInfo*));
  if (d->pArray_Name == NULL) {
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	    d->curArrSize);   
    free(d);
    return NULL;
  }
  if (d->pArray_ID == NULL) {
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
  struct UserInfo* pUser;
  struct UserInfo* nextp;
  

  if (d){
    for (int b = 0; b < d->curArrSize; b++){
      for (pUser = d->pArray_Name[b]; pUser != NULL; pUser = nextp) {
        nextp = pUser->next_Name;
        free(pUser->id);
        free(pUser->name);
        free(pUser);
      }

      

    }
    free(d->pArray_ID);
    free(d->pArray_Name);
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

  

  int index_Name = hash_function(name, d->curArrSize);
  int index_ID = hash_function(id, d->curArrSize);

  if (Table_search_Name(d, name, index_Name)!= NULL) {return (-1);}
  if (Table_search_ID(d, id, index_ID)!= NULL) {return (-1);}
  //resize Array
  if( d->numItems == (0.75 * (d->curArrSize)) && d->curArrSize != 1048576){
    resize_and_rehash(d);
  }
  //because curArrSize has changed
  index_Name = hash_function(name, d->curArrSize);
  index_ID = hash_function(id, d->curArrSize);
  
  struct UserInfo * new_user = (struct UserInfo*)calloc(1, sizeof(struct UserInfo));
  struct UserInfo * pUser_Name = *(d->pArray_Name + index_Name);
  struct UserInfo * pUser_ID = *(d->pArray_ID + index_ID);
  new_user->name = strdup(name);
  assert(new_user->name);
  new_user->id = strdup(id);
  assert(new_user->id);
  new_user->purchase = purchase;
  if (!pUser_ID){
    new_user->next_ID = NULL;
  }
  else{
    new_user->next_ID = pUser_ID->next_ID;
  }
  

  

  if (!pUser_Name){
    new_user->next_Name = NULL;
  }
  
  else{
    new_user->next_Name = pUser_Name->next_Name;
  }
  *(d->pArray_Name + index_Name) = new_user;
  *(d->pArray_ID + index_ID) = new_user;
  d->numItems++;
  return 0;
  
}
/*--------------------------------------------------------------------*/
int
UnregisterCustomerByID(DB_T d, const char *id)
{
  
  if (!d || !id){
    return (-1);
  }

  int index_ID = hash_function(id, d->curArrSize);
  struct UserInfo* pUser_ID = *(d->pArray_ID + index_ID);
  struct UserInfo* pUser_to_be_deleted;
  if (pUser_ID == NULL){
    return (-1);
  }

  if(strcmp(pUser_ID->id, id) == 0){
    *(d->pArray_ID+index_ID) = pUser_ID->next_ID;
    const char* name= pUser_ID->name;
    int index_Name = hash_function(name, d->curArrSize);
    struct UserInfo* pUser_Name = *(d->pArray_Name + index_Name);
    assert(pUser_Name);
    if(strcmp(pUser_Name->name, name) == 0){
      free(pUser_Name->name);
      free(pUser_Name->id);
      *(d->pArray_Name+index_Name) = pUser_Name->next_Name;
      free(pUser_Name);
      d->numItems --;
      return 0;
    }
    
    for (; pUser_Name != NULL; pUser_Name = pUser_Name->next_Name){
      if (strcmp(pUser_Name->next_Name->name, name) == 0) {
        pUser_Name->next_Name = pUser_Name->next_Name->next_Name;
        break;
      }
    }
    free(pUser_ID->name);
    free(pUser_ID->id);
    free(pUser_ID);
    d->numItems --;
    return 0;
  }
  
  for (; pUser_ID != NULL; pUser_ID = pUser_ID->next_ID){
    if (pUser_ID->next_ID == NULL) {
      pUser_ID = NULL;
      return (-1);
    }
    

    if (strcmp(pUser_ID->next_ID->id, id) == 0) {
      pUser_to_be_deleted = pUser_ID->next_ID;
      const char* name= pUser_to_be_deleted->name;
      int index_Name = hash_function(name, d->curArrSize);
      struct UserInfo* pUser_Name = *(d->pArray_Name + index_Name);
      pUser_ID->next_ID = pUser_ID->next_ID->next_ID;
      assert(pUser_Name);
      if(strcmp(pUser_Name->name, name) == 0){
        free(pUser_Name->name);
        free(pUser_Name->id);
        *(d->pArray_Name+index_Name) = pUser_Name->next_Name;
        free(pUser_Name);
        d->numItems --;
        return 0;
      }
      for (; pUser_Name != NULL; pUser_Name = pUser_Name->next_Name){
        if (strcmp(pUser_Name->next_Name->name, name) == 0) {
          pUser_Name->next_Name = pUser_Name->next_Name->next_Name;
          break;
        }
      }
      free(pUser_ID->next_ID->name);
      free(pUser_ID->next_ID->id);
      free(pUser_ID->next_ID);
      d->numItems --;
      break;
    }
  }

  //There is no element to delete
  if (pUser_ID == NULL){
    return (-1);
  }

  
  d->numItems --;
  return 0;
}

/*--------------------------------------------------------------------*/
int
UnregisterCustomerByName(DB_T d, const char *name)
{
  
    if (!d || !name){
    return (-1);
  }

  int index_Name = hash_function(name, d->curArrSize);
  struct UserInfo* pUser_Name = *(d->pArray_Name + index_Name);
  struct UserInfo* pUser_to_be_deleted;
  if (pUser_Name == NULL){
    return (-1);
  }

  if(strcmp(pUser_Name->name, name) == 0){
    *(d->pArray_Name+index_Name) = pUser_Name->next_Name;
    const char* id= pUser_Name->id;
    int index_ID = hash_function(id, d->curArrSize);
    struct UserInfo* pUser_ID = *(d->pArray_ID + index_ID);
    assert(pUser_ID);
    if(strcmp(pUser_ID->id, id) == 0){
      free(pUser_ID->id);
      free(pUser_ID->name);
      *(d->pArray_ID+index_ID) = pUser_ID->next_ID;
      free(pUser_ID);
      d->numItems --;
      return 0;
    }
    
    for (; pUser_ID != NULL; pUser_ID = pUser_ID->next_ID){
      assert(pUser_ID->next_ID->id);
      if (strcmp(pUser_ID->next_ID->id, id) == 0) {
        pUser_ID->next_ID = pUser_ID->next_ID->next_ID;
        break;
      }
    }
    free(pUser_Name->id);
    free(pUser_Name->name);
    free(pUser_Name);
    d->numItems --;
    return 0;
  }
  
  for (; pUser_Name != NULL; pUser_Name = pUser_Name->next_Name){
    if (pUser_Name->next_Name == NULL) {
      pUser_Name = NULL;
      return (-1);
    }
    

    if (strcmp(pUser_Name->next_Name->name, name) == 0) {
      pUser_to_be_deleted = pUser_Name->next_Name;
      const char* id= pUser_to_be_deleted->id;
      int index_ID = hash_function(id, d->curArrSize);
      struct UserInfo* pUser_ID = *(d->pArray_ID + index_ID);
      pUser_Name->next_Name = pUser_Name->next_Name->next_Name;
      assert(pUser_ID);
      if(strcmp(pUser_ID->id, id) == 0){
        free(pUser_ID->id);
        free(pUser_ID->name);
        *(d->pArray_ID+index_ID) = pUser_ID->next_ID;
        free(pUser_ID);
        d->numItems --;
        return 0;
      }
      for (; pUser_ID != NULL; pUser_ID = pUser_ID->next_ID){
        if (strcmp(pUser_ID->next_ID->id, id) == 0) {
          pUser_ID->next_ID = pUser_ID->next_ID->next_ID;
          break;
        }
      }
      free(pUser_Name->next_Name->id);
      free(pUser_Name->next_Name->name);
      free(pUser_Name->next_Name);
      d->numItems --;
      break;
    }
  }

  //There is no element to delete
  if (pUser_Name == NULL){
    return (-1);
  }

  
  d->numItems --;
  return 0;
}
/*--------------------------------------------------------------------*/
int
GetPurchaseByID(DB_T d, const char* id)
{

  if (!d || !id){
    return (-1);
  }
  int index_ID = hash_function(id, d->curArrSize);
  struct UserInfo* pUser = Table_search_ID(d, id, index_ID);
  if (pUser == NULL){
    return (-1);
  }
  int purchase = pUser->purchase;
  return purchase;
}
/*--------------------------------------------------------------------*/
int
GetPurchaseByName(DB_T d, const char* name)
{
  if (!d || !name){
    return (-1);
  }
  int index_Name = hash_function(name, d->curArrSize);
  struct UserInfo* pUser = Table_search_Name(d, name, index_Name);
  if (pUser == NULL){
    return (-1);
  }
  int purchase = pUser->purchase;
  return purchase;
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
  struct UserInfo* pUser;
  struct UserInfo* nextp;
  for (int b = 0; b < d->curArrSize; b++){
    for (pUser = d->pArray_Name[b]; pUser != NULL; pUser = nextp) {
      nextp = pUser->next_Name;
      int increment = (*fp)(pUser->id, pUser->name, pUser->purchase);
      sum+= increment;
    }
  }

  return sum;
}

