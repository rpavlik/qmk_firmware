# MCU name

MCU = RP2040
# BOARD = RP_PICO_RP2040
BOARD = GENERIC_RP_RP2040
# MCU_LDSCRIPT = RP2040_FLASH_TIMECRIT.ld
# Bootloader selection
BOOTLOADER = rp2040

# Build Options
#   change yes to no to disable
#
BOOTMAGIC_ENABLE = no       # Enable Bootmagic Lite
MOUSEKEY_ENABLE = no        # Mouse keys
EXTRAKEY_ENABLE = no        # Audio control and System control
CONSOLE_ENABLE = no         # Console for debug
COMMAND_ENABLE = no         # Commands for debug and configuration
NKRO_ENABLE = no            # Enable N-Key Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = no        # Enable keyboard RGB underglow
AUDIO_ENABLE = no           # Audio output
CUSTOM_MATRIX = yes

SRC = matrix.c led.c
