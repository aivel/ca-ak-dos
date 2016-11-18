#include "DefStructs.c"
#define UD_EXCEPTION 6

void __declspec( naked ) ud_handler(void)
    __asm{
		mov ax,1
		mov cs,ax//gpf
    }
}

void set_ud_handler(PSYSINFO sysinfo){
    uint32 offset_ud;
    uint16 segment;
    PIDTENTRY idt_table = (PIDTENTRY)sysinfo->idt.base;
    __asm{
        mov edx, offset ud_handler;
        mov offset_ud, edx;
        mov ax, seg ud_handler;
        mov segment, ax; 
    }

    idt_set_gate(idt_table, UD_EXCEPTION, offset_ud, 
            segment, idt_table[UD_EXCEPTION].flags);

}
void main(){

    SYSINFO sysinfo;
    memset(&sysinfo, 0, sizeof(sysinfo));
    get_sysinfo(&sysinfo);

    set_ud_handler(&sysinfo);

    __asm {
        xor ax,ax;
        mov cs,ax;
    }
}

