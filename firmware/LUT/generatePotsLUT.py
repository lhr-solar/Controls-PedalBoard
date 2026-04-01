ADC_MAX = 4095
SIZE = 4096

MIN_ADC = 800
MAX_ADC = 3600

with open("adc_percent_pots_lut.c", "w") as f:
    f.write("#include <stdint.h>\n#include \"ADC_init.h\"\n\n")
    f.write("/* --------------------------------------------------\n")
    f.write("    LUT for Pots\n")
    f.write("   -------------------------------------------------- */\n\n")

    f.write("const uint16_t adcPercentPotsLUT[4096] = {\n")

    for i in range(SIZE):

        if i <= MIN_ADC:
            value = 0
        elif i >= MAX_ADC:
            value = 100
        else:
            value = ((i - MIN_ADC) * 100) // (MAX_ADC - MIN_ADC)

        f.write(f"{value}")

        if i < SIZE - 1:
            f.write(", ")

        if (i + 1) % 16 == 0:
            f.write("\n")

    f.write("\n};\n")

print("Lookup table written to adc_percent_pots_lut.c")