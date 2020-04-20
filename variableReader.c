#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXWORD 100
#define BUFSIZE 100

char buf[BUFSIZE];
int bufp = 0;

typedef struct tnode tnode;
struct tnode {
  const char* word;
  int count;
  tnode* left;
  tnode* right;
};

tnode* addtree(struct tnode *, char *);
void treeprint(struct tnode *, int);
int getword(char *, int);

tnode *talloc(void);
char *_strdup(char *);

tnode *addtree(tnode *p, char *w) {
    int cond;

    if (p == NULL) {
        p = talloc();
        p->word = _strdup(w);
        p->count = 1;
        p-> left = p->right = NULL;
    } else if ((cond = strcmp(w, p->word)) == 0)
        p->count++;
    else if (cond < 0)
        p->left = addtree(p->left, w);
    else
        p->right = addtree(p->right, w);
    return p;
}

void treeprint(tnode *p, int n) {
    static int printPrevious = 1;
    static  tnode *previous;

    if (p != NULL) {
        treeprint(p->left, n);

        if (n == 0)
            printf("%4d %s\n", p->count, p->word);
        else {
            if (previous != NULL) {
                if (strncmp(previous->word, p->word, n) == 0) {
                    if (printPrevious == 2) {
                        printf("\n%4d %s\n", previous->count, previous->word);
                        printPrevious = 10;
                    } else if (printPrevious == 1) {
                        printf("%4d %s\n", previous->count, previous->word);
                        printPrevious = 10;
                    }
                    printf("%4d %s\n", p->count, p->word);
                } else
                    printPrevious = 2;
            }
            previous = p;
        }

        treeprint(p->right, n);
    }
}

tnode *talloc(void) {
    return ( tnode *) malloc(sizeof( tnode));
}

char *_strdup(char *s) {
    char *p;

    p = (char *) malloc(strlen(s) + 1);
    if (p != NULL)
        strcpy(p, s);
    return p;
}

int getch(void);
void ungetch(int);

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

int getch(void) {
    return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c) {
    if (bufp >= BUFSIZE)
        printf("ungetch: too many characters\n");
    else
        buf[bufp++] = c;
}

int main(int argc, char *argv[]) {
     tnode *root;
    char word[MAXWORD];
    int numCharactersMatch;

    if(argc == 1)
        numCharactersMatch = 6;
    else if(argc == 2)
        numCharactersMatch = atoi(argv[1]);
    else {
        printf("Incorrect number of arguments.\n");
        return 1;
    }

    root = NULL;
    while (getword(word, MAXWORD) != EOF)
        if (isalpha(word[0]))
            root = addtree(root, word);
    treeprint(root, numCharactersMatch);
    return 0;
}
