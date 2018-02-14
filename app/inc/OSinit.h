/*!****************************************************************************
 * @file		OSinit.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @copyright	GNU Lesser General Public License v3
 * @brief		Initialization operational system
 */

/*!****************************************************************************
 * Include
 */

/*!**** NAME ****************** size [4byte Word] ****************************/
#define SYSTEM_TSK_PRIO         7
#define UART_TSK_PRIO           6
#define NETIF_TSK_PRIO          5
#define TCPIP_TSK_PRIO          4
#define HTTP_TSK_PRIO           3
#define STARTUP_TSK_PRIO        1
#define BASE_TSK_PRIO           1
#define CHARG_TSK_PRIO          1
#define SETT_TSK_PRIO           1
#define CUBE_TSK_PRIO           1
#define BUBLE_TSK_PRIO          1

/*!**** NAME ****************** size [4byte Word] ****************************/
#define SYSTEM_TSK_SZ_STACK     512
#define UART_TSK_SZ_STACK       256
#define NETIF_SZ_STACK          512
#define TCPIP_SZ_STACK          1024
#define HTTP_TSK_SZ_STACK       512
#define STARTUP_TSK_SZ_STACK    256
#define BASE_TSK_SZ_STACK       256
#define CHARG_TSK_SZ_STACK      256
#define SETT_TSK_SZ_STACK       512
#define CUBE_TSK_SZ_STACK       256
#define BUBLE_TSK_SZ_STACK      256


/*!****************************************************************************
 * Function declaration
 */
void OSinit(void);

/*************** LGPL ************** END OF FILE *********** D_EL ************/
