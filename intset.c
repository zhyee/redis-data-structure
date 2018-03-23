#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define INTSET_ENC_INT16 sizeof(int16_t)
#define INTSET_ENC_INT32 sizeof(int32_t)
#define INTSET_ENC_INT64 sizeof(int64_t)


typedef struct intset {
    uint32_t encoding;
    uint32_t length;
    char elements[];
} intset;

void var_dump(intset *is);

intset *intsetNew() {
    intset *is = malloc(sizeof(intset));
    is->encoding = INTSET_ENC_INT16;
    is->length = 0;
    return is;
}

uint32_t getEncoding(int64_t ele) {
    if (ele < INT32_MIN || ele > INT32_MAX) {
        return INTSET_ENC_INT64;
    } else if (ele < INT16_MIN || ele > INT16_MAX) {
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

    uint32_t pos_min = 0, pos_max = is->length - 1, pos_bin;

    int64_t binele;    
    
    while(pos_min <= pos_max) {
        if (pos_min == pos_max) {
            *pos = pos_min;
        }
        pos_bin = pos_min + (pos_max - pos_min) / 2;
        binele = is->elements[pos_bin];
        if (ele < binele) {
            pos_max = pos_bin - 1;
        } else if (ele > binele) {
            pos_min = pos_bin + 1;
        } else {
            *pos = pos_bin;
            return 0;
        }
    }


          printf("pos = %u\n", pos);

    return -1;
}

union endian {
    unsigned short s;
    unsigned char buf[2];
};

/**
 * 判断本地字节序是否为小端法存储
 */
int isLittleEndian() {
    union endian e;
    e.s = 0x1234;
    return e.buf[0] == 0x34 ? 1 : 0;
}

int64_t getValByPos(const intset *is, uint32_t pos) {
    int64_t ret = 0;
    const char *ptr = &is->elements[is->encoding * pos];

    if (is->encoding == sizeof(ret)) {
        memcpy(&ret, ptr, is->encoding);
        return ret;
    }

    if (isLittleEndian()) {
        if (ptr[is->encoding-1] < 0) {
            memset(&ret, 0xFF, sizeof(ret));
        }
        memcpy(&ret, ptr, is->encoding);
    } else {
        if (ptr[0] < 0) {
            memset(&ret, 0xFF, sizeof(ret));
        }
        memcpy((char *)&ret + sizeof(ret) - is->encoding, ptr, is->encoding);
    }
    return ret;
}

int setValByPos(intset *is, uint32_t pos, int64_t val, uint32_t encoding) {
    char *ptr  = &is->elements[encoding * pos];
    if (encoding == sizeof(val)) {
        memcpy(ptr, &val, encoding);
        return 0;
    }
    
    if (isLittleEndian()) {
        memcpy(ptr, &val, encoding);
    } else {
        memcpy(ptr, (char *)&val + sizeof(val) - encoding, encoding);
    }
    return 0;
}

/**
 * 返回NULL 加入失败
 */
intset *intsetAdd(intset *is, int64_t ele) {
    int ret, i;
    uint32_t pos;
    int64_t tmp;

    uint32_t nenc = getEncoding(ele);
    uint32_t elesize = is->encoding;

    if (nenc > is->encoding) {
        elesize = nenc;
    }

    ret = intsetSearch(is, ele, &pos);

    if (ret == 0) {
        return NULL;
    }

    is = realloc(is, sizeof(intset) + elesize * (is->length + 1));

    if (is->length == 0) {
           setValByPos(is, pos, ele, elesize);
    } else {
        if (elesize > is->encoding) {
            for(i = is->length - 1; i>=0; i--) {
                tmp = getValByPos(is, i);
                setValByPos(is, i, tmp, elesize);
             }
        }
    }


    is->encoding = elesize;
    if (is->length > 0) {        
        int64_t posval = getValByPos(is, pos);
        if (ele < posval) {
            if (pos + 1 < is->length) {
                memmove(&is->elements[is->encoding * (pos+1)], &is->elements[is->encoding * pos], (is->length - pos -1) * is->encoding);
            }
            setValByPos(is, pos, ele, is->encoding);
        } else {
                if (pos + 2 < is->length) {
                     memmove(&is->elements[is->encoding * (pos + 2)], &is->elements[is->encoding * (pos + 1)], is->length - pos - 2);
                }
               setValByPos(is, pos+1, ele, is->encoding);
        }
    }

    is->length ++;
    return is;
}

void var_dump(intset *is) {
    printf("is->encoding = %u, is->length = %u\n", is->encoding, is->length);
    int i;
    for(i = 0; i < is->length; i++) {
        printf("%ld\n", getValByPos(is, i));
    }
}

int main() {
    intset *is = intsetNew();
    is = intsetAdd(is, 123);
    var_dump(is);
    is = intsetAdd(is, 456);
    var_dump(is);
    is = intsetAdd(is, 789);
    var_dump(is);
    is = intsetAdd(is, -123);
    var_dump(is);

    return 0;

}

