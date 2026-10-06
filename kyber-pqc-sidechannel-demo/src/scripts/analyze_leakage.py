import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy import stats
import sys

def analyze(filepath, output_plot):
    print(f"Loading {filepath}...")
    try:
        df = pd.read_csv(filepath)
    except FileNotFoundError:
        print(f"Error: {filepath} not found. Run make run_vuln first.")
        return

    cycles = df['cycles'].values
    # 去掉前面预热产生的异常值
    cycles = cycles[cycles > np.percentile(cycles, 5)]
    
    # 模拟按内部秘密系数高低分组 (这里按中位数分组)
    median_val = np.median(cycles)
    group_high = cycles[cycles > median_val]
    group_low = cycles[cycles <= median_val]

    # Welch's T-test
    t_stat, p_value = stats.ttest_ind(group_high, group_low, equal_var=False)
    
    print("="*40)
    print(f"Mean High: {np.mean(group_high):.2f} cycles")
    print(f"Mean Low:  {np.mean(group_low):.2f} cycles")
    print(f"T-statistic: {t_stat:.4f}")
    print(f"P-value: {p_value:.4e}")
    
    if p_value < 0.05:
        print(">>> Result: TIMING LEAKAGE DETECTED! (p < 0.05)")
    else:
        print(">>> Result: No significant leakage detected.")
    print("="*40)

    # 绘图
    fig, axes = plt.subplots(1, 2, figsize=(12, 5))
    axes[0].boxplot([group_low, group_high], labels=['Low', 'High'])
    axes[0].set_title('Cycle Count Distribution')
    axes[0].set_ylabel('Cycles')

    axes[1].hist(cycles, bins=60, alpha=0.7, color='steelblue')
    axes[1].axvline(median_val, color='red', linestyle='dashed')
    axes[1].set_title('Execution Time Histogram')
    axes[1].set_xlabel('Cycles')

    plt.tight_layout()
    plt.savefig(output_plot)
    print(f"Plot saved to {output_plot}")

if __name__ == "__main__":
    analyze("datasets/timing_data.csv", "docs/leakage_vulnerable.png")
    # 如果生成了修复版数据，取消注释下一行进行对比
    # analyze("datasets/timing_data_fixed.csv", "docs/leakage_fixed.png")