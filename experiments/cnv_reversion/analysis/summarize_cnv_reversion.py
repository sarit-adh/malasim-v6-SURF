#!/usr/bin/env python3
"""Summarize CNV reversion trajectories from extracted monthly frequencies."""

from __future__ import annotations

import argparse
import csv
from collections import defaultdict
from datetime import date
from pathlib import Path


EXPERIMENT_ROOT = Path(__file__).resolve().parents[1]
INPUT_CSV = EXPERIMENT_ROOT / "analysis" / "tables" / "cnv_frequencies.csv"
OUTPUT_CSV = EXPERIMENT_ROOT / "analysis" / "tables" / "cnv_reversion_summary.csv"
START_DATE = date(2000, 1, 1)


def day_offset(calendar_date: date) -> int:
    return (calendar_date - START_DATE).days


WITHDRAWAL_DAY = day_offset(date(2010, 1, 1))
TARGET_DAYS = {
    "frequency_year_10": WITHDRAWAL_DAY,
    "frequency_year_15": day_offset(date(2015, 1, 1)),
    "frequency_year_20": day_offset(date(2020, 1, 1)),
}


def as_float(value: str) -> float | None:
    if value == "":
        return None
    return float(value)


def nearest_frequency(rows: list[dict[str, str]], target_day: int) -> float | None:
    eligible = [row for row in rows if as_float(row["target_cnv_weighted_frequency"]) is not None]
    if not eligible:
        return None
    row = min(eligible, key=lambda item: abs(int(item["days_elapsed"]) - target_day))
    return as_float(row["target_cnv_weighted_frequency"])


def time_to_half_decline(rows: list[dict[str, str]]) -> float | None:
    baseline = nearest_frequency(rows, WITHDRAWAL_DAY)
    if baseline is None or baseline <= 0:
        return None
    threshold = baseline * 0.5
    for row in sorted(rows, key=lambda item: int(item["days_elapsed"])):
        day = int(row["days_elapsed"])
        if day < WITHDRAWAL_DAY:
            continue
        freq = as_float(row["target_cnv_weighted_frequency"])
        if freq is not None and freq <= threshold:
            return (day - WITHDRAWAL_DAY) / 365.0
    return None


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", type=Path, default=INPUT_CSV)
    parser.add_argument("--output", type=Path, default=OUTPUT_CSV)
    args = parser.parse_args()

    grouped = defaultdict(list)
    with args.input.open(newline="") as handle:
        for row in csv.DictReader(handle):
            key = (
                row["scenario_id"],
                row["arm"],
                row["policy"],
                row["job"],
                row["target_gene"],
                row["mutation_probability_per_locus"],
                row["cnv_reversion_multiplier"],
                row["cnv_daily_crs"],
            )
            grouped[key].append(row)

    output_rows = []
    for key, rows in sorted(grouped.items()):
        scenario_id, arm, policy, job, target_gene, mutation_rate, reversion_multiplier, cnv_cost = key
        summary = {
            "scenario_id": scenario_id,
            "arm": arm,
            "policy": policy,
            "job": job,
            "target_gene": target_gene,
            "mutation_probability_per_locus": mutation_rate,
            "cnv_reversion_multiplier": reversion_multiplier,
            "cnv_daily_crs": cnv_cost,
            "time_to_half_decline_years": time_to_half_decline(rows),
        }
        for label, day in TARGET_DAYS.items():
            summary[label] = nearest_frequency(rows, day)
        output_rows.append(summary)

    args.output.parent.mkdir(parents=True, exist_ok=True)
    fieldnames = [
        "scenario_id",
        "arm",
        "policy",
        "job",
        "target_gene",
        "mutation_probability_per_locus",
        "cnv_reversion_multiplier",
        "cnv_daily_crs",
        "frequency_year_10",
        "frequency_year_15",
        "frequency_year_20",
        "time_to_half_decline_years",
    ]
    with args.output.open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(output_rows)
    print(f"Wrote {len(output_rows)} trajectory summaries to {args.output}")


if __name__ == "__main__":
    main()
