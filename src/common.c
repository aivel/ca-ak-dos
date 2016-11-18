#define C_COMMON

void BSOD() {
    __asm {
        xor ax, ax
        mov cs, ax
    }
}
