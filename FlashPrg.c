/**************************************************************************//**
 * @file     FlashPrg.c
 * @brief    Flash Programming Functions adapted for New Device Flash
 * @version  V1.0.0
 * @date     10. January 2018
 ******************************************************************************/
/*
 * Copyright (c) 2010-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include ".\FlashOS.H"        // FlashOS Structures
#include <stdint.h>
#include "reg.h"
extern void FlashPrepare(int a1, int a2, int a3, int a4);
extern int FlashEraseChip(void);
extern int FlashEraseSector(uint32_t adr);
extern int FlashWrite(uint32_t adr, uint32_t sz, const uint8_t *buf);
extern void FlashCleanup(void);

/* 
   Mandatory Flash Programming Functions (Called by FlashOS):
                int Init        (unsigned long adr,   // Initialize Flash
                                 unsigned long clk,
                                 unsigned long fnc);
                int UnInit      (unsigned long fnc);  // De-initialize Flash
                int EraseSector (unsigned long adr);  // Erase Sector Function
                int ProgramPage (unsigned long adr,   // Program Page Function
                                 unsigned long sz,
                                 unsigned char *buf);

   Optional  Flash Programming Functions (Called by FlashOS):
                int BlankCheck  (unsigned long adr,   // Blank Check
                                 unsigned long sz,
                                 unsigned char pat);
                int EraseChip   (void);               // Erase complete Device
      unsigned long Verify      (unsigned long adr,   // Verify Function
                                 unsigned long sz,
                                 unsigned char *buf);

       - BlanckCheck  is necessary if Flash space is not mapped into CPU memory space
       - Verify       is necessary if Flash space is not mapped into CPU memory space
       - if EraseChip is not provided than EraseSector for all sectors is called
*/


/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */
int Init (unsigned long adr, unsigned long clk, unsigned long fnc) {
   
  /* Add your Code */
  uint32_t a4;
  FlashPrepare(adr, clk, fnc, a4);
  return (0);                                  // Finished without Errors
}


/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int UnInit (unsigned long fnc) {

  /* Add your Code */
  FlashCleanup();
  return (0);                                  // Finished without Errors
}


/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseChip (void) {

  /* Add your Code */
  return FlashEraseChip();
}


/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseSector (unsigned long adr) {

  /* Add your Code */
  return FlashEraseSector(adr);                                  // Finished without Errors
}


/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */

int ProgramPage (unsigned long adr, unsigned long sz, unsigned char *buf) {

  /* Add your Code */
  return FlashWrite(adr, sz, buf);;                                  // Finished without Errors
}

#if USE_NATIVE_VERIFY
unsigned long Verify(unsigned long adr, unsigned long sz, unsigned char *buf)
{
  int * start_address  = (int *)adr;

#if USE_CRC_VERIFY
  CRC_RESLT = 0xFFFFFFFF;
  for (uint32_t u32Count = 0u; u32Count < sz >> 2; u32Count++)
  {
      CRC_DAT0 = *(uint32_t *)buf;
      buf +=4;
  }
  int ref_data = CRC_RESLT;
  
  CRC_RESLT = 0xFFFFFFFF;
  for (uint32_t u32Count = 0u; u32Count < sz >> 2; u32Count++)
  {
      CRC_DAT0 = *(uint32_t *)(start_address++);
  }
  if(ref_data == CRC_RESLT){
    return adr + sz;
  }
  return adr;
#else
  uint32_t i; // r1
  int v6; // t1
  uint8_t *v7; // r7
  unsigned int j; // r3
  int v9; // t1
 
  for ( i = 0; i < sz >> 2; ++i )
  {
    v6 = *start_address++;
    if ( v6 != *(uint32_t *)buf )
      break;
    buf += 4;
  }
  v7 = (uint8_t *)start_address;
  for ( j = 0; (sz & 3) > j; ++j )
  {
    v9 = *v7++;
    if ( v9 != buf[j] )
      break;
  }
  return adr + 4 * i + j;
#endif
}
#endif
