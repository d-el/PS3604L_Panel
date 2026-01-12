/*!****************************************************************************
 * @file		rtc.c
 * @author		d_el
 * @version		V1.1
 * @date		13.02.2018
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Driver RTC module
 */

/*!****************************************************************************
 * Include
 */
#include "stm32f4xx.h"
#include "stdint.h"
#include "rtc.h"

/*!****************************************************************************
 * Define
 */
#define RTC_LSE_TIMEOUT				100000000
#define RTC_OFFSET_YEAR				1970
#define RTC_HOUR_PER_DAY			24
#define RTC_MINUTE_PER_HOUR			60
#define RTC_SECONDS_PER_DAY			86400
#define RTC_SECONDS_PER_HOUR		3600
#define RTC_SECONDS_PER_MINUTE		60

/*!****************************************************************************
 * MEMORY
 */
const uint8_t TM_RTC_Months[2][12] = {		///< Days in a month
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},	// Not leap year
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}	// Leap year
};

/*!****************************************************************************
 * @brief
 */
static uint32_t binToBcd(uint32_t x){
	return ((x / 10) << 4) | (x % 10);
}

/*!****************************************************************************
 * @brief
 */
static uint8_t yearIsLeap(uint16_t year){
	return (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0)) ? 1 : 0;

}

/*!****************************************************************************
 * @brief
 */
static uint16_t getDaysInYear(uint16_t year){
	return yearIsLeap(year) ? 366 : 365;
}

/*!****************************************************************************
 * @brief	Convert time to UNIX Time
 * @param	tm - Time structure
 * @return	Number of seconds since 00:00, Jan 1 1970 UTC, corresponding to POSIX time
 */
static time_t tmToUnixTime(struct tm *tm){
	time_t seconds = 0;
	uint32_t days = 0;
	uint16_t year = tm->tm_year + 1900;

	// Year is below offset year
	if(year < RTC_OFFSET_YEAR){
		return 0;
	}
	// Days in back years
	for(uint32_t i = RTC_OFFSET_YEAR; i < year; i++){
		days += getDaysInYear(i);
	}
	// Days in current year
	for(int32_t i = 0; i < tm->tm_mon; i++){
		if(yearIsLeap(year) != 0){
			days += TM_RTC_Months[1][i];
		}else{
			days += TM_RTC_Months[0][i];
		}

	}
	// Day starts with 1
	days += tm->tm_mday - 1;
	seconds = days * RTC_SECONDS_PER_DAY;
	seconds += tm->tm_hour * RTC_SECONDS_PER_HOUR;
	seconds += tm->tm_min * RTC_SECONDS_PER_MINUTE;
	seconds += tm->tm_sec;

	return seconds;
}

/*!****************************************************************************
 * @brief
 * @param
 * @retval
 */
bool rtc_init(void){
	volatile uint32_t timeout;

	if((RCC->BDCR & RCC_BDCR_RTCEN) == 0){					//RTC clock is enable
		RCC->APB1ENR	|= RCC_APB1ENR_PWREN;				//Включить тактирование PWR и Backup
		PWR->CR			|= PWR_CR_DBP;						//Разрешить доступ к Backup области
		RCC->BDCR		|= RCC_BDCR_BDRST;					//Сбросить Backup область
		RCC->BDCR		&= ~RCC_BDCR_BDRST;

		RCC->BDCR		&= ~RCC_BDCR_RTCEN;					//RTC clock enable
		RCC->BDCR		|= RCC_BDCR_RTCEN;					//RTC clock enable

		RCC->BDCR		|= RCC_BDCR_RTCSEL_0;				//LSE oscillator clock used as RTC clock
		RCC->BDCR		|= RCC_BDCR_LSEON;					//LSE oscillator enable
		timeout = RTC_LSE_TIMEOUT;
		while(((RCC->BDCR & RCC_BDCR_LSERDY) == 0) && (timeout != 0)){
			timeout--;													//Wait for LSE ready
		}
		if(timeout == 0){
			RCC->BDCR &= ~RCC_BDCR_RTCEN;
			return false;
		}

		while((RTC->ISR & RTC_ISR_RSF) == 0)
			;			//Wait for RTC APB registers synchronization

		RTC->WPR = 0xCA;									//Unlock write protection
		RTC->WPR = 0x53;									//Unlock write protection
		RTC->ISR = 0xffffff;								//Clear all flag
		RTC->ISR |= RTC_ISR_INIT;							//Enter initialization mode

		while((RTC->ISR & RTC_ISR_INITF) == 0)
			;		  //Wait for calendar registers update is allowed

		RTC->TR		= 0x000000;								//Setting time to 00.00.00
		RTC->DR		= 0x260101;								//Set date
		RTC->CR		&= ~RTC_CR_FMT;							//Set FMT 24H format

		RTC->ISR	&= ~RTC_ISR_INIT;						//Exit initialization mode

		RTC->WPR	= 0xFF;									//Enable the write protection for RTC registers
		PWR->CR		&= ~PWR_CR_DBP;							//Disable Backup Domain write protection
	}
	return true;
}

/*!****************************************************************************
 * @brief
 * @param
 * @retval
 */
time_t time(time_t *arg){
	time_t unixTime;
	struct tm tm;
	uint32_t rtctr = RTC->TR;
	uint32_t rtcdr = RTC->DR;

	tm.tm_sec = ((rtctr & RTC_TR_SU) >> RTC_TR_SU_Pos) + ((rtctr & RTC_TR_ST) >> RTC_TR_ST_Pos) * 10;
	tm.tm_min = ((rtctr & RTC_TR_MNU) >> RTC_TR_MNU_Pos) + ((rtctr & RTC_TR_MNT) >> RTC_TR_MNT_Pos) * 10;
	tm.tm_hour = ((rtctr & RTC_TR_HU) >> RTC_TR_HU_Pos) + ((rtctr & RTC_TR_HT) >> RTC_TR_HT_Pos) * 10;
	tm.tm_mday = ((rtcdr & RTC_DR_DU) >> RTC_DR_DU_Pos) + ((rtcdr & RTC_DR_DT) >> RTC_DR_DT_Pos) * 10;
	tm.tm_mon = ((rtcdr & RTC_DR_MU) >> RTC_DR_MU_Pos) + ((rtcdr & RTC_DR_MT) >> RTC_DR_MT_Pos) * 10 - 1;
	tm.tm_wday = ((rtcdr & RTC_DR_WDU) >> RTC_DR_WDU_Pos);
	tm.tm_year = ((rtcdr & RTC_DR_YU) >> RTC_DR_YU_Pos) + ((rtcdr & RTC_DR_YT) >> RTC_DR_YT_Pos) * 10 + 100;

	unixTime = tmToUnixTime(&tm);

	if(arg != NULL){
		*arg = unixTime;
	}
	return unixTime;
}

/*!****************************************************************************
 * @brief
 * @param
 * @retval
 */
bool rtc_setTime(const struct tm *t){
	uint32_t rtctr = 0;
	uint32_t rtcdr = 0;

	if((RCC->BDCR & RCC_BDCR_RTCEN) == 0 || (RCC->BDCR & RCC_BDCR_LSERDY) == 0){
		return false;
	}

	if((t->tm_sec > 59) || (t->tm_min > 59) || (t->tm_hour > 23) || (t->tm_mday > 31) || (t->tm_mon > 12) || (t->tm_year < 100) || (t->tm_year > 200)){
		return false;
	}

	rtctr |= binToBcd(t->tm_sec) << RTC_TR_SU_Pos;
	rtctr |= binToBcd(t->tm_min) << RTC_TR_MNU_Pos;
	rtctr |= binToBcd(t->tm_hour) << RTC_TR_HU_Pos;
	rtcdr |= binToBcd(t->tm_mday) << RTC_DR_DU_Pos;
	rtcdr |= binToBcd(t->tm_mon + 1) << RTC_DR_MU_Pos;
	rtcdr |= binToBcd(t->tm_year - 100) << RTC_DR_YU_Pos;

	PWR->CR |= PWR_CR_DBP;							//Unlock к Backup
	while((RTC->ISR & RTC_ISR_RSF) == 0)
		;			//Wait for RTC APB registers synchronization
	RTC->WPR = 0xCA;								//Unlock write protection
	RTC->WPR = 0x53;								//Unlock write protection
	RTC->ISR = 0xffffff;
	RTC->ISR |= RTC_ISR_INIT;						//Enter initialization mode
	while((RTC->ISR & RTC_ISR_INITF) == 0)
		;			//Wait for calendar registers update is allowed

	RTC->TR = rtctr;								//Setting time
	RTC->DR = rtcdr;								//Set date

	RTC->ISR &= ~RTC_ISR_INIT;						//Exit initialization mode
	RTC->WPR = 0xFF;								//Enable the write protection for RTC registers
	PWR->CR &= ~PWR_CR_DBP;							//Lock Backup

	return true;
}

/*!****************************************************************************
 * @brief
 * @param
 * @retval
 */
bool rtc_setTimeUnix(time_t timeUnix){
	struct tm tm;
	gmtime_r(&timeUnix, &tm);
	return rtc_setTime(&tm);
}

/******************************** END OF FILE ********************************/
