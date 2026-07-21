/*<#======================================================================#>*/
/*  FW RAM bring-up for the single-boot model (DS5).                        */
/*  In single-boot CORE never resets the FW, so __main/__scatterload never  */
/*  runs. fw_ram_init does what scatterload would: copy RW init data into   */
/*  the RW_FW window and zero the ZI, using the linker-generated region     */
/*  symbols. Requires the FW link with --datacompressor off (plain copy).   */
/*<#======================================================================#>*/

#include <string.h>
#include <stdint.h>

/* ARMCC region symbols for RW_FW (see app_fw_ds4_thin.sct).
 * ARMCC permits '$' in identifiers. */
extern uint32_t Load$$RW_FW$$Base;
extern uint32_t Image$$RW_FW$$Base;
extern uint32_t Image$$RW_FW$$Length;      /* RW (init data) length, excl. ZI */
extern uint32_t Image$$RW_FW$$ZI$$Base;
extern uint32_t Image$$RW_FW$$ZI$$Length;

void fw_ram_init(void)
{
    uint32_t rw_len = (uint32_t)&Image$$RW_FW$$Length;
    uint32_t zi_len = (uint32_t)&Image$$RW_FW$$ZI$$Length;

    if (rw_len != 0U)
    {
        memcpy((void*)&Image$$RW_FW$$Base,
               (const void*)&Load$$RW_FW$$Base,
               (size_t)rw_len);
    }
    if (zi_len != 0U)
    {
        memset((void*)&Image$$RW_FW$$ZI$$Base, 0, (size_t)zi_len);
    }
}
