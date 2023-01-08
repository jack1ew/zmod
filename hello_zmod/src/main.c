#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "./zmodlib/Zmod/zmod.h"
#include "./zmodlib/ZmodADC1410/zmodadc1410.h"
#include "./zmodlib/ZmodDAC1411/zmoddac1411.h"
#define TRANSFER_LEN 0x400
// ZMOD ADC parameters
#define ZMOD_ADC_BASE_ADDR  XPAR_AXI_ZMODADC1410_0_S00_AXI_BASEADDR
#define DMA_ADC_BASE_ADDR  XPAR_AXI_DMA_ADC_BASEADDR
#define IIC_BASE_ADDR  XPAR_PS7_I2C_1_BASEADDR
#define FLASH_ADDR_ADC   0x30
#define ZMOD_ADC_IRQ  XPAR_FABRIC_AXI_ZMODADC1410_0_LIRQOUT_INTR
#define DMA_ADC_IRQ  XPAR_FABRIC_AXI_DMA_ADC_S2MM_INTROUT_INTR
//ZMOD DAC parameters
#define ZMOD_DAC_BASE_ADDR  XPAR_AXI_ZMODDAC1411_V1_0_0_BASEADDR
#define DMA_DAC_BASE_ADDR  XPAR_AXI_DMA_DAC_BASEADDR
#define FLASH_ADDR_DAC   0x31
#define DMA_DAC_IRQ  XPAR_FABRIC_AXI_DMA_DAC_MM2S_INTROUT_INTR
#define IIC_BASE_ADDR  XPAR_PS7_I2C_1_BASEADDR
/*
* Simple ADC test, puts the ADC in the test mode (ramp),
* performs an acquisition under specific trigger conditions
* and verifies the acquired data to be consistent with these conditions.
*/
void testZMODADC1410Ramp_Auto()
{
ZMODADC1410 adcZmod(ZMOD_ADC_BASE_ADDR, DMA_ADC_BASE_ADDR, IIC_BASE_ADDR, FLASH_ADDR_ADC,
ZMOD_ADC_IRQ, DMA_ADC_IRQ);
if(adcZmod.autoTestRamp(1, 0, 0, 4, TRANSFER_LEN) == ERR_SUCCESS)
{
xil_printf("Success autotest ADC ramp\r\n");
}
else
{
xil_printf("Error autotest ADC ramp\r\n");
}
}
/*
* Format data contained in the buffer and sends it over UART.
* It displays the acquired value (in mV), raw value (as 14 bits hexadecimal value)
* and time stamp within the buffer (in time units).
* @param padcZmod - pointer to the ZMODADC1410 object
* @param acqBuffer - the buffer containing acquired data
* @param channel - the channel where samples were acquired
* @param gain - the gain for the channel
* @param length - the buffer length to be used
*/
void formatADCDataOverUART(ZMODADC1410 *padcZmod, uint32_t *acqBuffer, uint8_t channel, uint8_t gain, size_t length)
{
char val_formatted[15];
char time_formatted[15];
uint32_t valBuf;
int16_t valCh;
float val;
xil_printf("New acquisition ------------------------\r\n");
xil_printf("Ch1\tRaw\tTime\t\r\n");
for (size_t i = 0; i < length; i++)
{
valBuf = acqBuffer[i];
valCh = padcZmod->signedChannelData(channel, valBuf);
val = padcZmod->getVoltFromSignedRaw(valCh, gain);
padcZmod->formatValue(val_formatted, 1000.0*val, "mV");
if (i < 100)
{
padcZmod->formatValue(time_formatted, i*10, "ns");
}
else
{
padcZmod->formatValue(time_formatted, (float)(i)/100.0, "us");
}
xil_printf("%s\t%X\t%s\r\n", val_formatted, (uint32_t)(valCh&0x3FFF), time_formatted);
}
}
/*
* Simple ADC test, acquires data and sends it over UART.
* @param channel - the channel where samples will be acquired
* @param gain - the gain for the channel
* @param length - the buffer length to be used
*/
void adcDemo(uint8_t channel, uint8_t gain, size_t length) {
ZMODADC1410 adcZmod(ZMOD_ADC_BASE_ADDR, DMA_ADC_BASE_ADDR, IIC_BASE_ADDR, FLASH_ADDR_ADC,
ZMOD_ADC_IRQ, DMA_ADC_IRQ);
uint32_t *acqBuffer;
adcZmod.setGain(channel, gain);
while(1)
{
acqBuffer = adcZmod.allocChannelsBuffer(length);
adcZmod.acquireImmediatePolling(acqBuffer, length);
formatADCDataOverUART(&adcZmod, acqBuffer, channel, gain, length);
adcZmod.freeChannelsBuffer(acqBuffer, length);
sleep(2);
}
}
/*
* Simple DAC test, using simple ramp values populated in the buffer.
* @param offset - the voltage offset for the generated ramp
* @param amplitude - the amplitude for the generated ramp
* @param step - the step between two generated samples
* @param channel - the channel where samples will be generated
* @param frequencyDivider - the output frequency divider
* @param gain - the gain for the channel
*/
void dacRampDemo(float offset, float amplitude, float step, uint8_t channel, uint8_t frequencyDivider, uint8_t gain)
{
ZMODDAC1411 dacZmod(ZMOD_DAC_BASE_ADDR, DMA_DAC_BASE_ADDR, IIC_BASE_ADDR, FLASH_ADDR_DAC, DMA_DAC_IRQ);
uint32_t *buf;
float val;
uint32_t valBuf;
int16_t valRaw;
size_t length = (int)(amplitude/step) << 2;
int i;
if (length > ((1<<14) - 1))
{
// limit the length to maximum buffer size (1<<14 - 1)
length = ((1<<14) - 1);
// adjust step
step = amplitude/(length>>2);
}
buf = dacZmod.allocChannelsBuffer(length);
dacZmod.setOutputSampleFrequencyDivider(frequencyDivider);
dacZmod.setGain(channel, gain);
i = 0;
// ramp up
for(val = -amplitude; val < amplitude; val += step)
{
valRaw = dacZmod.getSignedRawFromVolt(val + offset, gain);
valBuf = dacZmod.arrangeChannelData(channel, valRaw);
buf[i++] = valBuf;
}
// ramp down
for(val = amplitude; val > -amplitude; val -= step)
{
valRaw = dacZmod.getSignedRawFromVolt(val + offset, gain);
valBuf = dacZmod.arrangeChannelData(channel, valRaw);
buf[i++] = valBuf;
}
// send data to DAC and start the instrument
dacZmod.setData(buf, length);
dacZmod.start();
dacZmod.freeChannelsBuffer(buf, length);
}
int main()
{
init_platform();
xil_printf("Hello Eclypse Z7!\n\r");
xil_printf("Testing ADC ZMOD...\n\r");
adcDemo(0, 0, TRANSFER_LEN);
xil_printf("Testing DAC ZMOD...\n\r");
dacRampDemo(2, 3, 0.01, 0, 2, 1);
cleanup_platform();
return 0;
}
