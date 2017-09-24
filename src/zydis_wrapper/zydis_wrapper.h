#ifndef ZYDIS_WRAPPER_H
#define ZYDIS_WRAPPER_H

#include "Zydis/Zydis.h"
#include <functional>

#define MAX_DISASM_BUFFER 16

class Zydis
{
public:
    static void GlobalInitialize();
    static void GlobalFinalize();
    Zydis();
    Zydis(const Zydis & capstone) = delete;
    ~Zydis();
    bool Disassemble(size_t addr, const unsigned char data[MAX_DISASM_BUFFER]);
    bool Disassemble(size_t addr, const unsigned char* data, int size);
    bool DisassembleSafe(size_t addr, const unsigned char* data, int size);
    const ZydisDecodedInstruction* GetInstr() const;
    bool Success() const;
    const char* RegName(ZydisRegister reg) const;
    std::string OperandText(int opindex) const;
    int Size() const;
    size_t Address() const;
    bool IsFilling() const;
    bool IsUnusual() const;
    bool IsNop() const;
    bool IsPushPop() const;
    ZydisMnemonic GetId() const;
    std::string InstructionText(bool replaceRipRelative = true) const;
    int OpCount() const;
    const ZydisDecodedOperand & operator[](int index) const;
    std::string Mnemonic() const;
    std::string MnemonicId() const;
    const char* MemSizeName(int size) const;
    size_t BranchDestination() const;
    size_t ResolveOpValue(int opindex, const std::function<size_t(ZydisRegister)> & resolveReg) const;
    bool IsBranchGoingToExecute(size_t cflags, size_t ccx) const;
    static bool IsBranchGoingToExecute(ZydisMnemonic id, size_t cflags, size_t ccx);
    bool IsConditionalGoingToExecute(size_t cflags, size_t ccx) const;
    static bool IsConditionalGoingToExecute(ZydisMnemonic id, size_t cflags, size_t ccx);

    enum RegAccessInfo : uint8_t
    {
        RAINone        = 0,
        RAIRead        = 1 << 0,
        RAIWrite       = 1 << 1,
        RAIImplicit    = 1 << 2,
        RAIExplicit    = 1 << 3
    };

    void RegInfo(uint8_t info[ZYDIS_REGISTER_MAX_VALUE + 1]) const;
    const char* FlagName(ZydisCPUFlag flag) const;

    enum BranchType : uint16_t
    {
        // Basic types.
        BTRet          = 1 << 0,
        BTCall         = 1 << 1,
        BTFarCall      = 1 << 2,
        BTSyscall      = 1 << 3, // Also sysenter
        BTSysret       = 1 << 4, // Also sysexit
        BTInt          = 1 << 5,
        BTInt3         = 1 << 6,
        BTInt1         = 1 << 7,
        BTIret         = 1 << 8,
        BTCondJmp      = 1 << 9,
        BTUncondJmp    = 1 << 10,
        BTFarJmp       = 1 << 11,
        BTXbegin       = 1 << 12,
        BTXabort       = 1 << 13,
        BTRsm          = 1 << 14,
        BTLoop         = 1 << 15,

        BTJmp          = BTCondJmp | BTUncondJmp,

        // Semantic groups (behaves like XX).
        BTCallSem      = BTCall | BTSyscall | BTInt,
        BTRetSem       = BTRet | BTSysret | BTIret | BTRsm | BTXabort,
        BTIntSem       = BTInt | BTInt1 | BTInt3 | BTSyscall,
        BTIretSem      = BTIret | BTSysret,
        BTJmpSem       = BTJmp | BTLoop,

        BTRtm          = BTXabort | BTXbegin,
        BTCtxSwitch    = BTIntSem | BTIretSem | BTRsm | BTFarCall | BTFarJmp,

        BTAny          = std::underlying_type_t<BranchType>(-1)
    };

    bool IsBranchType(std::underlying_type_t<BranchType> bt) const;

    // Shortcuts.
    bool IsRet() const { return IsBranchType(BTRet);    }
    bool IsCall() const { return IsBranchType(BTCall);   }
    bool IsJump() const { return IsBranchType(BTJmp);    }
    bool IsLoop() const { return IsBranchType(BTLoop);   }
    bool IsInt3() const { return IsBranchType(BTInt3);   }
private:
    static ZydisDecoder mDecoder;
    static ZydisFormatter mFormatter;
    static bool mInitialized;
    ZydisDecodedInstruction mInstr;
    char mInstrText[200];
    bool mSuccess;
    uint8_t mVisibleOpCount;
};

#endif //ZYDIS_WRAPPER_H