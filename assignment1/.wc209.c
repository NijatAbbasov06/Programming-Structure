#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<ctype.h>

/* defining DFA State: 
Out - outside the word
In - inside the word
Com_Action - Actions when comment is expected
Line_Com - Inside Line comment
Block_Com - Inside Block comment
Check - Actions when input is '*' inside Block_Com
*/
enum State {Out, In, Com_Action, Line_Com, Block_Com, Check};
//nc - newline count, wc - word count, cc - character count
int nc =0, wc =0, cc=0 ;
//variable for getchar()
char c;
//0 state for DFA
enum State state = Out;
//boolean for Block and Line states
char boolean;
int error_line;




int main(void){
  while((c = getchar()) != EOF){
    switch(state){
      
      
      case Out:
       
        if (c =='\n'){
          nc++;
          cc++;
        }
        
        //possibility of comment
        else if (c=='/'){
          wc++;
          state = Com_Action;
          boolean = 1;
        }
        
        else if (isspace(c)) cc++;


        else if (!isspace(c)) {
          cc++;
          wc++;
          state = In;
        }
        break;

      case In:

        if (c=='\n'){
          nc++;
          cc++;
          state = Out;
        }
        
        //possibility to be a comment
        else if (c=='/'){
          boolean = 0;
          state = Com_Action;
        }

        else if(!isspace(c)) cc++;

        else if(isspace(c)) {
          cc++;
          state = Out;
        }
        break;
      case Com_Action:

        if (c=='*'){
          state = Block_Com;
          error_line = nc;
          if (boolean == 1) wc--;
        }
  
        else if (c=='/'){
          state = Line_Com;
          if (boolean == 1) wc--;
        }
        
        //comment is not created and we go outside word
        else if (isspace(c)){
          if (c=='\n'){
            nc++;
          }
          cc=cc+2;
          state = Out;
        }
        //comment is not created and we go inside word
        else if (!isspace(c)){
          cc=cc+2;
          state = In;
        }
        break;
      case Line_Com:
        
        if (c=='\n'){
          nc++;
          cc++;
          state = Out;
        }
        break;

      case Block_Com:
        
        //possibility of escaping block comment

        if (c == '*') {
          
          state = Check;
        }
        //staying inside Block comment
        else {
          if (c=='\n'){
            nc++;
            cc++;
          }
        }
      break;
        
      case Check:
        
        //escaping block comment
        if (c == '/'){
          cc++;
          state = Out;
        }
        //failed to escape
        else{
          if (c=='\n'){
            nc++;
            cc++;
          }
          state = Block_Com;
        }
      break;
    }
  }

  if (state == Block_Com || state == Check){
    fprintf(stderr, "Error: line %d: unterminated comment\n", error_line);
  }

  printf("%d %d %d\n", nc, wc, cc);
  return EXIT_SUCCESS;
  
}
