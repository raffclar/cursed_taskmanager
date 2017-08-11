#include "Zalgo.h"

#define SUP_CAP 1
#define MID_CAP 1
#define SUB_CAP 1

// Data structures borrowed from https://eeemo.net/
// All credit to tchouky
const wchar_t Zalgo::super_script[] = {
        L'\u030d', L'\u030e', L'\u0304', L'\u0305',
        L'\u033f', L'\u0311', L'\u0306', L'\u0310',
        L'\u0352', L'\u0357', L'\u0351', L'\u0307',
        L'\u0308', L'\u030a', L'\u0342', L'\u0343',
        L'\u0344', L'\u034a', L'\u034b', L'\u034c',
        L'\u0303', L'\u0302', L'\u030c', L'\u0350',
        L'\u0300', L'\u0301', L'\u030b', L'\u030f',
        L'\u0312', L'\u0313', L'\u0314', L'\u033d',
        L'\u0309', L'\u0363', L'\u0364', L'\u0365',
        L'\u0366', L'\u0367', L'\u0368', L'\u0369',
        L'\u036a', L'\u036b', L'\u036c', L'\u036d',
        L'\u036e', L'\u036f', L'\u033e', L'\u035b',
        L'\u0346', L'\u031a'
};

const wchar_t Zalgo::mid_script[] = {
        L'\u0315', L'\u031b', L'\u0340', L'\u0341',
        L'\u0358', L'\u0321', L'\u0322', L'\u0327',
        L'\u0328', L'\u0334', L'\u0335', L'\u0336',
        L'\u034f', L'\u035c', L'\u035d', L'\u035e',
        L'\u035f', L'\u0360', L'\u0362', L'\u0338',
        L'\u0337', L'\u0361', L'\u0489'
};

const wchar_t Zalgo::sub_script[] = {
        L'\u0316', L'\u0317', L'\u0318', L'\u0319',
        L'\u031c', L'\u031d', L'\u031e', L'\u031f',
        L'\u0320', L'\u0324', L'\u0325', L'\u0326',
        L'\u0329', L'\u032a', L'\u032b', L'\u032c',
        L'\u032d', L'\u032e', L'\u032f', L'\u0330',
        L'\u0331', L'\u0332', L'\u0333', L'\u0339',
        L'\u033a', L'\u033b', L'\u033c', L'\u0345',
        L'\u0347', L'\u0348', L'\u0349', L'\u034d',
        L'\u034e', L'\u0353', L'\u0354', L'\u0355',
        L'\u0356', L'\u0359', L'\u035a', L'\u0323'
};

const int Zalgo::super_length = sizeof(super_script) / sizeof(*super_script);
const int Zalgo::mid_length = sizeof(mid_script) / sizeof(*mid_script);
const int Zalgo::sub_length = sizeof(sub_script) / sizeof(*sub_script);

wchar_t Zalgo::RandomChar(const wchar_t *characters, const int length) {
    int chosen = std::rand() % length;
    return characters[chosen];
}

std::wstring Zalgo::Corrupt(std::wstring szText) {
    std::wstring corrupted;

    for (wchar_t i : szText) {
        corrupted += i;

        if (std::rand() % 2 == 0) {
            // Superscript
            for (int j = 0; j < SUP_CAP; j++) {
                corrupted += RandomChar(super_script, super_length);
            }

            // Middlescript
            for (int j = 0; j < MID_CAP; j++) {
                corrupted += RandomChar(mid_script, mid_length);
            }

            // Subscript
            for (int j = 0; j < SUB_CAP; j++) {
                corrupted += RandomChar(sub_script, sub_length);
            }
        }
    }

    return corrupted;
}
