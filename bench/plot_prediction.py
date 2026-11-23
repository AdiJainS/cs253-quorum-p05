import argparse
import csv
import matplotlib.pyplot as plt


def main():
    parser = argparse.ArgumentParser(description="Plot the prefix baseline against the predicted linear shape")
    parser.add_argument("summary", help="prefix-summary.csv")
    parser.add_argument("output")
    parser.add_argument("--n", type=int, required=True)
    parser.add_argument("--operations", type=int, required=True)
    parser.add_argument("--update-percent", type=int, required=True)
    parser.add_argument("--seed", type=int, required=True)
    args = parser.parse_args()

    points = []
    with open(args.summary, newline="", encoding="utf-8-sig") as file:
        for row in csv.DictReader(file):
            if (row["structure"] == "prefix_fenwick" and int(row["n"]) == args.n
                    and int(row["operations"]) == args.operations
                    and int(row["update_percent"]) == args.update_percent
                    and int(row["seed"]) == args.seed):
                points.append((int(row["actual_decreases"]), float(row["replay_mean_ms"]),
                               float(row["replay_sd_ms"])))
    if len(points) < 3:
        parser.error("need the 0%, 50% and 100% decrease rows")
    points.sort()
    decreases, means, spreads = zip(*points)
    first = means[0]
    slope = (means[-1] - first) / decreases[-1]
    predicted = [first + slope * d for d in decreases]

    plt.errorbar(decreases, means, yerr=spreads, marker="o", capsize=3, label="measured (mean of 5 runs, sd)")
    plt.plot(decreases, predicted, linestyle="--", marker="x", label="model: a + b * U-")
    plt.xlabel("Actual decreases U- (count)")
    plt.ylabel("Replay time (ms)")
    plt.title(f"Prefix baseline, n={args.n}, operations={args.operations}\nupdates={args.update_percent}%, seed={args.seed}")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()
    plt.savefig(args.output, dpi=160)
    for d, m, p in zip(decreases, means, predicted):
        print(f"U-={d} measured={m:.3f} ms model={p:.3f} ms gap={m - p:+.3f} ms")


if __name__ == "__main__":
    main()
