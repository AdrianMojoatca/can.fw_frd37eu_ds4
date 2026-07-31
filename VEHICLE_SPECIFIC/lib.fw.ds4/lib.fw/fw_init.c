/*<#======================================================================#>*/
/*                          FILE HEADER GOES HERE                           */
/*<#======================================================================#>*/

/*==========================================================================*/
// $Id: fw_init.c 15282 2012-06-12 13:29:20Z louis-philippe.rispoli $
/*==========================================================================*/


/*==========================================================================*/
/*                             I N C L U D E S                              */
/*==========================================================================*/
#include "fw.h"
//#include "prog_private.h"
//#include "key.h"
#include "detect_private.h"
#include "core_contract_ds4_firmware_config.h"
#include "core_contract_ds4_can_config.h"
#include "core_contract_ds4_pts_ctl_config.h"
#include "core_contract_ds4_status_config.h"
#include "core_contract_ds4_prog_config.h"
#include "core_contract_ds4_prog_option_defs.h"
#include "core_contract_ds4_boot_handshake.h"   /* v1.1: api_table_version guard       */
#include "core_contract_ds4_dbg_config.h"        /* v1.1: FW-owned runtime debug config */
/*==========================================================================*/
/*      D E F I N E S  -  E N U M E R A T I O N S  -  T Y P E D E F S       */
/*==========================================================================*/

/* v1.1: FW-owned master debug/trace switch (this FW's choice; CORE honors it).
   1 = debug on (parity with the current bring-up); 0 = ship this FW silent. */
#ifndef FW_TRACE_ENABLE
#define FW_TRACE_ENABLE 1
#endif

/*==========================================================================*/
/*                  F U N C T I O N   P R O T O T Y P E S                   */
/*==========================================================================*/
static void fw_options_set( void );
/*==========================================================================*/
/*                            V A R I A B L E S                             */
/*==========================================================================*/
static Func_Cfg func_cfg ;
//extern Frd_Car      frd_car                     ;

//extern Detect_Vars     detect_vars  ;
/*==========================================================================*/
/*                 F U N C T I O N   D E F I N I T I O N S                  */
/*==========================================================================*/

/* S6: DS4 vehicle-specific config, ported from the proven DB3 fw_init.c and set to
   the DS4 source-of-truth values (lib.config.ds4). These override the (empty) weak
   apply_vehicle_specific stubs in the contract, and run from fw_identity_init before
   core_common1_init consumes the config. DS4 values == DB3 profile except CAN dev0
   buffers (5, not 8) and the FW identity strings. */

void core_contract_ds4_pts_ctl_config_apply_vehicle_specific(void)
{
    set_config_pts_ctl_version(1U);
    set_pts_ctl_timeout(750U);
    set_pts_ctl_pulse_timeout(4000U);
}

/*--------------------------------------------------------------------------*/

void core_contract_ds4_status_config_apply_vehicle_specific(void)
{
    set_status_vin_enable(1U);
    set_status_rke_enable(1U);
    set_status_key_enable(1U);
    set_status_rpm_enable(1U);
    set_status_door_enable(1U);
    set_status_speed_enable(1U);
    set_status_brake_enable(1U);
    set_status_trans_enable(1U);
    set_status_alarm_enable(1U);
    set_status_engine_enable(1U);
    set_status_comfort_enable(1U);
    set_status_temperature_enable(1U);
    set_status_req_input_enable(1U);
    set_status_dtc_enable(1U);
    set_status_mileage_enable(1U);
    set_status_tpms_enable(1U);
    set_status_doorlock_enable(1U);

    set_status_oil_alarm_enable(1U);
    set_status_tire_alarm_enable(1U);
    set_status_seat_belts_enable(1U);
    set_status_seat_weights_enable(1U);
    set_status_ev_battery_lvl_enable(1U);
    set_status_ev_range_enable(1U);
    set_status_oil_life_enable(1U);
    set_status_fuel_lvl_enable(1U);
    set_status_fuel_cap_enable(1U);
    set_status_cabin_temp_enable(1U);
    set_status_tire_pressure_enable(1U);
    set_status_odometer_enable(1U);
    set_status_battery_voltage_enable(1U);

    set_function_honks_enable(1U);
    set_function_headlights_enable(1U);
    set_function_rib_enable(1U);
}

/*--------------------------------------------------------------------------*/

void core_contract_ds4_prog_config_apply_vehicle_specific(void)
{
    /* Controlled door lock option flags */
    set_prog_ctl_doorlock_brake_enable(1U);
    set_prog_ctl_doorlock_ignition_enable(1U);
    set_prog_ctl_doorlock_speed_enable(1U);

    /* Custom1 available options (contract has opt2..opt5 only; opt1 is implicit) */
    set_prog_custom1_opt2_enable(1U);
    set_prog_custom1_opt3_enable(1U);
    set_prog_custom1_opt4_enable(1U);
    set_prog_custom1_opt5_enable(0U);

    /* Custom2 available options */
    set_prog_custom2_opt2_enable(0U);
    set_prog_custom2_opt3_enable(0U);
    set_prog_custom2_opt4_enable(0U);
    set_prog_custom2_opt5_enable(0U);

    /* Custom3 available options */
    set_prog_custom3_opt2_enable(0U);
    set_prog_custom3_opt3_enable(0U);
    set_prog_custom3_opt4_enable(0U);
    set_prog_custom3_opt5_enable(0U);

    /* Custom4 available options */
    set_prog_custom4_opt2_enable(1U);
    set_prog_custom4_opt3_enable(1U);
    set_prog_custom4_opt4_enable(1U);
    set_prog_custom4_opt5_enable(1U);

    /* Feature gate + selected value */
    set_prog_feat_auto_lock_enable(0U);
    set_prog_feat_auto_lock_value(1U);

    set_prog_feat_auto_unlock_enable(0U);
    set_prog_feat_auto_unlock_value(1U);

    set_prog_feat_aux1_default(8U);
    set_prog_feat_aux1_enable(0U);

    set_prog_feat_aux2_default(7U);
    set_prog_feat_aux2_enable(0U);

    set_prog_feat_aux3_default(4U);
    set_prog_feat_aux3_enable(0U);

    set_prog_feat_aux4_default(7U);
    set_prog_feat_aux4_enable(0U);

    /* Comfort closure */
    set_prog_feat_comfort_closure_enable(0U);
    set_prog_feat_comfort_closure_value(1U);

    /* Controlled door lock */
    set_prog_feat_ctl_doorlock_enable(0U);
    set_prog_feat_ctl_doorlock_value(1U);

    /* Takeover (custom1) */
    set_prog_feat_custom1_default_value(1U);
    set_prog_feat_custom1_enable(0U);

    /* Digital Aux1 (custom2) */
    set_prog_feat_custom2_value(1U);
    set_prog_feat_custom2_enable(0U);

    /* Digital Aux2 (custom3) */
    set_prog_feat_custom3_value(1U);
    set_prog_feat_custom3_enable(0U);

    /* Digital Aux3 (custom4) */
    set_prog_feat_custom4_value(1U);
    set_prog_feat_custom4_enable(0U);

    set_prog_feat_defroster_enable(0U);
    set_prog_feat_defroster_value(1U);

    set_prog_feat_ext_light_supervision_enable(0U);
    set_prog_feat_ext_light_supervision_value(1U);

    set_prog_feat_heated_seats_enable(0U);
    set_prog_feat_heated_seats_value(1U);

    set_prog_feat_horn_confirmation_enable(0U);
    set_prog_feat_horn_confirmation_value(1U);

    set_prog_feat_key_ign_arm_defeat_enable(0U);
    set_prog_feat_key_ign_arm_defeat_value(2U);

    set_prog_feat_light_control_enable(0U);
    set_prog_feat_light_control_value(1U);

    set_prog_feat_oem_security_enable(0U);
    set_prog_feat_oem_security_value(1U);

    set_prog_feat_relock_enable(0U);
    set_prog_feat_relock_value(1U);

    set_prog_feat_rf_output_enable(1U);
    set_prog_feat_rf_output_value(1U);

    set_prog_feat_seats_temp_control_enable(0U);
    set_prog_feat_seats_temp_control_value(1U);

    set_prog_feat_seats_temp_level_enable(0U);
    set_prog_feat_seats_temp_level_value(1U);

    set_prog_feat_secure_lock_enable(0U);
    set_prog_feat_secure_lock_value(3U);

    set_prog_feat_trunk_default(0U);
    set_prog_feat_trunk_enable(0U);

    /* Unlock driver priority */
    set_prog_feat_unlock_driver_enable(0U);
    set_prog_feat_unlock_driver_value(1U);

    /* Heated seats level options */
    set_prog_heated_seats_level_high_enable(0U);
    set_prog_heated_seats_level_low_enable(0U);
    set_prog_heated_seats_level_med_enable(0U);

    /* Trunk/Aux available outputs */
    set_prog_option_gas_cap_enable(0U);
    set_prog_option_head_lights_enable(0U);
    set_prog_option_left_slide_enable(0U);
    set_prog_option_right_slide_enable(0U);
    set_prog_option_sunroof_closure_enable(0U);
    set_prog_option_tail_glass_enable(0U);
    set_prog_option_trunk_enable(0U);
    set_prog_option_windows_closure_enable(0U);
    set_prog_option_windows_vent_enable(0U);

    /* Smart OEM alarm control */
    set_prog_secure_lock_smart_enable(1U);
}

/*--------------------------------------------------------------------------*/

/* DS5 single-boot identity hook. Called from ngmm_task_main (via the descriptor)
   right after fw_ram_init, BEFORE CORE's config consumers (core_common1_init...).
   The FW writes CORE's SINGLE config instance here (via the API-table config
   accessors). */
void fw_identity_init( void )
{
    /* g_*_config live in CORE BSS (zeroed at boot) and are read live; nothing in the DS4
       CORE re-inits them. Call the apply_* functions DIRECTLY (they are defined in this
       file) rather than the contract *_config_init_defaults() — whose .c files are not in
       the thin-FW build, so referencing them is an undefined symbol at link. Fields we
       don't set stay 0 (BSS), which is exactly what init_defaults (memset + apply) would
       give. This seeds CORE's single config instance before any CORE consumer runs, so the
       DS4 pts_ctl/prog/status config actually takes effect in the split. */
    set_fw_name("FRD37EU");
    set_app_version("1.00_04");
    set_fw_info("");

    core_contract_ds4_pts_ctl_config_apply_vehicle_specific();
    core_contract_ds4_prog_config_apply_vehicle_specific();
    core_contract_ds4_status_config_apply_vehicle_specific();

    /* CAN dev0 (FT/SW primary). Values from config_can.h (DS4: buffers 5, not DB3's 8). */
    set_can_dev_0_ft_sw(1);
    set_can_dev_0_speed(6);
    set_can_dev_0_rx_buffer_size(5);
    set_can_dev_0_tx_buffer_size(5);

    /* v1.1: FW owns the runtime debug config; CORE honors it (UART stays in CORE).
       Guarded on the table version so a pre-v1.1 CORE (no dbg_config slot) is untouched. */
    if (core_contract_ds4_boot_api_table_get()->api_table_version >= 12U)
    {
        dbg_config_set_defaults_from_defines();
        set_use_trace(FW_TRACE_ENABLE);
    }
}

/*--------------------------------------------------------------------------*/

void fw_init( void )
{
	TRACE("\r FW INIT!");

	if( detect_car_search() )
    {
        TRACE("\r\n CAR DETECTED \r\n");

		car.bits.can0_detected = TRUE;

        if( car.bits.can0_detected ) func_cfg.can_dev_id = CAN_DEV_0 ;

		func_init();

        prog_set_exit_callback((Prog_Func)fw_options_set);

        fw_options_set();
    }
}

/*--------------------------------------------------------------------------*/

void fw_options_set( void )
{
	func_cfg.comfort = prog_retrieve_option(FEAT_COMFORT_CLOSURE);

	tsk_rs_update_cfg(TSK_RS_TAKEOVER_DISABLE); // ADI - 14.11.2016

    func_cfg.aux1    = prog_retrieve_option(FEAT_CUSTOM2);
    func_cfg.aux2    = prog_retrieve_option(FEAT_CUSTOM3);
    func_cfg.aux3    = prog_retrieve_option(FEAT_CUSTOM4);
}

/*--------------------------------------------------------------------------*/
