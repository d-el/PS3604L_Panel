/*!****************************************************************************
 * @file		parametres.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		08.02.2017
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Parameters table
 */

//			label		,units,prm						,type			,chmod			,def		,min		,max		,step	,bigst	,pow,limtype		,savetype

/* RTC */
parametres(utcTime		,""	,unixTime					,unixDateFrmt	,chmodAlways	,0x58684680	,0x58684680	,0xFFFFFFFF	,86400	,1728000,0	,prmLimConst	,prmStepConst	,prmNotSave)
parametres(utcTime2		,""	,unixTime					,unixTimeFrmt	,chmodAlways	,0			,0			,0			,0		,0		,0	,prmLimVariable	,prmStepVariable,prmNotSave)
//parametres(year			,""	,timeStrct.tm_year			,s32Frmt		,chmodAlways	,117		,117		,217		,1		,1		,0	,prmLimConst	,prmStepConst	,prmNotSave)
//parametres(mon			,""	,timeStrct.tm_mon			,s32Frmt		,chmodAlways	,1			,1			,12			,1		,1		,0	,prmLimConst	,prmStepConst	,prmNotSave)
//parametres(mday			,""	,timeStrct.tm_mday			,s32Frmt		,chmodAlways	,1			,1			,31			,1		,1		,0	,prmLimConst	,prmStepConst	,prmNotSave)
//parametres(hour			,""	,timeStrct.tm_hour			,s32Frmt		,chmodAlways	,0			,0			,23			,1		,1		,0	,prmLimConst	,prmStepConst	,prmNotSave)
//parametres(min			,""	,timeStrct.tm_min			,s32Frmt		,chmodAlways	,0			,0			,59			,1		,10		,0	,prmLimConst	,prmStepConst	,prmNotSave)
//parametres(sec			,""	,timeStrct.tm_sec			,s32Frmt		,chmodAlways	,0			,0			,59			,1		,10		,0	,prmLimConst	,prmStepConst	,prmNotSave)
parametres(timezone		,"" ,fp.fpSet.timezone			,s8Frmt			,chmodAlways	,2			,-12		,12			,1		,1		,0	,prmLimConst	,prmStepConst	,prmEepSys)

//Base Task 0
parametres(bsSet0u		,""	,bs.set[0].u				,u16Frmt		,chmodAlways	,0			,0		,36.000		,0.010	,1.000	,3	,prmLimConst	,prmStepConst	,prmEep)
parametres(bsSet0i		,""	,bs.set[0].i				,u16Frmt		,chmodAlways	,0			,0		,4.000		,0.001	,0.500	,3	,prmLimConst	,prmStepConst	,prmEep)
parametres(bsSet0m		,""	,bs.set[0].mode				,u8Frmt			,chmodAlways	,0			,0		,2			,1		,1		,0	,prmLimConst	,prmStepConst	,prmEep)
//Base Task 1
parametres(bsSet1u		,""	,bs.set[1].u				,u16Frmt		,chmodAlways	,0			,0		,36.000		,0.010	,1.000	,3	,prmLimConst	,prmStepConst	,prmEep)
parametres(bsSet1i		,""	,bs.set[1].i				,u16Frmt		,chmodAlways	,0			,0		,4.000		,0.001	,0.500	,3	,prmLimConst	,prmStepConst	,prmEep)
parametres(bsSet1m		,""	,bs.set[1].mode				,u8Frmt			,chmodAlways	,0			,0		,2			,1		,1		,0	,prmLimConst	,prmStepConst	,prmEep)
//Base Task 2
parametres(bsSet2u		,""	,bs.set[2].u				,u16Frmt		,chmodAlways	,0			,0		,36.000		,0.010	,1.000	,3	,prmLimConst	,prmStepConst	,prmEep)
parametres(bsSet2i		,""	,bs.set[2].i				,u16Frmt		,chmodAlways	,0			,0		,4.000		,0.001	,0.500	,3	,prmLimConst	,prmStepConst	,prmEep)
parametres(bsSet2m		,""	,bs.set[2].mode				,u8Frmt			,chmodAlways	,0			,0		,2			,1		,1		,0	,prmLimConst	,prmStepConst	,prmEep)

//Charger Task
parametres(chu			,""	,ch.u						,u16Frmt		,chmodAlways	,0			,0		,36.000		,0.010	,1.000	,3	,prmLimConst	,prmStepConst	,prmEep)
parametres(chi			,""	,ch.i						,u16Frmt		,chmodAlways	,0			,0		,4.000		,0.001	,0.500	,3	,prmLimConst	,prmStepConst	,prmEep)
parametres(cht			,""	,ch.t						,u16Frmt		,chmodAlways	,0			,0		,4.000		,0.001	,0.500	,3	,prmLimConst	,prmStepConst	,prmEep)
parametres(chm			,""	,ch.mode					,u8Frmt			,chmodAlways	,0			,0		,1			,1		,1		,0	,prmLimConst	,prmStepConst	,prmEep)

//Regulator task
parametres(regu			,""	,fp.tf.task.u				,u32Frmt		,chmodAlways	,0			,0		,36.0000	,0.0001	,0.5000	,6	,prmLimConst	,prmStepConst	,prmNotSave)
parametres(regi			,""	,fp.tf.task.i				,u32Frmt		,chmodAlways	,0			,0		,4.0000		,0.00001,0.1000	,6	,prmLimConst	,prmStepConst	,prmNotSave)
parametres(regdacu		,""	,fp.tf.task.dacu			,u16Frmt		,chmodAlways	,0			,0		,4095		,1		,100	,0	,prmLimConst	,prmStepConst	,prmNotSave)
parametres(regdaci		,""	,fp.tf.task.daci			,u16Frmt		,chmodAlways	,0			,0		,4095		,1		,100	,0	,prmLimConst	,prmStepConst	,prmNotSave)

//Regulator meas
parametres(temperature	,""	,fp.tf.meas.temperatureLin	,u16Frmt		,chmodNone		,0			,0		,0			,1		,1		,1	,prmLimConst	,prmStepConst	,prmNotSave)
parametres(meas_uin		,""	,fp.tf.meas.uin				,u16Frmt		,chmodNone		,0			,0		,0			,1		,1		,3	,prmLimConst	,prmStepConst	,prmNotSave)
parametres(meas_u		,""	,fp.tf.meas.u				,u32Frmt		,chmodNone		,0			,0		,0			,1		,1		,6	,prmLimConst	,prmStepConst	,prmNotSave)
parametres(meas_i		,""	,fp.tf.meas.i				,u32Frmt		,chmodNone		,0			,0		,0			,1		,1		,6	,prmLimConst	,prmStepConst	,prmNotSave)
parametres(meas_adcu	,""	,fp.tf.meas.adcu			,u16Frmt		,chmodNone		,0			,0		,0			,1		,1		,0	,prmLimConst	,prmStepConst	,prmNotSave)
parametres(meas_adci	,""	,fp.tf.meas.adci			,u16Frmt		,chmodNone		,0			,0		,0			,1		,1		,0	,prmLimConst	,prmStepConst	,prmNotSave)

//Panel settings
parametres(startcnt		,""	,startCounter				,u32Frmt		,chmodAlways	,0			,0		,0xFFFFFFFF	,1		,1		,0	,prmLimConst	,prmStepConst	,prmEep)
parametres(brightness	,""	,fp.fpSet.lcdLight			,u16Frmt		,chmodAlways	,60.0		,10.0	,100.0		,0.1	,10.0	,1	,prmLimConst	,prmStepConst	,prmEepSys)
parametres(ipadr		,""	,fp.fpSet.ipadr				,ipAdrFrmt		,chmodAlways	,0xC0A8010A	,0		,0xFFFFFFFF	,1		,1		,0	,prmLimVariable	,prmStepVariable,prmEepSys) /* 192.168.1.15 */
parametres(netmask		,""	,fp.fpSet.netmask			,ipAdrFrmt		,chmodAlways	,0xFFFFFF00	,0		,0xFFFFFFFF	,1		,1		,0	,prmLimVariable	,prmStepVariable,prmEepSys) /* 255.255.255.0 */
parametres(gateway		,""	,fp.fpSet.gateway			,ipAdrFrmt		,chmodAlways	,0xC0A80101	,0		,0xFFFFFFFF	,1		,1		,0	,prmLimVariable	,prmStepVariable,prmEepSys) /* 192.168.1.1 */

/******************************** END OF FILE ********************************/
