/*
File    : main.c
Software "Kurs STM32 PCBtech"
Lesson 7: Timer1 complementary output PE9/PE8.
Student: antigo1989@gmail.com
*/

#include "main.h"

//global values


/*********************************main************************************/
int main(void) {
  //System init
  SystemInit();
  RCC_Init();
  __enable_irq();
  //Timer init
  TIM1_Init();  
  //GPIO init
  key_led_Init();
 
  while(1){
    __NOP();
  }
}

/***********************interrupts function**************************/
//keys
void EXTI15_10_IRQHandler(void){
  switch(EXTI->PR & (EXTI_PR_PR10|EXTI_PR_PR11|EXTI_PR_PR12)){
    case EXTI_PR_PR10:
      TIM1->EGR |= TIM_EGR_BG; //disable output
      break;
    case EXTI_PR_PR11:
      TIM1->BDTR |= TIM_BDTR_MOE;//enable output
      break;
    case EXTI_PR_PR12:
      __NOP();
      break;
  }
  EXTI->PR |= EXTI_PR_PR10|EXTI_PR_PR11|EXTI_PR_PR12;
}


/****************************** function**********************************/
void TIM1_Init(void){
  //PE9 and PE8
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
  GPIOE->MODER |= GPIO_MODER_MODE8_1|GPIO_MODER_MODE9_1;
  GPIOE->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8_1|GPIO_OSPEEDER_OSPEEDR9_1;
  GPIOE->AFR[1] |= (1<<GPIO_AFRH_AFSEL8_Pos)|(1<<GPIO_AFRH_AFSEL9_Pos);

  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
  TIM1->PSC = 1; //42MHz
  TIM1->CR1 |= TIM_CR1_CMS;
  TIM1->ARR = 1023; // ~20kHz
  TIM1->CCR1 = 511;
  TIM1->CCMR1 |= TIM_CCMR1_OC1M; //mode 2
  TIM1->CCMR1 &= ~(TIM_CCMR1_CC1S); //channel 1 - output
  TIM1->CCER |= TIM_CCER_CC1NE|TIM_CCER_CC1E; //enable channel 1 and channel 1N
  TIM1->BDTR |= (0b11100001<<TIM_BDTR_DTG_Pos)|TIM_BDTR_MOE; //enable output
  //Start Timer
  TIM1->CR1 |= TIM_CR1_CEN;
  TIM1->EGR |= TIM_EGR_UG;
}


void key_led_Init(void){
  // Clock BUS Initial
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PE|SYSCFG_EXTICR3_EXTI11_PE;
  SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PE;
  //GPIO 
  GPIOE->MODER |= GPIO_MODER_MODE13_0|GPIO_MODER_MODE14_0|GPIO_MODER_MODE15_0;
  GPIOE->MODER &= ~(GPIO_MODER_MODE10|GPIO_MODER_MODE11|GPIO_MODER_MODE12);
  //Interrupts keys
  EXTI->PR |= EXTI_PR_PR10|EXTI_PR_PR11|EXTI_PR_PR12;
  EXTI->FTSR |= EXTI_FTSR_TR10|EXTI_FTSR_TR11|EXTI_FTSR_TR12;
  EXTI->IMR |= EXTI_IMR_IM10|EXTI_IMR_IM11|EXTI_IMR_IM12;
  //Interrupt NVIC Enable
  NVIC_EnableIRQ(EXTI15_10_IRQn);
  //LED turn off
  GPIOE->BSRR |= GPIO_BSRR_BS13|GPIO_BSRR_BS14|GPIO_BSRR_BS15;
}
/*************************** End of file ****************************/
