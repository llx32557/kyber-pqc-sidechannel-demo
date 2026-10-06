# 开发笔记与实验记录

## 阶段1：环境搭建与审计
- 已拉取简化版 Kyber 参考代码
- **审计发现**：`src/kyber/poly.c` 中 `poly_compress` 函数包含 `if (coeff > KYBER_Q / 2)` 分支。
- 该分支导致不同秘密系数下，CPU 执行的指令路径长度不同，存在计时侧信道泄漏风险。

## 阶段2：高精度计时采样
- 使用 `__rdtscp` 读取 CPU 周期计数器。
- 采样前进行了 1000 次预热，减少 CPU 频率变化的影响。
- 添加 `_mm_mfence` 防止指令重排。
- 采样目标：`crypto_kem_dec` (解封装) 流程。

## 阶段3：统计检测
- 将样本按耗时中位数分为高/低两组。
- 使用 Welch's t-test 检验两组均值差异。
- 预期结果：P-value < 0.05，拒绝原假设，证明存在计时泄漏。

## 阶段4：常数时间修复
- 在 `src/constant_time_patch/poly_ct.c` 中，将 `if` 分支替换为位运算掩码：
  `mask = -(coeff > Q/2); coeff -= (Q & mask);`
- 待运行 `make fixed` 并重新采样，验证修复效果。