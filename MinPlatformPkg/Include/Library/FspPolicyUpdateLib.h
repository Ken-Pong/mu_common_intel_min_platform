/** @file
  Function prototype of FspPolicyUpdateLib.

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _FSP_POLICY_UPDATE_LIB_H_
#define _FSP_POLICY_UPDATE_LIB_H_

VOID
EFIAPI
FspmPolicyUpdate (
  IN OUT VOID    *FspmUpd
  );

VOID
EFIAPI
FspsPolicyUpdate (
  IN OUT VOID    *FspsUpd
  );

#endif
