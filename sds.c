#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define sdslen(sds) ((sds)->len)
#define sdsavail(sds) ((sds)->free)

typedef struct sds {
    int len;
    int free;
    char buf[0];
} sds;

/*
 * 使用指定c字符串创建一个新的sds
 * */
sds *sdsnew(const char *init) {
    size_t len = strlen(init);
    sds *s = calloc(1, sizeof(sds) + len + 1);
    memcpy(s->buf, init, len);
    s->buf[len] = '\0';
    s->len = len;
    s->free = 0;
    return s;
}

/**
 * 创建一个空的sds
 */
sds *sdsempty(void) {
    sds *s = malloc(sizeof(sds) + 1);
    s->len = 0;
    s->free = 0;
    s->buf[0] = '\0';
    return s;
}

void sdsfree(sds *s) {
    if (s != NULL) {
        free(s);
    }
}

sds *sdsdup(const sds *s) {
    size_t space = sizeof(sds) + s->len + s->free + 1;
    sds *s2 = malloc(space);
    memcpy(s2, s, space);
    return s2;
}

void sdsclear(sds *s) {
    s->buf[0] = '\0';
    s->free += s->len;
    s->len = 0;
}


/**
 * 向sds拼接一个C字符串
 */
sds *sdscat(sds *s, const char *str) {
    if (str == NULL) {
        return s;
    }
    size_t len = strlen(str), newspace, free;
    if (len > s->free) {
        newspace = sizeof(sds) + (s->len + len) * 2 + 1;
        free = s->len + len;
        s = realloc(s, newspace);
        s->free = free;
        memcpy(s->buf+s->len, str, len);
        s->len = free;
        s->buf[free] = '\0';
    } else {
        s->free = s->free - len;
        s->buf[s->len + len] = '\0';
        memcpy(s->buf+s->len, str, len);
        s->len += len;
    }


    return s;
}

var_dump(const sds *s) {
    printf("s->len: %d, s->free: %d, s->buf: %s\n", s->len, s->free, s->buf);
}

int main(){
    sds *s = sdsnew("hello world");
    var_dump(s);
  //  sdsfree(s);
  //  var_dump(s);
//    sdsclear(s);
//    var_dump(s);
    sds *s2 = sdsdup(s);
    var_dump(s2);
    s2 = sdscat(s2, " !!!!");
    var_dump(s2);
    return 0;
}
