#include "s21_cat.h"

int main(int argc, char** argv) {
  int b = 0, e = 0, n = 0, s = 0, t = 0, E = 0, T = 0, v = 0;  // flags
  int i = 0;
  while (i < argc) {
    if (argv[i][0] == '-' && argv[i][1] != '-') {
      b = b == 1 ? 1 : getFlagInt(argv[i], 'b');
      e = e == 1 ? 1 : getFlagInt(argv[i], 'e');
      n = n == 1 ? 1 : getFlagInt(argv[i], 'n');
      s = s == 1 ? 1 : getFlagInt(argv[i], 's');
      t = t == 1 ? 1 : getFlagInt(argv[i], 't');
      E = E == 1 ? 1 : getFlagInt(argv[i], 'E');
      T = T == 1 ? 1 : getFlagInt(argv[i], 'T');
      v = v == 1 ? 1 : getFlagInt(argv[i], 'v');
    }
    if (argv[i][0] == '-' && argv[i][1] == '-') {
      if (strcmp(&argv[i][0], "--number") == 0) n = 1;
      if (strcmp(&argv[i][0], "--number-nonblank") == 0) b = 1;
      if (strcmp(&argv[i][0], "--squeeze-blank") == 0) s = 1;
    }
    i++;
  }
  getFile(argv, b, e, n, s, t, T, E, v);
  return 0;
}

int getFlagInt(char* flags, char flag) {
  int result = 0;
  for (int i = 0; i < (int)strlen(flags); i++) {
    if (flags[i] == flag) {
      result = 1;
      break;
    }
  }
  return result;
}

void getFile(char** argv, int b, int e, int n, int s, int t, int T, int E,
             int v) {
  FILE* file;
  argv++;
  int result = 1;  // line number
  int prevSymb = '\n', firstFile = 1;
  while (*argv) {
    if (*argv[0] == '-') {
      argv++;
    } else {
      if ((file = fopen(*argv, "r")) == NULL) {
        argv++;
      } else {
        result = catOutput(b, e, n, s, t, T, E, v, file, result, &prevSymb,
                           &firstFile);
        fclose(file);
        argv++;
      }
    }
  }
}

int catOutput(int b, int e, int n, int s, int t, int T, int E, int v,
              FILE* file, int count, int* prev, int* firstFile) {
  int current;  // firstLineFlag = 1;
  int emptyLineCnt = 0;
  while ((current = getc(file)) != EOF) {
    if (s == 1) {
      if (current == '\n') emptyLineCnt++;
      if (current != '\n') {
        emptyLineCnt = 0;
        // firstLineFlag = 0;
        *firstFile = 0;
      }
    }
    if (b == 1 && emptyLineCnt < 3) {
      if ((*prev == '\n' && (current != '\n')) ||
          (count == 1 && firstFile == 0)) {
        printf("%6d\t", count);
        count++;
      }
    }
    if (n == 1 && b == 0 && emptyLineCnt < 3) {
      if ((*prev == '\n' || count == 1) &&
          !(emptyLineCnt == 2 && *firstFile == 1)) {
        printf("%6d\t", count);
        count++;
      }
    }
    if ((E == 1 || e == 1 || v == 1) && emptyLineCnt < 3) {
      if ((t == 1 || e == 1 || v == 1) && current >= 0 && current < 32 &&
          current != '\n' && current != '\t') {
        printf("^%c", 64 + current);
      } else if ((t == 1 || T == 1) && current == '\t') {
        printf("^I");
      } else if (current == 127) {
        printf("^?");
      } else if (current > 127 && current < 160) {
        printf("M-^%c", current + 192);
      } else if (current == 255) {
        printf("M-^?");
      } else if (current >= 160 && current < 255) {
        printf("M-%c", current + 128);
      } else if (current != '\n') {
        printf("%c", current);
      }
      if ((E == 1 || e == 1) && current == '\n' &&
          !(emptyLineCnt == 2 && *firstFile == 1)) {
        printf("$\n");
      } else if ((E == 0 && e == 0) && current == '\n' &&
                 !(emptyLineCnt == 2 && *firstFile == 1)) {
        printf("\n");
      }
      if (current != '\n') emptyLineCnt = 0;
    } else if ((T == 1 || t == 1) && emptyLineCnt < 3) {
      if (e == 0 && t == 1 && current >= 0 && current < 32 && current != '\n' &&
          current != '\t') {
        printf("^%c", 64 + current);
      } else if (current == 127) {
        printf("^?");
      } else if (current > 127 && current < 160) {
        printf("M-^%c", current + 192);
      } else if (current == 255) {
        printf("M-^?");
      } else if (current >= 160 && current < 255) {
        printf("M-%c", current + 128);
      } else if (current != '\t' && current != '\n') {
        printf("%c", current);
      } else if (e == 0 && E == 0 && current == '\n' &&
                 !(emptyLineCnt == 2 && *firstFile == 1)) {
        printf("\n");
      }
      if (current == '\t') {
        printf("^I");
      }
      if (current != '\n') emptyLineCnt = 0;
    } else {
      if ((((e == 1 || E == 1) && current == '\n') == 0 &&
           (emptyLineCnt < 3)) &&
          !(emptyLineCnt == 2 && *firstFile == 1)) {
        putchar(current);
      }
    }
    *prev = current;
  }
  return count;
}
