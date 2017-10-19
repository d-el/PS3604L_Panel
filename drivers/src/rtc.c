/*!****************************************************************************
 * @file		storage.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		05.12.2015
 * @copyright	GNU Lesser General Public License v3
 * @brief		RTC driver
 */

/*!****************************************************************************
 * Include
 */
#include "rtc.h"

/*!****************************************************************************
 * MEMORY
 */
time_t 		unixTime;	//At 00:00 hours, Jan 1, 1970 UTC
struct tm 	timeStrct;

/*!****************************************************************************
 * @brief
 * @param
 * @retval
 */
rtcStatus_type rtc_init(void){
	volatile uint32_t timeout;

	if((RCC->BDCR & RCC_BDCR_RTCEN) == 0){					//RTC clock is enable
		RCC->APB1ENR 	|= RCC_APB1ENR_PWREN;				//Включить тактирование PWR и Backup
		PWR->CR			|= PWR_CR_DBP;						//Разрешить доступ к Backup области
		RCC->BDCR 		|= RCC_BDCR_BDRST;					//Сбросить Backup область
		RCC->BDCR 		&= ~RCC_BDCR_BDRST;

		RCC->BDCR 		&= ~RCC_BDCR_RTCEN;					//RTC clock enable
		RCC->BDCR 		|= RCC_BDCR_RTCEN;					//RTC clock enable

		RCC->BDCR 		|= RCC_BDCR_RTCSEL_0;				//LSE oscillator clock used as RTC clock
		RCC->BDCR 		|= RCC_BDCR_LSEON;					//LSE oscillator enable
		timeout = RTC_LSE_TIMEOUT;
		while(((RCC->BDCR & RCC_BDCR_LSERDY) == 0) && (timeout != 0)){
			timeout--;                          						//Wait for LSE ready
		}
		if(timeout == 0){
			RCC->BDCR &= ~RCC_BDCR_RTCEN;
			return rtc_error;
		}

		while((RTC->ISR & RTC_ISR_RSF) == 0)
			;           //Wait for RTC APB registers synchronisation

		RTC->WPR = 0xCA;                             		//Unlock write protection
		RTC->WPR = 0x53;                             		//Unlock write protection
		RTC->ISR = 0xffffff;
		RTC->ISR |= RTC_ISR_INIT;                    		//Enter initialization mode

		while((RTC->ISR & RTC_ISR_INITF) == 0)
			;         //Wait for calendar registers update is allowed

		RTC->TR 	= 0x000000;                             //Setting time to 00.00.00
		RTC->DR 	= 0x170101;								//Set date to  2017-01-01
		RTC->CR 	&= ~RTC_CR_FMT;                        	//Set FMT 24H format

		RTC->ISR 	&= ~RTC_ISR_INIT;                      	//Exit initialization mode

		RTC->WPR 	= 0xFF;                                	//Enable the write protection for RTC registers
		PWR->CR 	&= ~PWR_CR_DBP;                       	//Запреть доступ к Backup области
		return rtc_Ok;
	}else{
		return rtc_wasOn;
	}
}

/*!****************************************************************************
 * @brief
 * @param
 * @retval
 */
void rtc_getTime(struct tm *time){
	uint32_t rtctr = RTC->TR;
	uint32_t rtcdr = RTC->DR;

	time->tm_sec = ((rtctr & RTC_TR_SU) >> RTC_TR_SU_Pos) + ((rtctr & RTC_TR_ST) >> RTC_TR_ST_Pos) * 10;

	time->tm_min = ((rtctr & RTC_TR_MNU) >> RTC_TR_MNU_Pos) + ((rtctr & RTC_TR_MNT) >> RTC_TR_MNT_Pos) * 10;

	time->tm_hour = ((rtctr & RTC_TR_HU) >> RTC_TR_HU_Pos) + ((rtctr & RTC_TR_HT) >> RTC_TR_HT_Pos) * 10;

	time->tm_mday = ((rtcdr & RTC_DR_DU) >> RTC_DR_DU_Pos) + ((rtcdr & RTC_DR_DT) >> RTC_DR_DT_Pos) * 10;

	time->tm_mon = ((rtcdr & RTC_DR_MU) >> RTC_DR_MU_Pos) + ((rtcdr & RTC_DR_MT) >> RTC_DR_MT_Pos) * 10;

	time->tm_wday = ((rtcdr & RTC_DR_WDU) >> RTC_DR_WDU_Pos);

	time->tm_year = ((rtcdr & RTC_DR_YU) >> RTC_DR_YU_Pos) + ((rtcdr & RTC_DR_YT) >> RTC_DR_YT_Pos) * 10 + 100;

	time->tm_yday = 0;
	time->tm_isdst = 0;

	mktime(time);
}

/*!****************************************************************************
 * @brief
 * @param
 * @retval
 */
time_t time(time_t *arg){
	time_t time;
	struct tm tm;

	rtc_getTime(&tm);
	time = mktime(&tm);

	if(arg != NULL){
		*arg = time;
	}
	return time;
}

/*!****************************************************************************
 * @brief
 * @param
 * @retval
 */
static uint8_t decToBcd(uint32_t x){
	return ((x / 10) << 4) | (x % 10);
}

/*!****************************************************************************
 * @brief
 * @param
 * @retval
 */
rtcStatus_type rtc_setTime(const struct tm *t){
	uint32_t rtctr = 0;
	uint32_t rtcdr = 0;

	if((t->tm_sec > 59) || (t->tm_min > 59) || (t->tm_hour > 23) || (t->tm_mday > 31) || (t->tm_mon > 12) || (t->tm_year < 100) || (t->tm_year > 200)){
		return rtc_error;
	}

	rtctr |= ((uint32_t) decToBcd(t->tm_sec) << RTC_TR_SU_Pos);                       // & (RTC_TR_SU & RTC_TR_ST);
	rtctr |= ((uint32_t) decToBcd(t->tm_min) << RTC_TR_MNU_Pos);                       // & (RTC_TR_MNU & RTC_TR_MNT);
	rtctr |= ((uint32_t) decToBcd(t->tm_hour) << RTC_TR_HU_Pos);                       // & (RTC_TR_MNU & RTC_TR_MNT);

	rtcdr |= ((uint32_t) decToBcd(t->tm_mday) << RTC_DR_DU_Pos);
	rtcdr |= ((uint32_t) decToBcd(t->tm_mon) << RTC_DR_MU_Pos);
	rtcdr |= ((uint32_t) (decToBcd(t->tm_year - 100)) << RTC_DR_YU_Pos);

	PWR->CR |= PWR_CR_DBP;                     	//Unlock к Backup
	while((RTC->ISR & RTC_ISR_RSF) == 0)
		;           //Wait for RTC APB registers synchronisation
	RTC->WPR = 0xCA;                             	//Unlock write protection
	RTC->WPR = 0x53;                             	//Unlock write protection
	RTC->ISR = 0xffffff;
	RTC->ISR |= RTC_ISR_INIT;                    	//Enter initialization mode
	while((RTC->ISR & RTC_ISR_INITF) == 0)
		;         //Wait for calendar registers update is allowed

	RTC->TR = rtctr;                                //Setting time
	RTC->DR = rtcdr;                                //Set date

	RTC->ISR &= ~RTC_ISR_INIT;                      //Exit initialization mode
	RTC->WPR = 0xFF;                                //Enable the write protection for RTC registers
	PWR->CR &= ~PWR_CR_DBP;                    	//Lock Backup

	return rtc_setOk;
}

/*!****************************************************************************
 * @brief
 * @param
 * @retval
 */
rtcStatus_type rtc_setTimeUnix(time_t timeUnix){
	struct tm time;

	gmtime_r(&timeUnix, &time);
	rtc_setTime(&time);

	return rtc_setOk;
}

/*!****************************************************************************
 * @brief
 * @param
 * @retval
 */
void rtc_writeToBkp(uint32_t val){
	PWR->CR |= PWR_CR_DBP;                     //Разрешить доступ к Backup области
	while((RTC->ISR & RTC_ISR_RSF) == 0)
		;           //Wait for RTC APB registers synchronisation
	RTC->WPR = 0xCA;                             //Unlock write protection
	RTC->WPR = 0x53;                             //Unlock write protection

	RTC->BKP0R = val;

	RTC->WPR = 0xFF;							//Enable the write protection for RTC registers
	PWR->CR &= ~PWR_CR_DBP;                   //Запреть доступ к Backup области
}

/*!****************************************************************************
 * @brief
 * @param
 * @retval
 */
uint32_t rtc_ReadFromBkp(void){
	return RTC->BKP0R;
}

/*************** LGPL ************** END OF FILE *********** D_EL ************/
