#include <windows.h>
#include <iostream>
#include <chrono>
#include <thread>

typedef void (*install_ptr)(uint32_t);

typedef void (*uninstall_ptr)();

typedef void (*win_msg_ptr)(wchar_t *, uint32_t);

typedef uint32_t (*win_msg_size)();

static install_ptr InstallHook = nullptr;
static uninstall_ptr UninstallHook = nullptr;
static win_msg_ptr GetWinMessageIdentifier = nullptr;
static win_msg_size GetWinMessageSize = nullptr;
static HWND taskmgr_handle = nullptr;

// LoadLibrary() method for possible of the packaging the 
// dll inside the executable
bool InitialiseLibrary(const std::wstring &library_name) {
    HMODULE hook_library = LoadLibraryW(library_name.c_str());

    if (hook_library == nullptr) {
        return false;
    }

    FARPROC install_addr = GetProcAddress(hook_library, "Install");
    FARPROC uninstall_addr = GetProcAddress(hook_library, "Uninstall");
    FARPROC win_msg_addr = GetProcAddress(hook_library, "GetWinMessageIdentifier");
    FARPROC win_msg_size_addr = GetProcAddress(hook_library, "GetWinMessageSize");
    InstallHook = reinterpret_cast<install_ptr>(install_addr);
    UninstallHook = reinterpret_cast<uninstall_ptr>(uninstall_addr);
    GetWinMessageIdentifier = reinterpret_cast<win_msg_ptr>(win_msg_addr);
    GetWinMessageSize = reinterpret_cast<win_msg_size>(win_msg_size_addr);

    return !(GetWinMessageSize == nullptr || InstallHook == nullptr);
}

bool InitialiseTaskManagerHook(HWND new_taskmgr_handle) {
    bool same_taskmgr = taskmgr_handle == new_taskmgr_handle;
    same_taskmgr = same_taskmgr && taskmgr_handle != nullptr;

    if (new_taskmgr_handle == nullptr || same_taskmgr) {
        return false;
    }

    uint32_t thread_id = GetWindowThreadProcessId(new_taskmgr_handle, nullptr);

    if (InstallHook != nullptr) {
        InstallHook(thread_id);
    } else {
        return false;
    }

    taskmgr_handle = new_taskmgr_handle;
    return true;
}

bool UninitialiseTaskManagerHook() {
    if (taskmgr_handle == nullptr) {
        return false;
    }

    if (UninstallHook != nullptr) {
        UninstallHook();
    } else {
        return false;
    }

    taskmgr_handle = nullptr;
    return true;
}

#pragma warning(suppress: 4100)
int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmd, int show) {
    if (!InitialiseLibrary(L"cursed_taskmgr.dll")) {
        return 1;
    }

    int tries = 0;
    bool hooked = false;

    while (tries < 100) {
        HWND new_taskmgr_handle = FindWindowW(L"TaskManagerWindow", L"Task Manager");

        // Hook if we found a task manager window and its handle is new
        if (new_taskmgr_handle != nullptr && new_taskmgr_handle != taskmgr_handle) {
            InitialiseTaskManagerHook(new_taskmgr_handle);
            tries = 0;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        if (hooked) {
            tries++;
        }
    }

    UninitialiseTaskManagerHook();
    return 0;
}
