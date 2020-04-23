#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "tree.h"
#include "utils.h"

static bool FREQ_SORT = false;
static bool FIRST_N = false;
static int N_CHARS = 2;


typedef struct inode inode;
struct inode {
  int line_num;
  inode* next;
};

inode* inode_create(int line_num) {
  inode* p = (inode*)malloc(sizeof(inode));
  p->line_num = line_num;
  p->next = NULL;

  return p;
}

void inode_append(inode* head, int line_num) {
  inode* p = head;
  while(p->next != NULL) {
    p = p->next;
  }
  if(p->line_num != line_num) {
    inode* q = inode_create(line_num);
    p->next = q;
  }
}

void inode_delete(inode* p) {
  while(p != NULL) {
    inode* q = p;
    p = p->next;
    free(q);
  }
}
//-------------------------------------------------
tnode* tnode_create(const char* word, int line_num) {
  tnode* p = (tnode*)malloc(sizeof(tnode));
  p->word = strdup(word);    // copy of word allocated on heap
  p->count = 1;
  p->line_node = inode_create(line_num);
  p->left = NULL;
  p->right = NULL;

  return p;
}

//====================================================================
void tnode_delete(tnode* p) {
  if(p == NULL) { return; }

  free((void*)p->word);
  inode_delete(p->line_node);
  free(p);
}

//====================================================================
tree* tree_create(void) {
  tree* p = (tree*)malloc(sizeof(tree));
  p->root = NULL;
  p->size = 0;

  return p;
}

//====================================================================
static void tree_deletenodes(tree* t, tnode* p) {
  if (p == NULL) { return; }

  tree_deletenodes(t, p->left);
  tree_deletenodes(t, p->right);
  tnode_delete(p);
  t->size--;
}

//====================================================================
void tree_clear(tree* t) {
  tree_delete(t);
  t->root = NULL;
  t->size = 0;
}

//====================================================================
void tree_delete(tree* t) { tree_deletenodes(t, t->root); }

//====================================================================
bool tree_empty(tree* t)  { return t->size == 0; }

//====================================================================
size_t tree_size(tree* t) { return t->size; }

//====================================================================
static tnode* tree_addnode(tree* t, tnode** p, const char* word, int line_num) {
  int compare;

  if (*p == NULL) {
    *p = tnode_create(word, line_num);
  } else if ((compare = strcmp(word, (*p)->word)) == 0) {
    (*p)->count++;
    inode_append((*p)->line_node, line_num);
  } else if (compare < 0) { tree_addnode(t, &(*p)->left, word, line_num);
  } else {
    tree_addnode(t, &(*p)->right, word, line_num);
  }

  return *p;
}

//====================================================================
static char* str_process(char* s, char* t) {
  char* p = s;
  char ignore[] = "\'\".,;;?!()/’";
  while (*t != '\0') {
    if (strchr(ignore, *t) == NULL || (*t == '\'' && (p != s || p != s + strlen(s) - 1))) {
      *p++ = tolower(*t);
    }
    ++t;
  }
  *p++ = '\0';
  return s;
}

//====================================================================
tnode* tree_add(tree* t, char* word, int line_num) {
  char buf[100];

  if (word == NULL) { return NULL; }
  str_process(buf, word);

  tnode* p = tree_addnode(t, &(t->root), buf, line_num);
  t->size++;

  return p;
}

//====================================================================
static void tree_printnode(tree* t, tnode* p) {
    if(p->count > 1) { printf("%5d -- ", p->count); }
    else {
        printf("        ");
    }
    printf("%-18s", p->word);

    if(p->line_node != NULL) {
        printf("    [");
        inode* plines = p->line_node;
        while(plines != NULL) {
            printf("%d", plines->line_num);
            if(plines->next != NULL) {
                printf(", ");
            }
            plines = plines->next;
        }
        printf("]");
    }
    printf("\n");
}

//====================================================================
static void tree_printnode_n(tree* t, tnode* p) {
    static char prev[100];
    static bool firsttime = true;
    if(firsttime) {
        memset(prev, 0, sizeof(prev));
        strcpy(prev, p->word);
    }

    int compare = strncmp(prev, p->word, N_CHARS);
    if(compare != 0) {
        printf("\n");
    }
    strcpy(prev, p->word);
    printf("%s ", p->word);

    firsttime = false;
}

//====================================================================
static void tree_printnodes(tree* t, tnode* p) {
  if (p == NULL) { return; }

  tree_printnodes(t, p->left);
  FIRST_N ? tree_printnode_n(t, p) : tree_printnode(t, p);
  tree_printnodes(t, p->right);
}

//====================================================================
static void tree_printnodes_preorder(tree* t, tnode* p) {
  if (p == NULL) { return; }

  tree_printnode(t, p);
  tree_printnodes_preorder(t, p->left);
  tree_printnodes_preorder(t, p->right);
}

//====================================================================
static void tree_printnodes_postorder(tree* t, tnode* p) {
  if (p == NULL) { return; }

  tree_printnodes_postorder(t, p->left);
  tree_printnodes_postorder(t, p->right);
  tree_printnode(t, p);
}

//====================================================================
static void tree_printnodes_reverseorder(tree* t, tnode* p) {
  if (p == NULL) { return; }

  tree_printnodes_reverseorder(t, p->right);
  tree_printnode(t, p);
  tree_printnodes_reverseorder(t, p->left);
}

//====================================================================
void tree_print(tree* t)              { tree_printnodes(t, t->root);               printf("\n"); }

//====================================================================
void tree_print_preorder(tree* t)     { tree_printnodes_preorder(t, t->root);      printf("\n"); }

//====================================================================
void tree_print_postorder(tree* t)    { tree_printnodes_postorder(t, t->root);     printf("\n"); }

//====================================================================
void tree_print_reverseorder(tree* t) { tree_printnodes_reverseorder(t, t->root);  printf("\n"); }

//====================================================================
void tree_test(tree* t) {
  printf("=============== TREE TEST =================================\n");
  printf("\n\nprinting in order...========================================\n");
  tree_print(t);
  printf("end of printing in order...=====================================\n\n");

  printf("\n\nprinting in reverse order...================================\n");
  tree_print_reverseorder(t);
  printf("end of printing in reverse order...=============================\n\n");
  printf("tree size is: %zu\n", tree_size(t));

  printf("clearing tree...\n");
  tree_clear(t);
  printf("after clearing tree, size is: %zu\n", tree_size(t));
  tree_print(t);

  printf("=============== END OF TREE TEST ==========================\n");
}

//====================================================================
tree* tree_from_stream(char* (*fget)(char*, int, FILE*), FILE* fin) {
    char buf[BUFSIZ];
    char delins[] = "\n";
    int size = 0;
    memset(buf, 0, sizeof(buf));

    int line_num = 1;
    tree* t = tree_create();
    while(fget(buf, BUFSIZ, fin)) {
        char* word = strtok(buf, delins);
        tree_add(t, word, line_num);
        ++size;

        while((word = strtok(NULL, delins)) != NULL) {
            tree_add(t, word, line_num);
            ++size;
        }
        ++line_num;
    }
    printf("\n%d words added... \n\n", size);
    if(fin != stdin) { fclose(fin); }

    return t;
}
//====================================================================

//====================================================================
tree* tree_from_file(int argc, const char* argv[]) {
  if (argc != 2 && argc != 3) { return NULL; }

  FILE* fin;
  const char* filename = argv[1];
  if ((fin = fopen(filename, "r")) == NULL) {
    fprintf(stderr, "Could not open file: '%s'\n", filename);
    exit(1);
  }

  int n;
  if(argc == 3){
      n = atoi(argv[2]);
      if(n > 0) {
          FIRST_N = true;
          N_CHARS = n;
      }
  }
  tree* t = tree_from_stream(fgets, fin);

  return t;
}


//====================================================================
int main(int argc, const char* argv[]) {
  tree* t = tree_from_file(argc, argv);
  if(t == NULL) {
      t = tree_from_stream(fgets, stdin);
  }
  tree_print(t);

  tree_delete(t);

  return 0;
}
