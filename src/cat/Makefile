.PHONY: all s21_cat test check clean

CC= gcc
CFLAGS= -Wall -Wextra -Werror -std=c11
SOURCE = s21_cat.c
CAT_EXEC = s21_cat
CAT_TEST_EXEC = tests/cat_test.out

all: s21_cat
	
s21_cat:
	make clean
	$(CC) $(CFLAGS) $(SOURCE) -o $(CAT_EXEC)

test: s21_cat
	bash tests/t_cat.sh

check:
	cppcheck --enable=all --force --check-config for details *.c *.h
	cp ../../materials/linters/.clang-format .clang-format
	clang-format -n --style=Google $(SOURCE)

clean:
	rm -f $(CAT_EXEC) $(CAT_TEST_EXEC) log.txt .clang-format
	rm -rf tests/cat_txt
