/*This file has been prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file manages the high level application device task.
//!
//! - Compiler:           IAR EWAVR and GNU GCC for AVR
//! - Supported devices:  ATmega32U4
//!
//! \author               Atmel Corporation: http://www.atmel.com \n
//!                       Support and FAQ: http://support.atmel.no/
//!
//! ***************************************************************************

/* Copyright (c) 2007, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//_____  I N C L U D E S ___________________________________________________

#include <Arduino.h>
#include <config.h>
#include <conf_usb.h>
#include <device_template_task.h>
#include <usb_drv.h>
#include <usb_descriptors.h>
#include <usb_standard_request.h>
#include <wdt_drv.h>


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________



//_____ D E C L A R A T I O N S ____________________________________________


volatile U8 cpt_sof;

U8 buf[256];
U8 rxok;


//!
//! @brief This function initializes the hardware/software resources
//! required for device application task.
//!
//!
//! @param none
//!
//! @return none
//!
//!/
void device_template_task_init(void)
{
   init();

   cpt_sof=0;
   Usb_enable_sof_interrupt();
   rxok=FALSE;

   //Call Arduino user setup
   setup();
}

//! @brief Entry point of the device application
//!
//! This function links the application with the USB bus.
//!
//! @param none
//!
//! @return none
void device_template_task(void)
{
   U8 i;
   U8 *ptr;
   static U8 dummy_data;

   //.. FIRST CHECK THE DEVICE ENUMERATION STATE
   if (Is_device_enumerated())
   {
      //.. The example bellow just perform a loop back transmition/reception
      //.. All data received wth the OUT endpoint are store in a ram buffer and
      //.. send back to the IN endpoint

      //.. First interface management
      //.. Select the OUT endpoint declared in descriptors
      //.. load the endpoint with the content of a ram buffer for example
      Usb_select_endpoint(EP_TEMP_OUT);
      if ( Is_usb_receive_out())
      {
         ptr=buf;
         for(i=Usb_byte_counter();i;i--)
         {
            *ptr++=Usb_read_byte();
         }
         Usb_ack_receive_out();
         rxok=TRUE;
      }
      //.. First interface management (cont)
      //.. Select the IN endpoint declared in descriptors
      //.. If we receive something, just store in the ram buffer
      Usb_select_endpoint(EP_TEMP_IN);
      if ( Is_usb_read_control_enabled()&&(rxok==TRUE))
      {
         ptr=buf;
         for(i=0;i<EP_IN_LENGTH_TEMP1;i++)
         Usb_write_byte(*ptr++);
         Usb_ack_in_ready();
         rxok=FALSE;
      }
   }

   //Call Arduino user loop
   loop();
}


//! @brief sof_action
//!
//! This function increments the cpt_sof counter each times
//! the USB Start Of Frame interrupt subroutine is executed (1ms)
//! Usefull to manage time delays
//!
//! @param none
//!
//! @return none
void sof_action()
{
   cpt_sof++;
}
