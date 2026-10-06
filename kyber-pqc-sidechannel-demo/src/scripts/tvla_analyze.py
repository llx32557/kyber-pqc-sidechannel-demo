#!/usr/bin/env python3
"""
TVLA (Test Vector Leakage Assessment) 分析
工业标准阈值：|t| > 4.5 判定存在泄漏
"""
import pandas as pd
import numpy as np
from scipy import stats
import matplotlib.pyplot as plt
import sys

TVLA_THRESHOLD = 4.5  # ISO/IEC 17825 标准

def tvla_analyze(filepath="datasets/tvla_data.csv", plot_path="docs/tvla_result.png"):
    try:
        df = pd.read_csv(filepath)
    except FileNotFoundError:
        print(f"[!] File not found: {filepath}")
        sys.exit(1)

    group_fixed = df[df['group'] == 0]['cycles'].values
    group_rand  = df[df['group'] == 1]['cycles'].values

    # Welch's t-test
    t_stat, p_value = stats.ttest_ind(group_fixed, group_rand, equal_var=False)

    print("=" * 55)
    print("  TVLA (Fixed vs Random) Assessment Result")
    print("=" * 55)
    print(f"  Fixed group  : n={len(group_fixed)}, mean={np.mean(group_fixed):.1f} cycles")
    print(f"  Random group : n={len(group_rand)}, mean={np.mean(group_rand):.1f} cycles")
    print(f"  t-statistic  : {t_stat:.4f}")
    print(f"  p-value      : {p_value:.4e}")
    print("-" * 55)
    if abs(t_stat) > TVLA_THRESHOLD:
        print(f"  [!] |t| = {abs(t_stat):.2f} > {TVLA_THRESHOLD}")
        print("  >>> LEAKAGE DETECTED (工业标准判定) <<<")
    else:
        print(f"  [OK] |t| = {abs(t_stat):.2f} <= {TVLA_THRESHOLD}")
        print("  >>> No leakage detected <<<")
    print("=" * 55)

    # 绘图
    fig, axes = plt.subplots(1, 2, figsize=(12, 4.5))

    axes[0].boxplot([group_fixed, group_rand], labels=['Fixed (sk)', 'Random (sk)'])
    axes[0].set_title('TVLA: Fixed vs Random Private Key')
    axes[0].set_ylabel('Cycles')

    axes[1].hist(group_fixed, bins=50, alpha=0.6, label='Fixed', color='red', density=True)
    axes[1].hist(group_rand,  bins=50, alpha=0.6, label='Random', color='blue', density=True)
    axes[1].set_title(f'Distribution (t = {t_stat:.2f})')
    axes[1].set_xlabel('Cycles')
    axes[1].set_ylabel('Density')
    axes[1].legend()

    plt.tight_layout()
    plt.savefig(plot_path)
    print(f"[*] Plot saved to {plot_path}")

if __name__ == "__main__":
    path = sys.argv[1] if len(sys.argv) > 1 else "datasets/tvla_data.csv"
    tvla_analyze(path)