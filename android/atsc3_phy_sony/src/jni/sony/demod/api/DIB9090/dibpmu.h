#ifndef ADAPTER_PMU_H
#define ADAPTER_PMU_H

#include "dibcommon.h"
#include "dibdatabus.h"

#ifdef __cplusplus
extern "C" {
#endif

struct dibPMU {	
	char name[MAX_INFO_NAME_LENGTH];	//const char name[MAX_INFO_NAME_LENGTH];

	struct {
        int (*reset)    (struct dibPMU *);
		void (*release) (struct dibPMU *);
	} ops;

	struct dibDataBusClient data_bus_client;

	void *priv;
};

extern void pmu_init(struct dibPMU *, const struct dibPMU *, void *priv);

#define pmu_reset(p) ((p) != NULL && (p)->ops.reset != NULL ? (p)->ops.reset(p) : DIB_RETURN_SUCCESS)

#define pmu_release(p) do { if ((p) != NULL && (p)->ops.release != NULL) (p)->ops.release(p); } while (0)

#define pmu_get_register_access(d) \
	((d) != NULL                                              ? &(d)->data_bus_client : NULL)

#ifdef __cplusplus
}
#endif

#endif
