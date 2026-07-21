/*<#======================================================================#>*/
/*  FW image descriptor (single-boot / thin-FW model, DS5).                 */
/*  Emitted at the FW flash base (0x00038000). CORE (ngmm_task_main) finds  */
/*  + validates this and invokes the FW entry points instead of BX-jumping  */
/*  into an FW reboot.                                                       */
/*<#======================================================================#>*/

#include "core_contract_ds4_fw_descriptor.h"

/* FW single-boot entry points (see fw_boot.c / fw_init.c). */
extern void fw_ram_init(void);       /* init FW RW/ZI in the RW_FW window; run FIRST      */
extern void fw_identity_init(void);  /* populate CORE's config BEFORE the config consumers */
extern void fw_init(void);           /* vehicle init                                      */

#if defined(__CC_ARM)
/* Placed FIRST in the FW exec region by the scatter (*(FW_DESC, +First)),
 * so the struct base lands exactly at CORE_DS4_FLASH_FW_START_ADDR (0x38000),
 * replacing the legacy crt0 trampoline that used to occupy that slot. */
#define FW_DESC_PLACEMENT __attribute__((section("FW_DESC"), used))
#else
#define FW_DESC_PLACEMENT
#endif

const core_ds4_fw_descriptor_t g_fw_descriptor FW_DESC_PLACEMENT =
{
    CORE_DS4_FW_DESC_MAGIC,
    ((uint32_t)CORE_DS4_ABI_VERSION_MAJOR << 16) | (uint32_t)CORE_DS4_ABI_VERSION_MINOR,
    sizeof(core_ds4_fw_descriptor_t),
    0U,                 /* flags */
    fw_ram_init,
    fw_identity_init,
    fw_init,
    0,                  /* fw_init_reset_fn: optional (CORE dispatcher is null-safe) */
    {0},                /* reserved[7] */
    0U                  /* crc32: unused (0) */
};
