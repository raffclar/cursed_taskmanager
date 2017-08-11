#ifndef CURSED_TASKMANAGER_DLL_ZALGO_HPP
#define CURSED_TASKMANAGER_DLL_ZALGO_HPP

#include <string>

class Zalgo {
private:
    static const int super_length;
    static const int mid_length;
    static const int sub_length;
    static const wchar_t super_script[];
    static const wchar_t mid_script[];
    static const wchar_t sub_script[];
public:
    static wchar_t RandomChar(const wchar_t *characters, int length);

    static std::wstring Corrupt(std::wstring);
};

#endif
