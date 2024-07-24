#include "reg.h"
#include "FlashOS.H"
uint8_t m_u32ReadMode = 0U;
uint8_t m_u8FourAddr  = 0U;


extern void WDT_Process(void);
extern void CLK_PLLInit(void);

void QSPI_EnterDirectCommMode(void)
{

    m_u32ReadMode = QSPI_CR & 0x07;
    QSPI_CR &= 0xFFFFFFF8;
    QSPI_CR |= 0x20;
}


void QSPI_ExitDirectCommMode(void)
{
    QSPI_CR &= 0xFFFFFFDF;
    QSPI_CR |= m_u32ReadMode;
}

int32_t QSPI_FLASH_CheckProcessDone(uint32_t u32Timeout)
{
    uint32_t u32Count;
    int32_t  i32Ret = 1;
    uint8_t  u8Status;

    u32Count = 40000 * u32Timeout;
    QSPI_EnterDirectCommMode();
    QSPI_DCOM = 5;
    while (u32Count-- != 0) {
        u8Status = QSPI_DCOM;
        if ((u8Status & 1) == 0) {
            i32Ret = 0;
            break;
        }
    }
    QSPI_ExitDirectCommMode();
    return i32Ret;
}

static void QSPI_FLASH_WordToByte(uint32_t u32Word, uint8_t *pu8Byte)
{
    uint32_t u32ByteNum = m_u8FourAddr ? 3 : 2;
    uint8_t  u8Count    = 0U;
    do {
        pu8Byte[u8Count++] = (uint8_t)(u32Word >> (u32ByteNum * 8U)) & 0xFFU;
    } while ((u32ByteNum--) != 0UL);
}

void QSPI_FLASH_WriteInstr(int u8Instr, uint8_t *pu8Addr, uint32_t u32AddrLen, uint8_t *pu8WriteBuf, uint32_t u32BufLen)
{

    QSPI_EnterDirectCommMode();
    QSPI_DCOM = u8Instr;
    if (pu8Addr && u32AddrLen) {
        for (uint32_t i = 0; i < u32AddrLen; ++i) {
            QSPI_DCOM = pu8Addr[i];
        }
    }
    if (pu8WriteBuf && u32BufLen) {
        for (uint32_t j = 0; j < u32BufLen; ++j) {
            QSPI_DCOM = pu8WriteBuf[j];
        }
    }
    QSPI_ExitDirectCommMode();
}

static void QSPI_FLASH_ReadInstr(uint8_t u8Instr, uint8_t *pu8Addr, uint32_t u32AddrLen, uint8_t *pu8ReadBuf,
                                 uint32_t u32BufLen)
{
    uint32_t i;

    QSPI_EnterDirectCommMode();
    QSPI_DCOM = u8Instr;
    if ((0UL != pu8Addr) && (0UL != u32AddrLen)) {
        for (i = 0UL; i < u32AddrLen; i++) {
            QSPI_DCOM = pu8Addr[i];
        }
    }
    if ((0UL != pu8ReadBuf) && (0UL != u32BufLen)) {
        for (i = 0UL; i < u32BufLen; i++) {
            pu8ReadBuf[i] = QSPI_DCOM;
        }
    }
    QSPI_ExitDirectCommMode();
}

void qspi_set_trans_protocol(uint32_t u32Line)
{
    uint32_t mode;
    if (u32Line == 1) {
        mode = 0;
        goto end;
    }
    if (u32Line == 2) {
        mode = 0x100 | 0x400 | 0x1000;
        goto end;
    }
    if (u32Line != 4) {
        mode = 0;
        goto end;
    }
    mode = 0x200 | 0x800 | 0x2000;
end:
    QSPI_CR &= 0xFFFFC0FF;
    QSPI_CR |= mode;
}


int32_t qspi_write_data(int u8Instr, uint8_t *pu8Addr, uint32_t u32AddrLen, uint8_t *pu8WriteBuf, uint32_t u32BufLen)
{
    m_u32ReadMode = QSPI_CR & 0x07;

    QSPI_CR &= 0xFFFFFFF8;
    QSPI_CR |= 0x07;

    GPIO_PCRB1 &= 0xFFFE;
    GPIO_PFSRB1 = 0;

    qspi_set_trans_protocol(1u);

    QSPI_CR |= 0x20;
    QSPI_DCOM = u8Instr;
    for (uint32_t i = 0; i < u32AddrLen; ++i) {
        QSPI_DCOM = pu8Addr[i];
    }
    QSPI_CR &= 0xFFFFFFDF;

    qspi_set_trans_protocol(4u);

    QSPI_CR |= 0x20;

    DMA_INTCLR1 |= 1;

    DMA_SAR0   = (uint32_t)pu8WriteBuf;
    DMA_DAR0   = (uint32_t)(&QSPI_DCOM);
    DMA_DTCTL  = u32BufLen | 0x10000;
    DMA_CH0CTL = 1;
    DMA_CHEN |= 1;
    AOS_INT_STRG = 1;

    for (uint32_t j = 0;; ++j) {
        if ((DMA_INTSTAT1 & 1) != 0) {
            break;
        }
    }

    QSPI_CR &= 0xFFFFFFDF;
    GPIO_PCRB1 |= 1;
    GPIO_PFSRB1 = 7;
    QSPI_CR |= m_u32ReadMode;

    return 0;
}


int FlashEraseChip(void)
{
    QSPI_FLASH_WriteInstr(0x06, 0, 0, 0, 0);
    QSPI_FLASH_WriteInstr(0xC7, 0, 0, 0, 0);
    return QSPI_FLASH_CheckProcessDone(200000u);
}

int FlashEraseSector(unsigned long adr)
{
    adr += 0x68000000; // from 0x98000000 to 0x00000000
    uint8_t  u8AddrBuf[4U];
    uint32_t len = m_u8FourAddr ? 4 : 3;
    for (uint32_t i = 0; i < MUTI_ERASE;i++){
    QSPI_FLASH_WordToByte(adr, u8AddrBuf);
    adr += SECTOR_SIZE;
    QSPI_FLASH_WriteInstr(6u, 0, 0, 0, 0);
    QSPI_FLASH_WriteInstr(0xD8u, u8AddrBuf, len, 0u, 0u);
    if (QSPI_FLASH_CheckProcessDone(20000U)) {
        WDT_Process();
        return 1;
    }
  }
    return 0;
}

int FlashWrite(uint32_t adr, uint32_t sz, const uint8_t *buf)
{
    adr += 0x68000000; // from 0x98000000 to 0x00000000

    uint32_t u32BufLen;
    uint32_t u32ByteNum    = m_u8FourAddr ? 4 : 3;
    int32_t  u32AddrOffset = 0;
    uint8_t  u8AddrBuf[4U];
    int32_t  i32Ret = 0;

    WDT_Process();

    while (sz != 0UL) {
        if (sz >= 0xFF) {
            u32BufLen = 0x100;
        } else {
            u32BufLen = sz;
        }
        QSPI_FLASH_WriteInstr(0x06, 0U, 0U, 0U, 0U);
        QSPI_FLASH_WordToByte(adr, u8AddrBuf);
//        QSPI_FLASH_WriteInstr(0X02, u8AddrBuf, u32ByteNum, (uint8_t *)&buf[u32AddrOffset], u32BufLen);
        qspi_write_data(0X32, u8AddrBuf, u32ByteNum, (uint8_t *)&buf[u32AddrOffset], u32BufLen);
        if (QSPI_FLASH_CheckProcessDone(5000U)) {
            WDT_Process();
            i32Ret = 1;
            break;
        }
        adr += u32BufLen;
        u32AddrOffset += u32BufLen;
        sz -= u32BufLen;
    }

    return i32Ret;
}


int32_t QSPI_FLASH_IsSupport4Addr()
{
    uint8_t u8RdBuf[36];
    uint8_t u8WrBuf[16];

    /* The basic parameter header is mandatory, is defined by this standard, and starts at byte offset 08h. */
    u8WrBuf[0] = 0;
    u8WrBuf[1] = 0;
    u8WrBuf[2] = 8;
    u8WrBuf[3] = 0xFF;
    QSPI_FLASH_ReadInstr(0x5Au, u8WrBuf, 4u, u8RdBuf, 8u);
    if (u8RdBuf[2] > 1u) { // JEDEC 216B fix SFDP Major Revision Number 1
        return -1;
    }
    if (u8RdBuf[3] < 9u) { // JEDEC 216B fix SFDP Major Revision Number 6 ,9 is SFDP 216D
        return -1;
    }

    /* read JEDEC basic flash parameter table */
    u8WrBuf[0] = (unsigned int)(u8RdBuf[4] | (u8RdBuf[5] << 8) | (u8RdBuf[6] << 16)) >> 16;
    u8WrBuf[1] = u8RdBuf[5];
    u8WrBuf[2] = u8RdBuf[4];
    u8WrBuf[3] = 0xFF;
    QSPI_FLASH_ReadInstr(0x5Au, u8WrBuf, 4u, u8RdBuf, 0x24u);
    int ret = (u8RdBuf[2] >> 1) & 3;
    if (ret && (ret == 1 || ret == 2)) {
        return 0;
    } else {
        return -1;
    }
}


void FlashPrepare(int a1, int a2, int a3, int a4)
{
    uint8_t u8AddrBuf[4];

    EFM_FAPRT  = 0xFFFF0123;
    EFM_FAPRT  = 0xFFFF3210;
    PWC_FCG0PC = 0xA5A50001;
    GPIO_PWPR  = 0xA501;
    PWC_FPRC   = 0xA507;
    SRAMC_WTPR = 0x77;
    EFM_FWMC &= 0xFFFFFEFF;
    EFM_FWMC |= 0x100;

    CLK_PLLInit();
    WDT_Process();
    GPIO_PWPR    = 0xA501;
    GPIO_PCCR    = 0xC000;
    GPIO_PCRB1   = 0x23; // 手动拉CS
    GPIO_PCRB2   = 0x20;
    GPIO_PCRB10  = 0x20;
    GPIO_PCRB12  = 0x20;
    GPIO_PCRB13  = 0x20;
    GPIO_PCRB14  = 0x20;
    GPIO_PFSRB1  = 7;
    GPIO_PFSRB2  = 7;
    GPIO_PFSRB10 = 7;
    GPIO_PFSRB12 = 7;
    GPIO_PFSRB13 = 7;
    GPIO_PFSRB14 = 7;

    PWC_FCG1 &= 0xFFFFFFF7;
    QSPI_CR     = 0x2000D; // 四线式输入输出快速读
    QSPI_CSCR   = 0x11;     // CS延迟32CLK
    QSPI_FCR    = 0x8372;   // 设置addr2字节
    GPIO_PCRC7  = 2;
    GPIO_PCRC15 = 2;

    u8AddrBuf[0] = 2;

    QSPI_FLASH_WriteInstr(6u, 0, 0, 0, 0);
    QSPI_FLASH_WriteInstr(0x31u, 0, 0, u8AddrBuf, 1u); // 使能四线模式
    if (QSPI_FLASH_CheckProcessDone(1000u)) {
        WDT_Process();
    }
    if (!QSPI_FLASH_IsSupport4Addr()) {
        QSPI_FCR = 0x8377;                        // 设置addr4字节
        QSPI_FLASH_WriteInstr(0xB7u, 0, 0, 0, 0); // 进入四字节地址模式
        m_u8FourAddr = 1;
    }
    WDT_Process();
    
    PWC_FCG0 &= 0xFFFD7FFF; // enable dma clock
    DMA_EN = 1;
    AOS_DMA2_TRGSEL0 = 319; // 319 is AOS_STRG 
#ifdef USE_CRC_VERIFY
    PWC_FCG0 &= ~0x00800000;// enable CRC
    CRC_CR = 0x02;//CRC32
#endif
}

void FlashCleanup(void)
{
    WDT_Process();
    EFM_FWMC &= 0xFFFFFEFF;
    EFM_FWMC  = 1;
    EFM_FWMC  = 0;
    EFM_FAPRT = 0xFFFFFFFF;
}
