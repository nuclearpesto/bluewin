/*
Author David Boeryd
*/
struct Stack
{
int size;
int capacity;
int *current_place;
};
typedef struct Stack stack;


void createstack(stack *s, int capacity); 

int push(stack *s, int nr);

int pop(stack *s);

