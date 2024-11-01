#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for skeleton code */
#include <unistd.h> /* for getopt */
#include <assert.h>
#include "str.h"

#define MAX_STR_LEN 1023

#define FALSE 0
#define TRUE  1



/* buf_main_code(): Recursive function that checks
    if the pattern exist in the text
    pattern: pointer for the pattern
    line: pointer for the text
    
    return: True or False depending if the pattern exists in the text*/

/*--------------------------------------------------------------------*/
/* PrintUsage()
   print out the usage of the Simple Grep Program                     */
/*--------------------------------------------------------------------*/

int buf_main_code(const char* pattern, char* line){
  assert(pattern);
  assert(line);
  
  if (!(*pattern)) return TRUE; //when the input is empty string
  /*To call the function again incase it encounters "*"*/
  if (*pattern == '*'){
    pattern++;
    return buf_main_code(pattern, line);
  }
  while(*line){
    //increases line untill there 
    //is a *line that matches 
    //first char of pattern.
    while(*line != *pattern && *line){
      line++;
    }
    //for the case when first char of pattern is not found
    if (!(*line)){
      return 0;
    }
    /*creates new variables to check if 
    the other chars untill "*" is also inside
    sequentially. */
    char* line_check = line;
    const char* pattern_check = pattern;
    while(*pattern_check && *line_check && *pattern_check == *line_check ){
      pattern_check++;
      line_check++;

      //incase we encounter "*" we need to call
      //function recursively.
      if (*pattern_check == '*'){
        pattern_check++;
        return buf_main_code(pattern_check, line_check);
      }
    }
    // checks if we reached the end of pattern_check
    //and returns true if that is true.
    if (!(*pattern_check)){
      return 1;
    }
    //changes the previous value of line
    //to continue checking for pattern.
    else{
      line++;
    }

  }
  return 0;
}

void
PrintUsage(const char* argv0)
{
  const static char *fmt =
	  "Simple Grep (sgrep) Usage:\n"
	  "%s pattern [stdin]\n";

  printf(fmt, argv0);
}
/*-------------------------------------------------------------------*/

/*-------------------------------------------------------------------*/
int
SearchPattern(const char *pattern)
{

  char buf[MAX_STR_LEN + 2];
  int line_exist = FALSE;

  /*
   *  TODO: check if pattern is too long
   */
  size_t pattern_length = StrGetLength(pattern);
  // +1 to account for '\0'
  if (pattern_length > (MAX_STR_LEN)){
    fprintf(stderr, "Error: pattern is too long\n");
    return FALSE;
  }


  /* Read one line at a time from stdin, and process each line */
  while (fgets(buf, sizeof(buf), stdin)) {

    /* TODO: check the length of an input line */
    /* TODO: fill out this function */


    char* buf_find_str = buf;
    //checks line length
    size_t line_length = StrGetLength((const char*)buf);
    if (line_length > MAX_STR_LEN){
      fprintf(stderr, "Error: input line is too long\n");
      return FALSE;
    }


    if (buf_main_code(pattern, buf_find_str)){
      line_exist = TRUE;
      printf("%s", buf_find_str);
    }
    else{
      #ifdef DEBUG
        printf("the line doesnt have pattern");
      #endif
      continue;
    }

  }
  return (line_exist) ? TRUE : FALSE;
}
/*-------------------------------------------------------------------*/
int main(const int argc, const char *argv[])
{
  /* Do argument check and parsing */
  if (argc < 2) {
	  fprintf(stderr, "Error: argument parsing error\n");
	  PrintUsage(argv[0]);
	  return (EXIT_FAILURE);
  }

  return SearchPattern(argv[1]) ? EXIT_SUCCESS:EXIT_FAILURE;
}


