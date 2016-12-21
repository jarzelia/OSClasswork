//Eric Wang
//eric_wang1@student.uml.edu
//code taken from the assign 5 data structs file.


#include "main.h"

int main(int argc, char *argv[]){

  //check for correct number of args
  if(!argc > 3){
    printf("\n");
    printf("You must provide the policy, the memory size, and the input file.\n");
    printf("You only provided %d of them.", argc);
    exit(1);
  }

  //convert mem size to an int
  int mem = atoi(argv[2]);

  //call best
  if(strcmp(argv[1], "best") == 0){
    best(mem, argv[3]);
  }

  //call first
  if(strcmp(argv[1], "first") == 0){
     first(mem, argv[3]);
  }

  //call buddy
  if(strcmp(argv[1], "buddy") == 0){
    buddy(mem, argv[3]);
  }

  return 0;


}
