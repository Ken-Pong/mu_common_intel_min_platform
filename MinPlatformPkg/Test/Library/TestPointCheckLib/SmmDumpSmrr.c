/** @file

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <PiSmm.h>
#include <Library/TestPointCheckLib.h>
#include <Library/TestPointLib.h>
#include <Library/DebugLib.h>

#include <Register/Cpuid.h>
#include <Register/Msr.h>

//
// Machine Specific Registers (MSRs)
//
#define  SMM_FEATURES_LIB_IA32_MTRR_CAP            0x0FE
#define  SMM_FEATURES_LIB_IA32_SMRR_PHYSBASE       0x1F2
#define  SMM_FEATURES_LIB_IA32_SMRR_PHYSMASK       0x1F3
#define  SMM_FEATURES_LIB_IA32_CORE_SMRR_PHYSBASE  0x0A0
#define  SMM_FEATURES_LIB_IA32_CORE_SMRR_PHYSMASK  0x0A1

BOOLEAN  mSmrrSupported = FALSE;
//
// Set default value to assume IA-32 Architectural MSRs are used
//
UINT32  mSmrrPhysBaseMsr = SMM_FEATURES_LIB_IA32_SMRR_PHYSBASE;
UINT32  mSmrrPhysMaskMsr = SMM_FEATURES_LIB_IA32_SMRR_PHYSMASK;

EFI_STATUS
TestPointDumpSmrr (
  VOID
  )
{
  UINT64      SmrrBase;
  UINT64      SmrrMask;
  
  UINT32  RegEax;
  UINT32  RegEdx;
  UINTN   FamilyId;
  UINTN   ModelId;
  BOOLEAN Result;
  
  DEBUG ((DEBUG_INFO, "==== TestPointDumpSmrr - Enter\n"));

  //
  // Retrieve CPU Family and Model
  //
  AsmCpuid (CPUID_VERSION_INFO, &RegEax, NULL, NULL, &RegEdx);
  FamilyId = (RegEax >> 8) & 0xf;
  ModelId  = (RegEax >> 4) & 0xf;
  if (FamilyId == 0x06 || FamilyId == 0x0f) {
    ModelId = ModelId | ((RegEax >> 12) & 0xf0);
  }

  //
  // Check CPUID(CPUID_VERSION_INFO).EDX[12] for MTRR capability
  //
  if ((RegEdx & BIT12) != 0) {
    //
    // Check MTRR_CAP MSR bit 11 for SMRR support
    //
    if ((AsmReadMsr64 (SMM_FEATURES_LIB_IA32_MTRR_CAP) & BIT11) != 0) {
      mSmrrSupported = TRUE;
    }
  }

  //
  // Intel(R) 64 and IA-32 Architectures Software Developer's Manual
  // Volume 3C, Section 35.3 MSRs in the Intel(R) Atom(TM) Processor Family
  //
  // If CPU Family/Model is 06_1CH, 06_26H, 06_27H, 06_35H or 06_36H, then
  // SMRR Physical Base and SMM Physical Mask MSRs are not available.
  //
  if (FamilyId == 0x06) {
    if (ModelId == 0x1C || ModelId == 0x26 || ModelId == 0x27 || ModelId == 0x35 || ModelId == 0x36) {
      mSmrrSupported = FALSE;
    }
  }

  //
  // Intel(R) 64 and IA-32 Architectures Software Developer's Manual
  // Volume 3C, Section 35.2 MSRs in the Intel(R) Core(TM) 2 Processor Family
  //
  // If CPU Family/Model is 06_0F or 06_17, then use Intel(R) Core(TM) 2
  // Processor Family MSRs
  //
  if (FamilyId == 0x06) {
    if (ModelId == 0x17 || ModelId == 0x0f) {
      mSmrrPhysBaseMsr = SMM_FEATURES_LIB_IA32_CORE_SMRR_PHYSBASE;
      mSmrrPhysMaskMsr = SMM_FEATURES_LIB_IA32_CORE_SMRR_PHYSMASK;
    }
  }

  if (mSmrrSupported) {
    SmrrBase = AsmReadMsr64 (mSmrrPhysBaseMsr);
    SmrrMask = AsmReadMsr64 (mSmrrPhysMaskMsr);
    DEBUG ((DEBUG_INFO, "SMRR : Base=%016lx Make=%016lx\n", SmrrBase, SmrrMask));
  }

  DEBUG ((DEBUG_INFO, "==== TestPointDumpSmrr - Exit\n"));
  
  // Check - TBD
  Result = TRUE;
  
  if (!Result) {
    TestPointLibAppendErrorString (
      PLATFORM_TEST_POINT_ROLE_PLATFORM_IBV,
      NULL,
      TEST_POINT_BYTE2_SMM_READY_TO_LOCK_ERROR_CODE_2 TEST_POINT_SMM_READY_TO_LOCK TEST_POINT_BYTE2_SMM_READY_TO_LOCK_ERROR_STRING_2
      );
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}
