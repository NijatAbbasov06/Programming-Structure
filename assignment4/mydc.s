# --------------------------------------------------------------------
# dc.s
#
# Desk Calculator (dc) (x86-64)
#
# Student ID: 20230929
# --------------------------------------------------------------------

    .equ   BUFFERSIZE, 32
    .equ   EOF, -1
    .equ   ASCII_NEG, 95
    .equ   ASCII_P, 112
    .equ   ASCII_Q, 113
    .equ   ASCII_F, 102
    .equ   ASCII_C, 99
    .equ   ASCII_PLUS, 43
    .equ   ASCII_SUB, 45
    .equ   ASCII_MULTIPLICATION, 42
    .equ   ASCII_DIVISION, 47
    .equ   ASCII_REMAINDER, 37
    .equ   ASCII_POWER, 94


# --------------------------------------------------------------------

.section ".rodata"
    
scanfFormat:
     .asciz "%s"

printfFormat_int:
     .asciz "%d\n"

remainder_zero:
     .asciz "dc: remainder by zero\n"

division_zero:
	 .asciz "dc: divide by zero\n"

overflow:
     .asciz "dc: overflow happens\n"

stack_empty:
     .asciz "dc: stack empty\n"
UNF_string:
     .asciz ""



# --------------------------------------------------------------------

    .section ".data"

# --------------------------------------------------------------------

    .section ".bss"



# --------------------------------------------------------------------
    .section ".text"

    # -------------------------------------------------------------
    # int powerfunc(int base, int exponent)
    # Runs the power function.  Returns result.
    # -------------------------------------------------------------


    .globl	powerfunc, multiplication, division
    .globl  remainder, subtraction, subtraction, peek 
    .globl  check_operand_above1, put_param


    .type   powerfunc, @function
    .type   multiplication, @function
    .type   division, @function
    .type   remainder, @function
    .type   subtraction, @function
    .type   subtraction, @function
    .type   peek, @function
    .type   put_param, @function
    .type   check_operand_above1, @function
    


    # base is stored in %rdi
    # exponent is stored in %rsi


powerfunc:
    #prolog
    pushq   %rbp
    movq    %rsp, %rbp
    #To make the bits zero and put the base to %eax
    movq    $0, %rax
    movl    %edi, %eax



    .loop:

    #the loop goes untill base=1,
    #I multiply base by itself (exp - 1) times
    cmpl    $1, %esi
    jle     .endloop

    #if overflow happens when multiplying,
    #go to .overflow
    imul    %edi
    jo     .overflow

    #decrease exponent by 1
    decl    %esi
    jmp     .loop
    
    .endloop:

    #equivalent with leave
    movq    %rbp, %rsp
    popq    %rbp
    ret    


multiplication:

    #to initialize registers
    movq    $0, %rax
    movq    $0, %rcx
    
    #upload the inputs to the registers
    movl    %edi, %eax
    movl    %esi, %ecx

    #go to .overflow if there is overflow
    imull   %ecx
    jo      .overflow

    movq    $1, %rbx
    ret


division:
    #initialize registers
    movq    $0, %rax
    movq    $0, %rcx

    #rdi is numerator, rsi denominator
    movl    %edi, %eax
    movl    %esi, %ecx

    #when the denonimanor is zero
    cmpl     $0, %ecx
    je      .zero_divide
    
    #when the numerator is not UFN
    cmpl     $-2147483648, %eax
    jne      .nooverflow
    
    #go to overflow if numerator is UFN and denominator -1
    cmpl     $-1, %ecx
    je       .overflow

    #go to nonoverflow if numerator is UFN and denominator 1
    cmpl      $1, %ecx
    je       .nooverflow

    ret


    .nooverflow:
        #convert double word to quad
        cdq

        idivl   %ecx

        #for telling main func to quit 
        #because of overflow 

        movq    $1, %rbx
        ret


    .zero_divide:
        movq    stderr, %rdi
        leaq    division_zero, %rsi
        movq    $0, %rdx
        movb    $0, %al

        #to align rsp to 16 byte
        subq    $8, %rsp
        call    fprintf

        #to tell main that there is no overflow
        movq    $0, %rbx
        ret

subtraction:
    movq    $0, %rax
    movq    $0, %rcx
    movq    %rdi, %rax
    movq    %rsi, %rcx
    subl    %ecx, %eax
    ret

addition:
    movq    $0, %rax
    movq    $0, %rcx
    movq    %rdi, %rax
    movq    %rsi, %rcx
    addl    %ecx, %eax
    jo      .overflow
    movq    $1, %rbx
    ret




remainder:
    movq    $0, %rax
    movq    $0, %rcx

    #rdi is numerator, rsi denominator
    movq    %rdi, %rax
    movq    %rsi, %rcx
    
    #go to zerodivide if denominator is 0 
    cmpl     $0, %ecx
    je       .zero_divide_remainder
    
    #go to no overflow if numerator is not UFN
    cmpl     $-2147483648, %eax
    jne      .nooverflow_remainder
    
    #go to nonoverflow if numerator is UFN
    #and denominator is not -1

    cmpl     $-1, %ecx
    jne      .nooverflow_remainder


    movl $0, %eax
    movq    $1, %rbx
    ret     


    .nooverflow_remainder:
        #for changing double word to quad,
        #we need to do it when doing division.
        #It sign extends our value
        cdq
        idivl   %ecx
        movq    $0, %rax
        #moving the remainder value to eax
        movl    %edx, %eax
        #for the main to understand there is overflow
        movq    $1, %rbx
        ret

    .zero_divide_remainder:
        movq    stderr, %rdi
        leaq    remainder_zero, %rsi
        movq    $0, %rdx
        movb    $0, %al
        subq    $8, %rsp
        call    fprintf
        movq    $0, %rbx
        ret

    

peek:
    #this function returns the difference between 
    #start of the buffer and the rsp.
    #it is basically len(stack)
    movq    %rax, %rcx
    leaq    -BUFFERSIZE(%rbp), %rdx
    subq    %rdx, %rax
    ret

check_operand_above1:
    #checking if len(stack) >= 32. Means there are more 
    #than one element
    call    peek
    cmp     $-32, %rax     
    jle     .calling_put_param
    #this will tell the main to .print_message
    movq    $0, %rax
    ret     

    .calling_put_param:
        #calls put_param
        movq     $1, %rax
        call     put_param
        ret


put_param:
    #sets up the parameters before 
    #calling the functions such as addition and etc.
    movq    8(%rcx), %rsi
    movq    24(%rcx), %rdi
    ret


.overflow:
    movq    stderr, %rdi
    leaq    overflow, %rsi
    movq    $0, %rdx
    movb    $0, %al
    subq    $8, %rsp
    call    fprintf
    movq    $0, %rbx
    ret




# -------------------------------------------------------------
# int main(void)
# Runs desk calculator program.  Returns 0.
# -------------------------------------------------------------

    .text
    .globl  main
    .type   main, @function

main:

    pushq   %rbp
    movq    %rsp, %rbp

    # char buffer[BUFFERSIZE]
    subq    $BUFFERSIZE, %rsp
    
    # Note %rsp must be 16-B aligned before call

.input:

    
    leaq    scanfFormat, %rdi
    leaq    -BUFFERSIZE(%rbp), %rsi
    movb    $0, %al
    call    scanf
    

    cmp	    $EOF, %eax
    je	    .quit
    

    movq    $0, %rdi
    movb    -BUFFERSIZE(%rbp), %dil
    movb    $0, %al
    call    isdigit

    leaq    -BUFFERSIZE(%rbp), %rdi
    
    

    test    %eax, %eax
    jz      .nondigit
    jmp     .isdigit_and_positive
    
  

.nondigit:

    movb    (%rdi), %dil

    #%dil is the first byte of the inputted string.
    #By checking that byte, we call the suitable function


    cmpb    $ASCII_Q, %dil
    jz      .quit

    cmpb    $ASCII_P, %dil
    jz      .print_stack
    

    cmpb    $ASCII_NEG, %dil
    jz      .isdigit_and_negative


    cmpb     $ASCII_F, %dil
    jz      .print_all_stack

    cmpb     $ASCII_C, %dil
    jz      .clear_stack

    cmpb     $ASCII_MULTIPLICATION, %dil
    jz       .call_multiplication     
     


    cmpb     $ASCII_DIVISION, %dil
    jz      .call_division
     

    cmpb     $ASCII_PLUS, %dil
    jz       .call_addition
     

    cmpb     $ASCII_POWER, %dil
    jz      .call_powerfunction
  
         

    cmpb     $ASCII_SUB, %dil
    jz      .call_subtraction
     


    cmpb     $ASCII_REMAINDER, %dil
    jz      .call_remainder
     

#the followings, are for calling the functions.
#The implementations include checking if the
#len(stack) is bigger than 2 and aligning the rsp to 16 byte. 
#We get rid of the last two elements in the stack after
#calling the function and align the rsp after pushing the result of the operation.


.call_addition:
    movq    %rsp, %rax
    call    check_operand_above1
    cmp     $0, %eax
    jz      .print_message
    call    addition
    cmp     $0, %rbx
    je      .quit
    addq    $32, %rsp
    pushq   %rax
    subq    $8, %rsp
    jmp     .input

.call_subtraction:
    movq    %rsp, %rax
    call    check_operand_above1
    cmp     $0, %eax
    jz      .print_message
    call    subtraction
    cmp     $0, %rbx
    je      .quit
    addq    $32, %rsp
    pushq   %rax
    subq    $8, %rsp
    jmp     .input
    

    
.call_powerfunction:
    movq    %rsp, %rax
    call    check_operand_above1
    cmp     $0, %eax
    jz      .print_message
    call    powerfunc
    cmp     $0, %rbx
    je      .quit
    addq    $32, %rsp
    pushq   %rax
    subq    $8, %rsp
    jmp     .input
    
    
    
.call_division:
    movq    %rsp, %rax
    call    check_operand_above1
    cmpl    $0, %eax
    jz      .print_message
    call    division
    cmp     $0, %rbx
    je      .quit
    addq    $32, %rsp
    pushq   %rax
    subq    $8, %rsp
    jmp     .input
    

    
.call_multiplication:
    movq    %rsp, %rax
    call    check_operand_above1
    cmpl    $0, %eax
    jz      .print_message
    call    multiplication
    cmp     $0, %rbx
    je      .quit
    addq    $32, %rsp
    pushq   %rax
    subq    $8, %rsp
    jmp     .input
    

.call_remainder:
    movq    %rsp, %rax
    call    check_operand_above1
    cmp     $0, %eax
    jz     .print_message
    call     remainder
    cmp     $0, %rbx
    je      .quit
    addq    $32, %rsp
    pushq   %rax
    subq    $8, %rsp
    jmp     .input
    

    

    

.print_message:
    #prints stack is empty
    movq    stderr, %rdi
    leaq    stack_empty, %rsi
    movq    $0, %rdx
    movb    $0, %al
    call    fprintf
    jmp     .input

   
.isdigit_and_positive:
    #when the digit is positive
    movb    $0, %al
    call    atoi
    leaq    -BUFFERSIZE(%rbp), %rdi
    pushq   %rax
    subq    $8, %rsp
    jmp     .input

.isdigit_and_negative:
    #when the string starts with _
    leaq    -BUFFERSIZE(%rbp), %rdi
    leaq    1(%rdi), %rdi
    movb    $0, %al
    call    atoi
    
    #negate because of _
    neg     %rax
    pushq    %rax
    subq    $8, %rsp
    jmp     .input




.print_stack:
    #we peek the stack, if the element is bigger than
    #0, we print the top element
    movq    %rsp, %rax
    call    peek
    test    %eax, %eax
    jz      .print_message
    leaq    printfFormat_int, %rdi
    movq    8(%rsp), %rsi  
    call    printf
    jmp     .input
  


.clear_stack:
    #increases the %rsp value to the
    #start of the buffer
    leaq    -BUFFERSIZE(%rbp), %rsp  
    jmp     .input



.print_all_stack:

    movq    %rsp, %rbx

    .loop_all_stack:
        #After loading rsp to rbx we increase rbx 
        #till the part when we reach the start of
        #the buffer
        leaq    -40(%rbp), %rcx
        cmpq    %rcx, %rbx
        jg      .endloop_all_stack
        
        #At each stack element, we print the element
        leaq    printfFormat_int, %rdi
        movq    8(%rbx), %rsi  
        call    printf
        addq    $16, %rbx
        jmp     .loop_all_stack


    .endloop_all_stack:
        jmp     .input


.quit:
    # return 0
    movq    $0, %rax
    addq    $BUFFERSIZE, %rsp
    movq    %rbp, %rsp
    popq    %rbp
    ret
