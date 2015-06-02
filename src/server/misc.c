/* Author Jonathan Aspenbom*/
#include <stdlib.h>
#include <stdio.h>
#include "misc.h"
#include "debug.h"
//Toturial taken from here
//http://www.thelearningpoint.net/computer-science/data-structures-stacks--with-c-program-source-code
//Published by: www.thelearningpoint.net
void createstack(stack *s,int capacity)//create a new stack
{
    s->current_place = (int *)malloc(sizeof(int )*capacity);
    s->capacity = capacity;
    s->size = 0;
}

int push(stack *s, int nr)
{

    if(s->size == s->capacity)
    {
      D( printf("Stack overflow"));//check if stack is full
        return 1;
    }

    //printf("pushing %d to stack\n", nr);
    *(s->current_place) = nr;
    s->size++;
    s->current_place++;
    return 0;
}

int pop(stack *s)//pop next number from stack
{
    if(s->size == 0)
    {
        return -1;
    }
    s->size--;
    s->current_place--;
    int nr = *(s->current_place);
    D(printf("popping %d from stack\n", nr));
    return nr;
}
void fill_int_stack(stack *s, int size){//fill the stack with all numbers from size -1 to 0, putting 0 on top of stack
int i;
  for(i=size;i>0;i--)
    {
      push(s, i-1);
    }
}
