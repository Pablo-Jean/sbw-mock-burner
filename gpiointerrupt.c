/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== gpiointerrupt.c ========
 */
#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/Timer.h>
#include <ti/sysbios/knl/Task.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#include "hGpio.h"

#include "ti/devices/cc13x2_cc26x2/driverlib/gpio.h"

#include "sbw.h"
#include "cjtag.h"

#include "icepick.h"

// GPIOS

#define dioTCK      15
#define dioTMS      21

#define dioTDO      11
#define dioTDI      12

#define dioRESET    29

/**
 * Firmware test
 */

uint8_t frmC400[] = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02, 0x20, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x31, 0x80, 0x06, 0x00,
                     0x3C, 0x40, 0xCC, 0x01, 0xB0, 0x13, 0xFA, 0xC4, 0x5C, 0x43, 0x1D, 0x43, 0xB0, 0x13, 0x9A, 0xC4,
                     0x5C, 0x43, 0x2D, 0x43, 0xB0, 0x13, 0x9A, 0xC4, 0xB0, 0x13, 0x2C, 0xC5, 0x5C, 0x43, 0x2D, 0x43,
                     0xB0, 0x13, 0xB8, 0xC4, 0x5C, 0x43, 0x1D, 0x43, 0xB0, 0x13, 0xCE, 0xC4, 0x91, 0x42, 0x60, 0x01,
                     0x04, 0x00, 0xB2, 0x40, 0x00, 0xA5, 0x60, 0x01, 0x91, 0x42, 0x60, 0x01, 0x04, 0x00, 0x5C, 0x43,
                     0x1D, 0x43, 0xB0, 0x13, 0xE4, 0xC4, 0x5C, 0x43, 0x2D, 0x43, 0xB0, 0x13, 0xE4, 0xC4, 0x00, 0x18,
                     0xF1, 0x40, 0x70, 0x27, 0x00, 0x00, 0x81, 0x93, 0x02, 0x00, 0x03, 0x20, 0x81, 0x93, 0x00, 0x00,
                     0xEE, 0x27, 0x91, 0x83, 0x00, 0x00, 0x81, 0x73, 0x02, 0x00, 0x81, 0x93, 0x02, 0x00, 0xF9, 0x23,
                     0x81, 0x93, 0x00, 0x00, 0xE4, 0x27, 0xF5, 0x3F, 0x03, 0x43, 0x4F, 0x4C, 0x5F, 0x02, 0x1F, 0x4F,
                     0x00, 0xC4, 0x5C, 0xB3, 0x02, 0x20, 0x4D, 0x4D, 0x8D, 0x10, 0x8F, 0xCD, 0x0A, 0x00, 0x8F, 0xCD,
                     0x0C, 0x00, 0x8F, 0xDD, 0x04, 0x00, 0x10, 0x01, 0x4F, 0x4C, 0x5F, 0x02, 0x1F, 0x4F, 0x00, 0xC4,
                     0x5C, 0xB3, 0x02, 0x20, 0x4D, 0x4D, 0x8D, 0x10, 0x8F, 0xDD, 0x02, 0x00, 0x10, 0x01, 0x4F, 0x4C,
                     0x5F, 0x02, 0x1F, 0x4F, 0x00, 0xC4, 0x5C, 0xB3, 0x02, 0x20, 0x4D, 0x4D, 0x8D, 0x10, 0x8F, 0xCD,
                     0x02, 0x00, 0x10, 0x01, 0x4F, 0x4C, 0x5F, 0x02, 0x1F, 0x4F, 0x00, 0xC4, 0x5C, 0xB3, 0x02, 0x20,
                     0x4D, 0x4D, 0x8D, 0x10, 0x8F, 0xED, 0x02, 0x00, 0x10, 0x01, 0x2F, 0x4C, 0x7F, 0xF0, 0x7F, 0x00,
                     0x7F, 0xD0, 0x80, 0x00, 0x4F, 0x4F, 0x3F, 0x50, 0x00, 0x5A, 0x8C, 0x4F, 0x00, 0x00, 0x10, 0x01,
                     0x31, 0x40, 0x00, 0x30, 0xB0, 0x13, 0x38, 0xC5, 0x0C, 0x43, 0xB0, 0x13, 0x1C, 0xC4, 0x1C, 0x43,
                     0xB0, 0x13, 0x32, 0xC5, 0x32, 0xD0, 0x10, 0x00, 0xFD, 0x3F, 0x03, 0x43, 0x92, 0xC3, 0x30, 0x01,
                     0x10, 0x01, 0x03, 0x43, 0xFF, 0x3F, 0x03, 0x43, 0x1C, 0x43, 0x10, 0x01};

uint8_t frmFF80[] = {0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF };

uint8_t frmFFDA[] = {0x24 , 0xC5 , 0x24 , 0xC5 , 0x24 , 0xC5 , 0x24 , 0xC5 , 0x24 , 0xC5 , 0x24 , 0xC5 , 0x24 , 0xC5 , 0x24 , 0xC5,
                     0x24 , 0xC5 , 0x24 , 0xC5 , 0x24 , 0xC5 , 0x24 , 0xC5 , 0x24 , 0xC5 , 0x24 , 0xC5 , 0x24 , 0xC5 , 0x24 , 0xC5,
                     0x24 , 0xC5 , 0x24 , 0xC5 , 0x10 , 0xC4 };

sbw_buf_t buffer;

uint8_t lS;

/*
 *  ======== gpioButtonFxn0 ========
 *  Callback function for the GPIO interrupt on CONFIG_GPIO_BUTTON_0.
 *
 *  Note: GPIO interrupts are cleared prior to invoking callbacks.
 */
void gpioButtonFxn0(uint_least8_t index)
{
    /* Toggle an LED */
    //GPIO_toggle(CONFIG_GPIO_LED_0);
}

/*
 *  ======== gpioButtonFxn1 ========
 *  Callback function for the GPIO interrupt on CONFIG_GPIO_BUTTON_1.
 *  This may not be used for all boards.
 *
 *  Note: GPIO interrupts are cleared prior to invoking callbacks.
 */
void gpioButtonFxn1(uint_least8_t index)
{
    /* Toggle an LED */
   // GPIO_toggle(CONFIG_GPIO_LED_1);
}

void __sbw_set_tdio_dir(uint8_t InO){
    if (InO == 0){
        GPIO_setOutputEnableDio(dioTMS, GPIO_OUTPUT_ENABLE);
    }
    else{
        //hGpio_changeToInput(0, SBWTDIO, hGPIO_INTERRUPT_OFF);
        GPIO_setOutputEnableDio(dioTMS, GPIO_OUTPUT_DISABLE);
    }
}

void __sbw_set_tdio(uint8_t state){
    //hGpio_write(0, SBWTDIO, (hGPIO_PIN_STATE_e)state);
    if (state)
        GPIO_setDio(dioTMS);
    else
        GPIO_clearDio(dioTMS);
}

uint8_t __sbw_read_tdio(){
    return (GPIO_readDio(dioTMS)>0);
}

uint8_t __sbw_read_tdo(){
    return (GPIO_readDio(dioTDO)>0);
}

void __sbw_set_tck(uint8_t state){
    //hGpio_write(0, SBWTCK, (hGPIO_PIN_STATE_e)state);
    if (state)
        GPIO_setDio(dioTCK);
    else
        GPIO_clearDio(dioTCK);
}

void __sbw_set_tdi(uint8_t state){
    //hGpio_write(0, SBWTCK, (hGPIO_PIN_STATE_e)state);
    if (state)
        GPIO_setDio(dioTDI);
    else
        GPIO_clearDio(dioTDI);
}

void __sbw_set_reset(uint8_t state){
    if (state)
        GPIO_setDio(dioRESET);
    else
        GPIO_clearDio(dioRESET);
}

void __delay_us(uint32_t us){
    while (us > 0){
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        asm(" nop");
        us--;
    }
}

void __delay_ms(uint32_t ms){
    Task_sleep(ms*1000/Clock_tickPeriod);
}


sbw_data_link_t data_link = {
    .fxn = {
        .sbwtdioSet = __sbw_set_tdio_dir,
        .sbwtdioW = __sbw_set_tdio,
        .sbwtdioR = __sbw_read_tdio,
        .sbwtckW = __sbw_set_tck,
        .delayUs = __delay_us,
        .delayMs = __delay_ms
    }
};

cjtag_t cjtag = {
     .pinFxn = {
        .tmsSetDir = __sbw_set_tdio_dir,
        .tmsSetIO = __sbw_set_tdio,
        .tmsGet = __sbw_read_tdio,
        .tdoGet = __sbw_read_tdo,
        .tckSetIO = __sbw_set_tck,
        .tdiSetIo = __sbw_set_tdi,
        .resetSetIO = __sbw_set_reset
     }
};

icepick_t icepick = {
     .linkHandle = (void*)&cjtag,
     .fxn = {
         .fxnInit = cjtag_init,
         .fxnDrShift = cjtag_dr_shift,
         .fxnIrShift = cjtag_ir_shift
     }
};

sbw_t SBW;
uint32_t ret;

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    uint8_t instruction = 0;
    uint16_t data;
    uint64_t tdo, tdo1, tdo2;
    word ret;
    Timer_Params params;

    /* Call driver init functions */
    GPIO_init();
    Timer_init();

    /* Configure the LED and button pins */
    //GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    //GPIO_setConfig(CONFIG_GPIO_LED_1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_BUTTON_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

    /* Turn on user LED */
    //GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

    /* Install Button callback */
    GPIO_setCallback(CONFIG_GPIO_BUTTON_0, gpioButtonFxn0);

    /* Enable interrupts */
    GPIO_enableInt(CONFIG_GPIO_BUTTON_0);

    /*
     *  If more than one input pin is available for your device, interrupts
     *  will be enabled on CONFIG_GPIO_BUTTON1.
     */
    if (CONFIG_GPIO_BUTTON_0 != CONFIG_GPIO_BUTTON_1) {
        /* Configure BUTTON1 pin */
        GPIO_setConfig(CONFIG_GPIO_BUTTON_1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

        /* Install Button callback */
        GPIO_setCallback(CONFIG_GPIO_BUTTON_1, gpioButtonFxn1);
        GPIO_enableInt(CONFIG_GPIO_BUTTON_1);
    }

    GPIO_clearDio(dioRESET);
    GPIO_setDio(dioTCK);
    GPIO_setDio(dioTMS);
    /*
    sbw_init(&SBW, &data_link);

    if (sbw_cmd(&SBW, SBW_GET_DEVICE_ID, NULL) == STATUS_FUSEBLOWN){
        sbw_cmd(&SBW, SBW_JTAG_PASSWORD_REMOVE, NULL);
        if (sbw_cmd(&SBW, SBW_GET_DEVICE_ID, NULL) != STATUS_OK){
            while(1);
        }
    }

    sbw_cmd(&SBW, SBW_DISABLE_WDT, NULL);

    sbw_cmd(&SBW, SBW_DISABLE_MPU, NULL);

    sbw_cmd(&SBW, SBW_TEST_RAM_WRITE, NULL);

    buffer.data = (uint16_t*)frmC400;
    buffer.stAddr = 0xC400;
    buffer.len = sizeof(frmC400)/2;
    ret = sbw_cmd(&SBW, SBW_WRITE_WITH_CHECK, &buffer);


    buffer.data = (uint16_t*)frmFF80;
    buffer.stAddr = 0xFF80;
    buffer.len = sizeof(frmFF80)/2;
    ret = sbw_cmd(&SBW, SBW_WRITE_WITH_CHECK, &buffer);


    buffer.data = (uint16_t*)frmFFDA;
    buffer.stAddr = 0xFFDA;
    buffer.len = sizeof(frmFFDA)/2;
    ret = sbw_cmd(&SBW, SBW_WRITE_WITH_CHECK, &buffer);

//    sbw_cmd(&SBW, SBW_JTAG_PASSWORD_WRITE, NULL);

    sbw_cmd(&SBW, SBW_RELEASE_DEVICE, NULL);
    */

    GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_ON);

//    cjtag_init(&cjtag, CJTAG_MODE_4PIN);
//
//    tdo = cjtag_ir_shift(&cjtag, 0x3f, 6, 1);
//    tdo = cjtag_dr_shift(&cjtag, 0x1f, 6, 1);
//    tdo = cjtag_ir_shift(&cjtag, 0x22, 6, 1);
//    tdo = cjtag_ir_shift(&cjtag, 0x5, 6, 1);
//    tdo1 = cjtag_dr_shift(&cjtag, 0x205, 32, 1);
//
//    tdo = cjtag_ir_shift(&cjtag, 0x7, 6, 1);
//    tdo = cjtag_dr_shift(&cjtag, 0x89, 8, 1);
//    tdo = cjtag_dr_shift(&cjtag, 0x0, 8, 1);
//    tdo = cjtag_dr_shift(&cjtag, 0x0, 8, 1);
//
//    tdo  = cjtag_ir_shift(&cjtag, 0x5, 6, 1);
//    tdo1 = cjtag_dr_shift(&cjtag, 0x0, 32, 1);
//
//    tdo  = cjtag_ir_shift(&cjtag, 0x4, 6, 1);
//    tdo2 = cjtag_dr_shift(&cjtag, 0x0, 32, 1);

    icepick_init(&icepick);


    GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF);

    while (1){
        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
        __delay_ms(500);

        GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
        __delay_ms(500);
    }

    return (NULL);
}
