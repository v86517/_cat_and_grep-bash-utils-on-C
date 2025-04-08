#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
  char f_i;
  char f_v;
  char f_c;
  char f_l;
  char f_n;
  char f_h;
  char f_s;
  char f_o;
  char f_ef;
} Flags;

int fillFlagsStruct(Flags *flags_struct, char *argv);
int getPatterns(int argc, char **argv, char *pattern, char **arrayOfStrToSearch,
                Flags *flags_struct);
int getFiles(int argc, char **argv, char **arrayOfFilesWithStrToSearch,
             Flags *flags_struct);
int searchPrepare(char **arrayOfFilesWithStrToSearch, char **arrayOfStrToSearch,
                  Flags flags_struct);
void free_arrays(char **arrayOfStrToSearch, char **arrayOfFilesWithStrToSearch);
char *searchPattern(char *oneLineFromFile, char **arrayOfStrToSearch,
                    int isFlag_i, Flags flags_struct);
void outputFlag_o(regex_t preg, char *line, char *file_name, char filesCount,
                  unsigned long totalStringCount, Flags flags_struct);
void output(char *file_name, char filesCount, unsigned long totalStringCount,
            Flags flags_struct);