/* SPDX-License-Identifier: GPL-3.0-only */
/* Studio Tess contributions — By: Inputsignwave.
 * See repository-root CREDITS.md and Tess_Developer_Handoff.md. */
/* Studio Tess: generator/harmony state lives in the same A/B commit as its
 * project's music. The former TSR1/TSR3 object is read before shared presets
 * may claim its sectors. Never use the factory key_mac sector for custom data. */
#define TESS_OBJ_MAGIC 0x33525354u
#define TESS_SLOT_BYTES (4u + TESS_FULL_BYTES)
#define TESS_OBJ_BYTES (4u + 4u * TESS_SLOT_BYTES)
static uint32_t tess_saved_hash[4];
static uint32_t tessera_live_hash(void)
{
    uint8_t state[TESS_FULL_BYTES]; tess_full_pack(state);
    return proj_hash(state, sizeof state);
}
static int tessera_store_save(uint32_t slot)
{
    tess_saved_hash[slot & 3u] = tessera_live_hash();
    return 0; /* bank_pack included it before the successful project commit */
}
static void tessera_store_retag(uint32_t slot, uint32_t old_tag) { (void)slot; (void)old_tag; }
#if MELODEE_FLASH
static uint8_t tess_blob[TESS_OBJ_BYTES] __attribute__((section(".pool")));
static uint8_t *tess_slot(uint32_t slot) { return tess_blob + 4u + (slot & 3u) * TESS_SLOT_BYTES; }
static int tess_blob_valid(const uint8_t *raw, uint32_t len)
{
    uint32_t magic; if (len != TESS_OBJ_BYTES && len != TESS_OLD_OBJ_BYTES) return 0;
    memcpy(&magic, raw, 4u);
    uint32_t old = len == TESS_OLD_OBJ_BYTES, stride = 4u + (old ? TESSERA_PK_BYTES : TESS_FULL_BYTES);
    if (magic != (old ? 0x31525354u : TESS_OBJ_MAGIC)) return 0;
    for (uint32_t slot = 0; slot < 4u; slot++) {
        uint32_t tag; memcpy(&tag, raw + 4u + slot * stride, 4u);
        if (tag && !tess_full_valid(raw + 8u + slot * stride, stride - 4u)) return 0;
    }
    return 1;
}
static void tess_expand_old(void)
{
    for (uint32_t slot = 4; slot-- > 0;) {
        uint8_t old[4u + TESSERA_PK_BYTES];
        memcpy(old, tess_blob + 4u + slot * sizeof old, sizeof old);
        memcpy(tess_slot(slot), old, sizeof old);
        features_defaults(tess_slot(slot) + sizeof old);
    }
    uint32_t magic = TESS_OBJ_MAGIC; memcpy(tess_blob, &magic, 4u);
}
/* Synthesize the old tagged representation for archive compatibility. Call
 * before capturing the runtime backup: project reads borrow proj_wire_u. */
static int tess_obj_read(void)
{
    uint32_t magic = TESS_OBJ_MAGIC, any = 0;
    memset(tess_blob, 0, sizeof tess_blob); memcpy(tess_blob, &magic, 4u);
    if (!flash_ok) return 0;
    for (uint32_t slot = 0; slot < 4u; slot++) {
        int n = proj_read_slot(slot);
        if (n && proj_scratch.tess_valid) {
            uint32_t tag = proj_hash(proj_wire_u.raw, (uint32_t)n);
            memcpy(tess_slot(slot), &tag, 4u);
            memcpy(tess_slot(slot) + 4u, proj_scratch.tess, TESS_FULL_BYTES); any = 1;
        }
    }
    return (int)any;
}
/* All blocks are validated before the first write. Already migrated projects
 * are skipped on retry; the old pair stays intact until every project commits. */
static uint32_t tess_restore_blob_projects(void)
{
    for (uint32_t slot = 0; slot < 4u; slot++) {
        uint32_t tag; memcpy(&tag, tess_slot(slot), 4u); if (!tag) continue;
        int n = proj_read_slot(slot); if (!n) continue;
        if (proj_hash(proj_wire_u.raw, (uint32_t)n) != tag) continue;
        if (!bank_full((uint32_t)n)) return 2;
        if (!proj_pack((project_store_t *)(proj_wire_u.raw + 8u), &proj_scratch)) return 2;
        bank_tess_set(proj_wire_u.raw, tess_slot(slot) + 4u);
        if (proj_write_slot(slot, proj_wire_u.raw, BANK_STORE_SIZE)) return 4;
    }
    return 0;
}
static uint32_t tessera_store_restore(const uint8_t *raw, uint32_t len)
{
    if (len && !tess_blob_valid(raw, len)) return 2;
    if (!flash_ok) return 4;
    memset(tess_blob, 0, sizeof tess_blob);
    if (len) memcpy(tess_blob, raw, len);
    if (len == TESS_OLD_OBJ_BYTES) tess_expand_old();
    return tess_restore_blob_projects();
}
static int tess_migrate_legacy(void)
{
    st_hdr_t header; int copy = st_current(OBJ_SHARED_EXT, &header);
    uint32_t magic = 0;
    if (copy < 0 || header.len < 4u || st_read_range(OBJ_SHARED_EXT, (uint32_t)copy, 0, &magic, 4u)) return 0;
    if (magic != TESS_OBJ_MAGIC && magic != 0x31525354u) return 0;
    int n = st_load(OBJ_SHARED_EXT, tess_blob, sizeof tess_blob);
    if (n <= 0 || !tess_blob_valid(tess_blob, (uint32_t)n)) goto failed;
    if ((uint32_t)n == TESS_OLD_OBJ_BYTES) tess_expand_old();
    BOOT_PRESET_BEGIN("MIGRATING TESS", 4u);
    if (tess_restore_blob_projects()) goto failed;
    /* The projects now durably own all matching states. Release only the old
     * final preset pair; migration of every other preset remains upstream's. */
    if (shared_wipe_bank(UP_BANKS - 1u, 0)) goto failed;
    up_cache_reset(); native_cache_reset();
    BOOT_WORK_PROGRESS(4u); BOOT_WORK_END(0);
    return 0;
failed:
    up_migration_error = 1;
    BOOT_PRESET_BEGIN("TESS MIGRATION ERROR", 1u); BOOT_WORK_END(1);
    return 1;
}
#else
static int tess_migrate_legacy(void) { return 0; }
#endif
