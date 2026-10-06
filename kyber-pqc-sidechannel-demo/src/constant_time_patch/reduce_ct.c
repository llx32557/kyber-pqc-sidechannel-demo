// 常数时间修复版取模逻辑 (占位，实际需配合 poly_ct 使用)
#include "../kyber/reduce.h"

int16_t barrett_reduce_ct(int16_t a) {
    return barrett_reduce(a); // 实际项目需替换为无分支实现
}