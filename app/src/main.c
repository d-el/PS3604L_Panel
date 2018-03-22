/*!******************************************************************************
 *																				*
 _____	 _____ ____	   __	___	 _	_	_	   ______							*
 |	__ \ / ____|___ \  / /	/ _ \| || | | |	   |  ____|							*
 | |__) | (___	 __) |/ /_ | | | | || |_| |	   | |__							*
 |	___/ \___ \ |__ <| '_ \| | | |__   _| |	   |  __|							*
 | |	 ____) |___) | (_) | |_| |	| | | |____| |								*
 |_|	|_____/|____/ \___/ \___/	|_| |______|_|								*
																				*
 *																				*
 * @file		main.c															*
 * @author		Storozhenko Roman - D_EL										*
 * @date		10.03.2013														*
 * @copyright	Copyright (C) 2017 Storozhenko Roman							*
 *				All rights reserved												*
 *				This software may be modified and distributed under the terms	*
 *				of the BSD license.	 See the LICENSE file for details			*
 * @mainpage	Main program body												*
 ********************************************************************************/

/*!****************************************************************************
 * Include
 */
#include "drivers.h"
#include "OSinit.h"

/*!****************************************************************************
 *
 */
void main(void){
	hardInit();
	OSinit();
	ram_f();
	while(1);
}

/***************** Copyright (C) Storozhenko Roman ******* END OF FILE *******/
