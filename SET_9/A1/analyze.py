from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd


def main():
    df = pd.read_csv("string_sort_results.csv")
    outdir = Path("analysis_plots")
    outdir.mkdir(parents=True, exist_ok=True)

    for dataset in sorted(df["dataset"].unique()):
        ddf = df[df["dataset"] == dataset]
        plt.figure(figsize=(10, 6))
        for alg in sorted(ddf["algorithm"].unique()):
            adf = ddf[ddf["algorithm"] == alg].sort_values("size")
            plt.plot(adf["size"], adf["avg_time_us"], marker="o", label=alg)

        plt.xlabel("size")
        plt.ylabel("avg time (us)")
        plt.title(f"Time vs size - {dataset}")
        plt.legend()
        plt.tight_layout()
        plt.savefig(outdir / f"avg_time_{dataset}.png")
        plt.close()

        plt.figure(figsize=(10, 6))
        for alg in sorted(ddf["algorithm"].unique()):
            adf = ddf[ddf["algorithm"] == alg].sort_values("size")
            plt.plot(adf["size"], adf["char_comparisons"], marker="o", label=alg)

        plt.xlabel("size")
        plt.ylabel("char comparisons")
        plt.title(f"Char comparisons vs size - {dataset}")
        plt.legend()
        plt.tight_layout()
        plt.savefig(outdir / f"char_comparisons_{dataset}.png")
        plt.close()


if __name__ == "__main__":
    main()
