#ifndef SERVO_TEST_H
#define SERVO_TEST_H

inline void init_servo(){
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);

    /*Configure PA1 PA2 PA3(TIMER1 CH1 CH2 CH3) as alternate function*/
    //gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    //gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER1);

    timer_deinit(TIMER1);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER1 configuration */
    timer_initpara.prescaler         = 9;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 53999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1, &timer_initpara);

    /* initialize TIMER channel output parameter struct */
    timer_channel_output_struct_para_init(&timer_ocinitpara);
    /* CH0, CH1 and CH2 configuration in PWM mode */
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    //timer_channel_output_config(TIMER1,TIMER_CH_1,&timer_ocinitpara);
    //timer_channel_output_config(TIMER1,TIMER_CH_2,&timer_ocinitpara);
    timer_channel_output_config(TIMER1,TIMER_CH_3,&timer_ocinitpara);
    
    /* CH3 configuration in PWM mode1,duty cycle 75% */
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_3,999);
    timer_channel_output_mode_config(TIMER1,TIMER_CH_3,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_3,TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
    /* auto-reload preload enable */
    timer_enable(TIMER1); 
}

inline void servo_set_angle(float deg){
    //                                                                               angle * value_1ms / per_90_deg + 1.5 * value_1ms                   
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_3, static_cast<uint16_t>((deg * (54000 / 20) / 90.0) + 1.5 * (54000 / 20)));
    //timer_channel_output_mode_config(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM0);
    //timer_channel_output_shadow_config(TIMER1,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);
}

#endif // SERVO_TEST_H
