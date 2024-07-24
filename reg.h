#ifndef __REG_H__
#define __REG_H__

#include <stdint.h>

#define REG32(x)            (*((volatile uint32_t *)(x)))
#define REG16(x)            (*((volatile uint16_t *)(x)))
#define REG8(x)             (*((volatile uint8_t *)(x)))

#define QSPI_CR             REG32(0X9C000000)
#define QSPI_CSCR           REG32(0X9C000004)
#define QSPI_FCR            REG32(0X9C000008)

#define QSPI_DCOM           REG32(0X9C000010)


#define SYSREG_CMU_HRCCR    REG8(0x40054036)
#define SYSREG_CMU_OSCSTBSR REG8(0x4005403C)
#define SYSREG_CMU_SCFGR    REG32(0x40054020)
#define SYSREG_CMU_PLLCFGR  REG32(0x40054100)
#define SYSREG_CMU_PLLCR    REG8(0x4005402A)

#define SRAMC_WTCR          REG32(0x40050800)
#define SRAMC_WTPR          REG32(0x40050804)


#define SYSREG_PWR_FPRC     REG16(0x400543FE)
#define SYSREG_PWR_PWRC2    REG8(0x40054402)
#define SYSREG_PWR_MDSWCR   REG8(0x4005440F)
#define SYSREG_CMU_CKSWR    REG8(0x40054026)

#define ICG0                REG32(0x40010680)
#define ICG1                REG32(0x40010684)

#define WDT_SR              REG32(0x40049004)
#define WDT_RR              REG32(0x40049008)
#define SWDT_SR             REG32(0x40049404)
#define SWDT_RR             REG32(0x40049408)


#define EFM_FAPRT           REG32(0x40010400)
#define EFM_FRMC            REG32(0x40010408)
#define EFM_FWMC            REG32(0x4001040C)

#define PWC_FCG0PC          REG32(0x40048010)
#define PWC_FPRC            REG16(0x400543FE)
#define PWC_FCG0            REG32(0x40048000)
#define PWC_FCG1            REG32(0x40048004)

#define GPIO_PWPR           REG16(0x40053BFC)
#define GPIO_PCCR           REG16(0x40053BF8)
#define GPIO_PCRB1          REG16(0x40053C44)
#define GPIO_PFSRB1         REG16(0x40053C46)
#define GPIO_PCRB2          REG16(0x40053C48)
#define GPIO_PFSRB2         REG16(0x40053C4A)
#define GPIO_PCRB10         REG16(0x40053C68)
#define GPIO_PFSRB10        REG16(0x40053C6A)
#define GPIO_PCRB12         REG16(0x40053C70)
#define GPIO_PFSRB12        REG16(0x40053C72)
#define GPIO_PCRB13         REG16(0x40053C74)
#define GPIO_PFSRB13        REG16(0x40053C76)
#define GPIO_PCRB14         REG16(0x40053C78)
#define GPIO_PFSRB14        REG16(0x40053C7A)
#define GPIO_PCRC7          REG16(0x40053C9C)
#define GPIO_PCRC15         REG16(0x40053CBC)

#define DMA_EN              REG32(0x40053400)
#define DMA_INTSTAT1        REG32(0x40053408)
#define DMA_INTCLR1         REG32(0x40053418)
#define DMA_CHEN            REG32(0x4005341C)
#define DMA_SAR0            REG32(0x40053440)
#define DMA_DAR0            REG32(0x40053444)
#define DMA_DTCTL           REG32(0x40053448)
#define DMA_CH0CTL          REG32(0x4005345C)


#define AOS_DMA2_TRGSEL0    REG32(0x40010824)
#define AOS_INT_STRG        REG32(0x42210000)

#define CRC_CR              REG32(0x40008C00)
#define CRC_RESLT           REG32(0x40008C04)
#define CRC_DAT0            REG32(0x40008C80)
#endif
