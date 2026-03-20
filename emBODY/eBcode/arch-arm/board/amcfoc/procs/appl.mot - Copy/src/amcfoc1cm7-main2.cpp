
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_app_bldc_theApplication.h"

#include "embot_app_board_amcfoc_1cm7_info.h"
#include "embot_app_board_amcfoc_1cm7_mbd.h"



#include "embot_core.h"
#include "embot_hw.h"
#include "embot_hw_sys.h"
#include "embot_hw_led.h"
#include "embot_hw_bsp_config.h"


#include "embot_hw_button.h"
#include "embot_hw_motor_bldc.h"
#include "embot_app_scope.h"
#include "embot_hw_sys.h"
#include "embot_core.h"
#include "embot_app_theLEDmanager.h"
#include "embot_hw_testpoint.h"
#include <array>



#if defined(theMBDmotor_MBD_code_removed)
#include "embot_app_bldc_MBD_interface.h"
#else
#include "embot_app_bldc_MBD_interface.h"
//// mdb components
//#include "AMC_BLDC.h"
#endif

#include "embot_hw_motor_hall.h"
#include "embot_hw_analog.h"


#include "embot_hw_motor_enc.h"



constexpr embot::app::bldc::theApplication::Config cfg 
{
    {embot::app::board::amcfoc::cm7::info::getCodePartition, embot::app::board::amcfoc::cm7::info::getCANgentCORE},
    {}, // systCfg: the default is typically OK 
    {}, // CommCfg: the default is typically OK
    {embot::app::board::amcfoc::cm7::mbd::Startup, embot::app::board::amcfoc::cm7::mbd::OnTick}  // CtrlCfg: the default stack is typically OK
};

static volatile uint64_t s_1mstickcount = 0; // it must be volatile
constexpr uint32_t s_rate1khz = 1000;
static void stm32hal_tick1msecinit()
{
    HAL_SYSTICK_Config(SystemCoreClock/s_rate1khz);
}

static uint32_t stm32hal_tick1msecget()
{
    return s_1mstickcount;
}

embot::core::Time get1microtime1()
{
    return embot::core::time1millisec * stm32hal_tick1msecget();
}
constexpr embot::hw::Config hwCFG {stm32hal_tick1msecinit, get1microtime1};
    
int main(void)
{  
    embot::hw::init(hwCFG);
    embot::hw::motor::bldc::init(embot::hw::MOTOR::one, {});
    embot::hw::motor::bldc::init(embot::hw::MOTOR::two, {});
    while(1)
    {
        embot::core::print("buh");
//        for(uint8_t i=0;i<250; i++){};
        HAL_Delay(100);
        //embot::core::wait(1000*embot::core::time1millisec);
        
        
    }
    
//    embot::app::bldc::theApplication::getInstance().start(cfg); 
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


