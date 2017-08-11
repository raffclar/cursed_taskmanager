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

bool InitialiseLibrary(const std::string &library_name) {
    HMODULE hook_library = LoadLibrary(library_name.c_str());

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

bool RunLoop() {
    std::string hook_lib_name("cursed_taskmgr.dll");

    bool loaded = InitialiseLibrary(hook_lib_name);

    // For cygwin compiled libraries
    if (!loaded) {
        loaded = InitialiseLibrary("cyg" + hook_lib_name);;
    }

    if (!loaded) {
        return false;
    }


    int tries = 0;
    bool success = false;

    while (tries < 100) {
        HWND taskmgr_handle = FindWindow("TaskManagerWindow", "Task Manager");

        if (taskmgr_handle != nullptr && !success) {
            success = InitialiseTaskManagerHook(taskmgr_handle);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        if (success) {
            tries++;
        }
    }

    UninitialiseTaskManagerHook();
}

int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmd, int show) {
    RunLoop();
    return 0;
}
