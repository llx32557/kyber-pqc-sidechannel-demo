CC = gcc
CFLAGS = -O2 -I src/kyber -I src/sidechannel -I src/constant_time_patch
LDFLAGS = -lm

# 核心算法源文件
KYBER_SRC = src/kyber/kem.c src/kyber/poly.c src/kyber/ntt.c src/kyber/reduce.c
# 侧信道采样模块
SIDECHANNEL_SRC = src/sidechannel/timing_collect.c
# 常数时间修复模块
CT_PATCH_SRC = src/constant_time_patch/poly_ct.c src/constant_time_patch/reduce_ct.c
# ★ 程序的入口
MAIN_SRC = tests/main.c

# 目标文件
all: vulnerable fixed

# 1. 编译有漏洞版本 (包含 main.c)
vulnerable: $(MAIN_SRC) $(KYBER_SRC) $(SIDECHANNEL_SRC)
	mkdir -p build datasets
	$(CC) $(CFLAGS) -o build/vulnerable $^

# 2. 编译常数时间修复版本 (包含 main.c，并定义宏 USE_CONSTANT_TIME)
fixed: $(MAIN_SRC) $(KYBER_SRC) $(SIDECHANNEL_SRC) $(CT_PATCH_SRC)
	mkdir -p build datasets
	$(CC) $(CFLAGS) -DUSE_CONSTANT_TIME -o build/fixed $^

clean:
	rm -f build/vulnerable build/fixed datasets/*.csv docs/*.png

# 快捷运行命令
run_vuln: vulnerable
	./build/vulnerable -o datasets/timing_data.csv -n 100000

run_fixed: fixed
	./build/fixed -o datasets/timing_data_fixed.csv -n 100000

analyze:
	python3 scripts/analyze_leakage.py

.PHONY: all clean run_vuln run_fixed analyze