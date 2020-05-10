#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "consts.h"

void idt_set_gate(
    PIDTENTRY idt, 
    uint8 num, 
    uint32 offset, 
    uint16 seg_sel, 
    uint8 flags) {

    idt[num].offset_l = offset & 0xFFFF;
    idt[num].offset_h = (offset >> 16) & 0xFFFF;
    idt[num].seg_sel = seg_sel;
    idt[num].zero = 0;
    idt[num].flags = flags;
}


uint32 PF_ADDR = 0x1FC00004; 
uint32 my_ptr =  0xF03C01FC;
uint32 counter = 0;

void __declspec( naked ) df_handler(void)
{
    __asm {
        push eax

        mov eax, my_ptr
        or dword ptr[eax], 1h
        invlpg [eax]
        lea eax, counter           
        add [eax], 1            

        pop eax

        add esp, 4
        iretd
    }
}

#define ADDR 0x1FC00000
#define MEMORY_ALLOCAE 8*1024*1024
#define OFFSET 4*1024*1024

void rewrite_df_exception(FILE* f, PSYSINFO sysinfo) {
    PIDTENTRY idt_table = (PIDTENTRY)sysinfo->idt.base;
    
    uint32 new_offset;
    uint16 new_segment;

    __asm {
        mov edx, offset df_handler
        mov new_offset, edx
        mov ax, seg df_handler
        mov new_segment, ax
    }

    fprintf(f, "Setting #DF handler\n");
    
    idt_set_gate(idt_table, 
        DF_EXCEPTION, 
        (uint32)new_offset, 
        new_segment, 
        idt_table[PF_EXCEPTION].flags);
    
    fprintf(f, "#DF handler has been set\n");
}

void enable_paging()
{
    int i;
    char* addr = (char*)ADDR;
    void* p = malloc(MEMORY_ALLOCAE);
    uint32 _p = (uint32)p;
    uint32 _p_aligned = (_p & 0xFFC00000) + OFFSET;
    uint32 _pd = _p_aligned + 0;
    PPTE pd = (PPTE)_pd;

    //trivial mapping
    for (i=0; i < 1024; i++) {
        pd[i].raw = i*0x400000;

        pd[i].raw |= (i < 512) ? PTE_TRIVIAL_LARGE : PTE_TRIVIAL_NONPRESENT;
    }
    //self-mapping
    pd[0x3c0].raw = _p_aligned | PTE_TRIVIAL_SELFMAP; //self-mapped to 0xF0000000
    //unmap 0x1FC00000 address, this addr should be less than phys mem available to VM!!!
    pd[0x7F].raw &= 0xFFFFFFFE; //virtual range 0x1FC00000-0x1FFFFFFF is unpresent
    __asm {
        pushfd
        cli
        mov eax, _p_aligned
        mov cr3, eax         //this also resets instruction cache
        mov eax, cr4
        or eax, 0x90
        mov cr4, eax        //enable CR4.PSE and CR4.PGE
        mov eax, cr0
        or eax, 0x80000000
        mov cr0, eax        //enable CR0.PG
        popfd
    }
}

void pf_test(PSYSINFO sysinfo)
{
    PIDTENTRY idt_table = (PIDTENTRY)sysinfo->idt.base;
    uint32 old_offset = idt_table[PF_EXCEPTION].offset_h << 16 | idt_table[PF_EXCEPTION].offset_l;
    uint16 old_segment = idt_table[PF_EXCEPTION].seg_sel;
    uint32 new_offset;
    uint16 new_segment = 0;
    uint32 *addr = NULL;

    printf("Counter BEFORE: %d\n", counter);

    idt_set_gate(idt_table,
        PF_EXCEPTION,
        new_offset, new_segment,
        idt_table[PF_EXCEPTION].flags);

    addr = (uint32 *)PF_ADDR;
    printf("Try reading memory 0x%X\n", *addr); // to recover page
    printf("Counter AFTER: %d\n", counter);
}

void main()
{
    SYSINFO sysinfo;
    FILE* f = fopen("a:\\df.txt", "w");

    memset(&sysinfo, 0, sizeof(sysinfo));
    get_sysinfo(&sysinfo);

    rewrite_df_exception(f, &sysinfo);
    enable_paging();
    pf_test(&sysinfo);

    fclose(f);
}
