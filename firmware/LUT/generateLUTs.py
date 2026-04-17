SIZE = 4096
OUT_OF_RANGE = 0


# ---------------- BRAKE CALIBRATION ----------------
BRAKE_MIN_LOW = 45
BRAKE_MIN_HIGH = 52

BRAKE_MAX_LOW = 3500
BRAKE_MAX_HIGH = 3600


# ---------------- ACCEL CALIBRATION ----------------
ACCEL_MIN_LOW = 95
ACCEL_MIN_HIGH = 106

ACCEL_MAX_LOW = 650
ACCEL_MAX_HIGH = 700


# ---------------- BRAKE PRESSURE 1 (FL FRONT) CALIBRATION ----------------
BRAKE_PRESSURE1_MIN_LOW = 200
BRAKE_PRESSURE1_MIN_HIGH = 220

BRAKE_PRESSURE1_MAX_LOW = 3500
BRAKE_PRESSURE1_MAX_HIGH = 3600


# ---------------- BRAKE PRESSURE 2 (FL BACK) CALIBRATION ----------------
BRAKE_PRESSURE2_MIN_LOW = 200
BRAKE_PRESSURE2_MIN_HIGH = 220

BRAKE_PRESSURE2_MAX_LOW = 3500
BRAKE_PRESSURE2_MAX_HIGH = 3600


def compute_percent(i, min_low, min_high, max_low, max_high):
    if i < min_low or i > max_high:
        return OUT_OF_RANGE
    if min_low <= i <= min_high:
        return 0
    if max_low <= i <= max_high:
        return 100
    span = max_low - min_high
    return ((i - min_high) * 100) // span


def write_lut(f, name, min_low, min_high, max_low, max_high):
    f.write(f"const uint8_t {name}[4096] = {{\n")
    for i in range(SIZE):
        percent = compute_percent(i, min_low, min_high, max_low, max_high)
        f.write(f"{percent}")
        if i < SIZE - 1:
            f.write(", ")
        if (i + 1) % 16 == 0:
            f.write("\n")
    f.write("\n};\n\n")


with open("Src/adc_percent_pots_lut.c", "w") as f:
    f.write("#include <stdint.h>\n\n")
    f.write("/* --------------------------------------------------\n")
    f.write("    LUTs for Brake, Accel, and Brake Pressure Sensors\n")
    f.write("   -------------------------------------------------- */\n\n")

    write_lut(f, "adcPercentBrakeLUT",
              BRAKE_MIN_LOW, BRAKE_MIN_HIGH,
              BRAKE_MAX_LOW, BRAKE_MAX_HIGH)

    write_lut(f, "adcPercentAccelLUT",
              ACCEL_MIN_LOW, ACCEL_MIN_HIGH,
              ACCEL_MAX_LOW, ACCEL_MAX_HIGH)

    write_lut(f, "adcPercentBrakePressure1LUT",
              BRAKE_PRESSURE1_MIN_LOW, BRAKE_PRESSURE1_MIN_HIGH,
              BRAKE_PRESSURE1_MAX_LOW, BRAKE_PRESSURE1_MAX_HIGH)

    write_lut(f, "adcPercentBrakePressure2LUT",
              BRAKE_PRESSURE2_MIN_LOW, BRAKE_PRESSURE2_MIN_HIGH,
              BRAKE_PRESSURE2_MAX_LOW, BRAKE_PRESSURE2_MAX_HIGH)

print("Lookup tables written to adc_percent_pots_lut.c")