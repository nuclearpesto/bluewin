#ifndef THREAD_COUNT
#define THREAD_COUNT 1000
#endif


struct Stack
{
int size;
int capacity;
int *current_place;
};
typedef struct Stack stack;

clients_t* create_client(int socket, stack *available_client_nr);

void createstack(stack *s, int capacity);

int push(stack *s, int nr);

int pop(stack *s);
