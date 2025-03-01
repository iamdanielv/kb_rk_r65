#VIA_ENABLE = yes
TAP_DANCE_ENABLE = yes
#LAYER_LOCK_ENABLE = yes
ENCODER_MAP_ENABLE = yes
SRC += features/indicator_queue.c
SRC += features/fn_mode.c
SRC += features/tap_hold.c
SRC += features/indicators.c
SRC += features/rgb_keys.c
SRC += features/dv_layer_lock.c

RGB_MATRIX_CUSTOM_USER = yes
