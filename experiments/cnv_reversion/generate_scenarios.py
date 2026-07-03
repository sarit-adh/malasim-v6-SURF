#!/usr/bin/env python3
"""Generate CNV reversion validation scenario configs and manifests."""

from __future__ import annotations

import csv
from dataclasses import dataclass
from pathlib import Path


EXPERIMENT_ROOT = Path(__file__).resolve().parent
REPO_ROOT = EXPERIMENT_ROOT.parents[1]
BASE_CONFIG = EXPERIMENT_ROOT / "base_input.yml"

MUTATION_RATES = [0.001983, 0.005, 0.01]
REVERSION_MULTIPLIERS = [0.0, 0.01, 0.05, 0.1, 0.2, 0.4]
CNV_COSTS = [0.0005, 0.001, 0.005, 0.01]
FIRST_PASS_JOBS = range(10)


@dataclass(frozen=True)
class Arm:
    key: str
    label: str
    initial_strategy_id: int
    target_gene: str
    target_chromosome: int
    selecting_therapy_id: int
    selecting_drug_id: int


ARMS = [
    Arm("al", "AL", 0, "Pfmdr1", 5, 6, 1),
    Arm("as_mq", "AS-MQ", 3, "Pfmdr1", 5, 9, 4),
    Arm("dha_ppq", "DHA-PPQ", 2, "Pfplasmepsin", 14, 8, 3),
]
POLICIES = ["control", "withdrawal"]
WITHDRAWAL_STRATEGY_ID = 1


def encode_value(value: float) -> str:
    text = f"{value:g}"
    return text.replace(".", "p")


def scenario_name(
    mutation_rate: float, reversion_multiplier: float, cnv_cost: float
) -> str:
    return (
        f"mp_{encode_value(mutation_rate)}"
        f"__rev_{encode_value(reversion_multiplier)}"
        f"__cost_{encode_value(cnv_cost)}"
    )


def set_simple_value(lines: list[str], key: str, value: str) -> list[str]:
    prefix = f"{key}:"
    updated = []
    for line in lines:
        stripped = line.lstrip()
        if stripped.startswith(prefix):
            indent = line[: len(line) - len(stripped)]
            updated.append(f"{indent}{key}: {value}\n")
        else:
            updated.append(line)
    return updated


def set_initial_strategy(lines: list[str], strategy_id: int) -> list[str]:
    return set_simple_value(lines, "initial_strategy_id", str(strategy_id))


def append_withdrawal_event(lines: list[str]) -> list[str]:
    insertion = [
        "\n",
        "  - name: change_treatment_strategy\n",
        "    info:\n",
        "      - date: 2010/1/1\n",
        f"        strategy_id: {WITHDRAWAL_STRATEGY_ID}\n",
    ]

    for index, line in enumerate(lines):
        if (
            line.startswith(
                "# ---------------------------------------------------------------"
            )
            and index > 0
        ):
            previous = "".join(lines[max(0, index - 5) : index])
            if (
                "population_events:" in "".join(lines[max(0, index - 20) : index])
                and "rapt_settings" not in previous
            ):
                return lines[:index] + insertion + lines[index:]

    for index, line in enumerate(lines):
        if line.startswith("rapt_settings:"):
            return lines[:index] + insertion + lines[index:]
    raise RuntimeError("Could not find population event insertion point.")


def tune_target_gene(
    lines: list[str], target_gene: str, reversion_multiplier: float, cnv_cost: float
) -> list[str]:
    updated = []
    in_target_gene = False
    target_indent = None

    for line in lines:
        stripped = line.lstrip()
        indent_len = len(line) - len(stripped)

        if stripped.startswith("- name: "):
            target_indent = indent_len
            in_target_gene = (
                f'"{target_gene}"' in stripped
                or stripped.rstrip().endswith(f": {target_gene}")
            )
        elif (
            in_target_gene
            and target_indent is not None
            and indent_len <= target_indent
            and stripped
        ):
            in_target_gene = False
            target_indent = None

        if in_target_gene and stripped.startswith("cnv_reversion_multiplier:"):
            indent = line[:indent_len]
            updated.append(
                f"{indent}cnv_reversion_multiplier: {reversion_multiplier:g}\n"
            )
            continue

        if in_target_gene and stripped.startswith("cnv_daily_crs:"):
            indent = line[:indent_len]
            updated.append(f"{indent}cnv_daily_crs: [0, {cnv_cost:g}]\n")
            continue

        updated.append(line)

    return updated


def build_config(
    base_text: str,
    arm: Arm,
    policy: str,
    mutation_rate: float,
    reversion_multiplier: float,
    cnv_cost: float,
) -> str:
    lines = base_text.splitlines(keepends=True)
    lines = set_simple_value(lines, "starting_date", "2000/1/1")
    lines = set_simple_value(lines, "ending_date", "2020/1/1")
    lines = set_simple_value(lines, "start_collect_data_day", "0")
    lines = set_simple_value(lines, "record_genome_db", "true")
    lines = set_simple_value(
        lines, "mutation_probability_per_locus", f"{mutation_rate:g}"
    )
    lines = set_initial_strategy(lines, arm.initial_strategy_id)
    lines = tune_target_gene(lines, arm.target_gene, reversion_multiplier, cnv_cost)

    if policy == "withdrawal":
        lines = append_withdrawal_event(lines)

    return "".join(lines)


def write_csv(path: Path, rows: list[dict[str, object]], fieldnames: list[str]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)


def main() -> None:
    base_text = BASE_CONFIG.read_text()
    scenarios = []
    runs = []

    for arm in ARMS:
        for policy in POLICIES:
            for mutation_rate in MUTATION_RATES:
                for reversion_multiplier in REVERSION_MULTIPLIERS:
                    for cnv_cost in CNV_COSTS:
                        name = scenario_name(
                            mutation_rate, reversion_multiplier, cnv_cost
                        )
                        config_rel = (
                            Path("experiments")
                            / "cnv_reversion"
                            / "configs"
                            / arm.key
                            / policy
                            / f"{name}.yml"
                        )
                        output_rel = (
                            Path("experiments")
                            / "cnv_reversion"
                            / "outputs"
                            / arm.key
                            / policy
                            / name
                        )
                        config_abs = REPO_ROOT / config_rel
                        output_abs = REPO_ROOT / output_rel
                        output_abs.mkdir(parents=True, exist_ok=True)
                        (output_abs / ".gitkeep").touch()

                        config_text = build_config(
                            base_text,
                            arm,
                            policy,
                            mutation_rate,
                            reversion_multiplier,
                            cnv_cost,
                        )
                        config_abs.parent.mkdir(parents=True, exist_ok=True)
                        config_abs.write_text(config_text)

                        scenarios.append(
                            {
                                "scenario_id": f"{arm.key}__{policy}__{name}",
                                "arm": arm.key,
                                "arm_label": arm.label,
                                "policy": policy,
                                "mutation_probability_per_locus": mutation_rate,
                                "cnv_reversion_multiplier": reversion_multiplier,
                                "cnv_daily_crs": cnv_cost,
                                "target_gene": arm.target_gene,
                                "target_chromosome": arm.target_chromosome,
                                "selecting_therapy_id": arm.selecting_therapy_id,
                                "selecting_drug_id": arm.selecting_drug_id,
                                "initial_strategy_id": arm.initial_strategy_id,
                                "withdrawal_strategy_id": WITHDRAWAL_STRATEGY_ID
                                if policy == "withdrawal"
                                else "",
                                "config_path": config_rel.as_posix(),
                                "output_path": output_rel.as_posix() + "/",
                            }
                        )

                        for job in FIRST_PASS_JOBS:
                            runs.append(
                                {
                                    "scenario_id": f"{arm.key}__{policy}__{name}",
                                    "job": job,
                                    "command": (
                                        "./build/bin/MalaSim "
                                        f"-i {config_rel.as_posix()} "
                                        f"-o {output_rel.as_posix()}/ "
                                        f"-j {job}"
                                    ),
                                }
                            )

    write_csv(
        EXPERIMENT_ROOT / "manifests" / "scenarios.csv",
        scenarios,
        [
            "scenario_id",
            "arm",
            "arm_label",
            "policy",
            "mutation_probability_per_locus",
            "cnv_reversion_multiplier",
            "cnv_daily_crs",
            "target_gene",
            "target_chromosome",
            "selecting_therapy_id",
            "selecting_drug_id",
            "initial_strategy_id",
            "withdrawal_strategy_id",
            "config_path",
            "output_path",
        ],
    )
    write_csv(
        EXPERIMENT_ROOT / "manifests" / "runs.csv",
        runs,
        ["scenario_id", "job", "command"],
    )
    print(
        f"Wrote {len(scenarios)} scenario configs and {len(runs)} first-pass run commands."
    )


if __name__ == "__main__":
    main()
