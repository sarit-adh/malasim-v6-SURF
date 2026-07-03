#!/usr/bin/env python3
"""Extract target CNV frequencies from generated SQLite reporter outputs."""

from __future__ import annotations

import argparse
import csv
import re
import sqlite3
from datetime import date
from pathlib import Path


EXPERIMENT_ROOT = Path(__file__).resolve().parents[1]
SCENARIOS_CSV = EXPERIMENT_ROOT / "manifests" / "scenarios.csv"
OUTPUT_CSV = EXPERIMENT_ROOT / "analysis" / "tables" / "cnv_frequencies.csv"
START_DATE = date(2000, 1, 1)
WITHDRAWAL_DATE = date(2010, 1, 1)
WITHDRAWAL_DAY = (WITHDRAWAL_DATE - START_DATE).days


def copy_number(genotype: str, target_gene: str, target_chromosome: int) -> int:
    chromosomes = genotype.split("|")
    chromosome_index = target_chromosome - 1
    if chromosome_index < 0 or chromosome_index >= len(chromosomes):
        raise ValueError(
            f"Chromosome {target_chromosome} is outside genotype with {len(chromosomes)} chromosomes: {genotype}"
        )
    token = chromosomes[chromosome_index].split(",")[0]

    match = re.search(r"(\d)$", token)
    if not match:
        raise ValueError(f"Could not parse {target_gene} copy number from genotype: {genotype}")
    return int(match.group(1))


def find_genome_table(connection: sqlite3.Connection) -> str:
    rows = connection.execute(
        "SELECT name FROM sqlite_master WHERE type='table' AND name LIKE 'monthly_genome_data_%' ORDER BY name"
    ).fetchall()
    if not rows:
        raise RuntimeError("No monthly_genome_data_* table found.")
    for (name,) in rows:
        if name == "monthly_genome_data_district":
            return name
    return rows[0][0]


def extract_db(db_path: Path, scenario: dict[str, str], job: int) -> list[dict[str, object]]:
    rows = []
    target_chromosome = int(scenario["target_chromosome"])
    with sqlite3.connect(db_path) as connection:
        genome_table = find_genome_table(connection)
        query = f"""
            SELECT
              md.days_elapsed,
              g.name,
              mg.occurrences,
              mg.clinical_occurrences,
              mg.weighted_occurrences
            FROM {genome_table} mg
            JOIN monthly_data md ON md.id = mg.monthly_data_id
            JOIN genotype g ON g.id = mg.genome_id
        """
        monthly = {}
        for days_elapsed, genotype, occurrences, clinical_occurrences, weighted_occurrences in connection.execute(query):
            month = monthly.setdefault(
                days_elapsed,
                {
                    "total_occurrences": 0,
                    "total_clinical_occurrences": 0,
                    "total_weighted_occurrences": 0.0,
                    "target_cnv_occurrences": 0,
                    "target_cnv_clinical_occurrences": 0,
                    "target_cnv_weighted_occurrences": 0.0,
                },
            )
            cn = copy_number(genotype, scenario["target_gene"], target_chromosome)
            month["total_occurrences"] += occurrences
            month["total_clinical_occurrences"] += clinical_occurrences
            month["total_weighted_occurrences"] += weighted_occurrences
            if cn > 1:
                month["target_cnv_occurrences"] += occurrences
                month["target_cnv_clinical_occurrences"] += clinical_occurrences
                month["target_cnv_weighted_occurrences"] += weighted_occurrences

        for days_elapsed, values in sorted(monthly.items()):
            weighted_total = values["total_weighted_occurrences"]
            occurrence_total = values["total_occurrences"]
            rows.append(
                {
                    "scenario_id": scenario["scenario_id"],
                    "arm": scenario["arm"],
                    "policy": scenario["policy"],
                    "job": job,
                    "days_elapsed": days_elapsed,
                    "simulation_year": START_DATE.year + (days_elapsed / 365.0),
                    "period": "withdrawal_followup" if days_elapsed >= WITHDRAWAL_DAY else "selection_burnin",
                    "target_gene": scenario["target_gene"],
                    "mutation_probability_per_locus": scenario["mutation_probability_per_locus"],
                    "cnv_reversion_multiplier": scenario["cnv_reversion_multiplier"],
                    "cnv_daily_crs": scenario["cnv_daily_crs"],
                    **values,
                    "target_cnv_weighted_frequency": (
                        values["target_cnv_weighted_occurrences"] / weighted_total if weighted_total else ""
                    ),
                    "target_cnv_occurrence_frequency": (
                        values["target_cnv_occurrences"] / occurrence_total if occurrence_total else ""
                    ),
                    "db_path": db_path.as_posix(),
                }
            )
    return rows


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--scenarios", type=Path, default=SCENARIOS_CSV)
    parser.add_argument("--output", type=Path, default=OUTPUT_CSV)
    args = parser.parse_args()

    all_rows = []
    with args.scenarios.open(newline="") as handle:
        for scenario in csv.DictReader(handle):
            output_dir = EXPERIMENT_ROOT.parents[1] / scenario["output_path"]
            for db_path in sorted(output_dir.glob("monthly_data_*.db")):
                match = re.search(r"monthly_data_(\d+)\.db$", db_path.name)
                if not match:
                    continue
                all_rows.extend(extract_db(db_path, scenario, int(match.group(1))))

    args.output.parent.mkdir(parents=True, exist_ok=True)
    fieldnames = [
        "scenario_id",
        "arm",
        "policy",
        "job",
        "days_elapsed",
        "simulation_year",
        "period",
        "target_gene",
        "mutation_probability_per_locus",
        "cnv_reversion_multiplier",
        "cnv_daily_crs",
        "total_occurrences",
        "total_clinical_occurrences",
        "total_weighted_occurrences",
        "target_cnv_occurrences",
        "target_cnv_clinical_occurrences",
        "target_cnv_weighted_occurrences",
        "target_cnv_weighted_frequency",
        "target_cnv_occurrence_frequency",
        "db_path",
    ]
    with args.output.open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(all_rows)
    print(f"Wrote {len(all_rows)} monthly CNV frequency rows to {args.output}")


if __name__ == "__main__":
    main()
