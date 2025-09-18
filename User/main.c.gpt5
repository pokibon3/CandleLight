// Daiso Candle Light 1/f drive Ver 0.1
// 2025.09.16 K.Ohe Based on http://radiopench.blog96.fc2.com/blog-entry-1355.html

#include <ch32v00x.h>
#include <debug.h>
#include <stdlib.h>

// Delay functions are provided by the WCH library
extern void Delay_Init(void);
extern void Delay_Ms(uint32_t n);

#define COIL_PIN GPIO_Pin_2   // Coil drive pin (use PC2 or PA2, see init below)
#define LED_PIN  GPIO_Pin_4   // PWM output (TIM1_CH4 -> PC4)

static void TIM1_PWMOut_Init(void)
{
    GPIO_InitTypeDef       GPIO_InitStructure  = {0};
    TIM_OCInitTypeDef      TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    // Enable clocks for GPIOC and TIM1
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOC | RCC_PB2Periph_TIM1, ENABLE);

    // TIM1_CH4 -> PC4 (PWM)
    GPIO_InitStructure.GPIO_Pin   = LED_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Coil drive pin: PC2 or PA2 (select one)
    GPIO_InitStructure.GPIO_Pin   = COIL_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    // GPIO_Init(GPIOC, &GPIO_InitStructure);   // for PC2
    GPIO_Init(GPIOA, &GPIO_InitStructure);      // for PA2

    // Configure TIM1 CH4 as PWM
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse       = 0;
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);

    TIM_TimeBaseInitStructure.TIM_Period        = 255;
    TIM_TimeBaseInitStructure.TIM_Prescaler     = 4;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    // Disable preload so compare updates apply immediately
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);

    // Enable PWM outputs and start timer
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}

// Generate 1/f-like fluctuation via intermittent chaos method
static float fluctuate(float x)
{
    if (x < 0.5f) {
        // Gradually increase: x + 2x^2
        x = x + 2.0f * x * x;
    } else {
        // Gradually decrease: x - 2(1-x)^2
        float dx = (1.0f - x);
        x = x - 2.0f * dx * dx;
    }

    // If out of bounds, reset into 0.1–0.9 range
    if (x < 0.05f || x > 0.95f) {
        x = (float)((rand() % 900) + 100) / 1000.0f;
    }
    return x;
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();

    TIM1_PWMOut_Init();

    float newBright  = 0.7f;
    float lastBright = 0.0f;
    float diffBright = 0.0f;

    while (1) {
        // Decide next brightness via chaos method
        newBright = fluctuate(newBright);

        // Drive coil if dimming amount exceeds threshold (tune as needed)
        diffBright = lastBright - newBright;
        if (diffBright > 0.35f) {
            GPIO_WriteBit(GPIOA, COIL_PIN, 1);
        } else {
            GPIO_WriteBit(GPIOA, COIL_PIN, 0);
        }

        // Smoothly ramp LED brightness to target to reduce flicker
        for (int i = 1; i <= 10; i++) {
            float step = lastBright + (newBright - lastBright) * (float)i / 10.0f;
            TIM_SetCompare4(TIM1, (uint16_t)(50.0f + step * 200.0f));
            Delay_Ms(20); // 10 steps → total transition time 200 ms
        }
        lastBright = newBright;
    }
}
