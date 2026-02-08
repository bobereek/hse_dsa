import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv('data.csv')

rel_err = (df['AvgEst'] - df['AvgExact']) / df['AvgExact']
b = 12
m = 2**b
sigma_1 = 1.04 / np.sqrt(m)
sigma_2 = 1.32 / np.sqrt(m)

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(14, 12),)

ax1.plot(df['Processed'], df['AvgExact'], 'k--', linewidth=2, alpha=0.7, label='True Cardinality')
ax1.plot(df['Processed'], df['AvgEst'], 'b-', linewidth=2, label='HLL Estimate')
ax1.fill_between(df['Processed'],
                 df['LowerBound'],
                 df['UpperBound'],
                 color='blue', alpha=0.15, label='±σ')
ax1.set_title('HyperLogLog Accuracy', fontsize=14, fontweight='bold')
ax1.set_ylabel('Cardinality', fontsize=12)
ax1.legend(loc='best', fontsize=10)
ax1.grid(True, alpha=0.3)

ax2.plot(df['Processed'], rel_err * 100, 'm-', linewidth=2, label='Relative Error')
ax2.axhline(sigma_1 * 100, color='red', linestyle='--', alpha=0.6, label=f'1.04/√m = ±{sigma_1 * 100:.2f}%')
ax2.axhline(-sigma_1 * 100, color='red', linestyle='--', alpha=0.6)
ax2.axhline(sigma_2 * 100, color='orange', linestyle='--', alpha=0.6, label=f'1.32/√m = ±{sigma_2 * 100:.2f}%')
ax2.axhline(-sigma_2 * 100, color='orange', linestyle='--', alpha=0.6)
ax2.fill_between(df['Processed'], -sigma_1 * 100, sigma_1 * 100, color='red', alpha=0.1)
ax2.set_title('Relative Error vs Theoretical Bounds', fontsize=14, fontweight='bold')
ax2.set_xlabel('Processed Items in Stream', fontsize=12)
ax2.set_ylabel('Relative Error (%)', fontsize=12)
ax2.legend(loc='best', fontsize=10)
ax2.grid(True, alpha=0.3)
ax2.set_ylim(-3, 3)

plt.tight_layout()
plt.savefig('hll_analysis.png', bbox_inches='tight')
plt.show()
