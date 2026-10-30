#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;

    uint32_t high = (uint32_t)(challenge >> 32);
    uint32_t mid  = (uint32_t)(challenge >> 16);
    uint32_t low  = (uint32_t)(challenge & 0xFFFFFFFF);

    uint32_t temp1 = (uint32_t)(high * 3 + mid);
    temp1 = (uint32_t)(temp1 ^ 0xfae1);

    uint32_t temp0 = (uint32_t)(high + 0x398b);
    temp0 = (uint32_t)(temp0 ^ low);

    uint32_t temp_xor = (uint32_t)(temp1 ^ 0x3fc3);
    uint32_t final_ans = (uint32_t)(temp_xor + (temp0 * 4));

    values[0] = final_ans;
    count = 1;

    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    values[0] = (uint32_t)((challenge >> 13) & 0xffu);
    for (size_t i = 1; i < 6; i++) {
        uint32_t x = (uint32_t)((challenge >> (6 + 4 * (i - 1))) & 0x3fu);
        values[i] = (x + (uint32_t)i) ^ (3u * values[i - 1]);
    }
    count = 6;
    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;

    uint32_t index = (uint32_t)challenge & 0x7;
    uint32_t val = ((uint32_t)(challenge >> 3)) ^ ((uint32_t)(challenge >> 32));

    values[0] = index;
    uint32_t res = 0;

    if (index == 0) {
        res = (val * 3) + 0x3d85;
    } else if (index == 1) {
        res = (val * 4) ^ 0xcc89;
    } else if (index == 2) {
        res = (val * 5) + 0x2c2d1;
    } else if (index == 3) {
        res = (val * 4) ^ 0x79ae;
    } else if (index == 4) {
        res = (val * 5) + 0x8bba;
    } else if (index == 5) {
        res = (val * 3) + 0x1b2b;
    } else if (index == 6) {
        res = (val ^ 0xe286) + (val >> 2);
    } else if (index == 7) {
        res = (val ^ 0x4a15) + (val >> 2);
    }

    values[1] = res;
    count = 2;

    return write_answer(out, cap, values, count);
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;

    uint32_t target = (uint32_t)challenge & 0x7f;
    uint32_t low = 0;
    uint32_t high = 0x7f;
    uint32_t xor_key = ((uint32_t)(challenge >> 7)) ^ ((uint32_t)(challenge >> 32));

    uint32_t path_ebx[10];
    uint32_t path_op[10];
    int depth = 0;

    while (1) {
        uint32_t mid = (high - low) / 2 + low;
        if (target == mid) {
            uint32_t res = (mid + 0x6d) ^ xor_key;
            for (int i = depth - 1; i >= 0; i--) {
                res = ((res * 5) ^ path_ebx[i]) ^ path_op[i];
            }
            values[0] = res;
            break;
        }
        path_ebx[depth] = mid + xor_key;
        if (target > mid) {
            path_op[depth] = 0x17;
            low = mid + 1;
        } else {
            path_op[depth] = 0x11;
            high = mid - 1;
        }
        depth++;
    }

    count = 1;

    return write_answer(out, cap, values, count);
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;

    uint32_t index = (uint32_t)challenge & 0x7;
    uint32_t steps = ((uint32_t)challenge >> 3) & 0x3;
    uint32_t eax = ((uint32_t)(challenge >> 6)) ^ ((uint32_t)(challenge >> 32)) ^ 0xc509;

    struct Node {
        int next;
        uint32_t val;
        uint32_t offset;
    };

    struct Node nodes[8][4] = {
        {{1, 0x21d, 0x278e}, {2, 0xb3e, 0xf5be}, {3, 0x62d, 0x713d}, {-1, 0x16b, 0x7bee}},
        {{1, 0xb3e, 0xf5be}, {2, 0x62d, 0x713d}, {3, 0x16b, 0x7bee}, {-1, 0x2a, 0xe0fd}},
        {{1, 0x62d, 0x713d}, {2, 0x16b, 0x7bee}, {3, 0x2a, 0xe0fd}, {-1, 0xa35, 0xc0cd}},
        {{1, 0x16b, 0x7bee}, {2, 0x2a, 0xe0fd}, {3, 0xa35, 0xc0cd}, {-1, 0x414, 0xc372}},
        {{1, 0x2a, 0xe0fd}, {2, 0xa35, 0xc0cd}, {3, 0x414, 0xc372}, {-1, 0x363, 0xb08b}},
        {{1, 0xa35, 0xc0cd}, {2, 0x414, 0xc372}, {3, 0x363, 0xb08b}, {-1, 0x765, 0xd20e}},
        {{1, 0x414, 0xc372}, {2, 0x363, 0xb08b}, {3, 0x765, 0xd20e}, {-1, 0x560, 0x72ca}},
        {{1, 0x363, 0xb08b}, {2, 0x765, 0xd20e}, {3, 0x560, 0x72ca}, {-1, 0x92e, 0x6bb0}}
    };

    int curr = 0;
    uint32_t current_val = nodes[index][curr].val;
    
    eax += nodes[index][curr].offset;
    eax = (eax * 5) ^ current_val;

    for (uint32_t i = 0; i < steps; i++) {
        curr = nodes[index][curr].next;
        current_val = nodes[index][curr].val;
        eax += nodes[index][curr].offset;
        eax = (eax * 5) ^ current_val;
    }

    values[0] = eax;
    values[1] = current_val;

    count = 2;
    return write_answer(out, cap, values, count);
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;

    uint32_t arg1 = ((uint32_t)(challenge >> 6)) ^ ((uint32_t)(challenge >> 32));
    uint32_t idx1 = (uint32_t)challenge & 0x7;
    uint32_t idx2 = ((uint32_t)challenge >> 3) & 0x7;

    uint32_t res1 = 0;
    if (idx1 == 0) {
        res1 = (arg1 ^ 0x1804) + (arg1 * 2);
    } else if (idx1 == 1) {
        res1 = ((arg1 << 5) + 0x9272) ^ (arg1 * 7);
    } else if (idx1 == 2) {
        res1 = ((arg1 << 5) | (arg1 >> 27)) ^ 0xafb6;
    } else if (idx1 == 3) {
        res1 = ((arg1 >> 7) ^ arg1) + 0xbbc3;
    } else if (idx1 == 4) {
        res1 = (arg1 ^ 0x1804) + (arg1 * 2);
    } else if (idx1 == 5) {
        res1 = (arg1 + 0x3d51) ^ (arg1 >> 2);
    } else if (idx1 == 6) {
        res1 = ((arg1 << 5) | (arg1 >> 27)) ^ 0xafb6;
    } else if (idx1 == 7) {
        res1 = (arg1 >> 1) | (arg1 << 31);
    }

    uint32_t arg2 = res1 ^ 0x4841;

    uint32_t res2 = 0;
    if (idx2 == 0) {
        res2 = (arg2 ^ 0x1804) + (arg2 * 2);
    } else if (idx2 == 1) {
        res2 = ((arg2 << 5) + 0x9272) ^ (arg2 * 7);
    } else if (idx2 == 2) {
        res2 = ((arg2 << 5) | (arg2 >> 27)) ^ 0xafb6;
    } else if (idx2 == 3) {
        res2 = ((arg2 >> 7) ^ arg2) + 0xbbc3;
    } else if (idx2 == 4) {
        res2 = (arg2 ^ 0x1804) + (arg2 * 2);
    } else if (idx2 == 5) {
        res2 = (arg2 + 0x3d51) ^ (arg2 >> 2);
    } else if (idx2 == 6) {
        res2 = ((arg2 << 5) | (arg2 >> 27)) ^ 0xafb6;
    } else if (idx2 == 7) {
        res2 = (arg2 >> 1) | (arg2 << 31);
    }

    values[0] = res2 + 0x5227;
    count = 1;

    return write_answer(out, cap, values, count);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    uint32_t a0 = (uint32_t)context & 0x3;

    uint32_t esi = ((uint32_t)(challenge >> 32)) ^ ((uint32_t)challenge);
    uint32_t r8d = ((uint32_t)(context >> 32)) ^ ((uint32_t)context);
    uint32_t edi = esi ^ r8d;

    uint32_t eax = ((edi >> 1) | (edi << 31)) ^ 0x4b23;

    uint32_t r9d = (a0 * 0x4a38) ^ ((uint32_t)(context >> 16));
    uint32_t a1 = eax ^ r9d;

    uint32_t esi_shifted = esi >> 1;
    uint32_t edx = 0x11b + r8d + a0 + esi_shifted;
    uint32_t a2 = a1 ^ edx;

    snprintf(out, cap, "%32sOPEN %u %u %u", "", a0, a1, a2);

    return 0;
}