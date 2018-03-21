#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>



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
    int lsize;
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
    node->val = val == NULL ? NULL : (char *)val;
    node->score = score;
    node->lsize = level;
    return node;
}

/**
 * 创建并初始化一个skiplist
 */
skiplist *skiplistCreate() {
    int i;
    skiplist *sl;
    sl = malloc(sizeof(skiplist));
    skiplistNode *header = nodeCreate(ZSKIPLIST_MAXLEVEL, 0, NULL);
    for(i = 0; i < ZSKIPLIST_MAXLEVEL; i++) {
        header->level[i].forward = NULL;
        header->level[i].span = 0;
    }
    header->backward = NULL;
    sl->head = header;
    sl->tail = NULL;
    sl->size = 0;
    sl->maxLevel = 1;

    return sl;
}


void addNode(skiplist *sl, int64_t score, const char *val) {
    int level = randomLevel();
    skiplistNode *node = nodeCreate(level, score, val), *cNode, *nNode;
    skiplistNode *header = sl->head;
    int i;
    cNode = header;
    for(i = level - 1; i>=0; i--) {
        nNode = cNode->level[i].forward;
        while(nNode != NULL) {
            if (nNode->score > score) {
                break;
            } else if (nNode->score == score) {
                if (strcmp(nNode->val, val) > 0) {
                    break;
                }
            }
            cNode = nNode;
            nNode = nNode->level[i].forward;
        }
        cNode->level[i].forward = node;
        node->level[i].forward = nNode;
    }

    node->backward = cNode;

    if (nNode == NULL) {
        sl->tail = node;
    } else {
        nNode->backward = node;
    }

    sl->size ++;

    if (sl->maxLevel < level) {
        sl->maxLevel = level;
    }
}

void rmNode(skiplist *sl, const char *val) {
    skiplistNode *cNode, *nNode;
    cNode = sl->head->level[0].forward;
    while(cNode != NULL) {
        if (strcmp(cNode->val, val) == 0) {
            break;
        }
        cNode = cNode->level[0].forward;
    }
    if (cNode != NULL && strcmp(cNode->val, val) != 0) {
        return;
    }

        printf("score: %d\n", cNode->backward->score);

    int i;
    for(i = cNode->lsize - 1; i >= 0; i--) {
        cNode->backward->level[i].forward = cNode->level[i].forward;
        printf("score: %d\n", cNode->backward->level[i].forward->score);
        printf("score: %d\n", sl->head->score);
    }
    if (cNode->level[0].forward != NULL) {
         cNode->level[0].forward->backward = cNode->backward;
    }
    sl->size --;
    if (sl->maxLevel == cNode->lsize) {
        for (i = sl->maxLevel - 1; i >= 0; i--) {
            if (sl->head->level[i].forward != NULL) {
                break;
            }
        }
        sl->maxLevel = i + 1;
    }
}

void var_dump(skiplist *sl) {
    int i;
    skiplistNode *head = sl->head, *tail = sl->tail, *cNode, *nNode;

    printf("sl->size: %d, sl->maxLevel: %d\n", sl->size, sl->maxLevel);
    for (i = sl->maxLevel - 1; i >= 0; i--) {
        printf("第 %d 层: \n", i + 1);
        cNode = head->level[i].forward;
        while(cNode != NULL) {
            printf("[score: %ld, val: %s]\n", cNode->score, cNode->val);
            cNode = cNode->level[i].forward;
        }
        printf("\n");
    }
}


int main(){
        skiplist *sl = skiplistCreate();
        addNode(sl, 99, "hello-99");
        addNode(sl, 12, "hello-12");
        addNode(sl, 1888, "hello-1888");
        addNode(sl, -1, "hello--1");
        addNode(sl, 99, "hello-9999");
        addNode(sl, 99, "hello-9");
        addNode(sl, -188, "hello--188");
        addNode(sl, 0, "hello-0");
        addNode(sl, 998, "hello-998");
        addNode(sl, 999, "hello-999");
        var_dump(sl);

//        rmNode(sl, "hello-99");
        rmNode(sl, "hello-0");

        var_dump(sl);

//        rmNode(sl, "hello-12");
//        var_dump(sl);

    return 0;
}
