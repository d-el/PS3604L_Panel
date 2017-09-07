/*!****************************************************************************
 * @file		parametres.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		08.02.2017
 * @copyright	GNU Lesser General Public License v3
 * @brief		Parameters table
 */

//			label		,units,prm						,type			,chmo			d,def	,min	,max		,step	,bigstep,power,save

/* RTC */
parametres(utcTime		,""	,unixTime					,unixDateFrmt	,chmodNone		,0		,0		,0			,0		,0		,0	,prmNotSave)
parametres(utcTime2		,""	,unixTime					,unixTimeFrmt	,chmodNone		,0		,0		,0			,0		,0		,0	,prmNotSave)
parametres(year			,""	,timeStrct.tm_year			,s32Frmt		,chmodAlways	,117	,117	,217		,1		,1		,0	,prmNotSave)
parametres(mon			,""	,timeStrct.tm_mon			,s32Frmt		,chmodAlways	,1		,1		,12			,1		,1		,0	,prmNotSave)
parametres(mday			,""	,timeStrct.tm_mday			,s32Frmt		,chmodAlways	,1		,1		,31			,1		,1		,0	,prmNotSave)
parametres(hour			,""	,timeStrct.tm_hour			,s32Frmt		,chmodAlways	,0		,0		,23			,1		,1		,0	,prmNotSave)
parametres(min			,""	,timeStrct.tm_min			,s32Frmt		,chmodAlways	,0		,0		,59			,1		,10		,0	,prmNotSave)
parametres(sec			,""	,timeStrct.tm_sec			,s32Frmt		,chmodAlways	,0		,0		,59			,1		,10		,0	,prmNotSave)

//Base Task 0
parametres(bsSet0u		,""	,bs.set[0].u				,u16Frmt		,chmodAlways	,0		,0		,36.000		,0.010	,1.000	,3	,prmEeprom)
parametres(bsSet0i		,""	,bs.set[0].i				,u16Frmt		,chmodAlways	,0		,0		,4.000		,0.001	,0.500	,3	,prmEeprom)
parametres(bsSet0m		,""	,bs.set[0].mode				,u8Frmt			,chmodAlways	,0		,0		,2			,1		,1		,0	,prmEeprom)
//Base Task 1
parametres(bsSet1u		,""	,bs.set[1].u				,u16Frmt		,chmodAlways	,0		,0		,36.000		,0.010	,1.000	,3	,prmEeprom)
parametres(bsSet1i		,""	,bs.set[1].i				,u16Frmt		,chmodAlways	,0		,0		,4.000		,0.001	,0.500	,3	,prmEeprom)
parametres(bsSet1m		,""	,bs.set[1].mode				,u8Frmt			,chmodAlways	,0		,0		,2			,1		,1		,0	,prmEeprom)
//Base Task 2
parametres(bsSet2u		,""	,bs.set[2].u				,u16Frmt		,chmodAlways	,0		,0		,36.000		,0.010	,1.000	,3	,prmEeprom)
parametres(bsSet2i		,""	,bs.set[2].i				,u16Frmt		,chmodAlways	,0		,0		,4.000		,0.001	,0.500	,3	,prmEeprom)
parametres(bsSet2m		,""	,bs.set[2].mode				,u8Frmt			,chmodAlways	,0		,0		,2			,1		,1		,0	,prmEeprom)

//Charger Task
parametres(chu			,""	,ch.u						,u16Frmt		,chmodAlways	,0		,0		,36.000		,0.010	,1.000	,3	,prmEeprom)
parametres(chi			,""	,ch.i						,u16Frmt		,chmodAlways	,0		,0		,4.000		,0.001	,0.500	,3	,prmEeprom)
parametres(cht			,""	,ch.t						,u16Frmt		,chmodAlways	,0		,0		,4.000		,0.001	,0.500	,3	,prmEeprom)
parametres(chm			,""	,ch.mode					,u8Frmt			,chmodAlways	,0		,0		,1			,1		,1		,0	,prmEeprom)

//Regulator task
parametres(regu			,""	,fp.tf.task.u				,u32Frmt		,chmodAlways	,0		,0		,36.0000	,0.0001	,0.5000	,6	,prmNotSave)
parametres(regi			,""	,fp.tf.task.i				,u32Frmt		,chmodAlways	,0		,0		,4.0000		,0.00001,0.1000	,6	,prmNotSave)
parametres(regdacu		,""	,fp.tf.task.dacu			,u16Frmt		,chmodAlways	,0		,0		,4095		,1		,100	,0	,prmNotSave)
parametres(regdaci		,""	,fp.tf.task.daci			,u16Frmt		,chmodAlways	,0		,0		,4095		,1		,100	,0	,prmNotSave)

//Regulator meas
parametres(temperature	,""	,fp.tf.meas.temperatureLin	,u16Frmt		,chmodNone		,0		,0		,0			,1		,1		,1	,prmNotSave)
parametres(meas_uin		,""	,fp.tf.meas.uin				,u16Frmt		,chmodNone		,0		,0		,0			,1		,1		,3	,prmNotSave)
parametres(meas_u		,""	,fp.tf.meas.u				,u32Frmt		,chmodNone		,0		,0		,0			,1		,1		,6	,prmNotSave)
parametres(meas_i		,""	,fp.tf.meas.i				,u32Frmt		,chmodNone		,0		,0		,0			,1		,1		,6	,prmNotSave)
parametres(meas_adcu	,""	,fp.tf.meas.adcu			,u16Frmt		,chmodNone		,0		,0		,0			,1		,1		,0	,prmNotSave)
parametres(meas_adci	,""	,fp.tf.meas.adci			,u16Frmt		,chmodNone		,0		,0		,0			,1		,1		,0	,prmNotSave)

//Panel settings
parametres(brightness	,""	,fp.fpSet.lcdLight			,u16Frmt		,chmodAlways	,60.0	,10.0	,100.0		,0.1	,10.0	,1	,prmEeprom)

/*************** LGPL ************** END OF FILE *********** D_EL ************/
