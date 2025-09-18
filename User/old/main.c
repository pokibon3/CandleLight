// Daiso Candle Light 1/f drive Ver 0.1
// 2025.09.16 K.Ohe 
// http://radiopench.blog96.fc2.com/blog-entry-1355.html
//
#include <ch32v00x.h>
#include <debug.h>
//#include <stdio.h>
#include <stdlib.h>

//void Delay_Init(void);
//void Delay_Ms(uint32_t n);

#define COIL_PIN GPIO_Pin_2     // digitalWrite使用
#define LED_PIN  GPIO_Pin_4     // analogWriteで使用

void TIM1_PWMOut_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    // GPIO、Timerのクロック有効化
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOC | RCC_PB2Periph_TIM1, ENABLE);


    // TIM1_CH4 -> PC4
    GPIO_InitStructure.GPIO_Pin = LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // GPIO PC2 or PA2
    GPIO_InitStructure.GPIO_Pin =  COIL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    //GPIO_Init(GPIOC, &GPIO_InitStructure);            // for PC2
    GPIO_Init(GPIOA, &GPIO_InitStructure);              // for PA2

    // Timer1 CH4をPWMで有効化
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);

    TIM_TimeBaseInitStructure.TIM_Period = 255;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 4;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);

    // カウンタの値を即時反映するため、プリロード機能をDisableにす�??
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);

    // PWM出力の有効化
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    // Timerの有効化
    TIM_Cmd(TIM1, ENABLE);
}

float fluctuate(float x)              // 間欠カオス法で 1/f波形を生成
{
    if (x < 0.5) {                        // 0.5以上なら
    x = x + 2 * x * x;                  // 2x^2で徐々に増加
    } else {                              // 0.5以下なら
        x = x - 2 * (1.0 - x) * (1.0 - x);  // 2(1-x)^2で徐々に減少
    }                                     //
    if (x < 0.05 || x > 0.95) {           // 結果が上下限を外れていたら、
//        x = random(100, 900) / 1000.0;    // 乱数で0.1-0.9の範囲に戻す
        x = (rand() % 900 + 100) / 1000.0;                       // 乱数で0.1-0.9の範囲に戻す
    }
    return x;
}

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();

//    SDI_Printf_Enable();

//    printf("init\r\n");

    TIM1_PWMOut_Init();

//    printf("start\r\n");

    float newBright = 0.7;
    float lastBright = 0;
    float diffBright = 0;

    while (1) {
        newBright = fluctuate(newBright);  // カオス法で新しい明るさを決定
        //printf(newBright);
        //Serial.print(", ");
        // コイルに通電して炎の板を揺らす
        diffBright = lastBright - newBright;              // 明るさの差（減光量）が、
//        printf("diffBright : %d", diffBright * 1000);
        if (diffBright > 0.35) {                          // 一定値を超えていたら(値は要調整）
            GPIO_WriteBit(GPIOA, COIL_PIN, 1);
//            printf("Coil ON\r\n");
        } else {                     // 超えてなければ
            GPIO_WriteBit(GPIOA, COIL_PIN, 0);
//            printf("Coil OFF\r\n");
        }
        // LEDの明るさを変える
        for (int i = 1; i <= 10; i++) {               // チラツキ防止のため、10回に分けて、
            diffBright = lastBright + (newBright - lastBright) * i / 10.0;  // 指定値まで直線補完で、
            TIM_SetCompare4(TIM1, 50 + diffBright * 200);
            Delay_Ms(20);                                  // 補間1ステップ時間（全体ではこの10倍）
        }
        lastBright = newBright;
    }
}
