#include "s21_grep.h"

int main(int argc, char **argv) {
  char strToSearch[1000] = {0};
  char *arrayOfStrToSearch[1024] = {0};
  char *arrayOfFilesWithStrToSearch[1024] = {0};
  int i = 1;
  int isError = 0;
  Flags flags_struct = {'0', '0', '0', '0', '0', '0', '0', '0', '0'};
  while ((argv[i]) != NULL) {
    if (argv[i][0] == '-') {
      isError = fillFlagsStruct(&flags_struct, argv[i]);
      if (isError == 1) {
        if (flags_struct.f_s == 's') {
          printf("grep: invalid option\n");
        }
      }
    }
    i++;
  }
  if (isError == 0) {
    isError =
        getPatterns(argc, argv, strToSearch, arrayOfStrToSearch, &flags_struct);
  }
  if (isError == 0) {
    isError = getFiles(argc, argv, arrayOfFilesWithStrToSearch, &flags_struct);
    if (isError == 0) {
      searchPrepare(arrayOfFilesWithStrToSearch, arrayOfStrToSearch,
                    flags_struct);
    }
  }
  free_arrays(arrayOfStrToSearch, arrayOfFilesWithStrToSearch);
  return 0;
}

int fillFlagsStruct(Flags *flags_struct, char *argv) {
  int error = 0;
  for (int i = 1; i < (int)strlen(argv); i++) {
    switch (argv[i]) {
      case 'i':
        flags_struct->f_i = 'i';  // Игнорирует различия регистра
        break;
      case 'v':
        flags_struct->f_v = 'v';  // Инвертирует смысл поиска соответствий
        break;
      case 'c':
        flags_struct->f_c = 'c';  // Выводит только количество совпадающих строк
        break;
      case 'n':
        flags_struct->f_n = 'n';  // Предваряет каждую строку вывода номером
                                  // строки из файла ввода
        break;
      case 'h':
        flags_struct->f_h =
            'h';  // Выводит совпадающие строки, не предваряя их именами файлов
        break;
      case 's':
        flags_struct->f_s = 's';  // Подавляет сообщения об ошибках о
                                  // несуществующих или нечитаемых файлах
        break;
      case 'o':
        flags_struct->f_o = 'o';  // Печатает только совпадающие (непустые)
                                  // части совпавшей строки
        break;
      case 'e':
        flags_struct->f_ef = '1';  //-e Шаблон
        break;
      case 'f':
        flags_struct->f_ef = '1';  //-f Получает регулярные выражения из файла
        break;
      case 'l':
        flags_struct->f_l = 'l';  // Выводит только совпадающие файлы
        break;
      default:
        error = 1;
        break;
    }
  }
  return error;
}

int getPatterns(int argc, char **argv, char *strToSearch,
                char **arrayOfStrToSearch, Flags *flags_struct) {
  char *patternForFlags = "^-";
  char *patternForFlag_e = "^-.*[f]";
  char *patternForFlag_f = "^-.*[e]";
  regex_t preg;
  regex_t preg_e;
  regex_t preg_f;

  regcomp(&preg, patternForFlags, REG_EXTENDED);
  regcomp(&preg_e, patternForFlag_e, REG_EXTENDED);
  regcomp(&preg_f, patternForFlag_f, REG_EXTENDED);

  regmatch_t pm;

  int error = 0;
  int countOfPatterns = 0;
  FILE *fileWithPattern;
  for (int i = 1; i < argc; i++) {
    if ((regexec(&preg_e, argv[i], 0, &pm, 0) == 0) ||
        (regexec(&preg_f, argv[i], 0, &pm, 0) == 0)) {
      if (i == argc - 1) {
        error = -1;
        break;
      }
      if (argv[i + 1] != NULL) {
        if ((regexec(&preg, argv[i], 0, &pm, 0) != 0)) {
          error = -1;
          break;
        }
        // если текущий флаг - е,
        if (strchr(argv[i], 'e')) {
          // копируем содержимое следующего аргумента в паттерн
          strcpy(strToSearch, argv[i + 1]);
          // выделяем память и заносим указатель в массив указателей на паттерны
          arrayOfStrToSearch[countOfPatterns] =
              malloc(sizeof(char) * (strlen(argv[i + 1]) + 1));
          // проверка на выделение памяти маллоком
          if (arrayOfStrToSearch[countOfPatterns] == NULL) {
            exit(1);
          }
          // в выделенную память заносим паттерн
          strcpy(arrayOfStrToSearch[countOfPatterns], argv[i + 1]);
          countOfPatterns++;  // инкременируем счетчик паттернов

          // если текущий флаг f
        } else if (strchr(argv[i], 'f')) {
          // открываем файл по адресу из следующего аргумента
          fileWithPattern = fopen(argv[i + 1], "r");
          // проверка на открытие файла
          if (fileWithPattern == NULL) {
            error = 2;
            // если нет флага s, выводим сообщение об ошибке
            if (flags_struct->f_s != 's') {
              printf("grep: %s: %s\n", argv[i + 1], strerror(2));
            }
            break;
          }
          // в цикле забираем строки из файла
          while (fgets(strToSearch, 1024, fileWithPattern) != NULL) {
            // пропускаем пустые строки
            if (strToSearch[0] != '\n') {
              // в конце строки убираем перенос строки
              if (strToSearch[strlen(strToSearch) - 1] == '\n') {
                strToSearch[strlen(strToSearch) - 1] = '\0';
              }
              // выделяем память и заносим указатель в массив указателей на
              // паттерны
              arrayOfStrToSearch[countOfPatterns] =
                  malloc(sizeof(char) * (strlen(strToSearch) + 1));
              if (arrayOfStrToSearch[countOfPatterns] == NULL) {
                exit(1);
              }
              // в выделенную память заносим паттерн
              strcpy(arrayOfStrToSearch[countOfPatterns], strToSearch);
              countOfPatterns++;  // инкременируем счетчик паттернов
            }
          }
          fclose(fileWithPattern);
        }
        // если следующий аргумент NULL
      } else {
        if (flags_struct->f_s != 's') {
          printf("File pattern not found\n");
        }
        break;
      }

    } else if ((regexec(&preg, argv[i], 0, &pm, 0) != 0) &&
               countOfPatterns == 0 && flags_struct->f_ef != '1' &&
               i != (argc - 1)) {
      arrayOfStrToSearch[countOfPatterns] =
          malloc(sizeof(char) * (strlen(argv[i + 1]) + 1024));
      if (arrayOfStrToSearch[countOfPatterns] == NULL) {
        exit(1);
      }
      strcpy(arrayOfStrToSearch[countOfPatterns], argv[i]);
      countOfPatterns++;
    }
  }
  if (countOfPatterns == 0) {
    error = -1;
  }
  regfree(&preg);
  regfree(&preg_e);
  regfree(&preg_f);
  return error;
}

int getFiles(int argc, char **argv, char **arrayOfFilesWithStrToSearch,
             Flags *flags_struct) {
  char patternForFlags[] = "^-";
  regex_t preg;
  regcomp(&preg, patternForFlags, REG_EXTENDED);
  regmatch_t pm;
  int error = 0;
  int countOfFiles = 0;
  if (flags_struct->f_ef == '1') {
  }
  for (int i = 2; i < argc; i++) {
    if (regexec(&preg, argv[i], 0, &pm, 0) != 0 && strcmp(argv[i - 1], "-e") &&
        strcmp(argv[i - 1], "-f") &&
        !(regexec(&preg, argv[i - 1], 0, &pm, 0) == 0 && i == 2)) {
      arrayOfFilesWithStrToSearch[countOfFiles] =
          malloc(sizeof(char) * (strlen(argv[i]) + 1));
      if (arrayOfFilesWithStrToSearch[countOfFiles] == NULL) {
        exit(1);
      }
      strcpy(arrayOfFilesWithStrToSearch[countOfFiles], argv[i]);
      countOfFiles++;
    }
  }
  regfree(&preg);
  return error;
}

int searchPrepare(char **arrayOfFilesWithStrToSearch, char **arrayOfStrToSearch,
                  Flags flags_struct) {
  FILE *file;
  regex_t preg;
  int isFlag_i = 0;
  if (flags_struct.f_i == 'i') {
    isFlag_i = 2;
  } else {
    isFlag_i = 1;
  }

  char bufferForStringCount[25] = {0};
  unsigned long stringCount = 0;
  unsigned long totalStringCount = 0;

  int filesCount = 0;
  while (arrayOfFilesWithStrToSearch[filesCount] != NULL) filesCount++;
  int iFile = 0;
  int flagEOF = 0;
  char c = '0';
  unsigned long charCount = 1;
  int result_regcomp = 0;
  char *oneLineFromFile = NULL;
  while (arrayOfFilesWithStrToSearch[iFile] != NULL) {
    file = fopen(arrayOfFilesWithStrToSearch[iFile], "r");
    if (file == NULL) {
      if (flags_struct.f_s != 's') {
        printf("grep: %s: %s", arrayOfFilesWithStrToSearch[iFile], strerror(2));
      }
      iFile++;
      continue;
    }
    flagEOF = 0;
    totalStringCount = 0;
    while (1) {
      oneLineFromFile = calloc(charCount + 1024, sizeof(char));
      if (oneLineFromFile == NULL) {
        exit(1);
      }
      while ((c = getc(file)) != '\n') {
        if (c == EOF) {
          oneLineFromFile[charCount - 1] = '\0';
          charCount++;
          flagEOF = 1;
          break;
        }
        charCount++;
        oneLineFromFile =
            realloc(oneLineFromFile, (sizeof(char) * charCount + 1024));
        oneLineFromFile[charCount - 2] = c;
      }
      if (c == EOF && charCount == 2) {
        charCount = 1;
        free(oneLineFromFile);
        oneLineFromFile = NULL;
        break;
      }
      totalStringCount++;
      oneLineFromFile[charCount - 1] = '\0';
      char *pattern = NULL;
      if (strchr(oneLineFromFile, '\0') != NULL) {
        pattern = searchPattern(oneLineFromFile, arrayOfStrToSearch, isFlag_i,
                                flags_struct);
      }
      if (pattern != NULL) {
        result_regcomp = 0;
      } else {
        result_regcomp = 1;
      }
      if (result_regcomp == 0 && flags_struct.f_v != 'v') {
        stringCount++;
      } else if (result_regcomp == 1 && flags_struct.f_v == 'v') {
        stringCount++;
      }

      if (pattern != NULL) {
        regcomp(&preg, pattern, isFlag_i);
      }
      if (flags_struct.f_c != 'c' && flags_struct.f_l != 'l') {
        if (result_regcomp == 0 && flags_struct.f_v != 'v') {
          if (flags_struct.f_o == 'o') {
            outputFlag_o(preg, oneLineFromFile,
                         arrayOfFilesWithStrToSearch[iFile], filesCount,
                         totalStringCount, flags_struct);
          } else {
            output(arrayOfFilesWithStrToSearch[iFile], filesCount,
                   totalStringCount, flags_struct);
            printf("%s\n", oneLineFromFile);
          }
        } else if (result_regcomp == 1 && flags_struct.f_v == 'v') {
          output(arrayOfFilesWithStrToSearch[iFile], filesCount,
                 totalStringCount, flags_struct);
          printf("%s\n", oneLineFromFile);
        }
      }
      if (pattern != NULL) {
        regfree(&preg);
      }
      charCount = 1;
      free(oneLineFromFile);
      oneLineFromFile = NULL;
      if (flagEOF == 1) {
        break;
      }
    }
    if (flags_struct.f_l == 'l') {
      if (stringCount > 0) {
        stringCount = 1;
      }
    }
    if (flags_struct.f_c == 'c') {
      sprintf(bufferForStringCount, "%lu", stringCount);
      output(arrayOfFilesWithStrToSearch[iFile], filesCount, 0, flags_struct);
      printf("%s\n", bufferForStringCount);
    }
    if (flags_struct.f_l == 'l' && stringCount > 0) {
      printf("%s\n", arrayOfFilesWithStrToSearch[iFile]);
    }
    fclose(file);
    stringCount = 0;
    iFile++;
  }
  return 0;
}

void free_arrays(char **arrayOfStrToSearch,
                 char **arrayOfFilesWithStrToSearch) {
  int i = 0;
  while (arrayOfStrToSearch[i] != NULL) {
    free(arrayOfStrToSearch[i]);
    i++;
  }
  i = 0;
  while (arrayOfFilesWithStrToSearch[i] != NULL) {
    free(arrayOfFilesWithStrToSearch[i]);
    i++;
  }
}
char *searchPattern(char *oneLineFromFile, char **arrayOfStrToSearch,
                    int isFlag_i, Flags flags_struct) {
  int iPattern = 0;
  regex_t preg;
  int err_regcomp = 0, result_regcomp = 0;
  regmatch_t pm;
  char *finded_pattern = NULL;
  while (arrayOfStrToSearch[iPattern] != NULL) {
    err_regcomp = regcomp(&preg, arrayOfStrToSearch[iPattern], isFlag_i);
    if (err_regcomp != 0) {
      if (flags_struct.f_s != 's') {
        printf("grep: Invalid pattern\n");
        regfree(&preg);
        iPattern++;
        continue;
      }
    }

    result_regcomp = regexec(&preg, oneLineFromFile, 1, &pm, 0);
    if (result_regcomp == 0) {
      finded_pattern = arrayOfStrToSearch[iPattern];
    }
    regfree(&preg);
    iPattern++;
  }
  return finded_pattern;
}

void outputFlag_o(regex_t preg, char *line, char *file_name, char filesCount,
                  unsigned long totalStringCount, Flags flags_struct) {
  char *oneLineFromFile = line;
  regmatch_t pm;
  int regerr;
  while ((regerr = regexec(&preg, oneLineFromFile, 1, &pm, 0)) == 0) {
    output(file_name, filesCount, totalStringCount, flags_struct);
    for (char *curr = (oneLineFromFile + pm.rm_so);
         curr < oneLineFromFile + pm.rm_eo; curr++) {
      putchar(*curr);
    }
    oneLineFromFile = oneLineFromFile + pm.rm_eo;
    printf("\n");
  }
}

void output(char *file_name, char filesCount, unsigned long totalStringCount,
            Flags flags_struct) {
  if (filesCount > 1 && flags_struct.f_h != 'h') {
    if (flags_struct.f_n == 'n' && totalStringCount != 0) {
      printf("%s:%lu:", file_name, totalStringCount);
    } else {
      printf("%s:", file_name);
    }
  }
  if (filesCount == 1 || flags_struct.f_h == 'h') {
    if (flags_struct.f_n == 'n' && totalStringCount != 0) {
      printf("%lu:", totalStringCount);
    }
  }
}
