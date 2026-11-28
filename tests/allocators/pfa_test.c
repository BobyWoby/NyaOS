/* Compile the real allocator in this translation unit so tests can supply a
 *
 * controlled bitmap without booting Limine. Unused boot paths are discarded. */
#include "../../kernel/arch/x86_64/pfa.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x)                                                \
    do {                                                        \
        if (!(x)) {                                             \
            printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #x); \
            return 1;                                           \
        }                                                       \
    } while (0)
static uint32_t test_bitmap[9]; /* extra word detects writes beyond npages */
static void reset_bitmap(void) {
    memset(test_bitmap, 0, sizeof(test_bitmap));
    bitmap = test_bitmap;
    npages = 256;
    bm_set(0);
    test_bitmap[8] = 0xa5a5a5a5;
}
static int ranges(void) {
    const size_t counts[] = {1, 2, 31, 32, 33};
    for (size_t n = 0; n < sizeof(counts) / sizeof(counts[0]); ++n) {
        reset_bitmap();
        size_t count = counts[n];
        uintptr_t a = (uintptr_t)kalloc_frames(count);
        CHECK(a && a % PAGE_SIZE == 0);
        size_t start = a / PAGE_SIZE;
        CHECK(start + count < npages);
        for (size_t i = 0; i < count; ++i) CHECK(bm_test(start + i));
        CHECK(!bm_test(start + count));
        kfree_frames((void*)a, count);
        for (size_t i = 0; i < count; ++i) CHECK(!bm_test(start + i));
        CHECK(bm_test(0));
        CHECK(test_bitmap[8] == 0xa5a5a5a5);
    }
    return 0;
}
static int boundaries(void) {
    const size_t starts[] = {1, 30, 31, 32, 33, 63};
    const size_t counts[] = {1, 2, 31, 32, 33};
    for (size_t i = 0; i < sizeof(starts) / sizeof(starts[0]); ++i) {
        for (size_t j = 0; j < sizeof(counts) / sizeof(counts[0]); ++j) {
            reset_bitmap();
            memset(bitmap, 0xff, 8 * sizeof(*bitmap));
            size_t start = starts[i], count = counts[j];
            for (size_t k = 0; k < count; ++k) bm_clear(start + k);
            uintptr_t result = (uintptr_t)kalloc_frames(count);
            CHECK(result == start * PAGE_SIZE);
            for (size_t k = 0; k < count; ++k) CHECK(bm_test(start + k));
            kfree_frames((void*)result, count);
            for (size_t k = 0; k < count; ++k) CHECK(!bm_test(start + k));
            CHECK(bm_test(start - 1) && bm_test(start + count));
            CHECK(test_bitmap[8] == 0xa5a5a5a5);
        }
    }
    return 0;
}
static int exhaustion(void) {
    reset_bitmap();
    memset(bitmap, 0xff, 8 * sizeof(*bitmap));
    CHECK(kalloc_frames(1) == NULL);
    CHECK(kalloc_frames(0) == NULL);
    CHECK(test_bitmap[8] == 0xa5a5a5a5);
    return 0;
}
static int singles(void) {
    reset_bitmap();
    uintptr_t a = (uintptr_t)kalloc_frame(), b = (uintptr_t)kalloc_frame();
    CHECK(a && b && a != b && !(a % PAGE_SIZE) && !(b % PAGE_SIZE));
    kfree_frame((void*)a);
    kfree_frame((void*)b);
    CHECK(!bm_test(a / PAGE_SIZE) && !bm_test(b / PAGE_SIZE));
    return 0;
}
int main(int argc, char** argv) {
    if (argc != 2) return 2;
    if (!strcmp(argv[1], "single")) return singles();
    if (!strcmp(argv[1], "ranges")) return ranges();
    if (!strcmp(argv[1], "boundaries")) return boundaries();
    if (!strcmp(argv[1], "exhaustion")) return exhaustion();
    return 2;
}
