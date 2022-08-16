#include "dibpmu.h"

void pmu_init(struct dibPMU *p, const struct dibPMU *tmpl, void *priv)
{
    memcpy(p, tmpl, sizeof(struct dibPMU));
    p->priv = priv;
}
