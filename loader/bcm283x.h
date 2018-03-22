#ifndef BCM283X_H
#define BCM283X_H

#define PRC_NUM 4

#define SDRAM_ADDR              0x00000000
#define SDRAM_SIZE              0x3f000000
#define IO0_ADDR                0x3f000000
#define IO0_SIZE                0x01000000
#define IO1_ADDR                0x40000000
#define IO1_SIZE                0x01000000

#define IRQNO_NUM               128U

#define IRQNO_CNTPS             0
#define IRQNO_CNTP              1
#define IRQNO_CNTHP             2
#define IRQNO_CNTV              3
#define IRQNO_MBOX0             4
#define IRQNO_MBOX1             5
#define IRQNO_MBOX2             6
#define IRQNO_MBOX3             7
#define IRQNO_GPU               8
#define IRQNO_PMU               9
#define IRQNO_AXIOUTSTAND       10
#define IRQNO_LTIMER            11
#define IRQNO_ARMTIMER          32
#define IRQNO_ARMMBOX           33
#define IRQNO_PENDING1          40
#define IRQNO_PENDING2          41
#define IRQNO_AUX               93
#define IRQNO_I2CSPISLV         107
#define IRQNO_PWA0              109
#define IRQNO_PWA1              110
#define IRQNO_SMI               112
#define IRQNO_GPIO0             113
#define IRQNO_GPIO1             114
#define IRQNO_GPIO2             115
#define IRQNO_GPIO3             116
#define IRQNO_I2C               117
#define IRQNO_SPI               118
#define IRQNO_PCM               119
#define IRQNO_UART              121
#define IRQNO_UNKNOWN           128

#define IRQ_PEND_B              0x3f00b200
#define IRQ_PEND_1              0x3f00b204
#define IRQ_PEND_2              0x3f00b208
#define FIQ_CTRL                0x3f00b20C
#define ENABLE_IRQ_1            0x3f00b210
#define ENABLE_IRQ_2            0x3f00b214
#define ENABLE_IRQ_B            0x3f00b218
#define DISABLE_IRQ_1           0x3f00b21C
#define DISABLE_IRQ_2           0x3f00b220
#define DISABLE_IRQ_B           0x3f00b224

#define GPIO_GPFSEL0            0x3f200000
#define GPIO_GPFSEL1            0x3f200004
#define GPIO_GPFSEL2            0x3f200008
#define GPIO_GPFSEL3            0x3f20000C
#define GPIO_GPFSEL4            0x3f200010
#define GPIO_GPFSEL5            0x3f200014
#define GPIO_GPSET0             0x3f20001C
#define GPIO_GPSET1             0x3f200020
#define GPIO_GPCLR0             0x3f200028
#define GPIO_GPCLR1             0x3f20002C
#define GPIO_GPLEV0             0x3f200034
#define GPIO_GPLEV1             0x3f200038
#define GPIO_GPEDS0             0x3f200040
#define GPIO_GPEDS1             0x3f200044
#define GPIO_GPREN0             0x3f20004C
#define GPIO_GPREN1             0x3f200050
#define GPIO_GPFEN0             0x3f200058
#define GPIO_GPFEN1             0x3f20005C
#define GPIO_GPHEN0             0x3f200064
#define GPIO_GPHEN1             0x3f200068
#define GPIO_GPLEN0             0x3f200070
#define GPIO_GPLEN1             0x3f200074
#define GPIO_GPAREN0            0x3f20007C
#define GPIO_GPAREN1            0x3f200080
#define GPIO_GPAFEN0            0x3f200088
#define GPIO_GPAFEN1            0x3f20008C
#define GPIO_GPPUD              0x3f200094
#define GPIO_GPPUDCLK0          0x3f200098
#define GPIO_GPPUDCLK1          0x3f20009C

#define AUX_IRQ                 0x3f215000 
#define AUX_ENABLES             0x3f215004 
#define MU_IO                   0x3f215040 
#define MU_IER                  0x3f215044 
#define MU_IIR                  0x3f215048 
#define MU_LCR                  0x3f21504c 
#define MU_MCR                  0x3f215050 
#define MU_LSR                  0x3f215054 
#define MU_MSR                  0x3f215058 
#define MU_SCRATCH              0x3f21505c 
#define MU_CNTL                 0x3f215060 
#define MU_STAT                 0x3f215064 
#define MU_BAUD                 0x3f215068 

#define AUX_ENABLES_MU_EN_BIT   (0x1 << 0)
#define AUX_ENABLES_SPI1_EN_BIT (0x1 << 1)
#define AUX_ENABLES_SPI2_EN_BIT (0x1 << 2)
#define MU_IO_DATA_SHIFT        0
#define MU_IO_DATA_MASK         (0xff << 0)
#define MU_IER_EN_TX_INT_BIT    (0x1 << 1)
#define MU_IER_EN_RX_INT_BIT    (0x1 << 0)
#define MU_IIR_FIFO_CLR         (0x3 << 1)
#define MU_LCR_DS_8BIT          (0x3 << 0)
#define MU_LSR_TX_IDLE_BIT      (0x1 << 6)
#define MU_LSR_TX_EMPTY_BIT     (0x1 << 5)
#define MU_LSR_RX_OVR_BIT       (0x1 << 1)
#define MU_LSR_DATA_RDY_BIT     (0x1 << 0)
#define MU_CNTL_TX_EN_BIT       (0x1 << 1)
#define MU_CNTL_RX_EN_BIT       (0x1 << 0)

#define CTRL_REG                0x40000000
#define CORE_TIMER_PRE          0x40000008
#define GPU_INT_ROUTE           0x4000000c
#define PMON_INT_RS             0x40000010
#define PMON_INT_RC             0x40000014
#define CORE_TIMER_LS           0x4000001c
#define CORE_TIMER_MS           0x40000020
#define LOCAL_INT_ROUTE         0x40000024
#define AXI_OUTSTAND_COUNT      0x4000002c
#define AXI_OUTSTAND_IRQ        0x40000030
#define LOCAL_TCS               0x40000034
#define LOCAL_TWF               0x40000038
#define CORE0_TICTL             0x40000040
#define CORE1_TICTL             0x40000044
#define CORE2_TICTL             0x40000048
#define CORE3_TICTL             0x4000004c
#define CORE0_MICTL             0x40000050
#define CORE1_MICTL             0x40000054
#define CORE2_MICTL             0x40000058
#define CORE3_MICTL             0x4000005c
#define CORE0_IRQ_SRC           0x40000060
#define CORE1_IRQ_SRC           0x40000064
#define CORE2_IRQ_SRC           0x40000068
#define CORE3_IRQ_SRC           0x4000006c
#define CORE0_FIQ_SRC           0x40000070
#define CORE1_FIQ_SRC           0x40000074
#define CORE2_FIQ_SRC           0x40000078
#define CORE3_FIQ_SRC           0x4000007c
#define CORE0_MBOX0_WS          0x40000080
#define CORE0_MBOX1_WS          0x40000084
#define CORE0_MBOX2_WS          0x40000088
#define CORE0_MBOX3_WS          0x4000008c
#define CORE1_MBOX0_WS          0x40000090
#define CORE1_MBOX1_WS          0x40000094
#define CORE1_MBOX2_WS          0x40000098
#define CORE1_MBOX3_WS          0x4000009c
#define CORE2_MBOX0_WS          0x400000a0
#define CORE2_MBOX1_WS          0x400000a4
#define CORE2_MBOX2_WS          0x400000a8
#define CORE2_MBOX3_WS          0x400000ac
#define CORE3_MBOX0_WS          0x400000b0
#define CORE3_MBOX1_WS          0x400000b4
#define CORE3_MBOX2_WS          0x400000b8
#define CORE3_MBOX3_WS          0x400000bc
#define CORE0_MBOX0_RC          0x400000c0
#define CORE0_MBOX1_RC          0x400000c4
#define CORE0_MBOX2_RC          0x400000c8
#define CORE0_MBOX3_RC          0x400000cc
#define CORE1_MBOX0_RC          0x400000d0
#define CORE1_MBOX1_RC          0x400000d4
#define CORE1_MBOX2_RC          0x400000d8
#define CORE1_MBOX3_RC          0x400000dc
#define CORE2_MBOX0_RC          0x400000e0
#define CORE2_MBOX1_RC          0x400000e4
#define CORE2_MBOX2_RC          0x400000e8
#define CORE2_MBOX3_RC          0x400000ec
#define CORE3_MBOX0_RC          0x400000f0
#define CORE3_MBOX1_RC          0x400000f4
#define CORE3_MBOX2_RC          0x400000f8
#define CORE3_MBOX3_RC          0x400000fc

#define CORE_TICTL_CNTPS_IRQ_BIT    0x01
#define CORE_TICTL_CNTP_IRQ_BIT     0x02
#define CORE_TICTL_CNTHP_IRQ_BIT    0x04
#define CORE_TICTL_CNTV_IRQ_BIT     0x08
#define CORE_TICTL_CNTPS_FIQ_BIT    0x10
#define CORE_TICTL_CNTP_FIQ_BIT     0x20
#define CORE_TICTL_CNTHP_FIQ_BIT    0x40
#define CORE_TICTL_CNTV_FIQ_BIT     0x80

#define CORE_MICTL_M0_IRQ_BIT   0x01
#define CORE_MICTL_M1_IRQ_BIT   0x02
#define CORE_MICTL_M2_IRQ_BIT   0x04
#define CORE_MICTL_M3_IRQ_BIT   0x08
#define CORE_MICTL_M0_FIQ_BIT   0x10
#define CORE_MICTL_M1_FIQ_BIT   0x20
#define CORE_MICTL_M2_FIQ_BIT   0x40
#define CORE_MICTL_M3_FIQ_BIT   0x80

#define CORE_IRQ_SRC_T0_BIT     0x0001
#define CORE_IRQ_SRC_T1_BIT     0x0002
#define CORE_IRQ_SRC_T2_BIT     0x0004
#define CORE_IRQ_SRC_T3_BIT     0x0008
#define CORE_IRQ_SRC_M0_BIT     0x0010
#define CORE_IRQ_SRC_M1_BIT     0x0020
#define CORE_IRQ_SRC_M2_BIT     0x0040
#define CORE_IRQ_SRC_M3_BIT     0x0080
#define CORE_IRQ_SRC_GPU_BIT    0x0100
#define CORE_IRQ_SRC_PMU_BIT    0x0200

#ifndef MACRO_ONLY

extern int prc_id(void);

#endif  /* MACRO_ONLY */

#endif /* BCM283X_H */

