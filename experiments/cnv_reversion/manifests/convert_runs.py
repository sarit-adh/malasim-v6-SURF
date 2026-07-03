#!/usr/bin/env python3
"""Convert runs.csv to runs.txt (extract command column, one per line)."""

import csv


def convert_csv_to_txt(csv_path: str, txt_path: str) -> int:
    """Extract command column from CSV and write as newline-separated commands."""
    count = 0
    with (
        open(csv_path, newline="", encoding="utf-8") as f_in,
        open(txt_path, "w", encoding="utf-8") as f_out,
    ):
        reader = csv.DictReader(f_in)
        for row in reader:
            cmd = row["command"].strip()
            if cmd:
                # replace ./build/bin with ../bin
                cmd = cmd.replace("./build/bin", "../bin")
                # replace experiments/cnv_reversion/configs with ../configs
                cmd = cmd.replace("experiments/cnv_reversion/configs", "../configs")
                # replace experiments/cnv_reversion/outputs with ../outputs
                cmd = cmd.replace("experiments/cnv_reversion/outputs", "../outputs")

                f_out.write(cmd + "\n")
                count += 1
    return count


if __name__ == "__main__":
    n = convert_csv_to_txt("runs.csv", "runs.txt")
    print(f"Converted {n} commands to runs.txt")
