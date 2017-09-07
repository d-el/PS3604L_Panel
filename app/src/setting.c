/*!****************************************************************************
 * @file    setting.c 
 * @author  Storozhenko Roman - D_EL
 * @version V1.0.0
 * @date    
 * @brief   
 */

/*!****************************************************************************
 * Include
 */

///******************************************************************************
//* Калибровка вольтметра
//*/
//void U_Adjustment(void){
//    syssetting_type     *s;
//    uint16_t            Uset, Udac, Uview;
//    uint8_t             varParam, fVarParamNew, i, redraw, point;
//   
//    s               = &ps.sysset;
//    uiTextColor     = RGB;
//    uiGraphicsColor = black;
//    varParam        = 0;
//    fVarParamNew    = 1;
//    redraw          = 1;
//    point           = 0;
//    Uset            = 0;
//    Udac            = 0;
//    
//    lcd_Clear();
//    IRQ_Limit_OFF();
//    
//    switchON();
//    startSampling();
//    setDacI(0x0FFF);  //~8 A!!!
//    kc.AutoPress = AutoPressON;
//    kc.scanningToFirstRewind = 30;
//    kc.scanningToRewind = 1;
//    enco.ntic = 5;
//    
//    while(1){
//        /**************************************
//        * Обработка клавиатуры
//        */		
//        if(BtScan()){
//            BeepTime(ui.beep.key.time, ui.beep.key.freq);
//        }
//        if(ks.next){
//            varParam++; 
//            fVarParamNew = 1;
//            if(varParam >= 3) varParam = 0;
//        }
//        if(ks.onOff){
//            s->pU[point].qval   = IntToIQ(Uset, 1000);
//            //s->pU[point].adc    = adcStct.filtered[CH_UADC];
//            s->pU[point].dac    = Udac;
//            BeepTime(ui.beep.ready.time, ui.beep.ready.freq);
//        }
//        if(ks.mode){
//            nvMem_savePrm(nvMem.nvMemBase), nvMem_savePrm(nvMem.nvMemBaseResrvCopy);
//            switchOFF();
//            setDacI(0);
//            setDacU(0);
//            switchOFF();
//            stopSampling();
//            enco.ntic = 0;
//            return;
//        }
//        if(ks.view){
//            lcd_Clear();
//            lcd_PutString6x8(0, 0, "Uset   adc  dac");
//            for(i = 0; i < CALIBR_POINTS; i++){
//                Uview = IQtoInt(s->pU[i].qval, 1000);
//                sprintf(str, "%02u.%03u %04u %04u", Uview/1000, Uview%1000, s->pU[i].adc, s->pU[i].dac);
//                lcd_PutString6x8(0, (i+1)*10, str); 
//            }
//            delay_ms(200);
//            while(!(BtScan()));
//            BeepTime(ui.beep.key.time, ui.beep.key.freq);
//            lcd_Clear();
//            redraw = 1;
//        }
//        if(ks.up){
//            enBigStepUp();
//        }
//        if(ks.down){
//            enBigStepDown();
//        }
//        if(ks.zero){
//            enWriteVal(0);
//        }
//
//        /**************************************
//        * Настройка клавиатуры, настройка энкодера
//        */
//        if(fVarParamNew !=0){
//            if(varParam == UpUST_VAR_Point){
//                kc.AutoPress = AutoPressOFF;
//                enSet(0, CALIBR_POINTS - 1, 1, 1,
//                      enNONCIRC, enU8, &point);
//            }
//            if(varParam == UpUST_VAR_Uset){
//                kc.AutoPress = AutoPressON;
//                kc.scanningToRewind = 5;
//                enSet(0, 30000, 1, 100,
//                      enNONCIRC, enU16, &Uset);
//            }
//            if(varParam == UpUST_VAR_Udac){
//                kc.AutoPress = AutoPressON;
//                kc.scanningToRewind = 10;
//                enSet(0, 4095, 1, 100,
//                      enNONCIRC, enU16, &Udac);
//            }
//        }
//
//        /**************************************
//        * Вынимаем значение с энкодера
//        */
//        enGet();
//        
//        /**************************************
//        * Установка напряжения
//        */
//        setDacU(Udac);
//        
//        /**************************************
//        * Вывод на дисплей
//        */
//        if(redraw != 0){
//            lcd_PutString6x8(10, 00, "Point =");
//            lcd_PutString6x8(10, 10, "Uset  =");    lcd_PutChar6x8  (94, 10, 'V');
//            lcd_PutString6x8(10, 20, "Udac  =");
//            lcd_PutString6x8(10, 30, "Uadc  =");
//            lcd_PutString6x8(10, 40, "CC/CV =");
//            redraw = 0;
//            fVarParamNew = 1;
//        }
//        //Изменение положения курсора
//        if(fVarParamNew !=0){
//            if(varParam == UpUST_VAR_Point){
//                lcd_PutChar6x8(0, 20, ' ');
//                lcd_PutChar6x8(0, 00, '>');
//            }
//            if(varParam == UpUST_VAR_Uset){
//                lcd_PutChar6x8(0, 00, ' ');
//                lcd_PutChar6x8(0, 10, '>');
//            }
//            if(varParam == UpUST_VAR_Udac){
//                lcd_PutChar6x8(0, 10, ' ');
//                lcd_PutChar6x8(0, 20, '>');
//            }
//            fVarParamNew = 0;
//        }
//        
//        //Печать значения point
//		sprintf(str, "%01u ", point);
//		lcd_PutString6x8(58, 00, str);
//        
//        //Печать значения уставки напряжения
//        sprintf(str, "%01u.%03u", Uset / 1000, Uset % 1000);
//        lcd_PutString6x8(58, 10, str);
//        
//        //Печать значения Udac
//		sprintf(str, "%04u", Udac);
//		lcd_PutString6x8(58, 20, str);
//
//        //Печать значения Uadc
//		//sprintf(str, "%04u", adcStct.filtered[CH_UADC]);
//		lcd_PutString6x8(58, 30, str); 
//
//        if(MODE_IS_CC() != 0){
//            lcd_PutString6x8(58, 40, "CC");
//        }else{
//            lcd_PutString6x8(58, 40, "CV");
//        }  
//      
//        delay_ms(5);
//    }
//}
//
///******************************************************************************
//* Калибровка амперметра
//*/
//void I_Adjustment(void){
//    syssetting_type     *s;
//    uint16_t            Iset, Iview, Idac;
//    uint8_t             varParam, fVarParamNew, i, point, redraw;
//
//    s               = &ps.sysset;
//    uiTextColor     = RGB;
//    uiGraphicsColor = black;
//    varParam        = 0;
//    fVarParamNew    = 1;
//    redraw          = 1;
//    point           = 0;
//    Iset            = 0;
//    Idac            = 0;
//    
//    lcd_Clear();
//
//    IRQ_Limit_OFF();
//
//    startSampling();
//    switchON();
//    setDacI(0);
//    setDacU(2048);
//    
//    kc.AutoPress = AutoPressON;
//    kc.scanningToFirstRewind = 15;
//    kc.scanningToRewind = 1;
//    enco.ntic = 5;
//    
//    while(1){  
//        /**************************************
//        * Обработка клавиатуры
//        */		
//        if(BtScan()){
//            BeepTime(ui.beep.key.time, ui.beep.key.freq);
//        }
//        if(ks.next){
//            varParam++; 
//            fVarParamNew = 1;
//            if(varParam >= 3) varParam = 0;
//        }
//        if(ks.onOff){
//            s->pI[point].qval   = IntToIQ(Iset, 1000);
//            //s->pI[point].adc    = adcStct.filtered[CH_IADC];
//            s->pI[point].dac    = Idac;
//            BeepTime(ui.beep.ready.time, ui.beep.ready.freq);
//        }
//        if(ks.mode){
//            nvMem_savePrm(nvMem.nvMemBase), nvMem_savePrm(nvMem.nvMemBaseResrvCopy);
//            switchOFF();
//            setDacI(0);
//            setDacU(0);
//            stopSampling();
//            enco.ntic = 0;
//            return;
//        }
//        if(ks.view){
//            lcd_Clear();
//            lcd_PutString6x8(0, 0, "Iset   adc  dac");
//            for(i = 0; i < CALIBR_POINTS; i++){
//                Iview = IQtoInt(s->pI[i].qval, 1000);
//                sprintf(str, "%02u.%03u %04u %04u", Iview/1000, Iview%1000, s->pI[i].adc, s->pI[i].dac);
//                lcd_PutString6x8(0, (i+1)*10, str); 
//            }
//            delay_ms(200);
//            while(!(BtScan()));
//            BeepTime(ui.beep.key.time, ui.beep.key.freq);
//            lcd_Clear();
//            redraw = 1;
//        }
//        if(ks.up){
//            enBigStepUp();            
//        }
//        if(ks.down){
//            enBigStepDown();
//        }
//        if(ks.zero){
//            enWriteVal(0);	//Обнуляем
//        }
//
//        /**************************************
//        * Настройка клавиатуры, настройка энкодера
//        */
//        if(fVarParamNew !=0){
//            if(varParam == IpUST_VAR_Point){
//                kc.AutoPress = AutoPressOFF;
//                kc.scanningToRewind = 20;
//                enSet(0, CALIBR_POINTS-1, 1, 1,
//                      enNONCIRC, enU8, &point);
//            }
//            if(varParam == IpUST_VAR_Iset){
//                kc.AutoPress = AutoPressON;
//                kc.scanningToRewind = 10;
//                enSet(0, 8000, 1, 100,
//                      enNONCIRC, enU16, &Iset);
//            }
//            if(varParam == IpUST_VAR_Udac){
//                kc.AutoPress = AutoPressON;
//                kc.scanningToRewind = 20;
//                enSet(0, 4095, 1, 100,
//                      enNONCIRC, enU16, &Idac);
//            }
//        }
//
//        /**************************************
//        * Вынимаем значение с энкодера
//        */
//        enGet();
//        
//        /**************************************
//        * Установка напряжения
//        */
//        setDacI(Idac);
//        
//        /**************************************
//        * Вывод на дисплей
//        */
//        if(redraw != 0){
//            lcd_PutString6x8(10, 00, "Point =");
//            lcd_PutString6x8(10, 10, "Iset  =");    lcd_PutChar6x8  (94, 10, 'A');
//            lcd_PutString6x8(10, 20, "Idac  =");
//            lcd_PutString6x8(10, 30, "Iadc  =");
//            lcd_PutString6x8(10, 40, "CC/CV =");
//            redraw = 0;
//            fVarParamNew = 1;
//        }
//        //Изменение положения курсора
//        if(fVarParamNew !=0){
//            if(varParam == IpUST_VAR_Point){
//                lcd_PutChar6x8(0, 20, ' ');
//                lcd_PutChar6x8(0, 00, '>');
//            }
//            if(varParam == IpUST_VAR_Iset){
//                lcd_PutChar6x8(0, 00, ' ');
//                lcd_PutChar6x8(0, 10, '>');
//            }
//            if(varParam == IpUST_VAR_Udac){
//                lcd_PutChar6x8(0, 10, ' ');
//                lcd_PutChar6x8(0, 20, '>');
//            }
//            fVarParamNew = 0;
//        }
//        
//        //Печать значения point
//		sprintf(str, "%01u", point);
//		lcd_PutString6x8(58, 00, str);
//        
//        //Печать значения уставки тока
//        sprintf(str, "%01u.%03u", Iset / 1000, Iset % 1000);
//        lcd_PutString6x8(58, 10, str);
//        
//        //Печать значения Idac
//		sprintf(str, "%04u", Idac);
//		lcd_PutString6x8(58, 20, str);
//
//        //Печать значения Iadc
//		//sprintf(str, "%04u", adcStct.filtered[CH_IADC]);
//		lcd_PutString6x8(58, 30, str); 
//
//        if(MODE_IS_CC() != 0){
//            lcd_PutString6x8(58, 40, "CC");
//        }else{
//            lcd_PutString6x8(58, 40, "CV");
//        }  
//      
//        delay_ms(5);
//    }
//}
//
///******************************************************************************
//*
//*/
//void Adjustment(void){
//    uint8_t varParam = 0, fVarParamNew = 1;
//    uint8_t redraw;
//    
//    redraw = 1;
//    uiTextColor     = RGB;
//    uiGraphicsColor = black;
//
//    while(1){
//        //======== обработка кнопок ========//		
//        if(BtScan()){
//            BeepTime(ui.beep.key.time, ui.beep.key.freq);
//        }
//        if(ks.next){
//            varParam++; 
//            fVarParamNew = 1;
//            if(varParam >= 2) varParam = 0;
//        }     
//        if(ks.mode){
//            return;
//        }
//        if(ks.view){
//            switch(varParam){
//                case 0:	U_Adjustment();   break;
//                case 1:	I_Adjustment();   break;
//            }
//            redraw = 1;
//        }
//        
//        //=========================================================
//        //================= Вывод на дисплей ======================
//        //=========================================================
//        
//        if(redraw != 0){
//            lcd_Clear();
//            //Печать статических символов
//            lcd_PutString6x8(10, 0, "U Adjustment");	
//            lcd_PutString6x8(10, 10, "I Adjustment");
//            redraw = 0;
//            fVarParamNew = 1;
//        }
//    
//        //Изменение положения курсора
//        if(fVarParamNew !=0){
//            if(varParam == 0){
//                lcd_PutChar6x8(0, 10, ' ');
//                lcd_PutChar6x8(0, 0, '>');
//            }
//            if(varParam == 1){
//                lcd_PutChar6x8(0, 0, ' ');
//                lcd_PutChar6x8(0, 10, '>');
//            }
//            fVarParamNew =0;
//        }
//      
//        delay_ms(10);
//    }
//}
//
///******************************************************************************
//*
//*/
//void SpsC(void){
////    uint8_t         VarParam = 0, fVarParamNew = 1;
////    uint16_t        spsUse;
////    uint16_t        voltage;
////    uint16_t        hvoltage;
////    uint16_t        kKorHighvoltage;
////    spsSt_type      spsSt;      //Статус текущей операции
////
////    voltage         = 25000;
////    spsUse          = ps.sysset.spsUse;
////    kKorHighvoltage = ps.sysset.kKorHighvoltage;
////    
////    switchON();
////    setDacI(2048);
////    setDacU(2048);
////    
////	uiGraphicsColor = 0;
////	uiTextColor = RGB; 	
////	lcd_Clear();
////    
////	while(1){
////        //======== обработка кнопок ========//		
////        if(BtScan()){
////            BeepTime(ui.beep.key.time, ui.beep.key.freq);
////        }
////        if(ks.next){
////            VarParam++; 
////            if(VarParam >= 3) VarParam = 0;
////            fVarParamNew = 1;
////        }
////        if(ks.mode){
////            ps.sysset.spsUse = spsUse;
////            ps.sysset.kKorHighvoltage = kKorHighvoltage;
////            nvMem_savePrm(nvMem.nvMemBase), nvMem_savePrm(nvMem.nvMemBaseResrvCopy);
////            switchOFF();
////            setDacI(0);
////            setDacU(0);
////            return;
////        }
////        if(ks.up){
////            enBigStepUp();
////        }
////        if(ks.down){
////            enBigStepDown();
////        }
////        if(ks.zero){
////            enWriteVal(0);	//Обнуляем
////        }
////        //======== ------------------------- ========//
////        
////        //Настройка клавиатуры, настройка энкодера
////        if(fVarParamNew != 0){
////            if(VarParam == 0){
////                kc.AutoPress = AutoPressOFF;                                    //Запертить автонажатие
////                enSet(0, 1, 1, 1,
////                      enNONCIRC, enU16, &spsUse);                               //Установка диапазона значения энкодера
////            }
////            if(VarParam == 1){
////                kc.AutoPress = AutoPressOFF;
////                enSet(10000, 33000, 100, 1000,
////                      enNONCIRC, enU16, &voltage);
////            }
////            if(VarParam == 2){
////                kc.AutoPress = AutoPressOFF;
////                enSet(800, 1500, 1, 100,
////                      enNONCIRC, enU16, &kKorHighvoltage);
////            }
////        }
////        
////        //Вынимаем значение с энкодера
////        enGet();
////
////        spsSt = spsRedMem();
////
////        if(spsSt == spsOk){
////            //Регулируем выходное напряжение импульсного источника питания
////            sps.scratchpad.vTask = voltage;
////            //Записываем в память SPS
////            spsSt = spsWriteMem();
////        }
////        
////        hvoltage = spsCodeToHV(sps.scratchpad.hvoltage, kKorHighvoltage);
////        
////        //=========================================================
////        //================= Вывод на дисплей ======================
////        //=========================================================
////        //Печать курсора
////        if(fVarParamNew !=0){
////            if(VarParam == 0){
////                lcd_PutChar6x8(0, 20, ' ');                                    //Удаляем старый курсор
////                lcd_PutChar6x8(0, 0, '>');                                  //Рисуем новый курсор
////            }
////            if(VarParam == 1){
////                lcd_PutChar6x8(0, 0, ' ');
////                lcd_PutChar6x8(0, 10, '>');
////            }
////            if(VarParam == 2){
////                lcd_PutChar6x8(0, 10, ' ');
////                lcd_PutChar6x8(0, 20, '>');
////            }
////            fVarParamNew = 0;
////        }
////        
////        sprintf(str, "use  = %01u ", spsUse);
////        lcd_PutString6x8(10, 0, str);
////        
////        sprintf(str, "setV = %02u.%03uV", voltage/1000, voltage%1000);
////        lcd_PutString6x8(10, 10, str);
////
////        sprintf(str, "hvk  = %01u.%03u", kKorHighvoltage/1000, kKorHighvoltage%1000);
////        lcd_PutString6x8(10, 20, str);
////        
////        sprintf(str, "v    = %02u.%03uV", sps.scratchpad.voltage/1000, sps.scratchpad.voltage%1000);
////        lcd_PutString6x8(10, 30, str);
////        
////        sprintf(str, "hv   = %03uV", hvoltage);
////        lcd_PutString6x8(10, 40, str);
////
////        sprintf(str, "t    = %03u.%01u°C", sps.scratchpad.temperature/10, sps.scratchpad.temperature%10);
////        lcd_PutString6x8(10, 50, str);
////
//////        switch(spsSt){
//////            case spsOwNotFound:
//////                lcd_PutString6x8(10, 60, "OwNotFound  ");
//////                break;       
//////            case spsOwShortCircle:
//////                lcd_PutString6x8(10, 60, "OwShortCircl");
//////                break;        
//////            case spsOk:
//////                lcd_PutString6x8(10, 60, "Ok          ");
//////                break;
//////            case spsIdError:
//////                lcd_PutString6x8(10, 60, "ID_ERROR    ");
//////                break;
//////        }
////        
////        //=========================================================
////        //=========================================================
////       
////        delay_ms(100);
////    }
//}
//
///******************************************************************************
//*
//*/
//void FanPWM(void){
//    uint16_t    fanPwm = 0;	
//    uint16_t    stopsFan;
//    uint16_t    varParam = 0, fVarParamNew = 1;
//
//    stopsFan = ps.sysset.stopsFan;
//    
//	uiGraphicsColor = 0;
//	uiTextColor = RGB; 	
//	lcd_Clear();
//    
//    kc.AutoPress = AutoPressON;
//    kc.scanningToFirstRewind = 30;
//    kc.scanningToRewind = 10;
//    enco.ntic = 1;
//
//    while(1){
//        //======== обработка кнопок ========//		
//        if(BtScan()){
//            BeepTime(ui.beep.key.time, ui.beep.key.freq);
//        }
//        if(ks.next){
//            varParam++; 
//            fVarParamNew = 1;
//            if(varParam > 3) varParam = 0;
//        }
//        if(ks.mode){
//            ps.sysset.stopsFan = stopsFan;
//            nvMem_savePrm(nvMem.nvMemBase), nvMem_savePrm(nvMem.nvMemBaseResrvCopy);
//            return;
//        }
//        if(ks.up){
//            enBigStepUp();
//        }
//        if(ks.down){
//            enBigStepDown();
//        }
//        if(ks.zero){
//            enWriteVal(0);	//Обнуляем
//        }
//        //======== ------------------------- ========//
//        
//        //Настройка клавиатуры, настройка энкодера, загрузка параметра в энкодер
//        if(fVarParamNew != 0){
//            if(varParam == 0){
//                kc.AutoPress = AutoPressON;                                     //Запертить автонажатие
//                enSet(0, 1000, 1, 100,
//                      enNONCIRC, enU16, &fanPwm);                               //Установка диапазона значения энкодера
//            }
//            if(varParam == 1){
//                kc.AutoPress = AutoPressOFF;
//                enSet(0, 1, 1, 1,
//                      enNONCIRC, enU16, &stopsFan);
//            }
//            if(varParam == 2){
//                kc.AutoPress = AutoPressON;
//                enSet(0, 99, 1, 5,
//                      enNONCIRC, enU16, &ps.sysset.hiTemp);
//            }
//            if(varParam == 3){
//                kc.AutoPress = AutoPressON;
//                enSet(0, 99, 1, 5,
//                      enNONCIRC, enU16, &ps.sysset.lowTemp);
//            }
//        }
//        
//        //Вынимаем значение с энкодера
//        enGet();
//
//        //Установка К. заполнения
//		setFanPwm(fanPwm);
//		
//        //=========================================================
//        //================= Вывод на дисплей ======================
//        //=========================================================
//        //Печать курсора
//        if(fVarParamNew !=0){
//            if(varParam == 0){
//                lcd_PutChar6x8(0, 30, ' ');                                    //Удаляем старый курсор
//                lcd_PutChar6x8(0, 0, '>');                                  //Рисуем новый курсор
//            }
//            if(varParam == 1){
//                lcd_PutChar6x8(0, 0, ' ');
//                lcd_PutChar6x8(0, 10, '>');
//            }
//            if(varParam == 2){
//                lcd_PutChar6x8(0, 10, ' ');                                    //Удаляем старый курсор
//                lcd_PutChar6x8(0, 20, '>');                                 //Рисуем новый курсор
//            }
//            if(varParam == 3){
//                lcd_PutChar6x8(0, 20, ' ');
//                lcd_PutChar6x8(0, 30, '>');
//            }
//            fVarParamNew = 0;
//        }
//        
//        sprintf(str, "pwm = %03u.%u", fanPwm/10, fanPwm%10);	
//        lcd_PutString6x8(10, 0, str);
//        
//        sprintf(str, "stopsFan = %01u", stopsFan);
//        lcd_PutString6x8(10, 10, str);
//        
//        sprintf(str, "hiTemp   = %02u", ps.sysset.hiTemp);
//        lcd_PutString6x8(10, 20, str);
//        
//        sprintf(str, "lowTemp  = %02u", ps.sysset.lowTemp);
//        lcd_PutString6x8(10, 30, str);
//        //=========================================================
//        //=========================================================
//        
//		delay_ms(30);
//    }     
//}
//
///******************************************************************************
//*
//*/
//void key(void){
//    uint16_t counter = 100;
//    
//	lcd_Clear();
//	
//	while(1){
//		//обработка кнопок
//		BtScan();
//		
//		if(ks.ANY){
//			BeepTime(ui.beep.key.time, ui.beep.key.freq);
//			counter = 100;
//		}
//		if(ks.mode) 
//			lcd_PutString6x8(0, 10, "mode");
//		if(ks.view) 
//			lcd_PutString6x8(0, 10, "view");
//		if(ks.onOff)
//			lcd_PutString6x8(0, 10, "onOff");
//		if(ks.next) 
//			lcd_PutString6x8(0, 10, "next");
//		if(ks.up) 
//			lcd_PutString6x8(0, 10, "up");
//		if(ks.down) 
//			lcd_PutString6x8(0, 10, "down");
//		if(ks.zero) 
//			lcd_PutString6x8(0, 10, "zero");
//		
//		delay_ms(100);
//		lcd_PutString8x12(0, 10, "      ");
//		sprintf(str,"Time = %u s  ", counter / 10);
//		lcd_PutString6x8(0, 0, str);
//		counter--;
//		
//		if(counter == 0){
//			BeepTime(ui.beep.ready.time, ui.beep.ready.freq);
//			return;
//		}
//	}
//}
//
///******************************************************************************
//*
//*/
//void information(void){
//    uiTextColor = RGB;
//    uiGraphicsColor = black;   
//    lcd_Clear();
//    
//    lcd_PutString6x8(0, 0, VERSOFT_STR);
//    
//    sprintf(str, "CRC = %04X", GetCrcPO());
//    lcd_PutString6x8(0, 10, str);
//    
//    sprintf(str, "starts = %u", ps.sysset.startCounter);
//    lcd_PutString6x8(0, 20, str);
//    
//    if(clockSource == clockSourceHse){
//        lcd_PutString6x8(0, 30, "use external osc");
//    }else{
//        lcd_PutString6x8(0, 30, "use internal osc");
//    }
//    
//    while(1){
//        //обработка кнопок
//        if(BtScan()){
//            BeepTime(ui.beep.key.time, ui.beep.key.freq);
//        }  
//        if(ks.mode){
//            return;
//        }    
//    }
//}
//
///******************************************************************************
//*
//*/
//void lcdSet(void){
//    uiTextColor     = RGB;
//    uiGraphicsColor = black;   
//    lcd_Clear();
//    lcd_PutString8x12(2, 2, "Set contrast");
//    
//    //TEST
//    uiTextColor     = R;
//    lcd_PutString8x12(2, 40, "TEST srting");
//    uiTextColor     = G;
//    lcd_PutString8x12(2, 52, "TEST srting");
//    uiTextColor     = B;
//    lcd_PutString8x12(2, 64, "TEST srting");
//
//    enSet(80, 120, 1, 5,
//          enNONCIRC, enU8, &ps.sysset.lcdContrast);
//
//    while(1){
//		//обработка кнопок
//        if(BtScan()){
//            BeepTime(ui.beep.key.time, ui.beep.key.freq);
//        }    
//		if(ks.mode){
//            nvMem_savePrm(nvMem.nvMemBase), nvMem_savePrm(nvMem.nvMemBaseResrvCopy);
//			return;	//Уходим
//		}
//		if(ks.up){
//            enBigStepUp();
//        }
//		if(ks.down){
//            enBigStepDown();
//        }
//        if(ks.zero){
//            enWriteVal(100);
//        }
//
//        //Вынимаем значение с энкодера
//        enGet();
//        
//        lcd_contrast(ps.sysset.lcdContrast);
//        
//        //Вывод на ЖКИ
//        sprintf(str, "%03u", ps.sysset.lcdContrast);	
//        lcd_PutString8x12(2, 20, str);
//		
//		delay_ms(20);
//    }
//}
//
///*****************************************************************************
//*
//*/
//uint8_t Setting(void){
//    uint8_t var_param = 0, fVarParamNew = 1;
//
//    uiTextColor = RGB;
//    uiGraphicsColor = black;
//
//    lcd_Clear();
//
//    //Печать статических символов
//    lcd_PutString6x8(10, 0,  "Adjustment");		
//    lcd_PutString6x8(10, 30, "FanPWM");	
//
//    while(1){
//        //======== обработка кнопок ========//
//        if(BtScan()){
//            BeepTime(ui.beep.key.time, ui.beep.key.freq);
//        }
//        if(ks.next){
//            var_param++; 
//            if(var_param > 7){
//                var_param = 0;
//            }
//            fVarParamNew = 1;
//        }	
//        if(ks.mode){
//            return 0;
//        }
//        if(ks.view){
//            switch(var_param){
//                case 0:	Adjustment();   break;
//                case 1:	FanPWM();       break;
//            }
//            return 1;
//        }
//
//        //Изменение положения курсора
//        if(fVarParamNew != 0){
//            if(var_param == 0) lcd_PutChar6x8(0, 70, ' '),	lcd_PutChar6x8(0, 0, '>');
//            if(var_param == 1) lcd_PutChar6x8(0, 0,  ' '),	lcd_PutChar6x8(0, 10, '>');
//            if(var_param == 2) lcd_PutChar6x8(0, 10, ' '),	lcd_PutChar6x8(0, 20, '>');
//            if(var_param == 3) lcd_PutChar6x8(0, 20, ' '),	lcd_PutChar6x8(0, 30, '>');
//            if(var_param == 4) lcd_PutChar6x8(0, 30, ' '),	lcd_PutChar6x8(0, 40, '>');
//            if(var_param == 5) lcd_PutChar6x8(0, 40, ' '),	lcd_PutChar6x8(0, 50, '>');
//            if(var_param == 6) lcd_PutChar6x8(0, 50, ' '),	lcd_PutChar6x8(0, 60, '>');
//            if(var_param == 7) lcd_PutChar6x8(0, 60, ' '),	lcd_PutChar6x8(0, 70, '>');
//            fVarParamNew = 0;
//        }
//
//        delay_ms(50);	
//    }
//}
/******************* (C) COPYRIGHT ***************** END OF FILE ********* D_EL *****/
