#include "move_base.h"
#include "key.h"

ActPoint initpoint;     //�㵽���㷨 ��ʼ��ṹ���ʼ��
float initangle = 0;
ActPoint targetpoint;		//Ŀ���ṹ���ʼ��
float targetangle = 0;
ActLine2 presentline;		//��ʼֱ�߽ṹ���ʼ��
ActLine2 targetline;		//Ŀ��ֱ�߽ṹ���ʼ��


PidTypeDef PID_distance;	//PID�ṹ���ʼ��
PidTypeDef PID_angle;
PidTypeDef PID_DisArc;		//�ǶȶԾ�������PID

fp32 PID_param_dis[3];		//����PID���������ṹ���ʼ��
fp32 PID_param_angle[3];//�Ƕ�PID���������ṹ���ʼ��
fp32 PID_param_DisArc[3];//����ת�Ƕ�PID���������ṹ���ʼ��

int32_t motor_v1=0;
int32_t motor_v2=0;


//Vel��ֵΪ����Ϊ��ת
//speedBaseΪ����ǰ���ٶ�
void motorCMD(int32_t motor1,int32_t motor2)//������� �ǵ㵽�������ƺ���
{
    if(motor1>9000.0f)
        motor1=9000;
    if(motor2>9000.0f)
        motor2=9000;
    if(motor1<-9000.0f)
        motor1=-9000;
    if(motor2<-9000.0f)
        motor2=-9000;
    give_motor1(motor1);
    give_motor2(-motor2);

//		if(motor1>0)
//		{
//    LCD_ShowString(60,170,200,16,16,"V left=");
//    LCD_ShowxNum(60,190,motor1,6,16,0X80);
//		}
//		else
//		{
//		
//		LCD_ShowString(60,170,200,16,16,"V left=");
//		LCD_ShowString(60,190,200,16,16,"-");
//		LCD_ShowxNum(60,190,-motor1,6,16,0X80);	
//		}
//		if(motor2>0)
//		{
//    LCD_ShowString(60,210,200,16,16,"V right=");
//    LCD_ShowxNum(60,230,motor2,6,16,0X80);
//		}
//		else{
//		LCD_ShowString(60,210,200,16,16,"V right=");
//		LCD_ShowString(60,210,200,16,16,"-");
//    LCD_ShowxNum(60,230,-motor2,6,16,0X80);
//		}
}


void motor_back_CMD(int32_t motor1,int32_t motor2)//������� �ǵ㵽�������ƺ���
{
    if(motor1>9000.0f)
        motor1=9000;
    if(motor2>9000.0f)
        motor2=9000;
    if(motor1<-9000.0f)
        motor1=-9000;
    if(motor2<-9000.0f)
        motor2=-9000;
    give_motor1(motor2);
    give_motor2(-motor1);

//    LCD_ShowString(60,170,200,16,16,"V left=");
//    LCD_ShowxNum(60,190,motor2,6,16,0X80);
//    LCD_ShowString(60,210,200,16,16,"V right=");
//    LCD_ShowxNum(60,230,motor1,6,16,0X80);
}


void ps2_move()
{
    u8 key=0;
    s16 speed;
    s16 swerve;
    delay_ms(100);
    key=PS2_DataKey();
    if(key!=0)                   //�а�������
    {
//				printf(" %5d %5d %5d %5d\r\n",PS2_AnologData(PSS_LX),PS2_AnologData(PSS_LY),
//		                              PS2_AnologData(PSS_RX),PS2_AnologData(PSS_RY) );
//				printf(" %5d %5d\r\n",PS2_AnologData(PSS_LY),PS2_AnologData(PSS_RX));

        if(key == 11)
        {
            speed = PS2_AnologData(PSS_LY)-127;
            swerve = (PS2_AnologData(PSS_RX)-128)*12*((float)abs(speed)/128); //	speedȡ����ֵ��	�������㣬�õ�ת������
            speed = -(PS2_AnologData(PSS_LY)-127)*20;	   //����ǰ����  ��������
//				delay_ms(10);
//				printf(" %5d \r\n",speed);
//				printf(" %5d \r\n",swerve);
            if(speed==0&&swerve==0)
            {
                give_motor1(0);
                give_motor2(0);
                PID_Move_Clear(&chassis_move);
            }
            else
            {
                if(speed > 0) //��ǰ
                {
                    if(swerve < 0)//��ת��
                    {
                        //						speed1 = speed + swerve;
                        //						speed2 = speed;
                        give_motor1(speed + swerve);
                        give_motor2(~speed+1);
                        //						printf(" %5d %5d\r\n",wheel_speed[0],wheel_speed[1]);
                    }
                    else          //��ת��
                    {
                        //						speed1 = speed;
                        //						speed2 = speed - swerve;
                        give_motor1(speed);
                        give_motor2(~(speed - swerve)+1);
                        //						printf(" %5d %5d\r\n",wheel_speed[0],wheel_speed[1]);
                    }
                }
                else if(speed < 0)//���
                {
                    if(swerve < 0)//��ת��
                    {
                        //						speed1 = speed - swerve;
                        //						speed2 = speed;
                        give_motor1(speed - swerve);
                        give_motor2(~(speed - swerve)+1);
                    }

                    else//��ת��
                    {
                        //						speed1 = speed;
                        //						speed2 = speed + swerve;
                        give_motor1(speed);
                        give_motor2(~(speed + swerve)+1);
                    }
                }
            }
        }
        else
        {
//				delay_ms(10);
            give_motor1(0);
            give_motor2(0);
            PID_Move_Clear(&chassis_move);
//				printf("  \r\n   %d  is  light  \r\n",Data[1]);//ID
//				printf("  \r\n   %d  is  pressed  \r\n",key);
//				printf("  \r\n   %d  is  pressed  \r\n",MASK[key]);
            if(key == 12)
            {
                PS2_Vibration(0x00,0xFF);  //�����𶯺��������ʱ  delay_ms(1000);
                delay_ms(500);
            }
            else
                PS2_Vibration(0x00,0x00);
        }
    }
    else
    {
        give_motor1(0);
        give_motor2(0);
        PID_Move_Clear(&chassis_move);
    }
}
void walk_point(int x,int y, int p)
{
    presentline.point.x=GetPosX();
    presentline.point.y=GetPosY();
    presentline.angle=GetAngle();
		targetline.point.x=x;
    targetline.point.y=y;
    targetline.angle=p;//����Ŀ���	x,y,p

	if(targetline.point.x<0)
        {
            LCD_ShowString(120,50,200,16,16,"x=");
            LCD_ShowString(115,70,200,16,16,"-");
            LCD_ShowxNum(120,70,-targetline.point.x,3,16,0X80);
        }
        else
        {
            LCD_ShowString(120,50,200,16,16,"x=");
            LCD_ShowString(115,70,200,16,16," ");

            LCD_ShowxNum(120,70,targetline.point.x,3,16,0X80);
        }
        if(y<0)
        {
            LCD_ShowString(120,90,200,16,16,"y=");
            LCD_ShowString(115,110,200,16,16,"-");
            LCD_ShowxNum(120,110,-targetline.point.y,3,16,0X80);
        }
        else
        {
            LCD_ShowString(120,90,200,16,16,"y=");
            LCD_ShowString(115,110,200,16,16," ");

            LCD_ShowxNum(120,110,targetline.point.y,3,16,0X80);
        }
        if(p<0)
        {
            LCD_ShowString(120,130,200,16,16,"Angle=");
            LCD_ShowString(115,150,200,16,16,"-");
            LCD_ShowxNum(120,150,-targetline.angle,3,16,0X80);
        }
        else
        {
            LCD_ShowString(120,130,200,16,16,"Angle=");
            LCD_ShowString(115,150,200,16,16," ");
            LCD_ShowxNum(120,150,targetline.angle,3,16,0X80);
        }
				
				
				
    MvByLine(presentline,targetline, 1000);
}

void set_point(u16 x,u16 y,u16 p)
{
    targetline.point.x=x;
    targetline.point.y=y;
    targetangle=p;
}

/**
* @brief ����PID
* @note	PID���ͣ�λ����PID
* @param valueSet�������趨ֵ
* @param valueNow����ǰ����ֵ
* @retval ���ǰ�������ٶ�
* @attention
*/
float DistancePid(float distance)
{
    fp32 valueOut;
    PID_param_dis[0]=55;
    PID_param_dis[1]=0;
    PID_param_dis[2]=0;

    PID_Init(&PID_distance,PID_POSITION,PID_param_dis,9000.0f,1000.0f);//PID��ʼ��
    valueOut=PID_Calc(&PID_distance,distance,0);


    return valueOut;
}

/**
* @brief ����ԽǶȵ�����PID
* @note	PID���ͣ�λ����PID
* @param valueSet�������趨ֵ
* @param valueNow����ǰ����ֵ
* @retval ���ǰ�������ٶ�
* @attention
*/
float Distance_Arc_Pid(float distance)
{
    float valueOut;
    PID_Init(&PID_DisArc,PID_POSITION,PID_param_DisArc,90.0f,10.0f);//λ����PID��ʼ��
    valueOut=PID_Calc(&PID_DisArc,0,distance);
//	if(valueOut<0)
//	{
//    LCD_ShowString(130,130,200,16,16,"ARCPID=");
//		    LCD_ShowString(125,150,200,16,16,"-");
//    LCD_ShowxNum(130,150,-valueOut,6,16,0X80);
//	}
//	else
//	{
//	 LCD_ShowString(130,130,200,16,16,"ARCPID=");
//    LCD_ShowxNum(130,150,valueOut,6,16,0X80);
//	
//	}
    return valueOut;
}

/**
* @brief �Ƕ�PID
* @note	PID���ͣ�λ����PID
* @param valueSet���Ƕ��趨ֵ
* @param valueNow����ǰ�Ƕ�ֵ
* @retval ����ٶȲ�ֵ
* @attention
*/
float AnglePid(float valueSet,float valueNow)
{
    float err=0;
    float valueOut=0;
    err=valueSet-valueNow;
    //�Ƕ�ͻ�䴦��
    if(err > 180)
    {
        err=err-360;
    }
    else if(err < -180)
    {
        err=360+err;
    }
    PID_Init(&PID_angle,PID_POSITION,PID_param_angle,9000.0f,20.0f);//PID��ʼ��
    valueOut=PID_Calc(&PID_angle,0,err);	//PID����ת���ֵ
//    if(valueOut>0)
//    {
//        LCD_ShowString(130,170,200,16,16,"AnglePID=");
//        LCD_ShowString(120,190,200,16,16," ");
//        LCD_ShowxNum(130,190,valueOut,6,16,0X80);
//    }
//    else
//    {
//        LCD_ShowString(130,170,200,16,16,"AnglePID=");
//        LCD_ShowString(120,190,200,16,16,"-");
//        LCD_ShowxNum(130,190,-valueOut,6,16,0X80);
//    }
    return valueOut;
}


/**
 * @brief  ����PID�ƶ���Ŀ��λ��
 * @note	����point2point�㷨
 * @param  x		Ŀ��λ��X����
 * @param  y		Ŀ��λ��Y����
 * @param  angleĿ��Ƕ�
 * @retval None
 */

//С����
void Back_Side(u8 SideNumber)
{
	int speed,kp=50;
	if(SideNumber==1)
	{
		speed=-GetPosY()*kp;
		back_Turn(0,speed);
	}
	if(SideNumber==2)
	{
		speed=-(2200-GetPosX())*kp;
		back_Turn(90,speed);
	}
	if(SideNumber==3)
	{
		speed=-(4400-GetPosY())*kp;
		back_Turn(180,speed);
	}
	if(SideNumber==4)
	{
		speed=(-2200-GetPosX())*kp;
		back_Turn(90,speed);
	}
}
void move_to_pos(float x,float y,float angle)
{
    float x_Now=GetPosX();
    float y_Now=GetPosX();
    float angle_Now=GetAngle();
    float forwardspeed,turnspeed;

    float aim_distance=sqrt(pow(x_Now-x,2)+pow(y_Now-y,2));  //��ǰ�㵽Ŀ����ֱ�߾���

    forwardspeed = DistancePid(aim_distance);		//����PID����ǰ���ٶȵ���Ŀ���			Ӧ����Ϊ���롪���Ƕ�PID
    turnspeed = AnglePid(angle,angle_Now);			//�Ƕ�PID�������ֲ��ٵ���Ŀ�귽��

    motorCMD(forwardspeed-turnspeed,forwardspeed+turnspeed);
}


/**
 * @brief  PID ��Сת��
 * @note		��ǰ���ٶ� ��Ϊ��������ת������ת
 * @param  angle�������Ƕ�,Ϊ����ת��Ϊ����ת
 * @param  gospeed�������ٶ�
 * @retval None
 */
//С���� ԭ����ת
void minimum_Turn(float angle)
{
    float getAngle=0.0f;
    float speed=0.0f;
    PID_param_angle[0]=55;// ת��10000����ת�����Ƕ�180 = 55
    PID_param_angle[1]=0;
    PID_param_angle[2]=0;
    getAngle=GetAngle();
    speed = AnglePid(angle,getAngle);	//���ݽǶ�PID���ת��Ĳ���
    motorCMD(speed,speed);
}

/**
 * @brief  PID ǰ��ת��
 * @note
 * @param  angle�������Ƕ�,Ϊ����ת��Ϊ����ת
 * @param  gospeed�������ٶ�
 * @retval None
 */
//С���� ��ǰ��ת
void forward_Turn(float angle,float gospeed)
{
    float getAngle=0.0f;
    float speed=0.0f;

	
    PID_param_angle[0]=30;
    PID_param_angle[1]=0;
    PID_param_angle[2]=0;

    getAngle=GetAngle();
    speed = AnglePid(angle,getAngle);	//���ݽǶ�PID���ת��Ĳ���
//		if(gospeed<0)
//		motor_back_CMD(gospeed-speed,gospeed+speed);
//		else
    motorCMD(gospeed+speed,gospeed-speed);
}



/**
 * @brief  PID ����ת��
 * @note
 * @param  angle�������Ƕ�,Ϊ����ת��Ϊ����ת
 * @param  gospeed�������ٶ�
 * @retval None
 */
//С���� ����ת��
void back_Turn(float angle,float gospeed)
{
    float getAngle=0.0f;
    float speed=0.0f;
    PID_param_angle[0]=5;
    PID_param_angle[1]=0;
    PID_param_angle[2]=0;

    getAngle=GetAngle();
    speed = AnglePid(angle,getAngle);	//���ݽǶ�PID���ת��Ĳ���
    motor_back_CMD(gospeed-speed,gospeed+speed);
}


/**
  * @brief  �µ���ֱ�߱ջ�
  * @note	Ax1+By1+C1=0 ֱ�߷���һ��ʽ
  * @note	����A1��B1��C1����ֱ�߷��̺���Ŀ��ֱ�߷�������
  * @note	ǰ���ٶȿ��ɾ���PID����  ����������PID��̫�õ�  �����Ժ���
  * @note angleAddΪ��ֵ

  * @param A1
  * @param B1
  * @param C1
  * @param dir:Ϊ0 ���ϻ����ߣ�Ϊ1 ���»�������
  * @param setSpeed���ٶ�
  * @retval return_value 0Ϊ δ����Ŀ��ֱ��  1Ϊ �ѵ���Ŀ��ֱ�߾��뷶Χ��

  * @note ���� ��Ŀ��ֱ�߹������ʻ ��ͨ�������ж���ֱ�ߵ��ĸ�λ��
	* @note �õ��˾���-�Ƕ�PID ���ӽ�Ŀ���� �Ƕ�PID����֤����ֱ�߹����ʻ
  * @note �õ�������С���� ��Ŀ��ֱ�߾���Զʱ ��ǰ��ת�����ӽ�
  * @note �ӽ�����Ŀ��ֱ��ʱԭ����ת������Ŀ�귽�� ����ǰ��ת����������ʻֱ��
  */
uint8_t straightLine(float A1,float B1,float C1,uint8_t dir,float setSpeed)
{
    fp32 setAngle=0;
    fp32 angleAdd=0;
    int	return_value=0;
    fp32 getAngleNow=GetAngle();
    fp32 getX=GetPosX();
    fp32 getY=GetPosY();
    float distance=((A1*getX)+(B1*getY)+C1)/sqrt((A1*A1)+(B1*B1));//��ǰ�㵽Ŀ��ֱ�߾���

//		PID_param_dis[0]=0.5f;
//		PID_param_dis[0]=0;
//		PID_param_dis[0]=0;

    PID_param_DisArc[0]=0.15;
    PID_param_DisArc[1]=0;
    PID_param_DisArc[2]=0;

    angleAdd=Distance_Arc_Pid(distance);
//	if(angleAdd<0)
//        {   LCD_ShowString(130,90,200,16,16,"angleAdd =");
//            LCD_ShowString(130,110,200,16,16,"-");
//            LCD_ShowxNum(130,110,-angleAdd,6,16,0X80);
//        }
//        else {
//            LCD_ShowString(130,90,200,16,16,"angleAdd =");
//            LCD_ShowString(130,110,200,16,16," ");
//            LCD_ShowxNum(130,110,angleAdd,6,16,0X80);
//        }
    //��ֱ��35����ʱ��ʾ����ֱ��
//    if((distance < 150) && (distance > -150))	//����ֱ��λ��ʱ����С�뾶��ת����ΪĿ��Ƕ�
//    {
//        angleAdd=0;
//        return_value=1;
//			forward_Turn(setAngle,setSpeed);
//    }
//    else		//δ����Ŀ��ֱ�߾��뷶Χ��ʱ����ǰ��ת�����ӽ�Ŀ��ֱ��
//    {
        if((B1 > -0.005f) && (B1 < 0.005f))
        {
            if(!dir)
            {
                setAngle=0;							//Ŀ��Ƕ�Ϊˮƽֱ�� ������dirȷ�� 0Ϊ�� 1Ϊ��
                forward_Turn(setAngle-angleAdd,setSpeed);
            }
            else
            {
                if(A1 > 0)
                {
                    setAngle=-180;			//Ŀ��Ƕ�Ϊˮƽֱ�� ������dirȷ�� 0Ϊ�� 1Ϊ��
                    forward_Turn(setAngle+angleAdd,setSpeed);//�Ƕ�PIDת��Ŀ��Ƕ�
                }
                else
                {
                    setAngle=180;
                    forward_Turn(setAngle+angleAdd,setSpeed);
                }
            }
        }
        else
        {
            if(!dir)
            {
                setAngle=(atan(-A1/B1)*180/PI)-90;
                forward_Turn(setAngle+angleAdd,setSpeed);
            }
            else
            {
                setAngle=(atan(-A1/B1)*180/PI)+90;
                forward_Turn(setAngle-angleAdd,setSpeed);
            }

        }
        return_value=0;
//    } 
//						LCD_ShowString(130,50,200,16,16,"get_detination ?");
//            LCD_ShowxNum(130,70,return_value,6,16,0X80);
	     if(setAngle<0)
         {   LCD_ShowString(130,90,200,16,16,"setAngle =");
            LCD_ShowString(130,110,200,16,16,"-");
            LCD_ShowxNum(130,110,-angleAdd,6,16,0X80);
         }
				
		else {
				LCD_ShowString(130,90,200,16,16,"setAngle =");
				LCD_ShowString(130,110,200,16,16," ");
				LCD_ShowxNum(130,110,angleAdd,6,16,0X80);
				 } 
}
/**
  * @brief  ����Բ��·��
  * @note		ֱ��б��Ϊ��	
	* @note		˼·Ϊ ֱ�߾�������ǶȲ�ֵ ����Բ���켣�� ֱ��-�Ƕ�PID���ټ��� ������Ƕ�PID
	* @note		PID�����Ƕ�ʼ��Ϊ��ǰ�н�λ��ָ��Բ�ĵķ���
	* @note		ֱ�ߡ����Ƕ�PID���Ƕȡ����ǶȲ���������ɺ󣬿�����ֱ�ߡ�������PID ����ǰ���ٶ� ʹ�õ��̸������Ŀ����
  * @param x��Բ��x����
  * @param y��Բ��y����
  * @param R���뾶
  * @param clock:Ϊ1 ˳ʱ�룬Ϊ2 ��ʱ��
  * @param v���ٶ�
  * @retval None


	* @note ���� ����Ŀ��Բ��Բ�ĵ� x,y,�Լ�Բ���뾶R����Ŀ��Բ���������ʻ ��ͨ�������ж���ֱ�ߵ��ĸ�λ��
	* @note �õ��˾���-�Ƕ�PID ���ӽ�Ŀ���� �Ƕ�PID����֤����Բ�������ʻ
  * @note �õ�������С���� ��Ŀ��ֱ�߾���Զʱ ��ǰ��ת�����ӽ�
  * @note �ӽ�����Ŀ��ֱ��ʱԭ����ת������Ŀ�귽�� ����ǰ��ת����������ʻֱ��
  * @note PID����ʱ  �Ƕ�PID 
  */

//void clock_antiwise_close_LargeToSmall(u8 SideNumber,u8 state)
//{
//	static u8 flag=1;    
//	if(SideNumber==1) 
//	{
//			if(state==0)               //state�����жϳ����Ƿ񿿱�
//		{
//			 if(flag==1)               //flag����1��������СԲ
//			{
//				closeRound(0,2200, 800,0,4000,1);//СȦ
//				LCD_ShowString(60,250,200,16,16,"Number1 step1");
//			if(GetPosX()>900&&GetPosX()<1000&&GetPosY()>0&&GetPosY()<2000)
//				{
//					flag=2;
//				}
//			}
//		}
//	}
//}
void clock_wise_close_LargeToSmall(u8 SideNumber) 
{
	static u8 flag=1;    
/*���ڳ���ȥ���ı���*/
	if(SideNumber==1)            //���ڳ������ϣ�
{
		 if(flag==1)               //flag����1��������СԲ
			{
				closeRound(0,2200, 800,1,4000,1);//СȦ
//				LCD_ShowString(60,250,200,16,16,"Number1 step1");
			if(GetPosX()>900&&GetPosX()<1000&&GetPosY()>0&&GetPosY()<2000)
				{
					flag=2;
				}
			}
			if(flag==2)                               //flag����2�����ж�i
			{
				static u8 i=0;
				if(i==0)                                //iΪ0�����ߴ�Բ
				{
				closeRound(0,2200,1700,1,3500,0);				//��Ȧ
//				LCD_ShowString(60,250,200,16,16,"Number1 step2");
				}
				if(i==1)                                //iΪ1��Ѱ��x=0��ֱ��
				{
//					LCD_ShowString(60,250,200,16,16,"Number1 step2_1");
					straightLine(1,0,0,0,1500);
					if((GetPosY()>1700&&GetPosY()<1800)&&(GetPosX()>-100&&GetPosX()<100))                                               /**********����*******/
					{
						flag=3;           
					}
				}
				  if(GetPosX()>1400&&GetPosX()<1500&&GetPosY()>0&&GetPosY()<2000)                    /**********����*******/
				{
					i=1;
				}
			}
			if(flag==3)             //flagΪ3����ʼ���˿��ߣ�
			{
//				LCD_ShowString(60,250,200,16,16,"Number1 step3");
				back_Turn(0,-2000);
			}
}
/*���ڳ��������ұ���*/
		if(SideNumber==2)            //���ڳ����ߵ��ұߵı��ϣ�
	{
		 if(flag==1)               //flag����1�������ߴ�Բ
			{
				closeRound(0,2200,800,1,4000,0);				//СȦ
//				LCD_ShowString(60,250,200,16,16,"Number1 step1");
				if(GetPosX()>900&&GetPosX()<1000&&GetPosY()>0&&GetPosY()<2000)
				{
					flag=2;
				} 
			}
			if(flag==2)             //flag����2�����ж�i
			{
				static u8 i=0;
				if(i==0)              //iΪ0������СԲ
				{
//				LCD_ShowString(60,250,200,16,16,"Number1 step2");
				closeRound(0,2200,1700,1,3500,1);//СȦ
				}
				if(i==1)              //iΪ1��Ѱ��x=0��ֱ��
				{
//					LCD_ShowString(60,250,200,16,16,"Number1 step2_1");
					straightLine(0,1,-2200,1,1500);
					if((GetPosX()>600&&GetPosX()<700)&&(GetPosY()>2200&&GetPosY()<2300))
					{
						flag=3;           
					}
				}
				  if(GetPosX()>400&&GetPosX()<2400&&GetPosY()>3800&&GetPosY()<3900)
				{
					i=1;
				}
			}
			if(flag==3)             //flagΪ3����ʼ���˿��ߣ�
			{
//				LCD_ShowString(60,250,200,16,16,"Number1 step3");
				back_Turn(90,-2000);
			}
	 }
/*���ڳ������Ķ������*/
			if(SideNumber==3)            //���ڳ����ߵĶ�����ϣ�
	{
		 if(flag==1)               //flag����1��������СԲ
			{
				closeRound(0,2200, 800,1,4000,1);//СȦ
//				LCD_ShowString(60,250,200,16,16,"Number1 step1");
			if(GetPosX()>900&&GetPosX()<1000&&GetPosY()>0&&GetPosY()<2000)
				{
					flag=2;
				}
			}
			if(flag==2)                               //flag����2�����ж�i
			{
				static u8 i=0;
				if(i==0)                                //iΪ0�����ߴ�Բ
				{
				closeRound(0,2200,1700,1,3500,0);				//��Ȧ
//				LCD_ShowString(60,250,200,16,16,"Number1 step2");
				}
				if(i==1)                                //iΪ1��Ѱ��x=0��ֱ��
				{
//					LCD_ShowString(60,250,200,16,16,"Number1 step2_1");
					straightLine(1,0,0,1,1500);
					if((GetPosY()>2700&&GetPosY()<2800)&&(GetPosX()>-100&&GetPosX()<100))                                               /**********����*******/
					{
						flag=3;           
					}
				}
				  if(GetPosX()>-1500&&GetPosX()<-1400&&GetPosY()>2800&&GetPosY()<4800)                    /**********����*******/
				{
					i=1;
				}
			}
			if(flag==3)             //flagΪ3����ʼ���˿��ߣ�
			{
//				LCD_ShowString(60,250,200,16,16,"Number1 step3");
				back_Turn(180,-2000);
			}
	}
	/*���ڳ���������߱���*/
		if(SideNumber==4)            //���ڳ����ߵ�����ϣ�
	{
		 if(flag==1)               //flag����1��������СԲ
			{
				closeRound(0,2200, 800,1,4000,1);//СȦ
//				LCD_ShowString(60,250,200,16,16,"Number1 step1");
			if(GetPosX()>900&&GetPosX()<1000&&GetPosY()>0&&GetPosY()<2000)
				{
					flag=2;
				}
			}
			if(flag==2)                               //flag����2�����ж�i
			{
				static u8 i=0;
				if(i==0)                                //iΪ0�����ߴ�Բ
				{
				closeRound(0,2200,1700,1,3500,0);				//��Ȧ
//				LCD_ShowString(60,250,200,16,16,"Number1 step2");
				}
				if(i==1)                                //iΪ1��Ѱ��x=0��ֱ��
				{
//				LCD_ShowString(60,250,200,16,16,"Number1 step2_1");
					straightLine(0,1,-2200,0,1500);
					if((GetPosY()>2100&&GetPosY()<2200)&&(GetPosX()>-400&&GetPosX()<-300))                                               /**********����*******/
					{
						flag=3;           
					}
				}
				  if(GetPosX()>-2400&&GetPosX()<-400&&GetPosY()>900&&GetPosY()<1000)                    /**********����*******/
				{
					i=1;
				}
			}
			if(flag==3)             //flagΪ3����ʼ���˿��ߣ�
			{
//				LCD_ShowString(60,250,200,16,16,"Number1 step3");
				back_Turn(-90,-2000);
			}
	}
}

void closeRound(float x,float y,float R,float clock,float forwardspeed,u8 Roundsize)
{
    float target_Distance=0;
    float k=0;
    float setangle=0,Agl=0,frontspeed=0;
    target_Distance=sqrt(pow(GetPosX()-x,2)+pow(GetPosY()-y,2))-R;
    k=(GetPosX()-x)/(y-GetPosY());		//ǰ��ֱ��б��
	if(Roundsize==0)//��Ȧ
	{
    PID_param_angle[0]=50.0f;
    PID_param_angle[1]=0.0f;
    PID_param_angle[2]=10.0f;

    PID_param_DisArc[0]=0.05;
    PID_param_DisArc[1]=0;
    PID_param_DisArc[2]=0;
	}
		if(Roundsize==1)//��Ȧ
{
    PID_param_angle[0]=45.0f;
    PID_param_angle[1]=0.0f;
    PID_param_angle[2]=0.0f;

    PID_param_DisArc[0]=0.05;
    PID_param_DisArc[1]=0;
    PID_param_DisArc[2]=0;
	}
if(Roundsize==2)//СȦ
{
    PID_param_angle[0]=30.0f;
    PID_param_angle[1]=0.0f;
    PID_param_angle[2]=0.0f;

    PID_param_DisArc[0]=0.03;
    PID_param_DisArc[1]=0;
    PID_param_DisArc[2]=0;
	}

//	˳1��2
    if(clock==1)
    {
        if(GetPosY()>y)
            Agl=-90+atan(k)*180/PI;
        else if(GetPosY()<y)
            Agl=90+atan(k)*180/PI;
        else if(GetPosY()==y&&GetPosX()>=x)
            Agl=180;
        else if(GetPosY()==y&&GetPosX()<x)
            Agl=0;
        setangle=Agl+Distance_Arc_Pid(target_Distance);//���븳ֵ���Ƕȵ����� AglΪ�Ƕ�ת������
//        if(setangle<0)
//        {   LCD_ShowString(130,50,200,16,16,"setAngle =");
//            LCD_ShowString(130,70,200,16,16,"-");
//            LCD_ShowxNum(130,70,-setangle,6,16,0X80);
//        }
//        else {
//            LCD_ShowString(130,50,200,16,16,"setAngle =");
//            LCD_ShowString(130,70,200,16,16," ");
//            LCD_ShowxNum(130,70,setangle,6,16,0X80);
//        }
        frontspeed=AnglePid(setangle,GetAngle());//�Ƕ�PID���������ٶȲ�ֵ
    }
    else if(clock==2)
    {
        if(GetPosY()>y)
            Agl=90+atan(k)*180/PI;
        else if(GetPosY()<y)
            Agl=-90+atan(k)*180/PI;
        else if(GetPosY()==y&&GetPosX()>=x)
            Agl=0;
        else if(GetPosY()==y&&GetPosX()<x)
            Agl=180;
        setangle=Agl+Distance_Arc_Pid(target_Distance);//���븳ֵ���Ƕȵ�����
        frontspeed=AnglePid(setangle,GetAngle());//�Ƕ�PID���������ٶȲ�ֵ
    }
    motorCMD(forwardspeed+frontspeed,forwardspeed-frontspeed);//�������
}
void VelCrl(unsigned char motorNum,int vel)//�������  �㵽����ƺ���
{
    if(motorNum==1)
		{
        give_motor1(vel);
//		if(vel>0)
//		{
//    LCD_ShowString(60,170,200,16,16,"V left=");
//    LCD_ShowxNum(60,190,vel,6,16,0X80);
//		}
//		else
//		{
//		
//		LCD_ShowString(60,170,200,16,16,"V left=");
//   	LCD_ShowString(60,190,200,16,16,"-");
//		LCD_ShowxNum(60,190,-vel,6,16,0X80);	
//		}
		}
    if(motorNum==2)
		{
        give_motor2(vel);
//		if(vel>0)
//		{
//    LCD_ShowString(60,210,200,16,16,"V right=");
//    LCD_ShowxNum(60,230,vel,6,16,0X80);
//		}
//		else
//		{
//		
//		LCD_ShowString(60,210,200,16,16,"V right=");
//    		LCD_ShowString(60,210,200,16,16,"-");
//			LCD_ShowxNum(60,230,-vel,6,16,0X80);	
//		}
		}
}