#include <stdio.h>
#include <string.h>
#include <unistd.h>

int getFlagInt(char* flags, char flag);
void getFile(char** argv, int b, int e, int n, int s, int t, int T, int E,
             int v);
int catOutput(int b, int e, int n, int s, int t, int T, int E, int v,
              FILE* file, int count, int* prev, int* firstFile);
