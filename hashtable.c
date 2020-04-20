
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define HASHSIZE 101

typedef struct hash_node {
  struct hash_node* next;
  const char*name;
  char* defn;
} hash_node;

static hash_node* hashtab[HASHSIZE];

unsigned hash(const char *s) {
  unsigned hashval;

  for (hashval = 0; *s != '\0'; ++s) {
    hashval = *s + 31 * hashval;
  }
  return hashval % HASHSIZE;
}

hash_node* hash_lookup(const char* s) {
  hash_node* np;

  for (np = hashtab[hash(s)]; np != NULL; np = np->next)
    if (strcmp(s, np->name) == 0)
      return np;  /* found */
  return NULL;        /* not found */
}

char *strdup(const char *s) {   /* make a duplicate of s */
  char *p;

  p = (char*) malloc(strlen(s)+1);  /* +1 for ′\0′ */
  if (p != NULL)
    strcpy(p, s);
  return p;
}

hash_node* hash_install(const char *name, const char *defn) {
  if (name == NULL) { return NULL; }

  hash_node* np;
  unsigned hashval;

  if ((np = hash_lookup(name)) == NULL) {  /* not found */
    np = (hash_node*) malloc(sizeof(*np));
    if (np == NULL || (np->name = strdup(name)) == NULL)
      return NULL;
    hashval = hash(name);
    np->next = hashtab[hashval];
    hashtab[hashval] = np;
  } else      /* already there */
    free((void*) np->defn);  /* free previous defn */
  if ((np->defn = strdup(defn)) == NULL)
    return NULL;
  return np;
}

// ============== 6.5 undef =============================================
hash_node* hash_undef(const char *name) {
  if (name == NULL) { return NULL; }

  hash_node* found;

  if ((found = hash_lookup(name)) == NULL) { return NULL; } /* not found and nothing to do */
  else {
    if (found->next != NULL) {
      found->next = found->next->next;
      found = found->next;
    } else {
      hashtab[hash(name)] = NULL;
      free((void*) found);
    }
  }
  return found;
}
// ======================================================================

void print_defn(const char* s) {
  hash_node* p = hash_lookup(s);
  printf("'%s': ", s);
  printf("%s\n", p ? p->defn : "not found");
}

void hash_print() {
  for (int i = 0; i < HASHSIZE; ++i) {
    printf("%3d...\n", i);
    hash_node* np = hashtab[i];
    while (np != NULL) {
      printf("\t");
      print_defn(np->name);
      np = np->next;
    }
  }
}

void hash_deletenode(hash_node* p) {
  if (p == NULL) { return; }
  free((void*)p->name);
  free(p->defn);
  free(p);
}

void hash_clear() {
  printf("\nclearing hash table...   ");
  for (int i = 0; i < HASHSIZE; ++i) {
    hash_node* p = hashtab[i];
    if (i == 72) {
      printf("72\n");
    }
    while (p != NULL) {
      hash_node* q = p;
      p = p->next;
      hash_deletenode(q);
    }
    hashtab[i] = NULL;
  }
  printf("cleared\n");
}

void test_hash_table() {
  printf("%s\n", "// ---------------------- TESTING HASH_TABLE ---------------------");

  hash_install("cat", "animal that likes fish");
  hash_install("dog", "animal that hates cats");
  hash_install("mouse", "animal eaten by cats");
  hash_install("virus", "pain in the neck");

  print_defn("cat");
  print_defn("pig");
  print_defn("dog");
  print_defn("pig");
  print_defn("mouse");
  print_defn("bacteria");
  print_defn("amoeba");
  print_defn("paramecium");
  print_defn("virus");

  hash_print();

  printf("\nredefining cat...\n");
  printf("redefining virus...\n");
  hash_install("cat", "animal that likes fish and mice and birds");
  hash_install("virus", "VERY EXPENSIVE pain in the neck");
  print_defn("cat");
  print_defn("virus");
  hash_print();

  printf("\nredefining cat...\n");
  printf("redefining dog...\n");
  hash_undef("cat");
  hash_install("dog", "animal that chases cats");
  print_defn("cat");
  print_defn("dog");
  hash_print();

  hash_clear();
  hash_print();
  printf("\n%s\n", "// ----------------END OF TESTING HASH_TABLE ---------------------");
}

void add_word_defn(const char* name, const char* defn, int* size) {
  if (hash_install(name, defn) != NULL) {
//    printf("adding %s\n", name);
    ++*size;
  }
}

void hash_test(int argc, const char* argv[]) {
  if (argc != 2) { fprintf(stderr, "Usage: ./hash filename\n");  exit(1); }

  FILE* fin;
  const char* filename = argv[1];
  if ((fin = fopen(filename, "r")) < 0) {
    fprintf(stderr, "Could not open file: '%s'\n", filename);
    exit(1);
  }

  char buf[BUFSIZ];
  char delims[] = " \n";
  int size = 0;
  memset(buf, 0, sizeof(buf));

  while (fgets(buf, BUFSIZ, fin)) {
    char* p = strtok(buf, delims);
    add_word_defn(p, "some defn", &size);

    while ((p = strtok(NULL, delims)) != NULL) {
      add_word_defn(p, "some defn", &size);
    }
  }
  printf("%d words added...\n", size);
  hash_print();

  hash_clear();
  hash_print();

  fclose(fin);
}

int main(int argc, const char* argv[]) {
  test_hash_table();
  hash_test(argc, argv);

  return 0;
}
