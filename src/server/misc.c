#include <stdlib.h>
#include <stdio.h>
#include "misc.h"



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
        printf("Stack overflow");
        return 1;
    }
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
    int nr = *(s->current_place);
    s->size--;
    s->current_place--;
    return nr;
}
void fill_int_stack(stack *s, int size){
int i;
  for(i=size;i>0;i--)
    {
      push(s, i-1);
    }
}
