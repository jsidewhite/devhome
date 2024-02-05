#pragma once

#include <codecvt>
#include <filesystem>
#include <locale>
#include <optional>
#include <sstream>
#include <type_traits>
#include <windows.h>
#include <shellapi.h>
#include <shlobj_core.h>
#include <wil/token_helpers.h>
#include <wil/result.h>

#define VERIFY_ARE_EQUAL(actual, expected) VerifyAreEqual((actual), (expected), __FILE__, __LINE__)

// Types of registry data
enum class REG
{
    SZ,
    EXPAND_SZ,
    DWORD
};

namespace details
{
    // Type traits for return types of registry queries
    template<REG T>
    struct return_type;

    template<>
    struct return_type<REG::SZ>
    {
        using type = std::wstring;
    };

    template<>
    struct return_type<REG::EXPAND_SZ>
    {
        using type = std::wstring;
    };

    template<>
    struct return_type<REG::DWORD>
    {
        using type = DWORD;
    };
};

// Registry Helpers

template <REG T>
typename details::return_type<T>::type GetRegistryValue(HKEY hiveKey, std::wstring const & path, std::optional<std::wstring> const & valueName);

template <>
inline typename details::return_type<REG::SZ>::type GetRegistryValue<REG::SZ>(HKEY hiveKey, std::wstring const & path, std::optional<std::wstring> const & valueName)
{
    PCWSTR pszValueName = nullptr;
    if (valueName)
    {
        pszValueName = valueName.value().c_str();
    }

    wchar_t buffer[256];
    DWORD cbBuffer = sizeof(buffer);
    THROW_IF_FAILED(HRESULT_FROM_WIN32(RegGetValueW(hiveKey, path.c_str(), pszValueName, RRF_RT_REG_SZ, nullptr, buffer, &cbBuffer)));
    return {buffer};
}

template <>
inline typename details::return_type<REG::EXPAND_SZ>::type GetRegistryValue<REG::EXPAND_SZ>(HKEY hiveKey, std::wstring const & path, std::optional<std::wstring> const & valueName)
{
    PCWSTR pszValueName = nullptr;
    if (valueName)
    {
        pszValueName = valueName.value().c_str();
    }

    wchar_t buffer[256];
    DWORD cbBuffer = sizeof(buffer);
    THROW_IF_FAILED(HRESULT_FROM_WIN32(RegGetValueW(hiveKey, path.c_str(), pszValueName, RRF_RT_REG_SZ, nullptr, buffer, &cbBuffer)));
    return {buffer};
}

template <>
inline typename details::return_type<REG::DWORD>::type GetRegistryValue<REG::DWORD>(HKEY hiveKey, std::wstring const & path, std::optional<std::wstring> const & valueName)
{
    PCWSTR pszValueName = nullptr;
    if (valueName)
    {
        pszValueName = valueName.value().c_str();
    }

    DWORD buffer;
    DWORD cbBuffer = sizeof(buffer);
    THROW_IF_FAILED(HRESULT_FROM_WIN32(RegGetValueW(hiveKey, path.c_str(), pszValueName, RRF_RT_DWORD, nullptr, &buffer, &cbBuffer)));
    return buffer;
}


// Test helpers for CoffeeCommands that don't have access to taef macros
inline std::string toNarrowString(std::wstring const & str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    std::string utf8String = converter.to_bytes(str);
    return utf8String;
}

template <typename T, typename U>
inline void VerifyAreEqual(T const & actual, U const & expected, char const * file, DWORD line)
{
    if (expected == actual)
    {
        return;
    }

    std::wstringstream message;
    message << file << L"(" << line << L"): Expected = '" << expected << L"', Got = '" << actual << L"'";

    auto utf8Message = toNarrowString(message.str());
    throw std::runtime_error(utf8Message.c_str());
}

template <>
inline void VerifyAreEqual<PCWSTR, PCWSTR>(PCWSTR const & actual, PCWSTR const & expected, char const * file, DWORD line)
{
    if (0 == wcscmp(expected, actual))
    {
        return;
    }

    std::wstringstream message;
    message << file << L"(" << line << L"): Expected = '" << expected << L"', Got = '" << actual << L"'";

    auto utf8Message = toNarrowString(message.str());
    throw std::runtime_error(utf8Message.c_str());
}

inline void TerminateAndCloseProcessHandle(HANDLE processHandle)
{
    if (processHandle)
    {
        ::TerminateProcess(processHandle, ERROR_SUCCESS);
        ::WaitForSingleObject(processHandle, INFINITE);
        ::CloseHandle(processHandle);
    }
}

using unique_process_handle_terminator = wil::unique_any<HANDLE, decltype(&TerminateAndCloseProcessHandle), TerminateAndCloseProcessHandle>;

namespace test
{
    inline unique_process_handle_terminator ShellExecute(PCWSTR path)
    {
        SHELLEXECUTEINFOW info = { 0 };
        info.cbSize = sizeof(info);
        info.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
        info.lpVerb = L"open";
        info.lpFile = path;
        info.nShow = SW_HIDE;
        THROW_IF_WIN32_BOOL_FALSE(::ShellExecuteExW(&info));
        return unique_process_handle_terminator{ info.hProcess };
    }

    inline std::filesystem::path GetKnownFolder(const KNOWNFOLDERID& id)
    {
        wil::unique_cotaskmem_string knownFolder;
        THROW_IF_FAILED(::SHGetKnownFolderPath(id, NULL, NULL, &knownFolder));
        return knownFolder.get();
    }

    inline std::wstring GetPackageFullNameFromToken(HANDLE token)
    {
        wchar_t packageFullName[PACKAGE_FULL_NAME_MAX_LENGTH + 1]{};
        UINT32 length = ARRAYSIZE(packageFullName);
        THROW_IF_WIN32_ERROR(::GetPackageFullNameFromToken(token, &length, packageFullName));
        return {packageFullName};
    }

    inline std::wstring GetAppUserModelIdFromToken(HANDLE token)
    {
        WCHAR aumid[APPLICATION_USER_MODEL_ID_MAX_LENGTH]{};
        UINT32 length = ARRAYSIZE(aumid);
        THROW_IF_WIN32_ERROR(::GetApplicationUserModelIdFromToken(token, &length, aumid));
        return {aumid};
    }

    inline ULONG GetTokenPackageClaimFlags(HANDLE token)
    {
        PS_PKG_CLAIM pkgClaim{};
        THROW_IF_NTSTATUS_FAILED(::RtlQueryPackageClaims(token, nullptr, nullptr, nullptr, nullptr, nullptr, &pkgClaim, nullptr));
        return pkgClaim.Flags;
    }

    inline LONG GetTokenRunLevel(HANDLE token)
    {
        auto mandatoryLabel = wil::get_token_information<TOKEN_MANDATORY_LABEL>(token);
        LONG levelRid = static_cast<SID*>(mandatoryLabel->Label.Sid)->SubAuthority[0];
        return levelRid;
    }

    inline std::wstring GetCommandLineArguments(HANDLE processHandle)
    {
        ULONG bufferSize = 0;
        const NTSTATUS status = ::NtQueryInformationProcess(processHandle, ProcessCommandLineInformation, nullptr, 0, &bufferSize);
        if (status != STATUS_INFO_LENGTH_MISMATCH)
        {
            THROW_IF_NTSTATUS_FAILED(status);
        }

        std::unique_ptr<UNICODE_STRING> buffer(reinterpret_cast<UNICODE_STRING*>(new BYTE[bufferSize]));
        THROW_IF_NTSTATUS_FAILED(::NtQueryInformationProcess(processHandle, ProcessCommandLineInformation, buffer.get(), bufferSize, nullptr));

        std::wstring commandLine(buffer->Buffer, buffer->Length / sizeof(wchar_t));
        return commandLine;
    }
}