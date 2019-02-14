/**
 * \file main.c
 *
 * \brief This is a short demo for manual testing for the external interrupt for samd21G18A
 *        This program runs on the user board developed by Sogeti.
 *
 * \author Keon Chen
 */

#include <asf.h>
#include <io.h>
#include <delay.h>

#define F_CPU 4000000UL //The clock speed
#define LED_BLUE PORT_PA06
#define LED_GREEN PORT_PA05
#define LED_RED PORT_PA04
#define PA15_EXINT_LINE 15 //The number has to correspond with the channel number

void blinkpurple()
{
	REG_PORT_OUTTGL0 = LED_RED; // toggle PORT_PA26
	REG_PORT_OUTTGL0 = LED_BLUE; // toggle PORT_PA26
	//REG_PORT_OUTTGL0 = LED_GREEN; // toggle PORT_PA26
	delay_s(1);
	REG_PORT_OUTTGL0 = LED_RED; // toggle PORT_PA26
	REG_PORT_OUTTGL0 = LED_BLUE; // toggle PORT_PA26
	//REG_PORT_OUTTGL0 = LED_GREEN; // toggle PORT_PA26
	delay_s(1);
}

/************************************************************************/
/* Configure the non-maskable external interrupt                        */
/************************************************************************/

void configure_nmi_extint(void)
{
  struct extint_nmi_conf eint_chan_conf;
  extint_nmi_get_config_defaults(&eint_chan_conf);
  eint_chan_conf.gpio_pin = PIN_PA08A_EIC_NMI;
  eint_chan_conf.gpio_pin_mux = PINMUX_PB08A_EIC_EXTINT8;
  eint_chan_conf.gpio_pin_pull      = EXTINT_PULL_UP;
  eint_chan_conf.filter_input_signal = false;
  eint_chan_conf.detection_criteria  = EXTINT_DETECT_BOTH;
  extint_nmi_set_config(0, &eint_chan_conf);
}

void NMI_Handler()
{
	blinkpurple();
	extint_nmi_clear_detected(0);
	//bool pin_state = port_pin_get_input_level(PIN_PB08A);	//port_pin_set_output_level(PIN_PB08A, pin_state);
}

/************************************************************************/
/* Configure the normal maskable external interrupt                     */
/************************************************************************/

void configure_extint_channel(void)
{
	struct extint_chan_conf config_extint_chan;
	extint_chan_get_config_defaults(&config_extint_chan);
	config_extint_chan.gpio_pin = PIN_PA15A_EIC_EXTINT15;
	config_extint_chan.gpio_pin_mux = PINMUX_PA15A_EIC_EXTINT15;
	config_extint_chan.gpio_pin_pull = EXTINT_PULL_UP;
	config_extint_chan.filter_input_signal = false;
	config_extint_chan.wake_if_sleeping = true;
	config_extint_chan.detection_criteria = EXTINT_DETECT_BOTH;
	extint_chan_set_config(PA15_EXINT_LINE, &config_extint_chan);
}

void extint_detection_callback(void)
{
	blinkpurple();
	extint_chan_clear_detected(PA15_EXINT_LINE);
}

void configure_extint_callbacks(void)
{
	extint_register_callback(extint_detection_callback,
	PA15_EXINT_LINE,
	EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(PA15_EXINT_LINE,
	EXTINT_CALLBACK_TYPE_DETECT);
}

/************************************************************************/
/* Main function                                                        */
/************************************************************************/

int main ()
{
	system_init(); // initialize system
	delay_init(); // initialize delay

	REG_PORT_DIRSET0 = LED_BLUE; // Direction set to OUTPUT for PB08
	REG_PORT_OUT0 |= LED_BLUE; // set pin to HIGH directly
	REG_PORT_DIRSET0 = LED_GREEN; // Direction set to OUTPUT for PB08
	REG_PORT_OUT0 |= LED_GREEN; // set pin to HIGH directly
	REG_PORT_DIRSET0 = LED_RED; // Direction set to OUTPUT for PB08
	REG_PORT_OUT0 |= LED_RED; // set pin to HIGH directly

	configure_extint_channel(); // establish the external interrupt channel  
	configure_extint_callbacks(); // register the callback function
	configure_nmi_extint(); // establish the non-maskable interrupt channel
	system_interrupt_enable_global(); //enable the interrupts		delay_s(1);

	while(1)
	{
		//don't have to do anything
		delay_s(1);
	}
	
}
