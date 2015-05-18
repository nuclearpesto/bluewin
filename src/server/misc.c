#include <stdlib.h>
#include <stdio.h>
#include "misc.h"
#include "debug.h"
//Toturial taken from here
//http://www.thelearningpoint.net/computer-science/data-structures-stacks--with-c-program-source-code

void createstack(stack *s,int capacity)
{
    s->current_place = (int *)malloc(sizeof(int )*capacity);
    s->capacity = capacity;
    s->size = 0;
}

int push(stack *s, int nr)
{

    if(s->size == s->capacity)
    {
      D( printf("Stack overflow"));
        return 1;
    }

    //printf("pushing %d to stack\n", nr);
    *(s->current_place) = nr;
    s->size++;
    s->current_place++;
    return 0;
}

int pop(stack *s)
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
void fill_int_stack(stack *s, int size){
int i;
  for(i=size;i>0;i--)
    {
      push(s, i-1);
    }
}
