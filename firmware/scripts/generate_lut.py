#!/usr/bin/env python3
"""
generate_lut.py
Generates PedalsLUT.h — ADC-to-percent and ADC-to-PSI lookup tables.

Brake pressure LUT: index = MCU 12-bit count (verbatim CAN `Brake_Pressure_ADC`); CAN `Brake_Pressure`
= tenths-PSI (DBC × 0.1). Transmitters specify 0.5–4.5 Vo on the usual brake/sensor rail (here 5 V
reference scale); the MCU converts counts using ADC reference (typically VDDA 3.3 V). LUT:
V_pin = count × ADC_VREF / 4095, Vo_est = V_pin × Vo_max_at_FS / ADC_VREF (divider maps sensor FS →
ADC FS volts), PSI from Vo_est over (Vo_min, Vo_max).

(Pedal redundant positions still use percent LUTs; redundant brake/accel ADC counts go on CAN directly.)

Regenerate (plain Python — no Nix):
  cd firmware && python3 scripts/generate_lut.py

Regenerate (preferred — toolchain from flake; run Python inside the shell):
  cd firmware/Embedded-Sharepoint && nix develop --command python3 ../scripts/generate_lut.py

From repo root:
  nix develop ./firmware/Embedded-Sharepoint --command python3 firmware/scripts/generate_lut.py
"""

from pathlib import Path

OUT = Path(__file__).resolve().parent.parent / "drivers/Inc/PedalsLUT.h"

SIZE = 4096

# -------- Calibration (ADC counts, 12-bit) --------
# min = 0%, max = 100%; values outside are clamped
BRAKE_MAIN       = (52,   3500)
BRAKE_REDUNDANT  = (52,   3500)
ACCEL_MAIN       = (1490,  1760)
ACCEL_REDUNDANT  = (390,  100)

# -------- Brake pressure sensor (5 V-scale analog output vs MCU ADC ref) --------
# Datasheet analog span is absolute volts on the usual 5 V supply/reference context (sensor “5 V scale”).
# MCU ADC resolves 0 .. ADC_VREF_V (typically VDDA ~3.3 V) → count→volt uses ADC_VREF_V only.
#
# Divider / conditioning is assumed roughly linear such that transmitter Vo_max (4.5 V here) aligns with
# ADC FS volts (≈ ADC_VREF). Then: Vo_est = V_pin × (Vo_max / ADC_VREF). Tune Vo_max / divider or
# override PIN_VOLTS_TO_SENSOR_OUTPUT_VOLTS_RATIO if BOM differs.

BRAKE_SENSOR_REF_VOLTAGE_V = 5.0  # supply / nominal signal reference scale per datasheet wiring
BRAKE_SENSOR_ANALOG_MIN_V = 0.5   # Vo at 0 PSI (with BRAKE_SENSOR_REF_VOLTAGE_V in spec)
BRAKE_SENSOR_ANALOG_MAX_V = 4.5  # Vo at rated full scale (≤ BRAKE_SENSOR_REF_VOLTAGE_V typically)

ADC_VREF_V = 3.3                 # VDDA/Vref+ used in count→V_pin — calibrate vs measured VDDA if needed

PSI_AT_RANGE_MIN, PSI_AT_RANGE_MAX = 0.0, 3000.0
ADC_FULL_SCALE_COUNT = SIZE - 1  # inclusive 4095: V_pin = adc_count × ADC_VREF_V / COUNT

PIN_VOLTS_TO_SENSOR_OUTPUT_VOLTS_RATIO = (
    BRAKE_SENSOR_ANALOG_MAX_V / ADC_VREF_V
)  # Vo_est from pin V; use 1.0 if MCU pin volts == Vo (no attenuation toward FS)


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


# ---------- Brake pressure PSI (datasheet volts → PSI, keyed by ADC count) ----------

def adc_count_to_pin_voltage_volts(adc_counts: int) -> float:
    """ADC DR right-aligned 12-bit: code 0 → 0 V, full-scale code → ADC_VREF_V."""
    return adc_counts * ADC_VREF_V / float(ADC_FULL_SCALE_COUNT)


def sensor_voltage_to_psi(volts: float) -> float:
    """Datasheet PSI vs transmitter output Vo on the brake sensor analog span."""
    v_lo, v_hi = BRAKE_SENSOR_ANALOG_MIN_V, BRAKE_SENSOR_ANALOG_MAX_V
    if volts <= v_lo:
        return PSI_AT_RANGE_MIN
    if volts >= v_hi:
        return PSI_AT_RANGE_MAX
    span_v = v_hi - v_lo
    return (volts - v_lo) / span_v * (PSI_AT_RANGE_MAX - PSI_AT_RANGE_MIN)


def brake_pressure_psi_tenths_for_adc_count(adc_counts: int) -> int:
    v_pin = adc_count_to_pin_voltage_volts(adc_counts)
    v_sensor_output = v_pin * PIN_VOLTS_TO_SENSOR_OUTPUT_VOLTS_RATIO
    psi = sensor_voltage_to_psi(v_sensor_output)
    return clamp(round(psi * 10), 0, 30000)


def psi_table():
    vals = [brake_pressure_psi_tenths_for_adc_count(i) for i in range(SIZE)]
    rows = [
        "    " + ", ".join(f"{v:5d}" for v in vals[i:i+12]) + ","
        for i in range(0, SIZE, 12)
    ]
    vl, vh = BRAKE_SENSOR_ANALOG_MIN_V, BRAKE_SENSOR_ANALOG_MAX_V
    return (
        f"// Brake pressure PSI: analog {vl:g}-{vh:g} V (sensor ~{BRAKE_SENSOR_REF_VOLTAGE_V:g} V ref scale) "
        f"->{PSI_AT_RANGE_MIN:.0f}-{PSI_AT_RANGE_MAX:.0f} PSI; ADC ref {ADC_VREF_V:g} V,\n"
        f"// V_pin=idx*{ADC_VREF_V:g}/{ADC_FULL_SCALE_COUNT}; Vo_est=V_pin*{PIN_VOLTS_TO_SENSOR_OUTPUT_VOLTS_RATIO:.6g}; "
        "stored = 0.1 PSI\n"
        f"static const uint16_t brakePressurePsiTenthsLUT[{SIZE}] = {{\n"
        + "\n".join(rows) +
        "\n};\n"
    )


# ---------- Generate ----------

OUT.write_text(
    "/**\n"
    " * @file PedalsLUT.h\n"
    " * @brief Auto-generated pedal ADC lookup tables — do not edit\n"
    " *        Regenerate: nix develop in firmware/Embedded-Sharepoint, then\n"
    " *        python3 ../scripts/generate_lut.py (see script docstring)\n"
    " *\n"
    " * Percent tables      : index = 12-bit ADC count (0-4095), value = 0-100 %\n"
    " * Brake pressure PSI    : transmitter Vo 0.5-4.5 V (vs ~5 V sensor ref scale) -> PSI; counts use\n"
    " *                       ADC_VREF (MCU) -> V_pin, then Vo_est via Vo_max/ADC_VREF divider model;\n"
    " *                       CAN tenths PSI (DBC 0.1)\n"
    " */\n\n"
    "#pragma once\n\n"
    "#include <stdint.h>\n\n"
    + percent_table("adcPercentBrakeMainLUT",      *BRAKE_MAIN)               + "\n"
    + percent_table("adcPercentBrakeRedundantLUT", *BRAKE_REDUNDANT)          + "\n"
    + percent_table("adcPercentAccelMainLUT",      *ACCEL_MAIN)               + "\n"
    + percent_table("adcPercentAccelRedundantLUT", *ACCEL_REDUNDANT, True)    + "\n"
    + psi_table()
)

print(f"Generated {OUT}  ({SIZE} entries per table)")
