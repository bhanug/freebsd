//===-- RegisterContextLinux_arm64.cpp -------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===---------------------------------------------------------------------===//

#include <stddef.h>
#include <vector>
#include <cassert>

#include "llvm/Support/Compiler.h"
#include "lldb/lldb-defines.h"

#include "RegisterContextLinux_arm64.h"

// Based on RegisterContextDarwin_arm64.cpp
#define GPR_OFFSET(idx) ((idx) * 8)
#define GPR_OFFSET_NAME(reg) (LLVM_EXTENSION offsetof (RegisterContextLinux_arm64::GPR, reg))

#define FPU_OFFSET(idx) ((idx) * 16 + sizeof (RegisterContextLinux_arm64::GPR))
#define FPU_OFFSET_NAME(reg) (LLVM_EXTENSION offsetof (RegisterContextLinux_arm64::FPU, reg) + sizeof (RegisterContextLinux_arm64::GPR))

#define EXC_OFFSET_NAME(reg) (LLVM_EXTENSION offsetof (RegisterContextLinux_arm64::EXC, reg) + sizeof (RegisterContextLinux_arm64::GPR) + sizeof (RegisterContextLinux_arm64::FPU))
#define DBG_OFFSET_NAME(reg) (LLVM_EXTENSION offsetof (RegisterContextLinux_arm64::DBG, reg) + sizeof (RegisterContextLinux_arm64::GPR) + sizeof (RegisterContextLinux_arm64::FPU) + sizeof (RegisterContextLinux_arm64::EXC))

#define DEFINE_DBG(reg, i)  #reg, NULL, sizeof(((RegisterContextLinux_arm64::DBG *)NULL)->reg[i]), DBG_OFFSET_NAME(reg[i]), lldb::eEncodingUint, lldb::eFormatHex, { LLDB_INVALID_REGNUM, LLDB_INVALID_REGNUM, LLDB_INVALID_REGNUM, LLDB_INVALID_REGNUM, dbg_##reg##i }, NULL, NULL, NULL, 0
#define REG_CONTEXT_SIZE (sizeof (RegisterContextLinux_arm64::GPR) + sizeof (RegisterContextLinux_arm64::FPU) + sizeof (RegisterContextLinux_arm64::EXC))


//-----------------------------------------------------------------------------
// Include RegisterInfos_arm64 to declare our g_register_infos_arm64 structure.
//-----------------------------------------------------------------------------
#define DECLARE_REGISTER_INFOS_ARM64_STRUCT
#include "RegisterInfos_arm64.h"
#undef DECLARE_REGISTER_INFOS_ARM64_STRUCT

static const lldb_private::RegisterInfo *
GetRegisterInfoPtr (const lldb_private::ArchSpec &target_arch)
{
    switch (target_arch.GetMachine())
    {
        case llvm::Triple::aarch64:
            return g_register_infos_arm64;
        default:
            assert(false && "Unhandled target architecture.");
            return NULL;
    }
}

static uint32_t
GetRegisterInfoCount(const lldb_private::ArchSpec &target_arch)
{
    switch (target_arch.GetMachine())
    {
        case llvm::Triple::aarch64:
            return static_cast<uint32_t>(sizeof(g_register_infos_arm64) / sizeof(g_register_infos_arm64[0]));
        default:
            assert(false && "Unhandled target architecture.");
            return 0;
    }
}

RegisterContextLinux_arm64::RegisterContextLinux_arm64(const lldb_private::ArchSpec &target_arch) :
    lldb_private::RegisterInfoInterface(target_arch),
    m_register_info_p(GetRegisterInfoPtr(target_arch)),
    m_register_info_count(GetRegisterInfoCount(target_arch))
{
}

size_t
RegisterContextLinux_arm64::GetGPRSize() const
{
    return sizeof(struct RegisterContextLinux_arm64::GPR);
}

const lldb_private::RegisterInfo *
RegisterContextLinux_arm64::GetRegisterInfo() const
{
    return m_register_info_p;
}

uint32_t
RegisterContextLinux_arm64::GetRegisterCount() const
{
    return m_register_info_count;
}
