#include "reg.h"


void DDL_DelayUS(uint32_t u32Count)
{
    volatile uint32_t i;

    while (u32Count-- != 0) {
        i = 20;
        while (i--)
            ;
    }
};


void CLK_PLLInit(void)
{
    SYSREG_CMU_HRCCR = 0; // 使能内部HRC
    while ((SYSREG_CMU_OSCSTBSR & 1) == 0) {
    }; // 等待内部HRC稳定
    DDL_DelayUS(30u);

    SYSREG_CMU_SCFGR = 0x116210; // PCLK0=SYSCLK,PCLK1=CLK/2,PCLK2=CLK/4,PCLK3=CLK/64,PCLK4=CLK,ExMC=LK/2,HCLK=CLK

    DDL_DelayUS(30u);

    if (ICG1 != 0) {
        SYSREG_CMU_PLLCFGR = 0x11103181; // MPLLP=MPLLQ=MPLLR=MPLL/2,SRC=HRC=20,PLLN=50 MPLLM=SRC/2,SYSCLK=200M
    } else {
        SYSREG_CMU_PLLCFGR = 0x11101380; // MPLLP=MPLLQ=MPLLR=MPLL/2,SRC=HRC=20,PLLN=20 MPLLM=SRC/1,SYSCLK=200M
    }

    SYSREG_CMU_PLLCR = 0; // 使能PLL倍频
    while ((SYSREG_CMU_OSCSTBSR & 0x20) == 0) {
    }; // 等待MPLL稳定

    DDL_DelayUS(30u);

    SRAMC_WTCR        = 0x11001111; // 设置SRAM读写周期
    GPIO_PCCR         = 0xC000;     // 设置IO读取等待周期3clk
    EFM_FRMC          = 0x50;       // 设置flash读周期5clk
    SYSREG_PWR_PWRC2  = 0xFF;       // 设置高速模式电源
    SYSREG_PWR_MDSWCR = 0x10;       // 使能高速模式电源
    DDL_DelayUS(30u);
    SYSREG_CMU_CKSWR = 0x05; // 选择MPLL作为SYSCLK
    DDL_DelayUS(30u);
};

void SWDT_Feed()
{
    SWDT_RR = 0xFFFF0123;
    SWDT_RR = 0xFFFF3210;
}

void swdt_judge(void)
{
    uint16_t swdt_sr;
    uint32_t swdt_reri;
    uint32_t swdt_wdpt;
    uint32_t cnt;

    swdt_sr = (uint16_t)SWDT_SR;

    swdt_reri = (ICG0 >> 2) & 0x03;

    swdt_wdpt = (ICG0 >> 8) & 0xF;

    if (swdt_reri) {
        if (swdt_reri == 1) {
            cnt = swdt_sr >> 10;
        } else if (swdt_reri == 2) {
            cnt = swdt_sr >> 12;
        } else {
            cnt = swdt_sr >> 14;
        }
    } else {
        cnt = swdt_sr >> 6;
    }

    if (!swdt_wdpt || swdt_wdpt == 0X0F) {
        goto feed;
    }

    if (((ICG0 >> 8) & 1) != 0) {
        if (cnt) {
            return;
        }
        goto feed;
    }
    if (((ICG0 >> 8) & 2) == 2) {
        if (cnt != 1) {
            return;
        }
        goto feed;
    }
    if (((ICG0 >> 8) & 4) == 4) {
        if (cnt != 2) {
            return;
        }
        goto feed;
    }
    if (((ICG0 >> 8) & 8) == 8 && cnt == 3) {
    feed:
        SWDT_Feed();
    }
}

void WDT_Feed()
{
    WDT_RR = 0xFFFF0123;
    WDT_RR = 0xFFFF3210;
}

void wdt_judge(void)
{
    uint16_t wdt_sr;
    uint32_t wdt_reri;
    uint32_t wdt_wdpt;
    uint32_t cnt;

    wdt_sr = (uint16_t)WDT_SR;

    wdt_reri = (ICG0 >> 16 >> 2) & 0x03;

    wdt_wdpt = (ICG0 >> 16 >> 8) & 0xF;

    if (wdt_reri) {
        if (wdt_reri == 1) {
            cnt = wdt_sr >> 10;
        } else if (wdt_reri == 2) {
            cnt = wdt_sr >> 12;
        } else {
            cnt = wdt_sr >> 14;
        }
    } else {
        cnt = wdt_sr >> 6;
    }

    if (!wdt_wdpt || wdt_wdpt == 0X0F) {
        goto feed;
    }

    if (((ICG0 >> 8) & 1) != 0) {
        if (cnt) {
            return;
        }
        goto feed;
    }
    if (((ICG0 >> 8) & 2) == 2) {
        if (cnt != 1) {
            return;
        }
        goto feed;
    }
    if (((ICG0 >> 8) & 4) == 4) {
        if (cnt != 2) {
            return;
        }
        goto feed;
    }
    if (((ICG0 >> 8) & 8) == 8 && cnt == 3) {
    feed:
        WDT_Feed();
    }
}

void WDT_Process(void)
{
    if ((ICG0 & 0x10000) == 0) {
        wdt_judge();
    }
    if ((ICG0 & 1) == 0) {
        swdt_judge();
    }
};
