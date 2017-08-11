#include <windows.h>
#include <fcntl.h>
#include <commctrl.h>
#include <string>
#include <locale>
#include <memory>
#include <iostream>
#include <vector>
#include "Zalgo.h"

HHOOK hook_handle;
HINSTANCE instance_handle;
HWND sys_list_view;
HWND sys_tab_ctrl;
HWND taskmgr_handle;

void SetSysListViewEntryText(HWND table_handle) {
    auto count = static_cast<int>(SendMessageW(table_handle, LVM_GETITEMCOUNT, 0, 0));

    for (int i = 0; i < count; i++) {
        for (int j = 0; j < 7; j++) {
            std::vector<wchar_t> buffer(1024);
            LVITEMW lvItem{0};
            lvItem.mask = LVIF_TEXT;
            lvItem.iItem = i;
            lvItem.iSubItem = j;
            lvItem.pszText = buffer.data();
            lvItem.cchTextMax = static_cast<int>(buffer.size());
            SendMessageW(table_handle, LVM_GETITEMW, 0, (LPARAM) &lvItem);
            std::wstring text(buffer.data());

            if (text[0] != L' ') {
                // Modify text
                text = L" " + Zalgo::Corrupt(text);
                buffer.assign(text.c_str(), text.c_str() + text.size());
                lvItem.pszText = buffer.data();
                lvItem.cchTextMax = static_cast<int>(buffer.size());
                SendMessageW(table_handle, LVM_SETITEMW, 0, (LPARAM) &lvItem);
            }
        }
    }

    count = static_cast<int>(SendMessageW(sys_tab_ctrl, TCM_GETITEMCOUNT, 0, 0));

    for (int i = 0; i < count; i++) {
        std::vector<wchar_t> buffer(1024);
        TCITEMW tci_item{0};
        tci_item.mask = TCIF_TEXT;
        tci_item.pszText = buffer.data();
        tci_item.cchTextMax = static_cast<int>(buffer.size());
        SendMessageW(sys_tab_ctrl, TCM_GETITEMW, i, (LPARAM) &tci_item);
        std::wstring text(buffer.data());

        if (text[0] != L' ') {
            text = L" " + Zalgo::Corrupt(text);
            buffer.assign(text.c_str(), text.c_str() + text.size());
            tci_item.pszText = buffer.data();
            tci_item.cchTextMax = static_cast<int>(buffer.size());
            SendMessageW(sys_tab_ctrl, TCM_SETITEMW, i, (LPARAM) &tci_item);
        }
    }
}

LRESULT CALLBACK CallWndProc(int code, WPARAM wParam, LPARAM lParam) {
    auto parameters = reinterpret_cast<PCWPSTRUCT>(lParam);
    bool action = code == HC_ACTION;
    bool is_redraw = parameters->message == WM_SETREDRAW;
    bool is_sys_list_view = parameters->hwnd == sys_list_view;
    bool is_sys_tab_ctrl = parameters->hwnd == sys_tab_ctrl;

    if (action) {
        if (is_redraw && parameters->wParam == TRUE && is_sys_list_view) {
            SetSysListViewEntryText(parameters->hwnd);
            std::wstring title = Zalgo::Corrupt(L"Task Manager");
            SetWindowTextW(taskmgr_handle, title.c_str());

            std::cout << "test 1" << std::endl;
            std::cout << sys_tab_ctrl << std::endl;
        } else if (is_redraw && is_sys_tab_ctrl) {
        }
    }

    return CallNextHookEx(hook_handle, code, wParam, lParam);
}

// Avoid C++ name mangling
extern "C" {
    __declspec(dllexport) void Install(DWORD thread_id) {
        hook_handle = SetWindowsHookExW(WH_CALLWNDPROC, CallWndProc, instance_handle, thread_id);
    }

    __declspec(dllexport) void Uninstall() {
        UnhookWindowsHookEx(hook_handle);
    }

    __declspec(dllexport) void GetWinMessageIdentifier(wchar_t *buffer, uint32_t size) {
        memcpy(buffer, L"taskmgr_mod", size);
    }

    __declspec(dllexport) uint32_t GetWinMessageSize() {
        return sizeof(L"taskmgr_mod");
    }
};

struct ChildWindowData {
    HWND *handle_pointer;
    std::wstring child_class;
};

BOOL CALLBACK EnumChildProc(HWND current_handle, LPARAM l_param) {
    auto child_data = reinterpret_cast<ChildWindowData *>(l_param);
    wchar_t class_name[MAX_CLASS_NAME];
    uint32_t copied = RealGetWindowClassW(current_handle, class_name, MAX_CLASS_NAME);

    if (copied == 0) {
        // End enumeration
        return FALSE;
    }

    if (class_name == child_data->child_class) {
        // We found it
        *child_data->handle_pointer = current_handle;
        return FALSE;
    }

    return TRUE;
}

HWND GetChildWindow(const std::string &parent_class, const std::string &parent_window, std::string child_class) {
    std::unique_ptr<wchar_t[]> parent_class_w(new wchar_t[parent_class.size() + 1]());
    std::unique_ptr<wchar_t[]> parent_window_w(new wchar_t[parent_window.size() + 1]());
    std::unique_ptr<wchar_t[]> child_class_w(new wchar_t[child_class.size() + 1]());

    mbstowcs(parent_class_w.get(), parent_class.c_str(), parent_class.size() + 1);
    mbstowcs(parent_window_w.get(), parent_window.c_str(), parent_window.size() + 1);
    mbstowcs(child_class_w.get(), child_class.c_str(), child_class.size() + 1);

    HWND parent_handle = FindWindowW(parent_class_w.get(), parent_window_w.get());

    if (parent_handle == nullptr) {
        return parent_handle;
    }

    HWND child_handle = nullptr;

    ChildWindowData child_data{
            &child_handle,
            child_class_w.get()
    };

    EnumChildWindows(parent_handle, EnumChildProc, (LPARAM)&child_data);

    return child_handle;
}

BOOL ProcessAttach(HINSTANCE handle) {
    instance_handle = handle;

    uint32_t task_pid;
    taskmgr_handle = FindWindowW(L"TaskManagerWindow", L"Task Manager");
    GetWindowThreadProcessId(taskmgr_handle,
                             reinterpret_cast<LPDWORD>(&task_pid));
    bool is_taskmgr = task_pid == GetCurrentProcessId();

    if (is_taskmgr) {
        sys_list_view = GetChildWindow("TaskManagerWindow",
                                       "Task Manager", "SysListView32");
        sys_tab_ctrl = GetChildWindow("TaskManagerWindow",
                                      "Task Manager", "SysTabControl32");
    }

    return TRUE;
}

BOOL ProcessDetach() {
    return TRUE;
}

extern "C" BOOL WINAPI DllMain(
        HINSTANCE hinstDLL,
        DWORD fdwReason,
        LPVOID lpvReserved
) {
    BOOL success = FALSE;

    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            success = ProcessAttach(hinstDLL);
            break;
        case DLL_PROCESS_DETACH:
            success = ProcessDetach();
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        default:
            break;
    }

    return success;
}
