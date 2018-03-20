#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define HT_MIN_SIZE 4

typedef struct hashentry {
    char *key;
    struct hashentry *next;
    char val[];
} hashentry;

typedef struct hashtable {
    long size;
    long mask;
    long used;
    hashentry **table;
} hashtable;

typedef struct dict {
    int rehashidx;
    hashtable *ht[2];
} dict;

/**
 * murmurhash2 散列函数
 */
unsigned int murmurhash2(const void *key, int len) {
        uint32_t seed = 5381;
        const uint32_t m = 0x5bd1e995;
        const int r = 24;

        uint32_t h = seed ^ len;
        const unsigned char *data = (const unsigned char *)key;
        while(len >= 4) {
                uint32_t k = *(uint32_t*)data;
                k *= m;
                k ^= k >> r;
                k *= m;

                h *= m;
                h ^= k;
                data += 4;
                len -= 4;
        }

        switch(len) {
                case 3: h ^= data[2] << 16;
                case 2: h ^= data[1] << 8;
                case 1: h ^= data[0]; h *= m;
        };
        h ^= h >> 13;
        h *= m;
        h ^= h >> 15;
        return (unsigned int)h;
}

hashtable *createHT(long size) {
    long cap = HT_MIN_SIZE;
    while(cap < size) {
        cap <<= 1;
    }
    hashtable *ht = malloc(sizeof(hashtable));
    ht->size = cap;
    ht->mask = cap - 1;
    ht->used = 0;
    ht->table = calloc(cap, sizeof(hashentry *));
    return ht;
}

dict *dictCreate(long cap){
    dict *d = malloc(sizeof(dict));
    d->rehashidx = -1;
    d->ht[0] = createHT(cap);
    return d;
}

/**
 * 创建一个新的key-value节点
 */
hashentry *createEntry(const char *key, const char *val) {
    int vlen = strlen(val);
    hashentry *entry = malloc(sizeof(hashentry) + vlen + 1);
    entry->key = (char *)key;
    entry->next = NULL;
    memcpy(entry->val, val, vlen);
    entry->val[vlen] = '\0';
    return entry;
}


unsigned int getHash(dict *d,  const char *key) {
    unsigned int idx = murmurhash2(key, strlen(key));
    if (d->rehashidx >= 0) {
        return idx & d->ht[1]->mask;
    }
    return idx & d->ht[0]->mask;
}

void addEntry(dict *d, const char *key, const char *val) {
    hashentry *he = createEntry(key, val);

    unsigned int idx = getHash(d, key);
    if (d->ht[0]->table[idx] != NULL) {
        he->next = d->ht[0]->table[idx];
    }
    d->ht[0]->table[idx] = he;
    d->ht[0]->used++;
}

hashtable *getActiveTable(dict *d) {
    if (d->rehashidx >= 0) {
        return d->ht[1];
    }
    return d->ht[0];
}

int dictIsRehashing(dict *d) {
    int ret = d->rehashidx >= 0 ? 1 : 0;
    return ret;
}

void removeEntry(dict *d, const char *key) {
    unsigned int idx = getHash(d, key);
    hashtable *ht;
    int i;
    for(i = 0; i < 2; i++){
                
    }
}

const char *getVal(dict *d, const char *key) {
    unsigned int idx = getHash(d, key);
    hashentry *he = d->ht[0]->table[idx];
    while (he != NULL) {
        if (strcmp(he->key, key) == 0) {
            return he->val;
        } else {
            he = he->next;
        }
    }
    return NULL;
}

void ht_var_dump(const hashtable *ht) {
        printf("ht->size = %d, ht->mask = %d, ht->used = %d\n", ht->size, ht->mask, ht->used);
        int i;
        hashentry *he;

        for(i = 0; i < ht->size; i++) {

                he = ht->table[i];

                printf("------------idx: %d-------------\n", i);
                while(he != NULL) {
                        printf("key: %s, val: %s\n", he->key, he->val);
                        he = he->next;
                }
                printf("########################## idx: %d end #########################\n", i);
        }

}

void var_dump(dict *d) {

        printf("\n");
    ht_var_dump(d->ht[0]);

    if (d->rehashidx >= 0) {
        ht_var_dump(d->ht[1]);
    }
    printf("\n");
}


int main() {
    dict *d = dictCreate(8);
    var_dump(d);
    addEntry(d, "foo", "hello world");
    addEntry(d, "name", "zyee");
    addEntry(d, "age", "27");
    addEntry(d, "gender", "male");
    addEntry(d, "height", "1.65");
    addEntry(d, "weight", "55kg");
    addEntry(d, "ID", "342622199004197093");

    printf("%s\n", getVal(d, "name"));
    var_dump(d);
    return 0;
}
