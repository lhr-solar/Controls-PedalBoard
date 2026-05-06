#!/usr/bin/env python3
"""
generate_lut.py
Generates PedalsLUT.h — ADC-to-percent, ADC-to-mV, and ADC-to-PSI lookup tables.

Regenerate: python3 scripts/generate_lut.py
"""

from pathlib import Path

OUT = Path(__file__).resolve().parent.parent / "drivers/Inc/PedalsLUT.h"

SIZE = 4096
VREF = 3.3

# -------- Calibration (ADC counts, 12-bit) --------
# min = 0%, max = 100%; values outside are clamped
BRAKE_MAIN       = (52,   3500)
BRAKE_REDUNDANT  = (52,   3500)
ACCEL_MAIN       = (106,  650)
ACCEL_REDUNDANT  = (106,  650)
BRAKE_PRESSURE1  = (220,  3500)
BRAKE_PRESSURE2  = (220,  3500)

# -------- Pressure sensor --------
V_PSI_MIN, V_PSI_MAX, PSI_MAX = 0.5, 4.5, 3000.0


# ---------- Helpers ----------

def clamp(x, lo, hi):
    return max(lo, min(hi, x))


# ---------- Percent (supports inversion for backwards-wired sensors) ----------

def percent(i, min_val, max_val, invert=False):
    if not invert:
        if i <= min_val: return 0
        if i >= max_val: return 100
        return ((i - min_val) * 100) // (max_val - min_val)
    else:
        if i >= min_val: return 0
        if i <= max_val: return 100
        return ((min_val - i) * 100) // (min_val - max_val)


def percent_table(name, min_val, max_val, invert=False):
    vals = [percent(i, min_val, max_val, invert) for i in range(SIZE)]
    rows = [
        "    " + ", ".join(f"{v:3d}" for v in vals[i:i+16]) + ","
        for i in range(0, SIZE, 16)
    ]
    return (
        f"static const uint8_t {name}[{SIZE}] = {{\n"
        + "\n".join(rows) +
        "\n};\n"
    )


# ---------- Brake redundant voltage (mV) ----------
# Sensor wired inverted: ADC=0 treated as fault (returns 50 mV floor);
# otherwise output = (4095 - i) * 3300 / 4096 + 50

def brake_redundant_mv(i):
    if i == 0:
        return 50
    return ((4095 - i) * 3300) // 4096 + 50


def voltage_table(name, func):
    vals = [func(i) for i in range(SIZE)]
    rows = [
        "    " + ", ".join(f"{v:4d}" for v in vals[i:i+12]) + ","
        for i in range(0, SIZE, 12)
    ]
    return (
        f"static const uint16_t {name}[{SIZE}] = {{\n"
        + "\n".join(rows) +
        "\n};\n"
    )


# ---------- Pressure ----------

def psi_tenths(i):
    v = i * VREF / 4095.0
    if v <= V_PSI_MIN:
        psi = 0.0
    elif v >= V_PSI_MAX:
        psi = PSI_MAX
    else:
        psi = (v - V_PSI_MIN) / (V_PSI_MAX - V_PSI_MIN) * PSI_MAX
    return clamp(round(psi * 10), 0, 30000)


def psi_table():
    vals = [psi_tenths(i) for i in range(SIZE)]
    rows = [
        "    " + ", ".join(f"{v:5d}" for v in vals[i:i+12]) + ","
        for i in range(0, SIZE, 12)
    ]
    return (
        "// 0.5 V = 0 PSI, 4.5 V = 3000 PSI; units = 0.1 PSI\n"
        f"static const uint16_t brakePressurePsiTenthsLUT[{SIZE}] = {{\n"
        + "\n".join(rows) +
        "\n};\n"
    )


# ---------- Generate ----------

OUT.write_text(
    "/**\n"
    " * @file PedalsLUT.h\n"
    " * @brief Auto-generated pedal ADC lookup tables — do not edit\n"
    " *        Regenerate: python3 scripts/generate_lut.py\n"
    " *\n"
    " * Percent tables : index = 12-bit ADC count (0-4095), value = 0-100 %\n"
    " * Voltage table  : index = 12-bit ADC count (0-4095), value = mV\n"
    " * PSI table      : index = 12-bit ADC count (0-4095), value = 0.1 PSI units\n"
    " */\n\n"
    "#pragma once\n\n"
    "#include <stdint.h>\n\n"
    + percent_table("adcPercentBrakeMainLUT",      *BRAKE_MAIN)               + "\n"
    + percent_table("adcPercentBrakeRedundantLUT", *BRAKE_REDUNDANT)          + "\n"
    + percent_table("adcPercentAccelMainLUT",      *ACCEL_MAIN)               + "\n"
    + percent_table("adcPercentAccelRedundantLUT", *ACCEL_REDUNDANT, True)    + "\n"
    + percent_table("adcPercentBrakePressure1LUT", *BRAKE_PRESSURE1)          + "\n"
    + percent_table("adcPercentBrakePressure2LUT", *BRAKE_PRESSURE2)          + "\n"
    + voltage_table("adcBrakeRedundantMilliVoltsLUT", brake_redundant_mv)     + "\n"
    + psi_table()
)

print(f"Generated {OUT}  ({SIZE} entries per table)")
