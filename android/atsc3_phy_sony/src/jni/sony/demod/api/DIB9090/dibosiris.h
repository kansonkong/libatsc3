#ifndef PMU_OSIRIS_H
#define PMU_OSIRIS_H

#include "dibcommon.h"

struct dibDataBusHost;

struct osiris_config {
  /* All voltages must be given in mV */
  uint16_t dcdc1v2;
  uint16_t dcdc2v;
  uint16_t ldo_sram;
  uint16_t ldo_tuner;
  uint16_t ldo_adc;
  uint16_t ldo_risc;
  uint16_t ldo_adc2;

  uint8_t enable_ldo_adc;     /* Enable LDO ADC2 (bit 1) and ADC1 (bit 0) */
  uint8_t dcdc2_current_sense;/* Enable current sense, can be disabled to increase
				 efficiency (be sure that the DCDC2 load won't generate
				 peak current above 600mA) */
  uint8_t dcdc2_divider;      /* DCDC2 clock divider: 
				 0 -> /2
				 1 -> /4
				 2 -> /6
				 3 -> /8
				 4 -> /10
				 5 -> /12
				 6 -> /14
				 7 -> /1
			      */
  uint8_t boost;              /* Disable soft start (cope w/ high loads) 
				 bit 4 -> LDO ADC1, 
				 bit 3 -> LDO SRAM, 
				 bit 2 -> LDO TUNER RF,
				 bit 1 -> LDO TUNER BB,
				 bit 0 -> LDO TUNER LO
			      */
};

extern struct dibPMU * osiris_create(struct dibDataBusHost *i2c, const struct osiris_config *cfg);

#endif
