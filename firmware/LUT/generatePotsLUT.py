SIZE = 4096
OUT_OF_RANGE = 255


# ---------------- BRAKE CALIBRATION ----------------
BRAKE_MIN_LOW = 80
BRAKE_MIN_HIGH = 107

BRAKE_MAX_LOW = 590
BRAKE_MAX_HIGH = 650


# ---------------- ACCEL CALIBRATION ----------------
ACCEL_MIN_LOW = 90
ACCEL_MIN_HIGH = 107

ACCEL_MAX_LOW = 620
ACCEL_MAX_HIGH = 650


def compute_percent(i, min_low, min_high, max_low, max_high):

    if i < min_low or i > max_high:
        return OUT_OF_RANGE

    if min_low <= i <= min_high:
        return 0

    if max_low <= i <= max_high:
        return 100

    # linear scaling region
    span = max_low - min_high
    return ((i - min_high) * 100) // span


with open("Src/adc_percent_pots_lut.c", "w") as f:

    f.write("#include <stdint.h>\n\n")
    f.write("/* --------------------------------------------------\n")
    f.write("    LUTs for Brake and Accel Pots\n")
    f.write("   -------------------------------------------------- */\n\n")


    # -------- Brake LUT --------
    f.write("const uint8_t adcPercentBrakeLUT[4096] = {\n")

    for i in range(SIZE):

        percent = compute_percent(
            i,
            BRAKE_MIN_LOW,
            BRAKE_MIN_HIGH,
            BRAKE_MAX_LOW,
            BRAKE_MAX_HIGH
        )

        f.write(f"{percent}")

        if i < SIZE - 1:
            f.write(", ")

        if (i + 1) % 16 == 0:
            f.write("\n")

    f.write("\n};\n\n")


    # -------- Accel LUT --------
    f.write("const uint8_t adcPercentAccelLUT[4096] = {\n")

    for i in range(SIZE):

        percent = compute_percent(
            i,
            ACCEL_MIN_LOW,
            ACCEL_MIN_HIGH,
            ACCEL_MAX_LOW,
            ACCEL_MAX_HIGH
        )

        f.write(f"{percent}")

        if i < SIZE - 1:
            f.write(", ")

        if (i + 1) % 16 == 0:
            f.write("\n")

    f.write("\n};\n")


print("Lookup tables written to adc_percent_pots_lut.c")