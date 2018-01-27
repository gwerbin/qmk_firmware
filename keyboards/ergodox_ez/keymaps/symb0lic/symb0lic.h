#include QMK_KEYBOARD_H

#define LAYOUT_ergodox_visual(                                       \
     k00,k01,k02,k03,k04,k05,k06,       k07,k08,k09,k0A,k0B,k0C,k0D, \
     k10,k11,k12,k13,k14,k15,k16,       k17,k18,k19,k1A,k1B,k1C,k1D, \
     k20,k21,k22,k23,k24,k25,               k28,k29,k2A,k2B,k2C,k2D, \
     k30,k31,k32,k33,k34,k35,k36,       k37,k38,k39,k3A,k3B,k3C,k3D, \
     k40,k41,k42,k43,k44,                       k49,k4A,k4B,k4C,k4D, \
                             k55,k56,   k57,k58,                     \
                                 k54,   k59,                         \
                         k53,k52,k51,   k5C,k5B,k5A )                \
    KEYMAP(                              \
        k00,k01,k02,k03,k04,k05,k06,     \
        k10,k11,k12,k13,k14,k15,k16,     \
        k20,k21,k22,k23,k24,k25,         \
        k30,k31,k32,k33,k34,k35,k36,     \
        k40,k41,k42,k43,k44,             \
                                k55,k56, \
                                    k54, \
                            k53,k52,k51, \
                                         \
            k07,k08,k09,k0A,k0B,k0C,k0D, \
            k17,k18,k19,k1A,k1B,k1C,k1D, \
                k28,k29,k2A,k2B,k2C,k2D, \
            k37,k38,k39,k3A,k3B,k3C,k3D, \
                    k49,k4A,k4B,k4C,k4D, \
        k57,k58,                         \
        k59,                             \
        k5C,k5B,k5A )
