// TAKEN FROM http://stackoverflow.com/questions/8742270/how-to-remove-all-debug-printf-statements-from-c-code
//answer 1

#ifdef DEBUG
#  define D(x) x
#else
#  define D(x) 
#endif
