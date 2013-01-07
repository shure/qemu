#include "reginfo.h"
#include "qemu-common.h"
static void LL_DMA_reginfo_init(void)
{
static RegInfo reginfo[] = {
{ .bitsize = 32, .name = "TX_NXTDESC_PTR", .offset = 0x00, .type="int" },
{ .bitsize = 32, .name = "TX_CURBUF_ADDR", .offset = 0x04, .type="int" },
{ .bitsize = 32, .name = "TX_CURBUF_LENGTH", .offset = 0x08, .type="int" },
{ .bitsize = 32, .name = "TX_CURDESC_PTR", .offset = 0x0C, .type="int" },
{ .bitsize = 32, .name = "TX_TAILDESC_PTR", .offset = 0x10, .type="int" },
{ .bitsize = 32, .name = "TX_CHANNEL_CTRL", .offset = 0x14, .type="int" },
{ .bitsize = 32, .name = "TX_IRQ_REG", .offset = 0x18, .type="int" },
{ .bitsize = 32, .name = "TX_STATUS_REG", .offset = 0x1C, .type="int" },
{ .bitsize = 32, .name = "RX_NXTDESC_PTR", .offset = 0x20, .type="int" },
{ .bitsize = 32, .name = "RX_CURBUF_ADDR", .offset = 0x24, .type="int" },
{ .bitsize = 32, .name = "RX_CURBUF_LENGTH", .offset = 0x28, .type="int" },
{ .bitsize = 32, .name = "RX_CURDESC_PTR", .offset = 0x2C, .type="int" },
{ .bitsize = 32, .name = "RX_TAILDESC_PTR", .offset = 0x30, .type="int" },
{ .bitsize = 32, .name = "RX_CHANNEL_CTRL", .offset = 0x34, .type="int" },
{ .bitsize = 32, .name = "RX_IRQ_REG", .offset = 0x38, .type="int" },
{ .bitsize = 32, .name = "RX_STATUS_REG", .offset = 0x3C, .type="int" },
{ .bitsize = 32, .name = "DMA_CONTROL_REG", .offset = 0x40, .type="int" },
{ .name = 0 }
};
add_reginfo("LL_DMA", reginfo);
}
block_init(LL_DMA_reginfo_init);

static void pl011_reginfo_init(void)
{
static RegInfo reginfo[] = {
{ .bitsize = 16, .name = "UARTDR", .offset = 0x00, .type="int" },
{ .bitsize = 16, .name = "UARTRSR_ECR", .offset = 0x04, .type="int" },
{ .bitsize = 16, .name = "UARTFR", .offset = 0x18, .type="int" },
{ .bitsize = 16, .name = "UARTIBRD", .offset = 0x24, .type="int" },
{ .bitsize = 16, .name = "UARTFBRD", .offset = 0x28, .type="int" },
{ .bitsize = 16, .name = "UARTLCR_H", .offset = 0x2C, .type="int" },
{ .bitsize = 16, .name = "UARTCR", .offset = 0x30, .type="int" },
{ .bitsize = 16, .name = "UARTIFLS", .offset = 0x34, .type="int" },
{ .bitsize = 16, .name = "UARTIMSC", .offset = 0x38, .type="int" },
{ .bitsize = 16, .name = "UARTRIS", .offset = 0x3C, .type="int" },
{ .bitsize = 16, .name = "UARTMIS", .offset = 0x40, .type="int" },
{ .bitsize = 16, .name = "UARTICR", .offset = 0x44, .type="int" },
{ .bitsize = 16, .name = "UARTDMACR", .offset = 0x48, .type="int" },
{ .bitsize = 16, .name = "UARTILPR", .offset = 0x20, .type="int" },
{ .bitsize = 8, .name = "UARTPeriphID0", .offset = 0xFE0, .type="int" },
{ .bitsize = 8, .name = "UARTPeriphID1", .offset = 0xFE4, .type="int" },
{ .bitsize = 8, .name = "UARTPeriphID2", .offset = 0xFE8, .type="int" },
{ .bitsize = 8, .name = "UARTPeriphID3", .offset = 0xFEC, .type="int" },
{ .bitsize = 8, .name = "UARTCellID0", .offset = 0xFF0, .type="int" },
{ .bitsize = 8, .name = "UARTCellID1", .offset = 0xFF4, .type="int" },
{ .bitsize = 8, .name = "UARTCellID2", .offset = 0xFF8, .type="int" },
{ .bitsize = 8, .name = "UARTCellID3", .offset = 0xFFC, .type="int" },
{ .name = 0 }
};
add_reginfo("pl011", reginfo);
}
block_init(pl011_reginfo_init);

static void pl080_reginfo_init(void)
{
static RegInfo reginfo[] = {
{ .bitsize = 32, .name = "DMACIntStatus", .offset = 0x0, .type="int" },
{ .bitsize = 32, .name = "DMACIntTCStatus", .offset = 0x4, .type="int" },
{ .bitsize = 32, .name = "DMACIntTCClear", .offset = 0x8, .type="int" },
{ .bitsize = 32, .name = "DMACIntErrorStatus", .offset = 0xC, .type="int" },
{ .bitsize = 32, .name = "DMACIntErrClr", .offset = 0x10, .type="int" },
{ .bitsize = 32, .name = "DMACRawIntTCStatus", .offset = 0x14, .type="int" },
{ .bitsize = 32, .name = "DMACRawIntErrorStatus", .offset = 0x18, .type="int" },
{ .bitsize = 32, .name = "DMACEnbldChns", .offset = 0x1C, .type="int" },
{ .bitsize = 32, .name = "DMACSoftBReq", .offset = 0x020, .type="int" },
{ .bitsize = 32, .name = "DMACSoftSReq", .offset = 0x024, .type="int" },
{ .bitsize = 32, .name = "DMACSoftLBReq", .offset = 0x028, .type="int" },
{ .bitsize = 32, .name = "DMACSoftLSReq", .offset = 0x02C, .type="int" },
{ .bitsize = 32, .name = "DMACConfiguration", .offset = 0x030, .type="int" },
{ .bitsize = 32, .name = "DMACSync", .offset = 0x034, .type="int" },
{ .bitsize = 32, .name = "DMACC0SrcAddr", .offset = 0x100, .type="int" },
{ .bitsize = 32, .name = "DMACC0DestAddr", .offset = 0x104, .type="int" },
{ .bitsize = 32, .name = "DMACC0LLI", .offset = 0x108, .type="int" },
{ .bitsize = 32, .name = "DMACC0Control", .offset = 0x10C, .type="int" },
{ .bitsize = 32, .name = "DMACC0Configuration", .offset = 0x110, .type="int" },
{ .bitsize = 32, .name = "DMACC1SrcAddr", .offset = 0x120, .type="int" },
{ .bitsize = 32, .name = "DMACC1DestAddr", .offset = 0x124, .type="int" },
{ .bitsize = 32, .name = "DMACC1LLI", .offset = 0x128, .type="int" },
{ .bitsize = 32, .name = "DMACC1Control", .offset = 0x12C, .type="int" },
{ .bitsize = 32, .name = "DMACC1Configuration", .offset = 0x130, .type="int" },
{ .bitsize = 32, .name = "DMACC2SrcAddr", .offset = 0x140, .type="int" },
{ .bitsize = 32, .name = "DMACC2DestAddr", .offset = 0x144, .type="int" },
{ .bitsize = 32, .name = "DMACC2LLI", .offset = 0x148, .type="int" },
{ .bitsize = 32, .name = "DMACC2Control", .offset = 0x14C, .type="int" },
{ .bitsize = 32, .name = "DMACC2Configuration", .offset = 0x150, .type="int" },
{ .bitsize = 32, .name = "DMACC3SrcAddr", .offset = 0x160, .type="int" },
{ .bitsize = 32, .name = "DMACC3DestAddr", .offset = 0x164, .type="int" },
{ .bitsize = 32, .name = "DMACC3LLI", .offset = 0x168, .type="int" },
{ .bitsize = 32, .name = "DMACC3Control", .offset = 0x16C, .type="int" },
{ .bitsize = 32, .name = "DMACC3Configuration", .offset = 0x170, .type="int" },
{ .bitsize = 32, .name = "DMACC4SrcAddr", .offset = 0x180, .type="int" },
{ .bitsize = 32, .name = "DMACC4DestAddr", .offset = 0x184, .type="int" },
{ .bitsize = 32, .name = "DMACC4LLI", .offset = 0x188, .type="int" },
{ .bitsize = 32, .name = "DMACC4Control", .offset = 0x18C, .type="int" },
{ .bitsize = 32, .name = "DMACC4Configuration", .offset = 0x190, .type="int" },
{ .bitsize = 32, .name = "DMACC5SrcAddr", .offset = 0x1A0, .type="int" },
{ .bitsize = 32, .name = "DMACC5DestAddr", .offset = 0x1A4, .type="int" },
{ .bitsize = 32, .name = "DMACC5LLI", .offset = 0x1A8, .type="int" },
{ .bitsize = 32, .name = "DMACC5Control", .offset = 0x1AC, .type="int" },
{ .bitsize = 32, .name = "DMACC5Configuration", .offset = 0x1B0, .type="int" },
{ .bitsize = 32, .name = "DMACC6SrcAddr", .offset = 0x1C0, .type="int" },
{ .bitsize = 32, .name = "DMACC6DestAddr", .offset = 0x1C4, .type="int" },
{ .bitsize = 32, .name = "DMACC6LLI", .offset = 0x1C8, .type="int" },
{ .bitsize = 32, .name = "DMACC6Control", .offset = 0x1CC, .type="int" },
{ .bitsize = 32, .name = "DMACC6Configuration", .offset = 0x1D0, .type="int" },
{ .bitsize = 32, .name = "DMACC7SrcAddr", .offset = 0x1E0, .type="int" },
{ .bitsize = 32, .name = "DMACC7DestAddr", .offset = 0x1E4, .type="int" },
{ .bitsize = 32, .name = "DMACC7LLI", .offset = 0x1E8, .type="int" },
{ .bitsize = 32, .name = "DMACC7Control", .offset = 0x1EC, .type="int" },
{ .bitsize = 32, .name = "DMACC7Configuration", .offset = 0x1F0, .type="int" },
{ .bitsize = 32, .name = "DMACITCR", .offset = 0x500, .type="int" },
{ .bitsize = 32, .name = "DMACITOP1", .offset = 0x504, .type="int" },
{ .bitsize = 32, .name = "DMACITOP2", .offset = 0x508, .type="int" },
{ .bitsize = 32, .name = "DMACITOP3", .offset = 0x50C, .type="int" },
{ .bitsize = 32, .name = "DMACPeriphID0", .offset = 0xFE0, .type="int" },
{ .bitsize = 32, .name = "DMACPeriphID1", .offset = 0xFE4, .type="int" },
{ .bitsize = 32, .name = "DMACPeriphID2", .offset = 0xFE8, .type="int" },
{ .bitsize = 32, .name = "DMACPeriphID3", .offset = 0xFEC, .type="int" },
{ .bitsize = 32, .name = "DMACPCellID0", .offset = 0xFF0, .type="int" },
{ .bitsize = 32, .name = "DMACPCellID1", .offset = 0xFF4, .type="int" },
{ .bitsize = 32, .name = "DMACPCellID2", .offset = 0xFF8, .type="int" },
{ .bitsize = 32, .name = "DMACPCellID3", .offset = 0xFFC, .type="int" },
{ .name = 0 }
};
add_reginfo("pl080", reginfo);
}
block_init(pl080_reginfo_init);

static void FLITFC_reginfo_init(void)
{
static RegInfo reginfo[] = {
{ .bitsize = 32, .name = "FACR", .offset = 0x00, .type="int" },
{ .bitsize = 32, .name = "FKEYR", .offset = 0x04, .type="int" },
{ .bitsize = 32, .name = "FOPTKEYR", .offset = 0x08, .type="int" },
{ .bitsize = 32, .name = "FSR", .offset = 0x0C, .type="int" },
{ .bitsize = 32, .name = "FCR", .offset = 0x10, .type="int" },
{ .bitsize = 32, .name = "FOPTCR", .offset = 0x14, .type="int" },
{ .name = 0 }
};
add_reginfo("FLITFC", reginfo);
}
block_init(FLITFC_reginfo_init);

static void sp805_reginfo_init(void)
{
static RegInfo reginfo[] = {
{ .bitsize = 32, .name = "WdogLoad", .offset = 0x00, .type="int" },
{ .bitsize = 32, .name = "WdogValue", .offset = 0x04, .type="int" },
{ .bitsize = 32, .name = "WdogControl", .offset = 0x08, .type="int" },
{ .bitsize = 32, .name = "WdogIntClr", .offset = 0x0C, .type="int" },
{ .bitsize = 32, .name = "WdogRIS", .offset = 0x10, .type="int" },
{ .bitsize = 32, .name = "WdogMIS", .offset = 0x14, .type="int" },
{ .bitsize = 32, .name = "WdogLock", .offset = 0xC00, .type="int" },
{ .bitsize = 32, .name = "WdogITCR", .offset = 0xF00, .type="int" },
{ .bitsize = 32, .name = "WdogITOP", .offset = 0xF04, .type="int" },
{ .bitsize = 32, .name = "WdogPeriphID0", .offset = 0xFE0, .type="int" },
{ .bitsize = 32, .name = "WdogPeriphID1", .offset = 0xFE4, .type="int" },
{ .bitsize = 32, .name = "WdogPeriphID2", .offset = 0xFE8, .type="int" },
{ .bitsize = 32, .name = "WdogPeriphID3", .offset = 0xFEC, .type="int" },
{ .bitsize = 32, .name = "WdogPCellID0", .offset = 0xFF0, .type="int" },
{ .bitsize = 32, .name = "WdogPCellID1", .offset = 0xFF4, .type="int" },
{ .bitsize = 32, .name = "WdogPCellID2", .offset = 0xFF8, .type="int" },
{ .bitsize = 32, .name = "WdogPCellID3", .offset = 0xFFC, .type="int" },
{ .name = 0 }
};
add_reginfo("sp805", reginfo);
}
block_init(sp805_reginfo_init);

static void ll_temac_reginfo_init(void)
{
static RegInfo reginfo[] = {
{ .bitsize = 32, .name = "RAF0", .offset = 0x000, .type="int" },
{ .bitsize = 32, .name = "TPF0", .offset = 0x004, .type="int" },
{ .bitsize = 32, .name = "IFGP0", .offset = 0x008, .type="int" },
{ .bitsize = 32, .name = "IS0", .offset = 0x00C, .type="int" },
{ .bitsize = 32, .name = "IP0", .offset = 0x010, .type="int" },
{ .bitsize = 32, .name = "IE0", .offset = 0x014, .type="int" },
{ .bitsize = 32, .name = "MSW0", .offset = 0x020, .type="int" },
{ .bitsize = 32, .name = "LSW0", .offset = 0x024, .type="int" },
{ .bitsize = 32, .name = "CTL0", .offset = 0x028, .type="int" },
{ .bitsize = 32, .name = "RDY0", .offset = 0x02C, .type="int" },
{ .bitsize = 32, .name = "RAF1", .offset = 0x040, .type="int" },
{ .bitsize = 32, .name = "TPF1", .offset = 0x044, .type="int" },
{ .bitsize = 32, .name = "IFGP1", .offset = 0x048, .type="int" },
{ .bitsize = 32, .name = "IS1", .offset = 0x04C, .type="int" },
{ .bitsize = 32, .name = "IP1", .offset = 0x050, .type="int" },
{ .bitsize = 32, .name = "IE1", .offset = 0x054, .type="int" },
{ .bitsize = 32, .name = "MSW1", .offset = 0x060, .type="int" },
{ .bitsize = 32, .name = "LSW1", .offset = 0x064, .type="int" },
{ .bitsize = 32, .name = "CTL1", .offset = 0x068, .type="int" },
{ .bitsize = 32, .name = "RDY1", .offset = 0x06C, .type="int" },
{ .name = 0 }
};
add_reginfo("ll_temac", reginfo);
}
block_init(ll_temac_reginfo_init);

static void pl031_reginfo_init(void)
{
static RegInfo reginfo[] = {
{ .bitsize = 32, .name = "RTCDR", .offset = 0x00, .type="int" },
{ .bitsize = 32, .name = "RTCMR", .offset = 0x04, .type="int" },
{ .bitsize = 32, .name = "RTCLR", .offset = 0x08, .type="int" },
{ .bitsize = 32, .name = "RTCCR", .offset = 0x0C, .type="int" },
{ .bitsize = 32, .name = "RTCIMSC", .offset = 0x10, .type="int" },
{ .bitsize = 32, .name = "RTCRIS", .offset = 0x14, .type="int" },
{ .bitsize = 32, .name = "RTCMIS", .offset = 0x18, .type="int" },
{ .bitsize = 32, .name = "RTCICR", .offset = 0x1C, .type="int" },
{ .name = 0 }
};
add_reginfo("pl031", reginfo);
}
block_init(pl031_reginfo_init);

static void pl330_reginfo_init(void)
{
static RegInfo reginfo[] = {
{ .bitsize = 32, .name = "DSR", .offset = 0x000, .type="int" },
{ .bitsize = 32, .name = "DPC", .offset = 0x004, .type="int" },
{ .bitsize = 32, .name = "INTEN", .offset = 0x020, .type="int" },
{ .bitsize = 32, .name = "INT_EVENT_RIS", .offset = 0x024, .type="int" },
{ .bitsize = 32, .name = "INTMIS", .offset = 0x028, .type="int" },
{ .bitsize = 32, .name = "INTCLR", .offset = 0x02C, .type="int" },
{ .bitsize = 32, .name = "FSRD", .offset = 0x030, .type="int" },
{ .bitsize = 32, .name = "FSRC", .offset = 0x034, .type="int" },
{ .bitsize = 32, .name = "FTRD", .offset = 0x038, .type="int" },
{ .bitsize = 32, .name = "FTR0", .offset = 0x040, .type="int" },
{ .bitsize = 32, .name = "FTR1", .offset = 0x044, .type="int" },
{ .bitsize = 32, .name = "FTR2", .offset = 0x048, .type="int" },
{ .bitsize = 32, .name = "FTR3", .offset = 0x04C, .type="int" },
{ .bitsize = 32, .name = "FTR4", .offset = 0x050, .type="int" },
{ .bitsize = 32, .name = "FTR5", .offset = 0x054, .type="int" },
{ .bitsize = 32, .name = "FTR6", .offset = 0x058, .type="int" },
{ .bitsize = 32, .name = "FTR7", .offset = 0x05C, .type="int" },
{ .bitsize = 32, .name = "CSR0", .offset = 0x100, .type="int" },
{ .bitsize = 32, .name = "CPC0", .offset = 0x104, .type="int" },
{ .bitsize = 32, .name = "CSR1", .offset = 0x108, .type="int" },
{ .bitsize = 32, .name = "CPC1", .offset = 0x10C, .type="int" },
{ .bitsize = 32, .name = "CSR2", .offset = 0x110, .type="int" },
{ .bitsize = 32, .name = "CPC2", .offset = 0x114, .type="int" },
{ .bitsize = 32, .name = "CSR3", .offset = 0x118, .type="int" },
{ .bitsize = 32, .name = "CPC3", .offset = 0x11C, .type="int" },
{ .bitsize = 32, .name = "CSR4", .offset = 0x120, .type="int" },
{ .bitsize = 32, .name = "CPC4", .offset = 0x124, .type="int" },
{ .bitsize = 32, .name = "CSR5", .offset = 0x128, .type="int" },
{ .bitsize = 32, .name = "CPC5", .offset = 0x12C, .type="int" },
{ .bitsize = 32, .name = "CSR6", .offset = 0x130, .type="int" },
{ .bitsize = 32, .name = "CPC6", .offset = 0x134, .type="int" },
{ .bitsize = 32, .name = "CSR7", .offset = 0x138, .type="int" },
{ .bitsize = 32, .name = "CPC7", .offset = 0x13C, .type="int" },
{ .bitsize = 32, .name = "SAR0", .offset = 0x400, .type="int" },
{ .bitsize = 32, .name = "DAR0", .offset = 0x404, .type="int" },
{ .bitsize = 32, .name = "CCR0", .offset = 0x408, .type="int" },
{ .bitsize = 32, .name = "LC0_0", .offset = 0x40C, .type="int" },
{ .bitsize = 32, .name = "LC1_0", .offset = 0x410, .type="int" },
{ .bitsize = 32, .name = "SAR1", .offset = 0x420, .type="int" },
{ .bitsize = 32, .name = "DAR1", .offset = 0x424, .type="int" },
{ .bitsize = 32, .name = "CCR1", .offset = 0x428, .type="int" },
{ .bitsize = 32, .name = "LC0_1", .offset = 0x42C, .type="int" },
{ .bitsize = 32, .name = "LC1_1", .offset = 0x430, .type="int" },
{ .bitsize = 32, .name = "SAR2", .offset = 0x440, .type="int" },
{ .bitsize = 32, .name = "DAR2", .offset = 0x444, .type="int" },
{ .bitsize = 32, .name = "CCR2", .offset = 0x448, .type="int" },
{ .bitsize = 32, .name = "LC0_2", .offset = 0x44C, .type="int" },
{ .bitsize = 32, .name = "LC1_2", .offset = 0x450, .type="int" },
{ .bitsize = 32, .name = "SAR3", .offset = 0x460, .type="int" },
{ .bitsize = 32, .name = "DAR3", .offset = 0x464, .type="int" },
{ .bitsize = 32, .name = "CCR3", .offset = 0x468, .type="int" },
{ .bitsize = 32, .name = "LC0_3", .offset = 0x46C, .type="int" },
{ .bitsize = 32, .name = "LC1_3", .offset = 0x470, .type="int" },
{ .bitsize = 32, .name = "SAR4", .offset = 0x480, .type="int" },
{ .bitsize = 32, .name = "DAR4", .offset = 0x484, .type="int" },
{ .bitsize = 32, .name = "CCR4", .offset = 0x488, .type="int" },
{ .bitsize = 32, .name = "LC0_4", .offset = 0x48C, .type="int" },
{ .bitsize = 32, .name = "LC1_4", .offset = 0x490, .type="int" },
{ .bitsize = 32, .name = "SAR5", .offset = 0x4A0, .type="int" },
{ .bitsize = 32, .name = "DAR5", .offset = 0x4A4, .type="int" },
{ .bitsize = 32, .name = "CCR5", .offset = 0x4A8, .type="int" },
{ .bitsize = 32, .name = "LC0_5", .offset = 0x4AC, .type="int" },
{ .bitsize = 32, .name = "LC1_5", .offset = 0x4B0, .type="int" },
{ .bitsize = 32, .name = "SAR6", .offset = 0x4C0, .type="int" },
{ .bitsize = 32, .name = "DAR6", .offset = 0x4C4, .type="int" },
{ .bitsize = 32, .name = "CCR6", .offset = 0x4C8, .type="int" },
{ .bitsize = 32, .name = "LC0_6", .offset = 0x4CC, .type="int" },
{ .bitsize = 32, .name = "LC1_6", .offset = 0x4D0, .type="int" },
{ .bitsize = 32, .name = "SAR7", .offset = 0x4E0, .type="int" },
{ .bitsize = 32, .name = "DAR7", .offset = 0x4E4, .type="int" },
{ .bitsize = 32, .name = "CCR7", .offset = 0x4E8, .type="int" },
{ .bitsize = 32, .name = "LC0_7", .offset = 0x4EC, .type="int" },
{ .bitsize = 32, .name = "LC1_7", .offset = 0x4F0, .type="int" },
{ .bitsize = 32, .name = "DBGSTATUS", .offset = 0xD00, .type="int" },
{ .bitsize = 32, .name = "DBGCMD", .offset = 0xD04, .type="int" },
{ .bitsize = 32, .name = "DBGINST0", .offset = 0xD08, .type="int" },
{ .bitsize = 32, .name = "DBGINST1", .offset = 0xD0C, .type="int" },
{ .bitsize = 32, .name = "CR0", .offset = 0xE00, .type="int" },
{ .bitsize = 32, .name = "CR1", .offset = 0xE04, .type="int" },
{ .bitsize = 32, .name = "CR2", .offset = 0xE08, .type="int" },
{ .bitsize = 32, .name = "CR3", .offset = 0xE0C, .type="int" },
{ .bitsize = 32, .name = "CR4", .offset = 0xE10, .type="int" },
{ .bitsize = 32, .name = "CRD", .offset = 0xE14, .type="int" },
{ .bitsize = 32, .name = "WD", .offset = 0xE80, .type="int" },
{ .bitsize = 32, .name = "periph_id_0", .offset = 0xFE0, .type="int" },
{ .bitsize = 32, .name = "periph_id_1", .offset = 0xFE4, .type="int" },
{ .bitsize = 32, .name = "periph_id_2", .offset = 0xFE8, .type="int" },
{ .bitsize = 32, .name = "periph_id_3", .offset = 0xFEC, .type="int" },
{ .bitsize = 32, .name = "pcell_id_0", .offset = 0xFF0, .type="int" },
{ .bitsize = 32, .name = "pcell_id_1", .offset = 0xFF4, .type="int" },
{ .bitsize = 32, .name = "pcell_id_2", .offset = 0xFF8, .type="int" },
{ .bitsize = 32, .name = "pcell_id_3", .offset = 0xFFC, .type="int" },
{ .name = 0 }
};
add_reginfo("pl330", reginfo);
}
block_init(pl330_reginfo_init);

static void pl350_reginfo_init(void)
{
static RegInfo reginfo[] = {
{ .bitsize = 32, .name = "memc_status", .offset = 0x0, .type="int" },
{ .bitsize = 32, .name = "memif_cfg", .offset = 0x4, .type="int" },
{ .bitsize = 32, .name = "memc_cfg_set", .offset = 0x8, .type="int" },
{ .bitsize = 32, .name = "memc_cfg_clr", .offset = 0xc, .type="int" },
{ .bitsize = 32, .name = "direct_cmd", .offset = 0x10, .type="int" },
{ .bitsize = 32, .name = "set_cycles", .offset = 0x14, .type="int" },
{ .bitsize = 32, .name = "set_opmode", .offset = 0x18, .type="int" },
{ .bitsize = 32, .name = "refresh_period_0", .offset = 0x20, .type="int" },
{ .bitsize = 32, .name = "refresh_period_1", .offset = 0x24, .type="int" },
{ .bitsize = 32, .name = "mem_cycles0_0", .offset = 0x100, .type="int" },
{ .bitsize = 32, .name = "opmode0_0", .offset = 0x104, .type="int" },
{ .bitsize = 32, .name = "mem_cycles0_1", .offset = 0x120, .type="int" },
{ .bitsize = 32, .name = "opmode0_1", .offset = 0x124, .type="int" },
{ .bitsize = 32, .name = "mem_cycles0_2", .offset = 0x140, .type="int" },
{ .bitsize = 32, .name = "opmode0_2", .offset = 0x144, .type="int" },
{ .bitsize = 32, .name = "mem_cycles0_3", .offset = 0x160, .type="int" },
{ .bitsize = 32, .name = "opmode0_3", .offset = 0x164, .type="int" },
{ .bitsize = 32, .name = "mem_cycles1_0", .offset = 0x180, .type="int" },
{ .bitsize = 32, .name = "opmode1_0", .offset = 0x184, .type="int" },
{ .bitsize = 32, .name = "mem_cycles1_1", .offset = 0x1a0, .type="int" },
{ .bitsize = 32, .name = "opmode1_1", .offset = 0x1a4, .type="int" },
{ .bitsize = 32, .name = "mem_cycles1_2", .offset = 0x1c0, .type="int" },
{ .bitsize = 32, .name = "opmode1_2", .offset = 0x1c4, .type="int" },
{ .bitsize = 32, .name = "mem_cycles1_3", .offset = 0x1e0, .type="int" },
{ .bitsize = 32, .name = "opmode1_3", .offset = 0x1e4, .type="int" },
{ .bitsize = 32, .name = "user_status", .offset = 0x200, .type="int" },
{ .bitsize = 32, .name = "user_config", .offset = 0x204, .type="int" },
{ .bitsize = 32, .name = "ecc_status", .offset = 0x300, .type="int" },
{ .bitsize = 32, .name = "ecc_memcfg", .offset = 0x304, .type="int" },
{ .bitsize = 32, .name = "ecc_memcommand1", .offset = 0x308, .type="int" },
{ .bitsize = 32, .name = "ecc_memcommand2", .offset = 0x30c, .type="int" },
{ .bitsize = 32, .name = "ecc_addr0", .offset = 0x310, .type="int" },
{ .bitsize = 32, .name = "ecc_addr1", .offset = 0x314, .type="int" },
{ .bitsize = 32, .name = "ecc_value0", .offset = 0x318, .type="int" },
{ .bitsize = 32, .name = "ecc_value1", .offset = 0x31c, .type="int" },
{ .bitsize = 32, .name = "ecc_value2", .offset = 0x320, .type="int" },
{ .bitsize = 32, .name = "ecc_value3", .offset = 0x324, .type="int" },
{ .bitsize = 32, .name = "ecc_value4", .offset = 0x328, .type="int" },
{ .bitsize = 32, .name = "int_cfg", .offset = 0xe00, .type="int" },
{ .bitsize = 32, .name = "int_inputs", .offset = 0xe04, .type="int" },
{ .bitsize = 32, .name = "int_outputs", .offset = 0xe08, .type="int" },
{ .bitsize = 32, .name = "periph_id_0", .offset = 0xfe0, .type="int" },
{ .bitsize = 32, .name = "periph_id_1", .offset = 0xfe4, .type="int" },
{ .bitsize = 32, .name = "periph_id_2", .offset = 0xfe8, .type="int" },
{ .bitsize = 32, .name = "periph_id_3", .offset = 0xfec, .type="int" },
{ .bitsize = 32, .name = "pcell_id_0", .offset = 0xff0, .type="int" },
{ .bitsize = 32, .name = "pcell_id_1", .offset = 0xff4, .type="int" },
{ .bitsize = 32, .name = "pcell_id_2", .offset = 0xff8, .type="int" },
{ .bitsize = 32, .name = "pcell_id_3", .offset = 0xffc, .type="int" },
{ .name = 0 }
};
add_reginfo("pl350", reginfo);
}
block_init(pl350_reginfo_init);

static void sp804_reginfo_init(void)
{
static RegInfo reginfo[] = {
{ .bitsize = 32, .name = "Timer1Load", .offset = 0x00, .type="int" },
{ .bitsize = 32, .name = "Timer1Value", .offset = 0x04, .type="int" },
{ .bitsize = 32, .name = "Timer1Control", .offset = 0x08, .type="int" },
{ .bitsize = 32, .name = "Timer1IntClr", .offset = 0x0c, .type="int" },
{ .bitsize = 32, .name = "Timer1RIS", .offset = 0x10, .type="int" },
{ .bitsize = 32, .name = "Timer1MIS", .offset = 0x14, .type="int" },
{ .bitsize = 32, .name = "Timer1BGLoad", .offset = 0x18, .type="int" },
{ .bitsize = 32, .name = "Timer2Load", .offset = 0x20, .type="int" },
{ .bitsize = 32, .name = "Timer2Value", .offset = 0x24, .type="int" },
{ .bitsize = 32, .name = "Timer2Control", .offset = 0x28, .type="int" },
{ .bitsize = 32, .name = "Timer2IntClr", .offset = 0x2c, .type="int" },
{ .bitsize = 32, .name = "Timer2RIS", .offset = 0x30, .type="int" },
{ .bitsize = 32, .name = "Timer2MIS", .offset = 0x34, .type="int" },
{ .bitsize = 32, .name = "Timer2BGLoad", .offset = 0x38, .type="int" },
{ .bitsize = 32, .name = "TimerITCR", .offset = 0xF00, .type="int" },
{ .bitsize = 32, .name = "TimerITOP", .offset = 0xF04, .type="int" },
{ .bitsize = 32, .name = "TimerPeriphID0", .offset = 0xFE0, .type="int" },
{ .bitsize = 32, .name = "TimerPeriphID1", .offset = 0xFE4, .type="int" },
{ .bitsize = 32, .name = "TimerPeriphID2", .offset = 0xFE8, .type="int" },
{ .bitsize = 32, .name = "TimerPeriphID3", .offset = 0xFEC, .type="int" },
{ .bitsize = 32, .name = "TimerPCellID0", .offset = 0xFF0, .type="int" },
{ .bitsize = 32, .name = "TimerPCellID1", .offset = 0xFF4, .type="int" },
{ .bitsize = 32, .name = "TimerPCellID2", .offset = 0xFF8, .type="int" },
{ .bitsize = 32, .name = "TimerPCellID3", .offset = 0xFFC, .type="int" },
{ .name = 0 }
};
add_reginfo("sp804", reginfo);
}
block_init(sp804_reginfo_init);

static void pl061_reginfo_init(void)
{
static RegInfo reginfo[] = {
{ .bitsize = 8, .name = "GPIODIR", .offset = 0x400, .type="int" },
{ .bitsize = 8, .name = "GPIOIS", .offset = 0x404, .type="int" },
{ .bitsize = 8, .name = "GPIOIBE", .offset = 0x408, .type="int" },
{ .bitsize = 8, .name = "GPIOIEV", .offset = 0x40C, .type="int" },
{ .bitsize = 8, .name = "GPIOIE", .offset = 0x410, .type="int" },
{ .bitsize = 8, .name = "GPIORIS", .offset = 0x414, .type="int" },
{ .bitsize = 8, .name = "GPIOMIS", .offset = 0x418, .type="int" },
{ .bitsize = 8, .name = "GPIOIC", .offset = 0x41C, .type="int" },
{ .bitsize = 8, .name = "GPIOAFSEL", .offset = 0x420, .type="int" },
{ .bitsize = 8, .name = "GPIOITCR", .offset = 0x600, .type="int" },
{ .bitsize = 8, .name = "GPIOITIP1", .offset = 0x604, .type="int" },
{ .bitsize = 8, .name = "GPIOITIP2", .offset = 0x608, .type="int" },
{ .bitsize = 8, .name = "GPIOITOP1", .offset = 0x60C, .type="int" },
{ .bitsize = 8, .name = "GPIOITOP2", .offset = 0x610, .type="int" },
{ .bitsize = 8, .name = "GPIOITOP3", .offset = 0x614, .type="int" },
{ .bitsize = 8, .name = "GPIOPeriphID0", .offset = 0xFE0, .type="int" },
{ .bitsize = 8, .name = "GPIOPeriphID1", .offset = 0xFE4, .type="int" },
{ .bitsize = 8, .name = "GPIOPeriphID2", .offset = 0xFE8, .type="int" },
{ .bitsize = 8, .name = "GPIOPeriphID3", .offset = 0xFEC, .type="int" },
{ .bitsize = 8, .name = "GPIOPCellID0", .offset = 0xFF0, .type="int" },
{ .bitsize = 8, .name = "GPIOPCellID1", .offset = 0xFF4, .type="int" },
{ .bitsize = 8, .name = "GPIOPCellID2", .offset = 0xFF8, .type="int" },
{ .bitsize = 8, .name = "GPIOPCellID3", .offset = 0xFFC, .type="int" },
{ .name = 0 }
};
add_reginfo("pl061", reginfo);
}
block_init(pl061_reginfo_init);

static void pl022_reginfo_init(void)
{
static RegInfo reginfo[] = {
{ .bitsize = 16, .name = "SSPCR0", .offset = 0x0, .type="int" },
{ .bitsize = 16, .name = "SSPCR1", .offset = 0x04, .type="int" },
{ .bitsize = 16, .name = "SSPDR", .offset = 0x08, .type="int" },
{ .bitsize = 16, .name = "SSPSR", .offset = 0x0C, .type="int" },
{ .bitsize = 16, .name = "SSPCPSR", .offset = 0x10, .type="int" },
{ .bitsize = 16, .name = "SSPIMSC", .offset = 0x14, .type="int" },
{ .bitsize = 16, .name = "SSPRIS", .offset = 0x18, .type="int" },
{ .bitsize = 16, .name = "SSPMIS", .offset = 0x1C, .type="int" },
{ .bitsize = 16, .name = "SSPICR", .offset = 0x20, .type="int" },
{ .bitsize = 16, .name = "SSPDMACR", .offset = 0x24, .type="int" },
{ .bitsize = 16, .name = "SSPTCR", .offset = 0x080, .type="int" },
{ .bitsize = 16, .name = "SSPITIP", .offset = 0x084, .type="int" },
{ .bitsize = 16, .name = "SSPITOP", .offset = 0x088, .type="int" },
{ .bitsize = 16, .name = "SSPTDR", .offset = 0x08C, .type="int" },
{ .bitsize = 16, .name = "SSPPeriphID0", .offset = 0xFE0, .type="int" },
{ .bitsize = 16, .name = "SSPPeriphID1", .offset = 0xFE4, .type="int" },
{ .bitsize = 16, .name = "SSPPeriphID2", .offset = 0xFE8, .type="int" },
{ .bitsize = 16, .name = "SSPPeriphID3", .offset = 0xFEC, .type="int" },
{ .bitsize = 16, .name = "SSPPCellID0", .offset = 0xFF0, .type="int" },
{ .bitsize = 16, .name = "SSPPCellID1", .offset = 0xFF4, .type="int" },
{ .bitsize = 16, .name = "SSPPCellID2", .offset = 0xFF8, .type="int" },
{ .bitsize = 16, .name = "SSPPCellID3", .offset = 0xFFC, .type="int" },
{ .name = 0 }
};
add_reginfo("pl022", reginfo);
}
block_init(pl022_reginfo_init);

static void pl192_reginfo_init(void)
{
static RegInfo reginfo[] = {
{ .bitsize = 32, .name = "VICIRQSTATUS", .offset = 0x000, .type="int" },
{ .bitsize = 32, .name = "VICFIQSTATUS", .offset = 0x004, .type="int" },
{ .bitsize = 32, .name = "VICRAWINTR", .offset = 0x008, .type="int" },
{ .bitsize = 32, .name = "VICINTSELECT", .offset = 0x00C, .type="int" },
{ .bitsize = 32, .name = "VICINTENABLE", .offset = 0x010, .type="int" },
{ .bitsize = 32, .name = "VICINTENCLEAR", .offset = 0x014, .type="int" },
{ .bitsize = 32, .name = "VICSOFTINT", .offset = 0x018, .type="int" },
{ .bitsize = 32, .name = "VICSOFTINTCLEAR", .offset = 0x01C, .type="int" },
{ .bitsize = 32, .name = "VICPROTECTION", .offset = 0x020, .type="int" },
{ .bitsize = 32, .name = "VICSWPRIORITYMASK", .offset = 0x024, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR0", .offset = 0x100, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR1", .offset = 0x104, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR2", .offset = 0x108, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR3", .offset = 0x10C, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR4", .offset = 0x110, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR5", .offset = 0x114, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR6", .offset = 0x118, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR7", .offset = 0x11C, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR8", .offset = 0x120, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR9", .offset = 0x124, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR10", .offset = 0x128, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR11", .offset = 0x12C, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR12", .offset = 0x130, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR13", .offset = 0x134, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR14", .offset = 0x138, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR15", .offset = 0x13C, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR16", .offset = 0x140, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR17", .offset = 0x144, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR18", .offset = 0x148, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR19", .offset = 0x14C, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR20", .offset = 0x150, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR21", .offset = 0x154, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR22", .offset = 0x158, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR23", .offset = 0x15C, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR24", .offset = 0x160, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR25", .offset = 0x164, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR26", .offset = 0x168, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR27", .offset = 0x16C, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR28", .offset = 0x170, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR29", .offset = 0x174, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR30", .offset = 0x178, .type="int" },
{ .bitsize = 32, .name = "VICVECTADDR31", .offset = 0x17C, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY0", .offset = 0x200, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY1", .offset = 0x204, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY2", .offset = 0x208, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY3", .offset = 0x20C, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY4", .offset = 0x210, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY5", .offset = 0x214, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY6", .offset = 0x218, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY7", .offset = 0x21C, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY8", .offset = 0x220, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY9", .offset = 0x224, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY10", .offset = 0x228, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY11", .offset = 0x22C, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY12", .offset = 0x230, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY13", .offset = 0x234, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY14", .offset = 0x238, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY15", .offset = 0x23C, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY16", .offset = 0x240, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY17", .offset = 0x244, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY18", .offset = 0x248, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY19", .offset = 0x24C, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY20", .offset = 0x250, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY21", .offset = 0x254, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY22", .offset = 0x258, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY23", .offset = 0x25C, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY24", .offset = 0x260, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY25", .offset = 0x264, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY26", .offset = 0x268, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY27", .offset = 0x26C, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY28", .offset = 0x270, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY29", .offset = 0x274, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY30", .offset = 0x278, .type="int" },
{ .bitsize = 32, .name = "VICVECTPRIORITY31", .offset = 0x27C, .type="int" },
{ .bitsize = 32, .name = "VICADDRESS", .offset = 0xF00, .type="int" },
{ .name = 0 }
};
add_reginfo("pl192", reginfo);
}
block_init(pl192_reginfo_init);

