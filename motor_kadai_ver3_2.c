//===========================< �w�b�_�t�@�C�� >============================

#include "stm32f10x.h"

//=========================< �e��V���{����` >============================

#define SW01 0x2000
#define Sensor02 0xf800
#define Sensor0 0x13	//USE_PA0~1,PA4

#define R_Base_Speed 8000
#define L_Base_Speed 8000

#define run				0x2000 	//�O�i
#define l_right			0x3000	//���E
#define right			0x3800	//�E
#define d_right			0x1800
#define l_left			0x6000	//��⍶
#define left			0xe000	//��
#define d_left			0xc000
#define	c_o				2	//�R�[�X�A�E�g
#define c_s				0x0	//�����_

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


//=======================< �O���[�o���E�f�[�^��` >============================

unsigned int Up = 0;   	// �\���^�C�~���O�y�уX�C�b�`�����������t���O�ϐ�
unsigned int Senser = 0;// �\���^�C�~���O�y�уX�C�b�`�����������t���O�ϐ�

unsigned int motor = 0;
volatile uint32_t tick = 0;

int SENSER = 0;
int SENSER2 = 0;
int num = 0;
int count = 0;
int state_last = 0;
//=========================< �R�[�h�E�Z�N�V���� >==============================

/* �֐��̃v���g�^�C�v�錾 */

void My_SystemInit(void);		// GPIO �|�[�g�y�� SysTick �^�C�}�̐ݒ���s���֐�
void Tim3_Init(void);
unsigned int Get_SW1(void);		// �X�C�b�`�̒l���擾����֐�
unsigned int Get_SW2(void);
unsigned int Get_Sensor(void);
unsigned int Get_Sensor2(void);
void Right_Motor_Run(double);
void Right_BackMotor_Run(double);
void Left_Motor_Run(double);
void Left_BackMotor_Run(double);
void delay_ms(uint32_t ms);

/*
**	main �֐�
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

		if (Up & SW01) {				// ���[�^�[�n��
			Up &= ~SW01;

			if (motor == 0) {
				motor = 1;


			}else {					//���[�^�[��~
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

			case RUN:			//���i
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



				//---------------------��������E��--------------------//
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

				//------------------�������獶��-------------------//
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


				//--------------------------��O����---------------------//
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

/* ���[�^����p�̃|�[�g PC6�APC7�APC8�APC9 ���o�͂ɐݒ� */
	AFIO->MAPR = 0x2000C00;
	AFIO->MAPR = 0xC00;	       //TIM3�̐M�������}�b�v�ACH3��PC8�ɐڑ�
	GPIOA->CRL = 0x44414411;
	GPIOC->CRL = 0xaa444444;	//PC8���I���^�l�[�g�o�͂ɐݒ�
	GPIOC->CRH = 0x444444aa;

	GPIOA->ODR =0xff;
	GPIOC->ODR = 0;

	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);	// SysTick �^�C�}�����݂̊Ԋu�� 1000us�@�ɐݒ�
														// �i�_�C�i�~�b�N�_���̎��g���� 2KHz �ɂȂ�j

	TIM3->PSC =23;//�v���X�P�[��23�@CLK��24MHz�Ȃ̂�1MHz�ŃJ�E���^������
	TIM3->ARR =9999; //�J�E���^��10ms�ōő�l9999


}

void Tim3_Init(void) {

// 	TIM3->CCMR2 = 0x68; //OC3M=110(PWM),OC3PE=1,CSS4S=00(�o��).
	TIM3->CCMR1 = 0x6868;//OC1M-OC2M=110(PWM),OC1PE-OC2PE=1,CSS4S=00(�o��)
	TIM3->CCMR2 = 0x6868;//OC3M-OC4M=110(PWM),OC3PE-OC4PE=1,CSS4S=00(�o��)
	TIM3->CCER =0x1111;  //CC3E=1(CH1�A3�o�̓C�l�[�u��/PC8/AIN1/�E�ԗ�)
	TIM3->CCR1 =1;		//��r�l�Z�b�g(PC6/BIN1/���ԗ�)
	TIM3->CCR2 =1;
	TIM3->CCR3 =1;
	TIM3->CCR4 =1;      //��r�l�Z�b�g(PC8/AIN1/�E�ԗ�)
	TIM3->CR1|=0x80;    //ARPE=1(�����v�����[�h)
	TIM3->CR1|=0x1;     //TIM�X�^�[�g
}


unsigned int Get_Sensor(void)
{
	unsigned int Sensor;

	Sensor = ~GPIOA->IDR;	// �X�C�b�`�̒l���i���_���Ȃ̂Łj�r�b�g���]���ēǂݍ���
	Sensor = Sensor & Sensor0;		// �X�C�b�`�ȊO�̃r�b�g���}�X�N����

	return Sensor;
}

unsigned int Get_Sensor2(void)
{
	unsigned int Sensor2;

	Sensor2 = GPIOB->IDR;	// �X�C�b�`�̒l��ǂݍ���
	Sensor2 = Sensor2 & Sensor02;		// �X�C�b�`�ȊO�̃r�b�g���}�X�N����

	return Sensor2;
}


unsigned int Get_SW1(void)
{
	unsigned int sw;

	sw = ~GPIOC->IDR;	// �X�C�b�`�̒l���i���_���Ȃ̂Łj�r�b�g���]���ǂݍ���
	sw = sw & SW01;		// �X�C�b�`�ȊO�̃r�b�g���}�X�N����

	return sw;
}



void SysTick_Handler(void)
{
	static unsigned int swc  = 0;	// ����ǂݍ��񂾃X�C�b�`�̒l
	static unsigned int swp1 = 0;	// �O��ǂݍ��񂾃X�C�b�`�̒l
	static unsigned int swp2 = 0;	// �O�X��ɓǂݍ��񂾃X�C�b�`�̒l

	static unsigned int sw_now  = 0;	// ����i���݁j�̃X�C�b�`�̊m��l
	static unsigned int sw_last = 0;	// �O��̃X�C�b�`�̊m��l

	static int chat_count = 0;	// �`���^�����O�����p�̃J�E���^�ϐ�

    if (tick > 0) {
        tick--;
    }

	if (chat_count == 1) {	// 1000us �� 10 ��J��Ԃ��� 10ms �����o��
		chat_count = 0;

		swp2 = swp1;
		swp1 = swc;
		swc  = Get_SW1();



		if ((swp2 == swp1) && (swp1 == swc)) {	// ����A�O��A�O�X��̒l���S�ē�����
			sw_now = swc;						// �ꍇ�A����̒l�����݂̊m��l�Ƃ���
		}



		if (sw_now != sw_last) {		// �O��̃X�C�b�`�̒l�ƈقȂ�ꍇ�����ȉ��̏������s��
			if (sw_now & ~sw_last) {		// �����i�X�C�b�`�̉����j�����o������
				Up |= SW01;					// �X�C�b�`�����̃t���O�𗧂Ă�
			}
			sw_last = sw_now;				// ���݂̃X�C�b�`�̒l�� sw_last �ɕۑ�����
		}

	} else {
		chat_count++;
	}
	SENSER2 = Get_Sensor2();
	SENSER = Get_Sensor();
}


