#include "dibfrontend.h"
#include "dib0090.h"

#include "dibx09x_common.h"

const struct dib0090_io_config dibx09x_io_30mhz_120 = {
    30000,

    0,
    1,
    1,
    8, // loop div

    8, // ADC clock ratio
};

const struct dib0090_io_config dibx09x_io_26mhz_120 = {
    26000,

    0,
    1,
    1,
    9, // loop div

    8, // ADC clock ratio
};

const struct dib0090_io_config dibx09x_io_12mhz_120 = {
    12000,

    0,
    1,
    1,
    20, // loop div

    8, // ADC clock ratio
};

const struct dib0090_io_config dibx09x_io_30mhz = {
    30000,

    0,
    1,
    3,
    22, // loop div

    8, // ADC clock ratio
};

const struct dib0090_io_config dibx09x_io_26mhz = {
    26000,

    0,
    1,
    4,
    33, // loop div

    8, // ADC clock ratio
};

const struct dib0090_io_config dibx09x_io_12mhz = {
    12000,

    0,
    1,
    3,
    55, // loop div

    8, // ADC clock ratio
};
