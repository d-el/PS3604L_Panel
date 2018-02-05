/*!****************************************************************************
 * @file		menuTree.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		10.02.2016
 * @copyright	GNU Lesser General Public License v3
 * @brief		Parameters table
 */

/*		name 			,label		,units	,prmHandle			,chmod				,pfPrm	,pfChanges	,pfSelect	,pfUnselect	,pfPeriod	,previous	,next		,parent		,child	*/
MENU_ITEM(voltmeter		,"Vmeter"	,""		,NULL				,chmodMenuAlways	,0		,NULL		,NULL		,NULL		,NULL		,voltmeter	,ammeter	,voltmeter	,vPoint1)
	/*		name 			,label		,units	,prmHandle			,chmod				,pfPrm	,pfChanges	,pfSelect	,pfUnselect	,pfPeriod	,previous	,next		,parent		,child	*/
	MENU_ITEM(vPoint1		,"Point1"	,""		,NULL				,chmodMenuAlways	,0		,NULL		,PrepareU	,regSave	,NULL		,vPoint1	,vPoint2	,voltmeter	,vSetU)
	MENU_ITEM(vPoint2		,"Point2"	,""		,NULL				,chmodMenuAlways	,1		,NULL		,PrepareU	,regSave	,NULL		,vPoint1	,vPoint3	,voltmeter	,vSetU)
	MENU_ITEM(vPoint3		,"Point3"	,""		,NULL				,chmodMenuAlways	,2		,NULL		,PrepareU	,regSave	,NULL		,vPoint2	,vPoint4	,voltmeter	,vSetU)
	MENU_ITEM(vPoint4		,"Point4"	,""		,NULL				,chmodMenuAlways	,3		,NULL		,PrepareU	,regSave	,NULL		,vPoint3	,vPoint4	,voltmeter	,vSetU)
		/*		name 			,label		,units	,prmHandle			,chmod				,pfPrm	,pfChanges	,pfSelect	,pfUnselect	,pfPeriod	,previous	,next		,parent		,child	*/
		MENU_ITEM(vSetU			,"Ureal"	,"V"	,&prmh[Nregu]		,chmodMenuAlways	,0		,savePointU	,NULL		,NULL		,NULL		,vSetU		,vSetDacU	,vPoint1	,vSetU)
		MENU_ITEM(vSetDacU		,"DacU"		,"lsb"	,&prmh[Nregdacu]	,chmodMenuAlways	,0		,savePointU	,NULL		,NULL		,NULL		,vSetU		,vSetAdcU	,vPoint1	,vSetDacU)
		MENU_ITEM(vSetAdcU		,"AdcU"		,"lsb"	,&prmh[Nmeas_adcu]	,chmodMenuNone		,0		,NULL		,NULL		,NULL		,NULL		,vSetDacU	,vSetAdcI	,vPoint1	,vSetAdcU)
		MENU_ITEM(vSetAdcI		,"AdcI"		,"lsb"	,&prmh[Nmeas_adci]	,chmodMenuNone		,0		,NULL		,NULL		,NULL		,NULL		,vSetAdcU	,vSetMEasU	,vPoint1	,vSetAdcI)
		MENU_ITEM(vSetMEasU		,"Umeas"	,"V"	,&prmh[Nmeas_u]		,chmodMenuNone		,0		,NULL		,NULL		,NULL		,NULL		,vSetAdcI	,vSetMEasU	,vPoint1	,vSetMEasU)

/*		name 			,label		,units	,prmHandle			,chmod				,pfPrm	,pfChanges	,pfSelect	,pfUnselect	,pfPeriod	,previous	,next		,parent		,child	*/
MENU_ITEM(ammeter		,"Ameter"	,""		,NULL				,chmodMenuAlways	,0		,NULL		,NULL		,NULL		,NULL		,voltmeter	,datetime	,ammeter	,iPoint1)
	/*		name 			,label		,units	,prmHandle			,chmod				,pfPrm	,pfChanges	,pfSelect	,pfUnselect	,pfPeriod	,previous	,next		,parent		,child	*/
	MENU_ITEM(iPoint1		,"Point1"	,""		,NULL				,chmodMenuAlways	,0		,NULL		,PrepareI	,regSave	,NULL		,iPoint1	,iPoint2	,voltmeter	,iSetI)
	MENU_ITEM(iPoint2		,"Point2"	,""		,NULL				,chmodMenuAlways	,1		,NULL		,PrepareI	,regSave	,NULL		,iPoint1	,iPoint3	,voltmeter	,iSetI)
	MENU_ITEM(iPoint3		,"Point3"	,""		,NULL				,chmodMenuAlways	,2		,NULL		,PrepareI	,regSave	,NULL		,iPoint2	,iPoint4	,voltmeter	,iSetI)
	MENU_ITEM(iPoint4		,"Point4"	,""		,NULL				,chmodMenuAlways	,3		,NULL		,PrepareI	,regSave	,NULL		,iPoint3	,iPoint4	,voltmeter	,iSetI)
		/*		  name 			,label		,units	,prmHandle			,chmod				,pfPrm	,pfChanges	,pfSelect	,pfUnselect	,pfPeriod	,previous	,next		,parent		,child	*/
		MENU_ITEM(iSetI			,"Ireal"	,"A"	,&prmh[Nregi]		,chmodMenuAlways	,0		,savePointI	,NULL		,NULL		,NULL		,iSetI		,iSetDacI	,iPoint1	,iSetI)
		MENU_ITEM(iSetDacI		,"DacI"		,"lsb"	,&prmh[Nregdaci]	,chmodMenuAlways	,0		,savePointI	,NULL		,NULL		,NULL		,iSetI		,iSetAdcU	,iPoint1	,iSetDacI)
		MENU_ITEM(iSetAdcU		,"AdcU"		,"lsb"	,&prmh[Nmeas_adcu]	,chmodMenuNone		,0		,NULL		,NULL		,NULL		,NULL		,iSetDacI	,iSetAdcI	,iPoint1	,iSetAdcU)
		MENU_ITEM(iSetAdcI		,"AdcI"		,"lsb"	,&prmh[Nmeas_adci]	,chmodMenuNone		,0		,NULL		,NULL		,NULL		,NULL		,iSetAdcU	,iSetMEasI	,iPoint1	,iSetAdcI)
		MENU_ITEM(iSetMEasI		,"Imeas"	,"A"	,&prmh[Nmeas_i]		,chmodMenuNone		,0		,NULL		,NULL		,NULL		,NULL		,iSetAdcI	,iSetMEasI	,iPoint1	,iSetMEasI)

/*		  name 			,label		,units	,prmHandle		,chmod				,pfPrm	,pfChanges	,pfSelect	,pfUnselect	,pfPeriod	,previous	,next		,parent		,child	*/
MENU_ITEM(datetime		,"Clock"	,""		,NULL			,chmodMenuAlways	,0		,NULL		,rtcSelect	,rtcUnselect,NULL		,ammeter	,lan		,datetime	,mdate)
	/*		name 		,label		,units	,prmHandle			,chmod				,pfPrm	,pfChanges	,pfSelect	,pfUnselect	,pfPeriod	,previous	,next		,parent		,child	*/
	MENU_ITEM(mdate		,"Date"		,""		,&prmh[NutcTime]	,chmodMenuAlways	,0		,NULL		,NULL		,NULL		,NULL		,mdate		,mtime		,datetime	,mdate)
	MENU_ITEM(mtime		,"Time"		,""		,&prmh[NutcTime2]	,chmodMenuAlways	,0		,NULL		,NULL		,NULL		,NULL		,mdate		,mtimezone	,datetime	,mtime)
	MENU_ITEM(mtimezone	,"Timezone"	,""		,&prmh[Ntimezone]	,chmodMenuAlways	,0		,NULL		,NULL		,NULL		,NULL		,mtime		,mtimezone	,datetime	,mtimezone)

/*		name 			,label		,units	,prmHandle		,chmod				,pfPrm	,pfChanges	,pfSelect	,pfUnselect	,pfPeriod	,previous	,next		,parent		,child	*/
MENU_ITEM(lan			,"LAN"		,""		,NULL			,chmodMenuAlways	,0		,NULL		,NULL		,netUpdate	,NULL		,datetime	,brightness	,lan		,ipaddr)
	/*		  name 			,label	,units	,prmHandle			,chmod				,pfPrm	,pfChanges	,pfSelect	,pfUnselect	,pfPeriod	,previous	,next		,parent		,child	*/
	MENU_ITEM(ipaddr		,"ip"	,""		,&prmh[Nipadr]		,chmodMenuAlways	,0		,NULL		,NULL		,NULL		,NULL		,ipaddr		,netmask	,lan		,ipaddr)
	MENU_ITEM(netmask		,"nm"	,""		,&prmh[Nnetmask]	,chmodMenuAlways	,0		,NULL		,NULL		,NULL		,NULL		,ipaddr		,gatewey	,lan		,netmask)
	MENU_ITEM(gatewey		,"gw"	,""		,&prmh[Ngateway]	,chmodMenuAlways	,0		,NULL		,NULL		,NULL		,NULL		,netmask	,gatewey	,lan		,gatewey)

/*		name 			,label		,units	,prmHandle			,chmod				,pfPrm	,pfChanges	,pfSelect	,pfUnselect	,pfPeriod	,previous	,next		,parent		,child	*/
MENU_ITEM(brightness	,"Bright"	,"%"	,&prmh[Nbrightness]	,chmodMenuAlways	,0		,setBright	,NULL		,NULL		,NULL		,lan		,brightness	,brightness	,brightness)


/*************** LGPL ************** END OF FILE *********** D_EL ************/
