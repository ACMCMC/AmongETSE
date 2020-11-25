ARCHS_C := main.c

OUTPUTS := $(ARCHS_C:.c=.out)

HEADERS := FuncionesAmongETSE.h abb.h cola.h grafo.h funciones.h
HEADERS_DIR := .
IMPLS_HEADERS := $(HEADERS:.h=.c)
OBJS_HEADERS := $(HEADERS:.h=.o)

OBJS := $(ARCHS_C:.c=.o) $(OBJS_HEADERS)


ARGS := -Wall -Wextra -pedantic -g

all: $(OUTPUTS)

$(OUTPUTS): $(OBJS)
	gcc $(@:.out=.o) $(OBJS_HEADERS) -o $(@:.out=) -L $(HEADERS_DIR) $(ARGS)

%.o: %.c
	gcc -o $@ -c $< $(ARGS)

clean:
	-find . -type f -name '*.o' -delete
	-find . -type f -name '*.out' -delete
	-rm $(OUTPUTS:.out=)