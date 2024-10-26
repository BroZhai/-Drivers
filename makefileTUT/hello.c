#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include"message.h" 
// This operation import the abstract method message, and was implemented in "message.c"

void main(){
    message(); // Directely calling the function
}

/*
  As we can tell, multiple compilation are required before running hello.c 
  but with the use of makefile, we can just directly write an 'auto-compiling' config file
  to automize this procedure.
  This is called "makefile", let's head to "makefile" and see how it works.
*/
