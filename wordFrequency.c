#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#define MAXWORD 1000 /* longest word that can be read by getword */

typedef struct tnode tnode;
struct tnode {
  const char* word;
  int count;
  tnode* left;
  tnode* right;
};

typedef struct wnode wnode;
struct wnode {
    int number;
     words *words;
     wnode *left;
     wnode *right;
};

typedef struct words words;
struct words {
    char *word;
     words *next;
};

tnode *addtree( tnode *, char *);
wnode *addwtree( wnode *, int, char*);
words *addword( words*, char *);
void printwords(const  words*, const int);

wnode *traverse(const  tnode *,  wnode *);
void treeprint(const  wnode *);
int getword(char *, int);

tnode *talloc(void);
wnode *numwordalloc(void);
words *wordsalloc(void);
char *_strdup(char *);

int getch(void);
void ungetch(int c);

#define IN 1
#define OUT 0

int getword(char *word, int lim) {
    int c;
    char *w = word;
    static int lineBeginning = 1;
    static int afterSlash = 0;
    int afterStar = 0;

    if(isspace(c = getch()))
        afterSlash = 0;
    while(isspace(c)) {
        if(c == '\n')
            lineBeginning = 1;
        c = getch();
    }

    if(c != EOF)
        *w++ = c;

    if(c == '#' && lineBeginning == 1) {
        while((c = getch()) != '\n' && c != EOF)
            ;
        return getword(word, lim);
    }
    lineBeginning = 0;

    if(c == '\\')
        afterSlash = afterSlash ? 0 : 1;

    else if(c == '/' ) {
        if((c = getch()) == '*' && !afterSlash) {
            while((c = getch()) != EOF) {
                if(c == '/') {
                    if(afterStar)
                        return getword(word, lim);
                }
                else if(c == '*' && !afterSlash)
                    afterStar = 1;
                else if(c == '\\')
                    afterSlash = afterSlash ? 0 : 1;
                else {
                    afterStar = 0;
                    afterSlash = 0;
                }
            }
        }

        afterSlash = 0;
        if(c != EOF)
            ungetch(c);
    }

    else if(c == '\"') {
        if(!afterSlash) {
            --w;
            while((c = getch()) != EOF) {
                if(c == '\"' && !afterSlash)
                    break;
                else if(c == '\\')
                    afterSlash = afterSlash ? 0 : 1;
                else
                    afterSlash = 0;
                *w++ = c;
            }
            *w = '\0';
            if(c == EOF)
                return EOF;
            else
                return getword(word, lim);
        }
        afterSlash = 0;
    }

    if(!isalpha(c) && c != '_') {
        *w = '\0';
        if(c != '\\')
            afterSlash = 0;
        return c;
    }

    afterSlash = 0;

    for( ; --lim > 0; ++w)
        if(!isalnum(*w = getch()) && *w != '_') {
            ungetch(*w);
            break;
        }
    *w = '\0';
    return word[0];
}

 tnode *addtree( tnode *p, char *w) {
    int cond;

    if(p == NULL) {
        p = talloc();
        p->word = _strdup(w);
        p->count = 1;
        p->left = p->right = NULL;
    }
    else if((cond = strcmp(w, p->word)) == 0)
        p->count++;
    else if(cond < 0)
        p->left = addtree(p->left, w);
    else
        p->right = addtree(p->right, w);
    return p;
}

 wnode *addwtree( wnode *p, int count, char* w) {
    if (p == NULL) {
        p = numwordalloc();
        p->number = count;
        p->words = NULL;
        p->words = addword(p->words, w);
    } else if (count > p->number)
        p->left = addwtree(p->left, count, w);
    else
        p->right = addwtree(p->right, count, w);
    return p;
}

 words *addword( words* list, char *w) {
    if (list == NULL) {
        list = wordsalloc();
        list->word = _strdup(w);
        list->next = NULL;
    } else {
        list->next = addword(list->next, w);
    }
    return list;
}

void treeprint(const  wnode *p) {
    if(p != NULL) {
        treeprint(p->left);
        printwords(p->words, p->number);
        treeprint(p->right);
    }
}

void printwords(const  words* w, const int count) {
    if (w != NULL) {
        printf("%4d\t%s\n", count, w->word);
        w = w->next;
    }
}

 wnode *traverse(const  tnode *p,  wnode *q) {
    if (p != NULL) {
        q = traverse(p->left, q);
        q = addwtree(q, p->count, p->word);
        q = traverse(p->right, q);
    }
    return q;
}

 tnode *talloc(void) {
    return ( tnode *) malloc(sizeof( tnode));
}

 wnode *numwordalloc(void) {
    return ( wnode *) malloc(sizeof( wnode));
}

 words *wordsalloc(void) {
    return ( words *) malloc(sizeof( words));
}

char *_strdup(char *s) {
    char *p;
    p = (char *) malloc(strlen(s) + 1);
    if(p != NULL)
        strcpy(p, s);
    return p;
}

#define BUFSIZE 100

char buf[BUFSIZE];
int bufp = 0;

int getch(void) {
    return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c) {
    if(bufp >= BUFSIZE)
        printf("ungetch: too many characters\n");
    else
        buf[bufp++] = c;
    return;
}

int main() {
     tnode *root;
     wnode *wroot;

    char word[MAXWORD];

    root = NULL;
    wroot = NULL;

    while(getword(word, MAXWORD) != 'x')
        if(isalpha(word[0]))
            root = addtree(root, word);

    wroot = traverse(root, wroot);
    treeprint(wroot);
    return 0;
}
