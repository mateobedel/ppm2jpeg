CC = gcc
LD = gcc

# -O0 désactive les optimisations à la compilation
# C'est utile pour débugger, par contre en "production"
# on active au moins les optimisations de niveau 2 (-O2).
CFLAGS = -Wall -Wextra -std=c99 -Iinclude -O3 -g
LDFLAGS = 

# Par défaut, on compile tous les fichiers source (.c) qui se trouvent dans le répertoire src/
SRC_FILES=$(wildcard src/*.c)

# Par défaut, la compilation de src/toto.c génère le fichier objet obj/toto.o
OBJ_FILES=$(patsubst src/%.c,obj/%.o,$(SRC_FILES))

# Spécifiez ici le fichier source contenant la fonction main du programme principal
MAIN_OBJ=obj/main.o

# Fichiers objets du programme principal sans le fichier contenant main
NON_MAIN_OBJ_FILES=$(filter-out $(MAIN_OBJ), $(OBJ_FILES))

# Tous les fichiers de test se terminant par .test.c dans le répertoire test/
TEST_SRC_FILES=$(wildcard test/*.test.c)

# Les fichiers objets correspondants
TEST_OBJ_FILES=$(patsubst test/%.test.c,obj/test_%.o,$(TEST_SRC_FILES))

# Les exécutables de test correspondants
TEST_EXES=$(patsubst test/%.test.c,bin/%.test,$(TEST_SRC_FILES))

all: ppm2jpeg

ppm2jpeg: $(OBJ_FILES)
	$(LD) $(OBJ_FILES) $(LDFLAGS) -o $@ -lm

obj/%.o: src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

obj/test_%.o: test/%.test.c
	$(CC) -c $(CFLAGS) $< -o $@

bin/%.test: obj/test_%.o $(NON_MAIN_OBJ_FILES)
	$(LD) $< $(NON_MAIN_OBJ_FILES) $(LDFLAGS) -o $@ -lm

.PHONY: clean test

test: $(TEST_EXES)
	@for test in $(TEST_EXES); do \
		echo "Running $$test"; \
		./$$test; \
	done

clean:
	rm -rf ppm2jpeg $(OBJ_FILES) $(TEST_OBJ_FILES) $(TEST_EXES)
