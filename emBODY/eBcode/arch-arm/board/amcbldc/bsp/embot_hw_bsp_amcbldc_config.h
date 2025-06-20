
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_BSP_AMCBLDC_CONFIG_H_
#define _EMBOT_HW_BSP_AMCBLDC_CONFIG_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"

#include "stm32hal.h"


#if   defined(STM32HAL_BOARD_AMCBLDC)

    #define EMBOT_ENABLE_hw_bsp_specialize
    #define EMBOT_ENABLE_hw_gpio
    #define EMBOT_ENABLE_hw_flash
    #define EMBOT_ENABLE_hw_flash_SINGLEBANK
    #define EMBOT_ENABLE_hw_led
    #define EMBOT_ENABLE_hw_can
    
#if defined(EMBOT_bootloader)
    // bootloader does not need them
#else
    #define EMBOT_ENABLE_hw_motor
    #define EMBOT_ENABLE_hw_analog_ish
    #define EMBOT_ENABLE_hw_button
#endif

    // #define EMBOT_ENABLE_hw_pwm 
    // #define EMBOT_ENABLE_hw_hall 
        
#else
    #error this is the bsp config of STM32HAL_BOARD_AMCBLDC ...
#endif


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


