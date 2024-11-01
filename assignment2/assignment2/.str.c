#include <assert.h> /* to use assert() */
#include <stdio.h>
#include <stdlib.h> /* for strtol() */
#include <string.h>
#include <strings.h>
#include "str.h"
#include <limits.h>
#include <ctype.h>


//to convert a character to a lower case char.
char lower(char c){
  if (c>= 'A' && c<= 'Z'){
    return (c + 'a' - 'A');
  }
  return c;
}





/*StrGetLength() : finds the length of the string 
excluding null terminator

    Parameters: input is a const char* type pointer 
    to the string 
    
    Returns: the length of the string in type size_t 
    which is unsigned integer form.
     */

/* Part 1 */
/*Finds the length of a string*/
size_t StrGetLength(const char* pcSrc)
{
  const char *pcEnd;
  assert(pcSrc); /* NULL address, 0, and FALSE are identical. */
  pcEnd = pcSrc;
	
  while (*pcEnd) /* null character and FALSE are identical. */
    pcEnd++;

  return (size_t)(pcEnd - pcSrc);
}

/*The following function copies the string pointed 
to by src pointer, and it includes the '\0'*/
char *StrCopy(char *pcDest, const char* pcSrc)
{
  /* TODO: fill this function */
  assert(pcSrc);
  assert(pcDest);
  //copy the destination pointer 
  //to be able to return that later.
  char *pcDest_copy = pcDest;
  /*it copies the values that the pcEnd points 
  to the values that pcDest points to*/
  while(*pcSrc){
    //update the pcDest_copy
    *pcDest_copy = *pcSrc; 
    pcDest_copy ++;
    pcSrc++;
  }
  //put the terminating null character
  *pcDest_copy = '\0';


  return pcDest;
}



/*------------------------------------------------------------------------*/
int StrCompare(const char* pcS1, const char* pcS2)
{
  /* TODO: fill this function */
  assert(pcS1);
  assert(pcS2);
  
  while(*pcS1 == *pcS2){
    //Checks if the strings have the same chars till
    //'\0'.
    if (*pcS1=='\0'){
      return (int) 0;
    }
    pcS1++;
    pcS2++;

  }


  return (*pcS1 > *pcS2) - (*pcS1 < *pcS2);
  
}
/*The following function returns a pointer to 
the spesific character that is inside the string.
If the input character is '\0', then it also returns
pointer to the char '\0' as it is considered 
to be inside the string*/
char *StrFindChr(const char* pcHaystack, int c)
{
  /* TODO: fill this function */
  assert(pcHaystack);
  const char* pcHaystack_End = pcHaystack;
  while(1){
    if (*pcHaystack_End == (char) c){
      return (char *)pcHaystack_End;

      }
    else if(*pcHaystack_End == '\0'){
      return NULL;
    }
    pcHaystack_End++;
  }
}
/*------------------------------------------------------------------------*/
char *StrFindStr(const char* pcHaystack, const char *pcNeedle)
{
  /* TODO: fill this function */
  assert(pcHaystack);
  assert(pcNeedle);

  const char* pcHaystack_copy = pcHaystack;


  const char* pcNeedle_copy = pcNeedle;
  const char* pcHaystack_ans;
  for (; *pcHaystack; pcHaystack++){
    if (*pcHaystack == *pcNeedle) {
      pcHaystack_ans = pcHaystack;
      for (; *pcNeedle != '\0'; pcNeedle++){
        if (*pcNeedle != *pcHaystack){
          pcNeedle = pcNeedle_copy;
          break;
        }
        else{
          pcHaystack ++;
          continue;
        }

      }
      if (!*pcNeedle ){
        return (char*) pcHaystack_ans;
      }
    }
    else continue;
  }
  
  if(!*pcNeedle_copy){
    #ifdef DEBUG
      printf("%c \n", *pcNeedle_copy);
      printf("YEa: [%s]\n", pcHaystack);
      if (!(pcHaystack == pcHaystack_copy)) {
        printf("change");
      }
    #endif
    return (char*) pcHaystack_copy;
  }
  else if(!*pcHaystack){
    return NULL;
  }
  else {
    assert(0);
    return NULL;

  }
}

/*------------------------------------------------------------------------*/
char *StrConcat(char *pcDest, const char* pcSrc){
  /* TODO: fill this function */
  assert(pcDest);
  assert(pcSrc);
  char* pcDest_copy = pcDest;

 
  while (*pcDest) pcDest++;
  pcDest = StrCopy(pcDest, pcSrc);
  
  return pcDest_copy;
}

/*------------------------------------------------------------------------*/
long int StrToLong(const char *nptr, char **endptr, int base)
{
  /* handle only when base is 10 */

  assert(nptr);
  long int answer = 0;
  long int sign = 0;

  if (base != 10) return 0;

  while(*nptr && isspace(*nptr)){
    nptr++;
  }

  switch (*nptr)
  {
  case '+':
    sign = +1;
    nptr++;
    break;
  case '-':
    sign = -1;
    nptr++;
    break;
  
  default:
    sign = 1;
    break;
  }

  const char* nptr_copy = nptr;
  if (endptr != NULL){
    while(isdigit(*nptr_copy)){
    nptr_copy++;
    }

    *endptr = (char*) nptr_copy;
  }

  int temp;
  while(isdigit(*nptr) && *nptr){
    temp = *nptr - '0';
   
    if (answer > (LONG_MAX - temp)/10) {
      if (sign == -1){
        if (answer * (-1) < (LONG_MIN)/10 - temp /10){
          return LONG_MIN;
        }
      }
      return LONG_MAX;
    }


    if (answer < (LONG_MIN)/10 - temp /10) {
      if(sign == -1){
        if (answer *(-1) > (LONG_MAX -temp)/10){
          return LONG_MAX;
        }
      }
      
      return LONG_MIN; 
    }

    answer = answer * 10  + temp;


    #ifdef DEBUG
      printf("%ld\n", answer);
    #endif

    nptr++;
  }

  answer = answer * sign;
  return answer;

}

/*------------------------------------------------------------------------*/
int StrCaseCompare(const char *pcS1, const char *pcS2)
{
  /* TODO: fill this function */
  assert(pcS1);
  assert(pcS2);
  
  
  while(lower(*pcS1) == lower(*pcS2)){
    //Checks if the strings have the same chars till
    //'\0'.
    char c1 = lower(*pcS1);
    if (c1 =='\0'){
      return (int) 0;
    }
    pcS1++;
    pcS2++;
  }

  char c1 = lower(*pcS1);
  char c2 = lower(*pcS2);

  return (c1>c2) - (c1<c2);

}