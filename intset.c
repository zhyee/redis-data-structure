#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define INTSET_ENC_INT16 sizeof(int16_t)
#define INTSET_ENC_INT32 sizeof(int32_t)
#define INTSET_ENC_INT64 sizeof(int64_t)

typedef struct intset {
    uint32_t encoding;
    uint32_t length;
    int16_t elements[];
} intset;

intset *intsetNew() {
    intset *is = malloc(sizeof(intset));
    is->encoding = INTSET_ENC_INT16;
    is->length = 0;
    return is;
}

uint32_t getEncoding(int64_t ele) {
    if (ele < INT32_MIN || ele > INT32_MAX) {
        return INTSET_ENC_INT64;
    } else if (ele < INT16_MIN || ele > INT32_MAX) {
        return INTSET_ENC_INT32;
    }

    return INTSET_ENC_INT16;
}

/**
 * 寻找指定元素的位置,成功返回0，pos存储找到的节点位置,失败返回-1,pos存储该元素应该插入的位置
 */
int intsetSearch(intset *is, int64_t ele, uint32_t *pos) {
    int i;
    uint32_t encoding = getEncoding(ele);
    if (encoding > is->encoding || is->length == 0) {
        *pos = is->length;
        return -1;
    }

    uint32_t pos_min = 0, pos_max = is->length - 1;

    int64_t binele;    
    
    while(pos_min <= pos_max) {
        if (pos_min == pos_max) {
            *pos = pos_min;
        }
        pos_bin = pos_min + (pos_max - pos_min) / 2;
        ele = is->elements[pos_bin];
        if (ele < binele) {
            pos_max = pos_bin - 1;
        } else if (ele > binele) {
            pos_min = pos_bin + 1;
        } else {
            *pos = pos_bin;
            return 0;
        }
    }

    return -1;
}

/**
 * 返回NULL 加入失败
 */
intset *intsetAdd(intset *is, int64_t ele) {
    int ret;
    uint32_t pos;

    if (getEncoding(ele) > is->encoding) {
        
    }

    ret = intsetSearch(is, ele, &pos);
    if (ret == 0) {
        return NULL;
    }

    is = realloc(is, )

    return 0;
}

int main() {
    
    printf("%d\n", 0x7FFFFFFF);
    printf("%d\n", 0x80000000);
    printf("%d\n", 0xFFFFFFFF);
    printf("%d\n", INT32_MIN);
    printf("%d\n", INT32_MAX);
    return 0;

}

