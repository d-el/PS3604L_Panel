/*
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006-2018 Christian Walter <cwalter@embedded-solutions.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbconfig.h"

/* ----------------------- Defines ------------------------------------------*/
#define MB_PDU_FUNC_BYTECNT_OFF            ( MB_PDU_DATA_OFF + 0)
#define MB_PDU_FUNC_SUBREQ_OFF             ( MB_PDU_DATA_OFF + 1)

#define MB_REQ_REFTYPE_OFF                 ( 0 )
#define MB_REQ_FILENUMBER_OFF              ( 1 )
#define MB_REQ_RECNUMBER_OFF               ( 3 )
#define MB_REQ_RECLEN_OFF                  ( 5 )
#define MB_REQ_DATA_OFF                    ( 6 )

#define MB_PDU_FUNC_RESDATALEN_OFF         ( MB_PDU_DATA_OFF + 0)

#define MB_READ_RES_LEN_OFF                ( 0 )
#define MB_READ_RES_REFTYPE_OFF            ( 1 )
#define MB_READ_RES_DATA_OFF               ( 2 )

#define MB_WRITE_REQ_DATA_OFF              ( 7 )

#define MB_REFTYPE                         ( 6 )
#define MB_SIZE                            ( 7 )
#define MB_BYTECNT_MAX                     ( 0XF5 )
#define MB_REC_MAX                         ( 0X270F )

/* ----------------------- Static functions ---------------------------------*/
eMBException    prveMBError2Exception( eMBErrorCode eErrorCode );

/* ----------------------- Start implementation -----------------------------*/

#if MB_FUNC_READ_FILE_RECORD_ENABLED > 0

eMBException
eMBFuncReadFileRecord( UCHAR * pucFrame, USHORT * usLen )
{
    eMBException    eStatus = MB_EX_NONE;
    eMBErrorCode    eFileStatus;
    UCHAR           subReq[253];
    UCHAR           records;
    UCHAR           *pucFrameCur;
    UCHAR           *readFrameCur;
    USHORT          usFile;
    USHORT          usRecord;
    USHORT          usRecordLen;

    if( pucFrame[MB_PDU_FUNC_BYTECNT_OFF] >= MB_SIZE &&
        pucFrame[MB_PDU_FUNC_BYTECNT_OFF] <= MB_BYTECNT_MAX &&
        pucFrame[MB_PDU_FUNC_BYTECNT_OFF] % MB_SIZE == 0 )
    {
        // Copy to request buffer
        for( UCHAR i = 0; i < pucFrame[MB_PDU_FUNC_BYTECNT_OFF]; i++ )
        {
            subReq[i] = pucFrame[i + MB_PDU_FUNC_SUBREQ_OFF];
        }

        records = pucFrame[MB_PDU_FUNC_BYTECNT_OFF] / MB_SIZE;
        pucFrameCur = &pucFrame[MB_PDU_FUNC_SUBREQ_OFF];
        readFrameCur = &subReq[0];
        *usLen = MB_PDU_FUNC_OFF;

        for( UCHAR r = 0; r < records; r++ )
        {
            usFile = ( USHORT )( readFrameCur[MB_REQ_FILENUMBER_OFF] << 8 );
            usFile |= ( USHORT )( readFrameCur[MB_REQ_FILENUMBER_OFF + 1] );
            usRecord = ( USHORT )( readFrameCur[MB_REQ_RECNUMBER_OFF] << 8 );
            usRecord |= ( USHORT )( readFrameCur[MB_REQ_RECNUMBER_OFF + 1] );
            usRecordLen = ( USHORT )( readFrameCur[MB_REQ_RECLEN_OFF] << 8 );
            usRecordLen |= ( USHORT )( readFrameCur[MB_REQ_RECLEN_OFF + 1] );

            pucFrameCur[MB_READ_RES_LEN_OFF] = 1 + usRecordLen * 2;
            pucFrameCur[MB_READ_RES_REFTYPE_OFF] = MB_REFTYPE;
            eFileStatus = eMBFileRecordCB( &pucFrameCur[MB_READ_RES_DATA_OFF], usFile, usRecord, usRecordLen, MB_REG_READ );
            if( eFileStatus != MB_ENOERR )
            {
            	eStatus = prveMBError2Exception( eFileStatus );
            }
            *usLen += 2 + usRecordLen * 2;

            readFrameCur += MB_SIZE;
            pucFrameCur += 2 + usRecordLen * 2;
        }

        pucFrame[MB_PDU_FUNC_RESDATALEN_OFF] = *usLen;
        *usLen += 2;

    }
    else
    {
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
    return eStatus;
}
#endif

#if MB_FUNC_WRITE_FILE_RECORD_ENABLED > 0

eMBException
eMBFuncWriteFileRecord( UCHAR * pucFrame, USHORT * usLen )
{
    eMBException    eStatus = MB_EX_NONE;
    eMBErrorCode    eFileStatus;
    UCHAR           requestDataLength;
    UCHAR           *pucFrameCur;
    UCHAR           *readFrameCur;
    USHORT          usFile;
    USHORT          usRecord;
    USHORT          usRecordLen;

    if( pucFrame[MB_PDU_FUNC_BYTECNT_OFF] >= MB_SIZE &&
    	pucFrame[MB_PDU_FUNC_BYTECNT_OFF] <= MB_BYTECNT_MAX )
    {
        requestDataLength = pucFrame[MB_PDU_FUNC_BYTECNT_OFF];
        pucFrameCur = &pucFrame[MB_PDU_FUNC_SUBREQ_OFF];
        readFrameCur = &pucFrame[MB_PDU_FUNC_SUBREQ_OFF];
        *usLen = MB_PDU_FUNC_OFF;

        while( readFrameCur < &pucFrame[MB_PDU_FUNC_SUBREQ_OFF] + requestDataLength )
        {
            usFile = ( USHORT )( readFrameCur[MB_REQ_FILENUMBER_OFF] << 8 );
            usFile |= ( USHORT )( readFrameCur[MB_REQ_FILENUMBER_OFF + 1] );
            usRecord = ( USHORT )( readFrameCur[MB_REQ_RECNUMBER_OFF] << 8 );
            usRecord |= ( USHORT )( readFrameCur[MB_REQ_RECNUMBER_OFF + 1] );
            usRecordLen = ( USHORT )( readFrameCur[MB_REQ_RECLEN_OFF] << 8 );
            usRecordLen |= ( USHORT )( readFrameCur[MB_REQ_RECLEN_OFF + 1] );

            eFileStatus = eMBFileRecordCB( &pucFrameCur[MB_WRITE_REQ_DATA_OFF], usFile, usRecord, usRecordLen, MB_REG_WRITE );
            if( eFileStatus != MB_ENOERR )
            {
            	eStatus = prveMBError2Exception( eFileStatus );
            }
            *usLen += 1 + 2 + 2 + 2 + usRecordLen * 2;

            readFrameCur += 1 + 2 + 2 + 2 + usRecordLen * 2;
            pucFrameCur += 1 + 2 + 2 + 2 + usRecordLen * 2;
        }

        pucFrame[MB_PDU_FUNC_RESDATALEN_OFF] = *usLen;
        *usLen += 2;

    }
    else
    {
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
    return eStatus;
}

#endif
