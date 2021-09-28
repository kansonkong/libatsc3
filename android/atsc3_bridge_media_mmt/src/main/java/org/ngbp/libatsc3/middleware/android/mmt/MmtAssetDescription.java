package org.ngbp.libatsc3.middleware.android.mmt;

public class MmtAssetDescription {
    public final String asset_id;
    public final String description;

    public MmtAssetDescription(String asset_id, String description) {
        this.asset_id = asset_id;
        this.description = description;
    }

    @Override
    public String toString() {
        return String.format("asset_id: %s, description: %s", asset_id, description);
    }
}
