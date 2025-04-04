
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_chip_KSZ8563.h"

// --------------------------------------------------------------------------------------------------------------------
// - test code. i place this section in here because .... we just need the .h file
// --------------------------------------------------------------------------------------------------------------------
  
#if defined(EMBOT_HW_CHIP_KSZ8563_enable_test)    

// it tests the chip and offers an example of use
bool embot::hw::chip::testof_KSZ8563()
{    
    return false;
}


#endif
  
  
// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------    

// we dont actually need stm32hal in here ... just embot::hw gpio and spi
//#if defined(USE_STM32HAL)
//    #include "stm32hal.h"
//#else
//    #warning this implementation is only for stm32hal
//#endif

#include "embot_hw_gpio.h"
#include "embot_hw_spi.h"


using namespace std;

using namespace embot::hw;


// PHY Basic Status Register
union phyBSR
{
	struct Fields
	{
		uint16_t extended_cap		        :1; /* */
		uint16_t jabber_detect		        :1; /* */
		uint16_t link_status		        :1;	/* */
		uint16_t autoneg_cap			    :1; /* */
		uint16_t remote_fault	            :1; /* */
		uint16_t autoneg_complete	        :1; /* */
		uint16_t MF_preample_suppression	:1; /* */
		uint16_t reserved				    :1; /* */
		uint16_t extended_status		    :1; /* */
		uint16_t reserved_2				    :2; /* */
		uint16_t half_duplex_10Mb	        :1;	/* */
		uint16_t full_duplex_10Mb		    :1; /* */
		uint16_t half_duplex_100BASE	    :1; /* */
		uint16_t full_duplex_100BASE	    :1; /* */
		uint16_t T4_100Base				    :1; /* */
	};
    
    uint16_t value {0};
    Fields fields;	
    
    phyBSR() = default;
};

   
struct embot::hw::chip::KSZ8563::Impl
{ 
    static constexpr embot::core::relTime deftimeout {5*embot::core::time1millisec};
    static constexpr size_t maxsizeofdataxchange {8};
    
    bool _initted {false};
    Config _config {};
    
    embot::core::Data _TXdata{};
    embot::core::Data _rxdata{};
    uint8_t _TXbuffer[maxsizeofdataxchange] = {0, 0, 0, 0, 0, 0, 0, 0}; 
    uint8_t _rxbuffer[maxsizeofdataxchange] = {0, 0, 0, 0, 0, 0, 0, 0}; 

    
    Impl() = default;    
    ~Impl() { deinit(); }
    
    bool init(const Config &cfg);   
    bool deinit();
    bool read(PHY phy, Link &link, embot::core::relTime timeout);
    //bool read0(PHY phy, Link &link, embot::core::relTime timeout);
    
    bool readCRC(PORT port, MIB mib, MIBdata &data, embot::core::relTime timeout);
    uint32_t make_address_CRC(PORT port, REG_MIB MIB_register, COMMAND cmd);
//    uint32_t set_address(COMMAND cmd, uint16_t address);
    
    void initpincontrol();
    void chipselect(bool enable);
    
    
    
    
    
    
    
private:

    static constexpr uint16_t reverse(uint16_t v) { return (v>>8) | ((v&0x00ff)<<8); } // return __REV16(adr);
    static constexpr uint32_t reverse(uint32_t v) { return ((v&0x000000ff)<<24) | ((v&0x0000ff00)<<8) | ((v&0x00ff0000)>>8) | ((v&0xff000000)>>24); } // return __REV(adr);
};    


bool embot::hw::chip::KSZ8563::Impl::init(const Config &cfg)
{    
    if((true == _initted) && (false == cfg.isvalid()))
    {
        return false;
    }
    
    _config = cfg;
    
    _TXdata.load(_TXbuffer, 0);
    _rxdata.load(_rxbuffer, 0);
    
    initpincontrol();

    if(resOK == embot::hw::spi::init(_config.spi, _config.spicfg))
    {
        _initted =  true;
    }
    
    return _initted; 
}

bool embot::hw::chip::KSZ8563::Impl::deinit()
{  
    if(_initted)
    {
        embot::hw::spi::deinit(_config.spi);
        _initted = false;
    }
    
    return !_initted; 
}


void embot::hw::chip::KSZ8563::Impl::chipselect(bool enable)
{
    embot::hw::gpio::set(_config.pincontrol.nS, enable ? embot::hw::gpio::State::RESET : embot::hw::gpio::State::SET);
}

void embot::hw::chip::KSZ8563::Impl::initpincontrol()
{
    if(true == _config.pincontrol.config.isvalid())
    {
        // marco.accame: cube-mx sets the value of the pin before initialization,
        // so in here we do the same. NOTE that embot::hw::gpio::set() allows that.
        embot::hw::gpio::set(_config.pincontrol.nS, embot::hw::gpio::State::SET);
        
        embot::hw::gpio::init(_config.pincontrol.nS, _config.pincontrol.config);    
    }    
}


// it is ok
bool embot::hw::chip::KSZ8563::Impl::read(PHY phy, Link &link, embot::core::relTime timeout)
{       
    static constexpr uint32_t linkregs[numberofPHYs] = 
    {
        reverse(static_cast<uint32_t>(0x60022040)), 
        reverse(static_cast<uint32_t>(0x60042040))
    };
    
    constexpr size_t BSRtxrxsize {6}; 
    constexpr size_t transactionlength {6};
    
#if 0
    the transaction over spi is 4+2 bytes long.
    we transmit the 4 bytes of the wanted link register and after that we read the 2 bytes of reply
    we can use embot::hw::spi::writeread() with length 6 bytes
    TX: V0 V1 V2 V3 00 00
    RX: xx xx xx xx R1 R2
    where ... 
    V0 V1 V2 V3 is either 40 20 02 60 for phy1 or 40 20 04 60 for phy2
    0xR1R2 is the reply ...
#endif    

    // i fill V0 V1 V2 V3 depending on PHY1 or PHI2
    std::memmove(_TXbuffer, &linkregs[(phy == PHY::one) ? 0 : 1], sizeof(uint32_t));
    _TXdata.load(_TXbuffer, transactionlength);
    std::memset(_rxbuffer, 0, transactionlength);
    _rxdata.load(_rxbuffer, transactionlength);
    
    chipselect(true);       
    result_t r = embot::hw::spi::writeread(_config.spi, _TXdata, _rxdata, timeout);   
    chipselect(false);

    // i get R1 R2, i revert and i move it inside phyBSR, 
    // or i just check bit 2. see DS00002418D-page 136 
    uint16_t rx16reverted = static_cast<uint16_t>(_rxbuffer[5]) | (static_cast<uint16_t>(_rxbuffer[4]) << 8);
//    phyBSR bsr { rx16reverted };
//    link = (1 == bsr.fields.link_status) ? Link::UP : Link::DOWN;
    link = (true == embot::core::binary::bit::check(rx16reverted, 2)) ? Link::UP : Link::DOWN;
    return (embot::hw::resOK == r) ? true : false;
} 


//bool embot::hw::chip::KSZ8563::Impl::read0(PHY phy, Link &link, embot::core::relTime timeout)
//{   
//        static constexpr uint32_t linkregs[numberofPHYs] = 
//    {
//        reverse(static_cast<uint32_t>(0x60022040)), 
//        reverse(static_cast<uint32_t>(0x60042040))
//    };   
//    
//    // we transmit 4 bytes and after them we have 2 bytes.
//    // so i need txbuffer and rx buffer both of 6 bytes for calling embot::hw::spi::writeread()
//    uint8_t tx[6] = {0};
//    uint16_t rx[3] {0};
//    std::memmove(tx, &linkregs[(phy == PHY::one) ? 0 : 1], sizeof(uint32_t));
//    embot::core::Data frankzappa {&tx, sizeof(tx)};    
//    embot::core::Data hotrats {&rx, sizeof(rx)};
//    
//    chipselect(true);       
//    embot::hw::spi::writeread(_config.spi, frankzappa, hotrats, 5*1000);   
//    chipselect(false);

//    phyBSR bsr {reverse(rx[2])};
//    link = (1 == bsr.fields.link_status) ? Link::UP : Link::DOWN;
//    
//    return true;
//} 
    

bool embot::hw::chip::KSZ8563::Impl::readCRC(PORT port, MIB mib, MIBdata &data, embot::core::relTime timeout)
{   
    constexpr MIBdata mibdata    {MIBdata::Size::bits30, false, 7, 0};
    constexpr MIBdata mibdataok  {MIBdata::Size::bits30, false, 0, 0};
    data = mibdataok; 
    constexpr uint32_t MIB_CSR_COUNTER_VALID = 1<<25;

   
    uint32_t address = make_address_CRC(port, REG_MIB::REG_MIB_CSR, COMMAND::WRITE);
    
    std::memmove(_TXbuffer, &address, sizeof(uint32_t));
    //chip want MSB first
    uint32_t reversed_data = reverse((uint32_t)MIB_data_to_send);
    std::memmove(_TXbuffer+4, &reversed_data, sizeof(uint32_t));
    _TXdata.load(_TXbuffer, 8);


    chipselect(true);       
    result_t r = embot::hw::spi::write(_config.spi, _TXdata, timeout);   
    chipselect(false);
    #warning control on result SPI?
    
    
    

    
    //    address = make_address_CRC(port, REG_MIB::REG_MIB_DR, COMMAND::WRITE);

    
    
//    address = MIB_data_to_send;
    
    
#if 0
    we need three stages:
    
    1. write to `Port MIB Control and Status Register`:
       - bit 25 = 1,
       - bits [23; 16] w/ the mib index
    2. read back the `Port MIB Control and Status Register` until its bit 25 is 0
    3. read the `Port MIB Data Register` whuihc will contain the value32
       other values are in the previous `Port MIB Control and Status Register`.
    
    we need to use out timeout budget for all these oeprations. i cannot use a forever loop
    in step2 w/out a timeout....
    
    this part is yet to be done.    
    
    
#endif

    
    return true;
}


uint32_t embot::hw::chip::KSZ8563::Impl::make_address_CRC(PORT port, REG_MIB MIB_register, COMMAND cmd)
{
    
//    #define KSZ8563_REGADR(PORT_SPACE, FUNC_SPACE, REG_ADDR)	((PORT_SPACE << PORT_SPACE_POS) | (FUNC_SPACE << FUNC_SPACE_POS) | (REG_ADDR << REG_SPACE_POS))
    /* Those fields describe how the address is composed */

    constexpr uint8_t PORT_SPACE_POS = 12;
    constexpr uint8_t FUNC_SPACE_POS = 8;
    constexpr uint8_t REG_SPACE_POS  = 0;
    constexpr uint8_t PORT_FUNC_MIB_COUNTERS = 0x05;				/* MIB Counters (Management Information Base) */
    
    /* Command pos */
    constexpr uint8_t COMMAND_POS = 29;
    /* Address Pos */
    constexpr uint8_t ADDRESS_POS = 5;
    
    //port +1 bcs of chip driver
    #warning 1 here? better in other way?
    uint32_t adr_32 =  ((embot::core::tointegral(port)+1) << PORT_SPACE_POS) | PORT_FUNC_MIB_COUNTERS << FUNC_SPACE_POS | (static_cast<uint8_t>(MIB_register) << REG_SPACE_POS);
    
    
    adr_32 = (static_cast<uint8_t>(cmd) << COMMAND_POS) | (adr_32 << ADDRESS_POS);
     //chip want MSB first, so i reverse the address
    uint32_t _adr_32 = reverse((uint32_t)adr_32);
    
    return _adr_32;
}





	



// --------------------------------------------------------------------------------------------------------------------
// - interface to the pimpl
// --------------------------------------------------------------------------------------------------------------------

    
embot::hw::chip::KSZ8563::KSZ8563()
: pImpl(new Impl)
{ 

}

embot::hw::chip::KSZ8563::~KSZ8563()
{   
    delete pImpl;
}

bool embot::hw::chip::KSZ8563::init(const Config &config)
{
    return pImpl->init(config);
}

bool embot::hw::chip::KSZ8563::deinit()
{
    return pImpl->deinit();
}

bool embot::hw::chip::KSZ8563::read(PHY phy, Link &link, embot::core::relTime timeout)
{
    return pImpl->read(phy, link, timeout);
}

bool embot::hw::chip::KSZ8563::readCRC(PORT port, MIB mib, MIBdata &data, embot::core::relTime timeout)
{
    return pImpl->readCRC(port, mib, data, timeout);
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

