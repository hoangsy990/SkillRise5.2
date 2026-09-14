#pragma once

// Slayer isolated QA only. Credentials are read from the explicitly named
// Windows Generic Credential selected by RISE_QA_CREDENTIAL; nothing is
// enumerated, echoed, persisted to source, or written to QA logs.
#ifdef RISE_SLAYER_RUNTIME_QA
#include <windows.h>
#include <wincred.h>
#include <wchar.h>
#include <string.h>
#pragma comment(lib, "Advapi32.lib")

namespace rise { namespace slayerqa {

inline bool ReadCredentials(char* user, size_t userSize,
    char* password, size_t passwordSize)
{
    wchar_t target[256] = {};
    const DWORD length = GetEnvironmentVariableW(L"RISE_QA_CREDENTIAL",
        target, static_cast<DWORD>(sizeof(target) / sizeof(target[0])));
    if (length <= 8 || length >= sizeof(target) / sizeof(target[0])
        || wcsncmp(target, L"RISE_QA:", 8) != 0)
        return false;

    PCREDENTIALW credential = NULL;
    if (!CredReadW(target, CRED_TYPE_GENERIC, 0, &credential))
        return false;

    bool valid = credential->UserName != NULL
        && credential->CredentialBlob != NULL;
    const size_t chars = credential->CredentialBlobSize / sizeof(wchar_t);
    valid = valid
        && credential->CredentialBlobSize % sizeof(wchar_t) == 0
        && chars > 0 && chars < passwordSize;

    if (valid)
    {
        const size_t userChars = wcslen(credential->UserName);
        valid = userChars > 0 && userChars < userSize;
        for (size_t i = 0; valid && i < userChars; ++i)
        {
            const wchar_t c = credential->UserName[i];
            valid = c >= 33 && c <= 126;
            user[i] = static_cast<char>(c);
        }
        if (valid)
            user[userChars] = 0;

        const wchar_t* blob = reinterpret_cast<const wchar_t*>(
            credential->CredentialBlob);
        for (size_t i = 0; valid && i < chars; ++i)
        {
            valid = blob[i] >= 33 && blob[i] <= 126;
            password[i] = static_cast<char>(blob[i]);
        }
        if (valid)
            password[chars] = 0;
    }

    if (credential->CredentialBlob)
        SecureZeroMemory(credential->CredentialBlob,
            credential->CredentialBlobSize);
    CredFree(credential);
    if (!valid)
    {
        SecureZeroMemory(user, userSize);
        SecureZeroMemory(password, passwordSize);
    }
    return valid;
}

} }
#endif
