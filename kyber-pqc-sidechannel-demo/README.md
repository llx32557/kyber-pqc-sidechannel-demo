# CRYSTALS-Kyber 后量子 KEM 算法的软件计时侧信道泄漏检测与防护验证

> **Software Timing Side-Channel Leakage Detection and Countermeasure Verification for CRYSTALS-Kyber PQC KEM Algorithm**

[![Platform](https://img.shields.io/badge/Platform-Ubuntu%2024.04-orange.svg)](https://ubuntu.com/)
[![Compiler](https://img.shields.io/badge/Compiler-GCC%2013.3-blue.svg)](https://gcc.gnu.org/)
[![Python](https://img.shields.io/badge/Python-3.12-green.svg)](https://www.python.org/)
[![Standard](https://img.shields.io/badge/TVLA-ISO%2FIEC%2017825-red.svg)]()
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)]()

---

## 📌 项目简介

本项目针对 **NIST PQC 标准算法 CRYSTALS-Kyber** 的官方参考实现，基于 **软件计时侧信道（Timing Side-Channel）** 方法，检测其 `poly_compress` 函数中存在的**数据依赖分支**所导致的计时泄漏，并通过**常数时间（Constant-Time）掩码修复**验证防护效果。

项目**不做硬件级物理攻击**（无需示波器、电磁探头），仅使用 PC 端的 **CPU 周期计数器（rdtsc）** 进行高精度软件层计时测量，适合作为密码工程 / 硬件安全方向的入门到进阶项目。

### 🎯 核心目标

1. 定位 Kyber 参考实现中数据相关的可变时间代码段
2. 通过 **Welch's t-test** 和 **TVLA（Test Vector Leakage Assessment）** 双方法验证泄漏
3. 对比原版可变时间实现 vs 常数时间修复版本，量化验证防护效果

---

## 📊 实验结果摘要

### ✅ 时序检测（T-test）

| 版本 | 高耗时组均值 | 低耗时组均值 | t-statistic | p-value | 判定 |
|---|---|---|---|---|---|
| **有漏洞版** | 567.13 cycles | 519.31 cycles | 4.3198 | **1.57e-05** | 🚨 **存在泄漏** |

> **结论：** p-value < 0.05，统计上高度显著，证明 `poly_compress` 存在数据依赖计时泄漏。

### ✅ TVLA 验证（Fixed vs Random，阈值 |t| > 4.5）

| 版本 | Fixed 组均值 | Random 组均值 | \|t\| 值 | 判定 |
|---|---|---|---|---|
| **有漏洞版** | 532.5 cycles | 586.9 cycles | **7.17** | 🚨 **泄漏被工业标准证实** |
| **修复版** | 537.9 cycles | 575.3 cycles | **3.88** | ✅ **泄漏被消除** |

> **结论：** 修复后 \|t\| 从 7.17 降至 3.88，降幅 **45.8%**，满足 ISO/IEC 17825 标准阈值，泄漏消除。

### 📈 可视化结果

| 图表 | 说明 |
|---|---|
| `docs/leakage_vulnerable.png` | 有漏洞版本时序分布（T-test） |
| `docs/tvla_result.png` | TVLA Fixed vs Random 分布对比 |

---

## 🧠 技术原理

### 泄漏根源

Kyber 参考实现中 `poly_compress` 函数存在如下**数据依赖分支**：

```c
if (coeff > KYBER_Q / 2) {
    coeff = coeff - KYBER_Q;   // ★ 分支跳跃导致执行路径变化
}
当秘密多项式的系数大小不同时，代码会进入不同的分支，导致 CPU 执行路径长度不同，进而体现为运行时间差异。

检测方法
高精度计时：使用 rdtsc 指令读取 CPU 周期计数器，精度可达纳秒级。

分组 T-test：将样本按耗时中位数分组，检验两组均值是否存在统计显著差异。

TVLA：固定私钥 vs 随机私钥，若 |t| > 4.5 则判定存在泄漏。

修复方案
将条件分支替换为无分支的位运算掩码：

c
int16_t mask = -(coeff > (KYBER_Q / 2));  // 全 1 或全 0
coeff -= (KYBER_Q & mask);                 // 常数时间条件减法

项目结构：
kyber-pqc-sidechannel-demo/
├── README.md                        # 项目主页
├── Makefile                         # 一键编译（漏洞版/修复版）
├── .gitignore
│
├── src/
│   ├── kyber/                       # Kyber 算法核心
│   │   ├── kem.c / kem.h            # KEM 顶层接口
│   │   ├── poly.c / poly.h          # 多项式运算（★ 泄漏点）
│   │   ├── ntt.c / ntt.h            # 数论变换
│   │   ├── reduce.c / reduce.h      # 模约简
│   │   └── params.h                 # Kyber512/768 参数
│   │
│   ├── sidechannel/                 # 侧信道采样模块
│   │   ├── timing_collect.c         # rdtsc 计时采样
│   │   ├── timing_collect.h
│   │   ├── tvla_collect.c           # TVLA 采样（Fixed vs Random）
│   │   └── tvla_collect.h
│   │
│   └── constant_time_patch/         # 常数时间修复
│       ├── poly_ct.c                # 无分支掩码版 poly_compress
│       └── reduce_ct.c              # 常数时间模约简
│
├── scripts/                         # Python 统计分析
│   ├── analyze_leakage.py           # T-test + 皮尔逊 + 绘图
│   └── tvla_analyze.py              # TVLA 分析
│
├── tests/                           # 测试入口
│   ├── main.c                       # 采样程序入口
│   └── test_kem.c                   # KEM 功能测试
│
├── datasets/                        # 采集的时序数据（*.csv）
│   ├── timing_data.csv
│   ├── tvla_data.csv
│   └── tvla_data_fixed.csv
│
└── docs/                            # 图表与实验记录
    ├── leakage_vulnerable.png
    ├── tvla_result.png
    ├── timing_result.txt
    ├── tvla_vulnerable.txt
    ├── tvla_fixed.txt
    └── notes.md

    快速开始
环境依赖
OS：Ubuntu 22.04 / 24.04 LTS

编译器：GCC 13.3+（需支持 x86intrin.h）

Python：3.10+，依赖 numpy、scipy、pandas、matplotlib

安装依赖
bash
sudo apt update
sudo apt install build-essential make python3 python3-pip -y
sudo apt install python3-pandas python3-numpy python3-scipy python3-matplotlib -y


编译与运行
bash
# 1. 克隆仓库
git clone https://github.com/yourname/kyber-pqc-sidechannel-demo.git
cd kyber-pqc-sidechannel-demo

# 2. 一键编译（生成 build/vulnerable 与 build/fixed）
make

# 3. 采集有漏洞版本时序数据（10 万次）
./build/vulnerable -o datasets/timing_data.csv -n 100000

# 4. 采集 TVLA 数据（有漏洞版 + 修复版）
./build/vulnerable --tvla -o datasets/tvla_data.csv -n 5000
./build/fixed      --tvla -o datasets/tvla_data_fixed.csv -n 5000

# 5. 运行 Python 统计分析
python3 scripts/analyze_leakage.py
python3 scripts/tvla_analyze.py datasets/tvla_data.csv
python3 scripts/tvla_analyze.py datasets/tvla_data_fixed.csv
使用 Makefile 快捷命令
bash
make run_vuln        # 编译 + 运行有漏洞版采样
make run_fixed       # 编译 + 运行修复版采样
make run_tvla_vuln   # TVLA 采样（有漏洞版）
make run_tvla_fixed  # TVLA 采样（修复版）
make analyze         # 运行 Python 分析
make clean           # 清理构建产物

实验环境
项目	配置
操作系统	Ubuntu 24.04 LTS（VMware 虚拟机）
编译器	GCC 13.3.0
优化级别	-O2
计时方式	__rdtscp（CPU 周期计数器）
采样次数	T-test：100,000 次；TVLA：每组 5,000 次
统计方法	Welch's t-test（不假设方差相等）
判定阈值	p < 0.05（T-test）；|t| > 4.5（TVLA，ISO/IEC 17825）


📖 引用与参考
Kyber 官方参考实现：https://github.com/pq-crystals/kyber

KyberSlash 漏洞（2024）：KyberSlash: Exploiting secret-dependent division timings in Kyber implementations

NIST PQC 标准：FIPS 203 (ML-KEM)

TVLA 标准：ISO/IEC 17825:2016

📜 License
本项目基于 MIT License 发布，仅用于学术研究与教学用途。