/* Exercise the actual slab code with a page-aligned mock physical allocator. */
#include <kernel/pager.h>
#include <kernel/pfa.h>
#include <kernel/slab.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* kmalloc(size_t size);
void kfree(void* ptr);
extern kmem_cache* caches[];
slab_ht ht_init(void);
void ht_set(slab_ht* ht, uintptr_t key, kmem_bufctl* val);
kmem_bufctl* ht_get(slab_ht* ht, uintptr_t key);
bool ht_erase(slab_ht* ht, uintptr_t key);
#define CHECK(x)                                                \
    do {                                                        \
        if (!(x)) {                                             \
            printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #x); \
            return 1;                                           \
        }                                                       \
    } while (0)
static size_t live_allocations;
static long fail_after = -1;
static void* frames(size_t n) {
    void* p = NULL;
    if (!n || n > SIZE_MAX / PAGE_SIZE || fail_after == 0) return NULL;
    if (fail_after > 0) --fail_after;
    if (posix_memalign(&p, PAGE_SIZE, n * PAGE_SIZE)) return NULL;
    memset(p, 0, n * PAGE_SIZE);
    ++live_allocations;
    return p;
}
pageframe_t kalloc_frame(void) { return frames(1); }
pageframe_t kalloc_frames(size_t n) { return frames(n); }
void kfree_frame(pageframe_t p) {
    if (p) {
        --live_allocations;
        free(p);
    }
}
void kfree_frames(pageframe_t p, size_t n) {
    (void)n;
    kfree_frame(p);
}
uintptr_t phys_to_virt(uintptr_t p) { return p; }
uint64_t virt_to_phys(uintptr_t p) { return p; }

static int initial_lists(void) {
    slab_alloc_init();
    for (size_t i = 0; i < INITIAL_SLAB_CNT; ++i) {
        kmem_cache* c = caches[i];
        CHECK(c && c->head && c->tail);
        CHECK(c->head->next == c->tail && c->tail->prev == c->head);
    }
    return 0;
}
static int creation(void) {
    slab_alloc_init();
    kmem_cache* c = kmem_cache_create("test", 32, 8);
    CHECK(c != NULL);
    CHECK(c->size == 32 && c->align >= 8);
    CHECK(c->head->next == c->tail && c->tail->prev == c->head);
    return 0;
}
static int object_test(size_t size, size_t alignment, size_t count) {
    slab_alloc_init();
    kmem_cache* c = kmem_cache_create("test", size, (int)alignment);
    CHECK(c != NULL);
    void* objects[300];
    CHECK(count <= 300);
    for (size_t i = 0; i < count; ++i) {
        objects[i] = kmem_cache_alloc(c);
        CHECK(objects[i] != NULL);
        CHECK((uintptr_t)objects[i] % alignment == 0);
        for (size_t j = 0; j < i; ++j) {
            uintptr_t a = (uintptr_t)objects[i], b = (uintptr_t)objects[j];
            CHECK(a + size <= b || b + size <= a);
        }
        memset(objects[i], (int)(i % 251 + 1), size);
    }
    for (size_t i = 0; i < count; ++i) {
        unsigned char* p = objects[i];
        for (size_t j = 0; j < size; ++j) CHECK(p[j] == (unsigned char)(i % 251 + 1));
    }
    for (size_t i = count; i > 0; --i) kmem_cache_free(c, objects[i - 1]);
    void* again = kmem_cache_alloc(c);
    CHECK(again != NULL);
    memset(again, 0x5a, size);
    kmem_cache_free(c, again);
    kmem_cache_reap(c);
    CHECK(c->head->next == c->tail && c->tail->prev == c->head);
    return 0;
}
static int generic(void) {
    slab_alloc_init();
    const size_t sizes[] = {2, 17, 32, 512, 513, 1024};
    void* p[sizeof(sizes) / sizeof(sizes[0])];
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); ++i) {
        p[i] = kmalloc(sizes[i]);
        CHECK(p[i] != NULL);
        memset(p[i], (int)(i + 1), sizes[i]);
    }
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); ++i) {
        for (size_t j = 0; j < sizes[i]; ++j) CHECK(((unsigned char*)p[i])[j] == i + 1);
        kfree(p[i]);
    }
    kfree(NULL);
    return 0;
}
static int hash_growth(void) {
    slab_ht ht = ht_init();
    kmem_bufctl values[200];
    for (size_t i = 0; i < 200; ++i) ht_set(&ht, (uintptr_t)(i + 1), &values[i]);
    for (size_t i = 0; i < 200; ++i) CHECK(ht_get(&ht, (uintptr_t)(i + 1)) == &values[i]);
    for (size_t i = 0; i < 200; i += 2) CHECK(ht_erase(&ht, (uintptr_t)(i + 1)));
    for (size_t i = 0; i < 200; ++i)
        CHECK(ht_get(&ht, (uintptr_t)(i + 1)) == (i % 2 ? &values[i] : NULL));
    return 0;
}
static int reap_live(void) {
    slab_alloc_init();
    kmem_cache* c = kmem_cache_create("reap", 32, 8);
    CHECK(c != NULL);
    void* objects[200];
    for (size_t i = 0; i < 200; ++i) {
        objects[i] = kmem_cache_alloc(c);
        CHECK(objects[i] != NULL);
        memset(objects[i], (int)(i % 251 + 1), 32);
    }
    for (size_t i = 0; i < 100; ++i) kmem_cache_free(c, objects[i]);
    kmem_cache_reap(c);
    for (size_t i = 100; i < 200; ++i)
        for (size_t j = 0; j < 32; ++j)
            CHECK(((unsigned char*)objects[i])[j] == (unsigned char)(i % 251 + 1));
    for (size_t i = 100; i < 200; ++i) kmem_cache_free(c, objects[i]);
    kmem_cache_reap(c);
    CHECK(c->head->next == c->tail && c->tail->prev == c->head);
    return 0;
}
static int failure(void) {
    slab_alloc_init();
    kmem_cache* c = kmem_cache_create("failure", 32, 8);
    CHECK(c != NULL);
    size_t before = live_allocations;
    fail_after = 0;
    CHECK(kmem_cache_alloc(c) == NULL);
    fail_after = -1;
    CHECK(live_allocations == before);
    void* p = kmem_cache_alloc(c);
    CHECK(p != NULL);
    kmem_cache_free(c, p);
    return 0;
}
int main(int argc, char** argv) {
    if (argc != 2) return 2;
    if (!strcmp(argv[1], "initial_lists")) return initial_lists();
    if (!strcmp(argv[1], "creation")) return creation();
    if (!strcmp(argv[1], "small")) return object_test(32, 8, 200);
    if (!strcmp(argv[1], "tiny")) return object_test(3, 8, 200);
    if (!strcmp(argv[1], "large")) return object_test(1024, 8, 24);
    if (!strcmp(argv[1], "generic")) return generic();
    if (!strcmp(argv[1], "failure")) return failure();
    if (!strcmp(argv[1], "hash_growth")) return hash_growth();
    if (!strcmp(argv[1], "reap_live")) return reap_live();
    return 2;
}
