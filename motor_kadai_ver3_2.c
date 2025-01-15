//===========================< ヘッダファイル >============================

#include "stm32f10x.h"

//=========================< 各種シンボル定義 >============================

#define SW01 0x2000
#define Sensor02 0xf800
#define Sensor0 0x13	//USE_PA0~1,PA4

#define R_Base_Speed 8000
#define L_Base_Speed 8000

#define run				0x2000 	//前進
#define l_right			0x3000	//やや右
#define right			0x3800	//右
#define d_right			0x1800
#define l_left			0x6000	//やや左
#define left			0xe000	//左
#define d_left			0xc000
#define	c_o				2	//コースアウト
#define c_s				0x0	//交差点

#define led_run 0xf3
#define led_l_r 0xf2
#define led_right 0xf0
#define led_d_r 0xe2
#define led_l_l 0xe3
#define led_left 0xe1
#define led_d_l 0xe2
#define led_couseout 0xe1
#define led_crossing 0xe3
#define led_stop 0xe0

#define Delay_time 10000


//=======================< グローバル・データ定義 >============================

unsigned int Up = 0;   	// 表示タイミング及びスイッチ押下を示すフラグ変数
unsigned int Senser = 0;// 表示タイミング及びスイッチ押下を示すフラグ変数

unsigned int motor = 0;
volatile uint32_t tick = 0;

int SENSER = 0;
int SENSER2 = 0;
int num = 0;
int count = 0;
int state_last = 0;
//=========================< コード・セクション >==============================

/* 関数のプロトタイプ宣言 */

void My_SystemInit(void);		// GPIO ポート及び SysTick タイマの設定を行う関数
void Tim3_Init(void);
unsigned int Get_SW1(void);		// スイッチの値を取得する関数
unsigned int Get_SW2(void);
unsigned int Get_Sensor(void);
unsigned int Get_Sensor2(void);
void Right_Motor_Run(double);
void Right_BackMotor_Run(double);
void Left_Motor_Run(double);
void Left_BackMotor_Run(double);
void delay_ms(uint32_t ms);

/*
**	main 関数
*/

int main(void)
{
 	My_SystemInit();
	Tim3_Init();
	enum STATE{
		  STOP,
		  RUN,
		  L_RIGHT,
		  RIGHT,
		  D_RIGHT,
		  L_LEFT,
		  LEFT,
		  D_LEFT,
		  COURSEOUT,
		  CROSSING,
	};


	while(1) {

		if (Up & SW01) {				// モーター始動
			Up &= ~SW01;

			if (motor == 0) {
				motor = 1;


			}else {					//モーター停止
				motor = 0;
				TIM3->CCR3 = 0;
				TIM3->CCR1 = 0;
			}
		}


		if (motor == 1) {

			SENSER2 = Get_Sensor2();

			switch(num){
			case STOP:
				if(SENSER2 == run){
					num = RUN;
				}else{
					GPIOA->ODR = led_stop;
					Right_Motor_Run(0);
					Left_Motor_Run(0);
					Right_BackMotor_Run(0);
					Left_BackMotor_Run(0);
					count = 0;
				}
				break;

			case RUN:			//直進
				if(SENSER2 == run){
					Right_BackMotor_Run(0);
					Left_BackMotor_Run(0);
					Right_Motor_Run(1);
					Left_Motor_Run(1);
					GPIOA->ODR = led_run;
					num = RUN;
				}else if(SENSER2 == l_right){
					num = L_RIGHT;
				}else if(SENSER2 == l_left){
					num = L_LEFT;
				}else if(SENSER2 == c_o){
					num = CROSSING;
				};
				break;



				//---------------------ここから右折--------------------//
			case L_RIGHT:
				if(SENSER2 == l_right){
					Right_BackMotor_Run(0);
					Left_BackMotor_Run(0);
					Right_Motor_Run(0.35);
					Left_Motor_Run(0.95);
					GPIOA->ODR = led_l_r;
					num = L_RIGHT;
				}else if(SENSER2 == right){
					num = RIGHT;
				}else if(SENSER2 == run){
					num = RUN;
//				}else if(SENSER2 == d_right){
//					num = D_RIGHT;
//				}else{

				};
				break;



			case RIGHT:
				if(SENSER2 == right){
					Right_BackMotor_Run(0);
					Left_BackMotor_Run(0);
					Right_Motor_Run(0);
					Left_Motor_Run(0.5);
					GPIOA->ODR = led_right;
					num = RIGHT;
				}else if(SENSER2 == l_right){
					num = L_RIGHT;
				}else if(SENSER2 == d_right){
					num = D_RIGHT;
				}else{
					num = COURSEOUT;
					state_last = RIGHT;
				};
				break;

			case D_RIGHT:
				if(SENSER2 == d_right){
					Left_BackMotor_Run(0);
					Right_BackMotor_Run(0.4);
					Left_Motor_Run(0.4);
					GPIOA->ODR = led_d_r;
					num = D_RIGHT;
				}else if(SENSER2 == right){
					num = RIGHT;
				}else{
					num = COURSEOUT;
					state_last = D_RIGHT;
				};
				break;

				//------------------ここから左折-------------------//
			case L_LEFT:
				if(SENSER2 == l_left){
					Right_BackMotor_Run(0);
					Left_BackMotor_Run(0);
					Right_Motor_Run(0.95);
					Left_Motor_Run(0.35);
					GPIOA->ODR = led_l_r;
					num = L_LEFT;
				}else if(SENSER2 == left){
					num = LEFT;
				}else if(SENSER2 == run){
					num = RUN;
//				}else if(SENSER2 == d_left){
//					num = D_LEFT;
				}else{

				};
//				}else if(SENSER2 == c_o){
//					num = CROSSING;
				break;

			case LEFT:
				if(SENSER2 == left){
					Right_BackMotor_Run(0);
					Left_BackMotor_Run(0);
					Right_Motor_Run(0.5);
					Left_Motor_Run(0);
					GPIOA->ODR = led_right;
					num = LEFT;
				}else if(SENSER2 == l_left){
					num = L_LEFT;
				}else if(SENSER2 == d_left){
					num = D_LEFT;
				}else{
					num = COURSEOUT;
					state_last = LEFT;
				};
				break;

			case D_LEFT:
				if(SENSER2 == d_left){
					Right_BackMotor_Run(0);
					Right_Motor_Run(0.4);
					Left_BackMotor_Run(0.4);
					GPIOA->ODR = led_d_r;
					num = D_LEFT;
				}else if(SENSER2 == left){
					num = LEFT;
				}else{
					num = COURSEOUT;
					state_last = D_LEFT;
				};
				break;


				//--------------------------例外処理---------------------//
			case COURSEOUT:
				count = 0;
				GPIOA->ODR = led_couseout;
				while(count <= 1000){
					if(GPIOA->ODR != led_couseout){
						count = 100000;
					}else{
						count++;
					}

				}
				while(count <= 50000){
					Right_Motor_Run(0);
					Left_Motor_Run(0);
					Right_BackMotor_Run(0.5);
					Left_BackMotor_Run(0.5);
					count++;
				}
				count = 0;
				num = STOP;
				state_last = 0;
				break;

			case CROSSING:
				if(count <= 1500000){
					if(SENSER2 == c_o){
						Right_Motor_Run(1);
						Left_Motor_Run(1);
						GPIOA->ODR = led_crossing;
						num = CROSSING;
						count++;
					}else if(SENSER2 == run){
						num = RUN;
						count = 0;
					};
				}else{
					count = 0;
					GPIOA->ODR = led_couseout;
					while(count <= 150000){
						Right_BackMotor_Run(1);
						Left_BackMotor_Run(1);
						count++;
					}
				}
				break;


			default:
				while(count <= 150000){
					Right_BackMotor_Run(1);
					Left_BackMotor_Run(1);
					count++;
				};
					count = 0;
					break;

			};

		}else{
			Right_Motor_Run(0);
			Left_Motor_Run(0);
			Right_BackMotor_Run(0);
			Left_BackMotor_Run(0);
		}
	}
}


void delay_ms(uint32_t ms) {
    tick = ms;
    while (tick != 0);
}


void Right_Motor_Run(double speed)
{
	TIM3->CCR1 =R_Base_Speed*speed;
}

void Right_BackMotor_Run(double speed)
{
	TIM3->CCR2 =R_Base_Speed*speed;
}

void Left_Motor_Run(double speed)
{
	TIM3->CCR3 =L_Base_Speed*speed;
}

void Left_BackMotor_Run(double speed)
{
	TIM3->CCR4 =L_Base_Speed*speed;
}



void My_SystemInit(void)
{

	RCC->APB1ENR = RCC_APB1ENR_TIM3EN;

	RCC->APB2ENR = RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN |
				   RCC_APB2ENR_IOPCEN |
				   RCC_APB2ENR_AFIOEN;

/* モータ制御用のポート PC6、PC7、PC8、PC9 を出力に設定 */
	AFIO->MAPR = 0x2000C00;
	AFIO->MAPR = 0xC00;	       //TIM3の信号をリマップ、CH3をPC8に接続
	GPIOA->CRL = 0x44414411;
	GPIOC->CRL = 0xaa444444;	//PC8もオルタネート出力に設定
	GPIOC->CRH = 0x444444aa;

	GPIOA->ODR =0xff;
	GPIOC->ODR = 0;

	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);	// SysTick タイマ割込みの間隔を 1000us　に設定
														// （ダイナミック点灯の周波数は 2KHz になる）

	TIM3->PSC =23;//プリスケーラ23　CLKが24MHzなので1MHzでカウンタが動作
	TIM3->ARR =9999; //カウンタは10msで最大値9999


}

void Tim3_Init(void) {

// 	TIM3->CCMR2 = 0x68; //OC3M=110(PWM),OC3PE=1,CSS4S=00(出力).
	TIM3->CCMR1 = 0x6868;//OC1M-OC2M=110(PWM),OC1PE-OC2PE=1,CSS4S=00(出力)
	TIM3->CCMR2 = 0x6868;//OC3M-OC4M=110(PWM),OC3PE-OC4PE=1,CSS4S=00(出力)
	TIM3->CCER =0x1111;  //CC3E=1(CH1、3出力イネーブル/PC8/AIN1/右車輪)
	TIM3->CCR1 =1;		//比較値セット(PC6/BIN1/左車輪)
	TIM3->CCR2 =1;
	TIM3->CCR3 =1;
	TIM3->CCR4 =1;      //比較値セット(PC8/AIN1/右車輪)
	TIM3->CR1|=0x80;    //ARPE=1(自動プリロード)
	TIM3->CR1|=0x1;     //TIMスタート
}


unsigned int Get_Sensor(void)
{
	unsigned int Sensor;

	Sensor = ~GPIOA->IDR;	// スイッチの値を（負論理なので）ビット反転して読み込む
	Sensor = Sensor & Sensor0;		// スイッチ以外のビットをマスクする

	return Sensor;
}

unsigned int Get_Sensor2(void)
{
	unsigned int Sensor2;

	Sensor2 = GPIOB->IDR;	// スイッチの値を読み込む
	Sensor2 = Sensor2 & Sensor02;		// スイッチ以外のビットをマスクする

	return Sensor2;
}


unsigned int Get_SW1(void)
{
	unsigned int sw;

	sw = ~GPIOC->IDR;	// スイッチの値を（負論理なので）ビット反転し読み込む
	sw = sw & SW01;		// スイッチ以外のビットをマスクする

	return sw;
}



void SysTick_Handler(void)
{
	static unsigned int swc  = 0;	// 今回読み込んだスイッチの値
	static unsigned int swp1 = 0;	// 前回読み込んだスイッチの値
	static unsigned int swp2 = 0;	// 前々回に読み込んだスイッチの値

	static unsigned int sw_now  = 0;	// 今回（現在）のスイッチの確定値
	static unsigned int sw_last = 0;	// 前回のスイッチの確定値

	static int chat_count = 0;	// チャタリング除去用のカウンタ変数

    if (tick > 0) {
        tick--;
    }

	if (chat_count == 1) {	// 1000us を 10 回繰り返して 10ms を作り出す
		chat_count = 0;

		swp2 = swp1;
		swp1 = swc;
		swc  = Get_SW1();



		if ((swp2 == swp1) && (swp1 == swc)) {	// 今回、前回、前々回の値が全て等しい
			sw_now = swc;						// 場合、今回の値を現在の確定値とする
		}



		if (sw_now != sw_last) {		// 前回のスイッチの値と異なる場合だけ以下の処理を行う
			if (sw_now & ~sw_last) {		// 立上り（スイッチの押下）を検出したら
				Up |= SW01;					// スイッチ押下のフラグを立てる
			}
			sw_last = sw_now;				// 現在のスイッチの値を sw_last に保存する
		}

	} else {
		chat_count++;
	}
	SENSER2 = Get_Sensor2();
	SENSER = Get_Sensor();
}


