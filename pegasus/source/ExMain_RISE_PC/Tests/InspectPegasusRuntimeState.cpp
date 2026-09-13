#include <windows.h>
#include <tlhelp32.h>

#include <cstdio>
#include <cstdint>
#include <cstdlib>

namespace {
constexpr std::uintptr_t kPreferredImageBase = 0x00400000u;
constexpr std::uintptr_t kCurrentProtocolStateVa = 0x05F1AC54u;
constexpr std::uintptr_t kHeroPointerVa = 0x07D865C4u;
constexpr std::uintptr_t kAutoQaCharacterStartedVa = 0x09C6FB0Eu;
constexpr std::uintptr_t kAutoQaCommandSentVa = 0x09C6FB0Fu;
constexpr std::uintptr_t kAutoQaPoolReportedVa = 0x09C6FB28u;

template <typename T>
bool ReadValue(HANDLE process, std::uintptr_t address, T& value)
{
    SIZE_T read = 0;
    return ReadProcessMemory(process, reinterpret_cast<const void*>(address),
        &value, sizeof(value), &read) != FALSE && read == sizeof(value);
}

const char* ProtocolName(int state)
{
    switch (state)
    {
    case 0: return "REQUEST_JOIN_SERVER";
    case 2: return "RECEIVE_JOIN_SERVER_SUCCESS";
    case 50: return "REQUEST_CHARACTERS_LIST";
    case 51: return "RECEIVE_CHARACTERS_LIST";
    case 60: return "REQUEST_JOIN_MAP_SERVER";
    case 61: return "RECEIVE_JOIN_MAP_SERVER";
    default: return "OTHER";
    }
}
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::fprintf(stderr, "usage: InspectPegasusRuntimeState PID\n");
        return 2;
    }
    const DWORD pid = static_cast<DWORD>(std::strtoul(argv[1], nullptr, 10));
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE |
        TH32CS_SNAPMODULE32, pid);
    if (snapshot == INVALID_HANDLE_VALUE)
    {
        std::fprintf(stderr, "CreateToolhelp32Snapshot failed: %lu\n",
            GetLastError());
        return 3;
    }
    MODULEENTRY32 module = {};
    module.dwSize = sizeof(module);
    if (!Module32First(snapshot, &module))
    {
        std::fprintf(stderr, "Module32First failed: %lu\n", GetLastError());
        CloseHandle(snapshot);
        return 4;
    }
    CloseHandle(snapshot);

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION |
        PROCESS_VM_READ, FALSE, pid);
    if (!process)
    {
        std::fprintf(stderr, "OpenProcess failed: %lu\n", GetLastError());
        return 5;
    }
    const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(module.modBaseAddr);
    const auto runtimeAddress = [base](std::uintptr_t preferredVa) {
        return base + (preferredVa - kPreferredImageBase);
    };

    int protocol = -1;
    std::uint32_t hero = 0;
    unsigned char characterStarted = 0;
    unsigned char commandSent = 0;
    unsigned char poolReported = 0;
    const bool ok = ReadValue(process, runtimeAddress(kCurrentProtocolStateVa), protocol) &&
        ReadValue(process, runtimeAddress(kHeroPointerVa), hero) &&
        ReadValue(process, runtimeAddress(kAutoQaCharacterStartedVa), characterStarted) &&
        ReadValue(process, runtimeAddress(kAutoQaCommandSentVa), commandSent) &&
        ReadValue(process, runtimeAddress(kAutoQaPoolReportedVa), poolReported);
    CloseHandle(process);
    if (!ok)
    {
        std::fprintf(stderr, "ReadProcessMemory failed: %lu\n", GetLastError());
        return 6;
    }

    std::printf("pid=%lu base=0x%p protocol=%d(%s) hero=0x%08X "
        "auto_character=%u auto_command=%u auto_pool=%u\n",
        pid, module.modBaseAddr, protocol, ProtocolName(protocol), hero,
        characterStarted, commandSent, poolReported);
    return 0;
}
