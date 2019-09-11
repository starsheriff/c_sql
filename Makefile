 NAME=csql
 OBJECTS=table.o
 CFLAGS= -g -Wall
 CC=clang


$(NAME): $(OBJECTS)

table_test: $(OBJECTS)
