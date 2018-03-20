#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>



#define ZSKIPLIST_MAXLEVEL 32

typedef struct skiplistLevel skiplistLevel;
typedef struct skiplistNode skiplistNode;
typedef struct skiplist skiplist;

struct skiplistLevel {
    skiplistNode *forward;
    unsigned int span;
};

struct skiplistNode {
    char *val;
    int64_t score;
    skiplistNode *backward;
    skiplistLevel level[];
};

struct skiplist {
    skiplistNode *head, *tail;
    unsigned long size;
    int maxLevel;
};

int randomLevel(void) {
    int level = 1;
    while ((random() & 0xFFFF) < (0xFFFF / 4)) {
        level++;
    }
    return level < ZSKIPLIST_MAXLEVEL ? level : ZSKIPLIST_MAXLEVEL;
}

skiplistNode *nodeCreate(int level, int64_t score, const char *val) {
    skiplistNode *node = malloc(sizeof(skiplistNode) + level * sizeof(skiplistLevel));
    node->val = val;
    node->score = score;
    node->backward = NULL;
    return node;
}

skiplist *skiplistCreate() {
    int i;
    skiplist *sl;
    sl = malloc(sizeof(skiplist));
    skiplistNode *head = nodeCreate(ZSKIPLIST_MAXLEVEL, 0, NULL);
    for(i = 0; i < ZSKIPLIST_MAXLEVEL; i++) {
        head->level[i].forward = NULL;
        head->level[i].span = 0;
    }
    sl->head = head;
    sl->tail = NULL;
    sl->size = 0;
    sl->maxLevel = 0;

    return sl;
}


int main(){
        printf("%d\n", randomLevel());
        printf("%d\n", randomLevel());
    return 0;
}
