/*!****************************************************************************
 * @file    setting.h 
 * @author  Storozhenko Roman - D_EL
 * @version V1.0.0
 * @date    
 * @brief   
 */
#ifndef SETTING_H
#define SETTING_H

/*!****************************************************************************
 * User define
 */

/*!****************************************************************************
 * User enum
 */
enum {
	VAR_Uust, VAR_Udac, VAR_Iust, VAR_Idac
};

enum {
	UpUST_VAR_Point, UpUST_VAR_Uset, UpUST_VAR_Udac
};

enum {
	IUST_VAR_Udac, IUST_VAR_Ikor
};

enum {
	IpUST_VAR_Point, IpUST_VAR_Iset, IpUST_VAR_Udac
};

enum {
	LUST_VAR_Udac, LUST_VAR_Lkor, LUST_VAR_Ilim
};

/*!****************************************************************************
 * Prototypes for the functions
 */
void Adjustment(void);
void SpsC(void);
void Beep(void);
void FanPWM(void);
void key(void);
void CPUID(void);
void RTC_Set(void);
void ZeNeR(void);
void Palette(void);
uint8_t Setting(void);

#endif //SETTING_H
/*************** (C) COPYRIGHT ************** END OF FILE ********* D_EL *****/
