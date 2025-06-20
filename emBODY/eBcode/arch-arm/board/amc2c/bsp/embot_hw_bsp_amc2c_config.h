
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_BSP_AMC2C_CONFIG_H_
#define _EMBOT_HW_BSP_AMC2C_CONFIG_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"

#include "stm32hal.h"


#if   defined(STM32HAL_BOARD_AMC2C)

//    #define EMBOT_ENABLE_hw_bsp_redefine_puts
    
    #define EMBOT_ENABLE_hw_sys_emulateRAND
    #define EMBOT_REDEFINE_hw_bsp_DRIVER_uniqueid
    #define EMBOT_ENABLE_hw_bsp_specialize
    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_led
    #define EMBOT_ENABLE_hw_testpoint
    #define EMBOT_ENABLE_hw_button
    
    #define EMBOT_ENABLE_hw_motor
    #define EMBOT_ENABLE_hw_analog_ish
    #define EMBOT_ENABLE_hw_flash
    #define EMBOT_ENABLE_hw_timer
    #define EMBOT_ENABLE_hw_can
    
    #define EMBOT_ENABLE_hw_mtx
    
#if defined(ENABLE_TEST)
    #define EMBOT_ENABLE_hw_mtx
#else

    #define EMBOT_ENABLE_hw_icc_sig
    #define EMBOT_ENABLE_hw_icc_mem
    #define EMBOT_ENABLE_hw_icc_ltr
//    #define EMBOT_ENABLE_hw_icc_printer
#endif

#else
    #error this is the bsp config of STM32HAL_BOARD_AMC2C ...
#endif


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


