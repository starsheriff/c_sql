 NAME=csql
 OBJECTS=table.o tokenizer.o
 CFLAGS= -g -Wall
 CC=clang


$(NAME): $(OBJECTS)

table_test: $(OBJECTS)

test: table_test
	./table_test
