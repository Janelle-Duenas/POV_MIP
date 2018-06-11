/*
 * Copyright (c) 2016, Texas Instruments Incorporated
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
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 * * READ ME!
 *
 * This code has been modified from TI adcsinglechannel_CC2650_LAUNCH_TI_CC2650F128
 * although the name has been kept to be the side, for comparison, this code connects to the
 * PCB created by the Coordinated Robotics Laboratory found at the link https://github.com/Janelle-Duenas/POV_MIP
 * The current code will display a PacMan
 *
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 */


/*
 *  ======== adcsinglechannel.c ========
 */
/* XDCtools Header files*/
#include <stdio.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>

//uses this for usleep
//#include <unistd.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* Driver Header files */
#include <ti/drivers/ADC.h>
#include <ti/drivers/PIN.h>
#include <time.h>

#include <ti/drivers/pin/PINCC26XX.h>///janelle added this
#if defined(CC2650DK_7ID) || defined(CC1310DK_7XD)
#include <ti/drivers/PIN.h>

#include <ti/drivers/pin/PINCC26XX.h>///janelle added this
#endif

volatile uint32_t i,j;

/*
 *  ======== Pin set-up ========
 */

PIN_Config ledPinTable[] = {

    IOID_0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,        //OE
    IOID_1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,        //MOSI(DIN)
    IOID_3 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,        //CLK
    IOID_4 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,        //LE


   // IOID_7 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,        //IN1
   // IOID_8 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,        //EN1
    IOID_9 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,        //IN2
    IOID_10 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,       //EN2
    IOID_11 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,        //IN3
    IOID_12 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,        //EN3
    IOID_14 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,        //nsleep
    ///IOID_7 | PIN_INPUT_EN | PIN_PULLDOWN,  //analog pin
    ///IOID_8 | PIN_INPUT_EN | PIN_PULLDOWN,  //analog pin

    PIN_TERMINATE

};


static PIN_Handle ledPinHandle;
static PIN_State ledPinState;




/* Example/Board Header files */
#include "Board.h"

/* ADC sample count */
#define ADC_SAMPLE_COUNT  (300)

/*Task Specific defines */
#define TASKSTACKSIZE     (768)

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];

Task_Struct task1Struct;
Char task1Stack[TASKSTACKSIZE];

/* ADC conversion result variables */
uint16_t adcValue0;
uint16_t adcValue_current;
uint16_t adcValue1[ADC_SAMPLE_COUNT];
//double adcValue1[ADC_SAMPLE_COUNT];
double Time[ADC_SAMPLE_COUNT];
    int r=3;
/*
 *  ======== taskFxn0 ========
 *  Open an ADC instance and get a sampling result from a one-shot conversion.
 */
Void taskFxn0(UArg arg0, UArg arg1)
{
    ADC_Handle   adc;
    ADC_Params   params;
    int_fast16_t res;

    ADC_Params_init(&params);
    adc = ADC_open(Board_ADC0, &params);

    if (adc == NULL) {
        System_abort("Error initializing ADC channel 0\n");
    }
    else {
       // System_printf("ADC channel 0 initialized\n");
    }

    /* Blocking mode conversion */
    res = ADC_convert(adc, &adcValue0);

    if (res == ADC_STATUS_SUCCESS) {
       System_printf("ADC channel 0 convert result: 0x%x\n", adcValue0);
    }
    else {
        System_printf("ADC channel 0 convert failed\n");
    }

    ADC_close(adc);


    System_flush();
}

/*
 *  ======== taskFxn1 ========
 *  Open a ADC handle and get a array of sampling results after
 *  calling several conversions.
 */
Void taskFxn1(UArg arg0, UArg arg1)
{
    uint16_t     i=0;
    ADC_Handle   adc;
    ADC_Params   params;
    int_fast16_t res;

    ADC_Params_init(&params);
    adc = ADC_open(Board_ADC0, &params);

    if (adc == NULL) {
        System_abort("Error initializing ADC channel 1\n");
    }
    else {
      //  System_printf("ADC channel 1 initialized\n");
    }
////////////////////////////////////////////////////////////////////////////
   /*for (i = 0; i < ADC_SAMPLE_COUNT; i++) {
     //while(true){
        //res = ADC_convert(adc, &adcValue1[i]);
       res = ADC_convert(adc, &adcValue_current);

       if(1<ADC_SAMPLE_COUNT){
       adcValue1[i]=adcValue_current;
       time_t x=time(0);
        Time[i]=x;
       }
        //adcValue1[i]=hextoint(adcValue1[i]);

        if (res == ADC_STATUS_SUCCESS) {

        //  System_printf("ADC channel 1 convert result (%d): %d\n", i,//0x%x
           // adcValue1[i]);
        }
        else {
           // System_printf("ADC channel 1 convert failed (%d)\n", i);
            System_printf("ADC channel 1 convert failed\n");
        }

        if(adcValue_current>2250){//hexacecimal for 2250 or 2.25 V
           PIN_setOutputValue(ledPinHandle, IOID_0, Board_LED_OFF );    //OE TO TURN OF LE
           // printf("should be on\n");
           // printf("on");
           // printf("%d\n",adcValue1[i]);

        }
        else{
            PIN_setOutputValue(ledPinHandle, IOID_0, Board_LED_ON );    //OE TO TURN OF LE
          //  printf("should be off\n");
            //printf("off");
            //printf("%d\n",adcValue1[i]);

        }

        //i=i+1;*/

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        while(true){
           //res = ADC_convert(adc, &adcValue1[i]);
           res = ADC_convert(adc, &adcValue_current);
          // time_t x=time(0);
           //Time[i]=x;
           //adcValue1[i]=hextoint(adcValue1[i]);

           if (res == ADC_STATUS_SUCCESS) {

           //  System_printf("ADC channel 1 convert result (%d): %d\n", i,//0x%x
              // adcValue1[i]);
           }
           else {
              // System_printf("ADC channel 1 convert failed (%d)\n", i);
               System_printf("ADC channel 1 convert failed\n");
           }

           //if hall sensor passes over magnet turn on OE in turns it turns off LEDS
           //THEN DELAY

           ///This turns on leds
           if(adcValue_current<1850){//hexacecimal for 2200 or 2.2 V
              PIN_setOutputValue(ledPinHandle, IOID_0, Board_LED_OFF );    //OE TO TURN OF LE
             //printf("should be on\n");
              // printf("on");
             // printf("%d\n",adcValue_current);
              // sleep(100);
            //  Tsk_sleep();
             // TSK_sleep(500);

             // delay(10);

           }
           //This turns off leds
           else{
               PIN_setOutputValue(ledPinHandle, IOID_0, Board_LED_ON );    //OE TO TURN OF LE
            // printf("should be off\n");



               Task_sleep(200);
               //printf("off");
              //printf("%d\n",adcValue_current);

           }
           ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        System_flush();
    }
   /* if(i==500){
        System_printf("Done");
        System_flush();
    }*/


   ADC_close(adc);
}


/*
 *  ======== main ========
 */
int main(void)
{
    //struct tm *mytime = localtime(&now);

   // float k=0;
    Task_Params taskParams;

    /* Call board init functions */
    Board_initGeneral();
    Board_initADC();
// printf("hello, world\n");
    /* Create tasks */
/*
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)taskFxn0, &taskParams, NULL);
*/

    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task1Stack;
    Task_construct(&task1Struct, (Task_FuncPtr)taskFxn1, &taskParams, NULL);

    //allocating pins to handle
       ledPinHandle = PIN_open(&ledPinState, ledPinTable);


           if(!ledPinHandle) {

               System_abort("Error initializing board LED pins\n");

           }


           //PIN_setOutputValue(ledPinHandle, IOID_1, Board_LED_ON );    //MOSI,DIN
           PIN_setOutputValue(ledPinHandle, IOID_3, Board_LED_OFF );    //CLK
           PIN_setOutputValue(ledPinHandle, IOID_4, Board_LED_OFF );    //LE

           PIN_setOutputValue(ledPinHandle, IOID_0, Board_LED_ON );    //OE TO TURN OF LES (on turns off leds)

           int age[48]={1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1, //led 11R-16R
               1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,
               //0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
               0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0};


           for ( j= 1; j<=48; j++){


/*           if(age[j]==1){
                 PIN_setOutputValue(ledPinHandle, IOID_1, Board_LED_ON);// change led color
                 PIN_setOutputValue(ledPinHandle, IOID_3, Board_LED_OFF );    // CLK = 0
                          //for(i=1000; i>0; i--);
                 PIN_setOutputValue(ledPinHandle, IOID_3, Board_LED_ON );
                          //for(i=1000; i>0; i--); // CLK = 1
                 }

            else{
                 PIN_setOutputValue(ledPinHandle, IOID_1, Board_LED_OFF);// change led color
                 PIN_setOutputValue(ledPinHandle, IOID_3, Board_LED_OFF );    // CLK = 0
                 //for(i=1000; i>0; i--);
                 PIN_setOutputValue(ledPinHandle, IOID_3, Board_LED_ON );
                 //for(i=1000; i>0; i--); // CLK = 1



           PIN_setOutputValue(ledPinHandle, IOID_4, Board_LED_ON );    //LE
           PIN_setOutputValue(ledPinHandle, IOID_4, Board_LED_OFF );    //LE

             }*/

          PIN_setOutputValue(ledPinHandle, IOID_1, Board_LED_OFF);// change led color
           PIN_setOutputValue(ledPinHandle, IOID_3, Board_LED_OFF );    // CLK = 0
           //for(i=1000; i>0; i--);
           PIN_setOutputValue(ledPinHandle, IOID_3, Board_LED_ON );
           //for(i=1000; i>0; i--); // CLK = 1

           PIN_setOutputValue(ledPinHandle, IOID_1, Board_LED_OFF);// change led color //ALL WORKING
           PIN_setOutputValue(ledPinHandle, IOID_3, Board_LED_OFF );    // CLK = 0
           //for(i=1000; i>0; i--);
           PIN_setOutputValue(ledPinHandle, IOID_3, Board_LED_ON );
           //for(i=1000; i>0; i--); // CLK = 1

           PIN_setOutputValue(ledPinHandle, IOID_1, Board_LED_ON);// change led color// ALL WOKING
           PIN_setOutputValue(ledPinHandle, IOID_3, Board_LED_OFF );    // CLK = 0
           //for(i=1000; i>0; i--);
           PIN_setOutputValue(ledPinHandle, IOID_3, Board_LED_ON );
           //for(i=1000; i>0; i--); // CLK = 1
           PIN_setOutputValue(ledPinHandle, IOID_4, Board_LED_ON );    //LE
           PIN_setOutputValue(ledPinHandle, IOID_4, Board_LED_OFF );    //LE


           }



// if the sensor is working the voltage should be higher than zero
           //therefore if value is greater than certain value
           //and code will go into loop turning off OE
           //WHICH turns ON leds

         /*  if(Board_ADC0>898){//hexacecimal for 2200 or 2.2 V
              PIN_setOutputValue(ledPinHandle, IOID_0, Board_LED_OFF );    //OE TO TURN OF LE
               printf("should be on\n");
           }
           else{
               PIN_setOutputValue(ledPinHandle, IOID_0, Board_LED_ON );    //OE TO TURN OF LE
               printf("should be off\n");
               printf("%d\n",Board_ADC0);
           }
*/



/*
 * The CC2650DK_7ID and CC1310DK_7XD measure an ambient light sensor in this example.
 * It is not powered by default to avoid high current consumption in other examples.
 * The code below turns on the power to the sensor.
 */
#if defined(CC2650DK_7ID) || defined(CC1310DK_7XD)
    PIN_State pinState;

    PIN_Config AlsPinTable[] =
    {
        Board_ALS_PWR    | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH    | PIN_PUSHPULL, /* Turn on ALS power */
        PIN_TERMINATE                                                            /* Terminate list */
    };

    /* Turn on the power to the ambient light sensor */
    PIN_open(&pinState, AlsPinTable);
#endif

   // System_printf("Starting the ADC Single Channel example\nSystem provider is "
       // "set to SysMin.  Halt the target to view any SysMin contents in ROV.\n");

    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    BIOS_start();

    return (0);
}
