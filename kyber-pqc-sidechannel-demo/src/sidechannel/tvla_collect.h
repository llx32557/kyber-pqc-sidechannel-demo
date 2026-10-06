#ifndef TVLA_COLLECT_H
#define TVLA_COLLECT_H

// 固定输入 vs 随机输入 的 TVLA 采样
// group 0 = 固定私钥 (Fixed)
// group 1 = 随机私钥 (Random)
void run_tvla_collection(const char *filename, int samples_per_group);

#endif