// Standard
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// Project
#include "consts.h"

// Helpers

void BSOD() {
    __asm {
        xor ax, ax
        mov cs, ax
    }
}

// End of helpers

// For task 1
// Task 1
char* code_data_descr_type_decode(uint8 type) {
    switch(type) {
        case 0:
            return "Data, Read-Only";
        case 1:
            return "Data, Read-Only, accessed";
        case 2:
            return "Data, Read/Write";
        case 3:
            return "Data, Read/Write, accessed";
        case 4:
            return "Data, Read-Only, expand-down";
        case 5:
            return "Data, Read-Only, expand-down, accessed";
        case 6:
            return "Data, Read/Write, expand-down";
        case 7:
            return "Data, Read/Write, expand-down, accessed";
        case 8:
            return "Code, Execute-Only";
        case 9:
            return "Code, Execute-Only, accessed";
        case 10:
            return "Code, Execute/Read";
        case 11:
            return "Code, Execute/Read, accessed";
        case 12:
            return "Code, Execute-Only, conforming";
        case 13:
            return "Code, Execute-Only, conforming, accessed";
        case 14:
            return "Code, Execute/Read, conforming";
        case 15:
            return "Code, Execute/Read, conforming, accessed";
        default:
            return "Error!";
    }
}

char* system_descr_type_decode(uint8 type) {
    switch(type) {
        case 0:
            return "Reserved";
        case 1:
            return "16-bit TSS (Available)";
        case 2:
            return "LDT";
        case 3:
            return "16-bit TSS (Busy)";
        case 4:
            return "16-bit Call Gate";
        case 5:
            return "Task Gate";
        case 6:
            return "16-bit Interrupt Gate";
        case 7:
            return "16-bit Trap Gate";
        case 8:
            return "Reserved";
        case 9:
            return "32-bit TSS (Available)";
        case 10:
            return "Reserved";
        case 11:
            return "32-bit TSS (Busy)";
        case 12:
            return "32-bit Call Gate";
        case 13:
            return "Reserved";
        case 14:
            return "32-bit Interrupt Gate";
        case 15:
            return "32-bit Trap Gate";
        default:
            return "Error!";
    }
}

void fprint_descripor(FILE* f, PDESCRIPTOR d) {
    fprintf(f, "\tVALUE=0x%08X-%08X PRESENT=[%s] \n", d->raw.high, d->raw.low, d->desc.p ? "yes":"no");
    
    if (d->desc.p) {
        fprintf(f, "\tBASE=0x%08X LIMIT=0x%08X \n", BASE_FROM_DESCRIPTOR(d), LIMIT_FROM_DESCRIPTOR(d));
        fprintf(f, "\tRING=%d TYPE=[%s] SYSTEM=[%s] DB=[%s]\n", d->desc.dpl, d->desc.s ? code_data_descr_type_decode(d->desc.type) : system_descr_type_decode(d->desc.type), d->desc.s ? "segment":"system", d->desc.db ? "32bit":"16bit");
    }
}


void fprint_desctable(FILE* f, uint32* base, uint32 limit) {
    int i;

    for(i = 0;; i++) { //i is an index in the array of 64bit descriptors
        DESCRIPTOR d;

        if (i * 8 > limit)
            break;

        fprintf(f, "Element %d (selector = %04X): \n", i, i<<3);

        d.raw.low = base[i*2];
        d.raw.high = base[i*2+1];

        fprint_descripor(f, &d);
    }
}

void fprint_tables(PSYSINFO sysinfo) {
    FILE* gdt_dump;
    FILE* idt_dump;
    FILE* ldt_dump;
    FILE* tss_dump;
    
    DESCRIPTOR d;
    
    //print GDT
    gdt_dump = fopen("a:\\gdt_dump.txt", "w");

    if (0 == gdt_dump) {
        printf("ERROR: cannot fopen gdt_dump \n");
    } else {
        fprint_desctable(gdt_dump, (uint32*)sysinfo->gdt.base, sysinfo->gdt.limit);
    }

    //print IDT
    idt_dump = fopen("a:\\idt_dump.txt","w");

    if (0 == idt_dump) {
        printf("ERROR: cannot fopen idt_dump \n");
    } else {
        fprint_desctable(idt_dump, (uint32*)sysinfo->idt.base, sysinfo->idt.limit);
    }
    
    //print LDT
    ldt_dump =  fopen("a:\\ldt_dump.txt","w");  

    if (0 == ldt_dump) {
        printf("ERROR: cannot fopen ldt_dump \n");
    } else {
        uint32* ldt_base;
        uint32 ldt_lim;
        uint32* base = (uint32*)(sysinfo->gdt.base);
        d.raw.low = base[((sysinfo->ldtr)>>3)*2];
        d.raw.high = base[((sysinfo->ldtr)>>3)*2+1];
        ldt_base = (uint32*) (BASE_FROM_DESCRIPTOR((&d)));
        ldt_lim = LIMIT_FROM_DESCRIPTOR((&d));

        if(d.desc.p){
            fprint_desctable(ldt_dump, ldt_base, ldt_lim);
        } else {
            fprintf(ldt_dump, "Segment is not present\n");
        }
    }
    
    //print TSS

    tss_dump =  fopen("a:\\tss_dump.txt","w");  

    if (0 == tss_dump) {
        printf("ERROR: cannot fopen tss_dump \n");
    } else {
        //fprint_desctable(tss_dump, (uint32*)sysinfo->idt.base, sysinfo->idt.limit);
    }

    fclose(gdt_dump);
    fclose(idt_dump);
    fclose(ldt_dump);
    fclose(tss_dump);
}

void print_info(PSYSINFO sysinfo){
    printf("Protected Mode: %s \n", (sysinfo->cr0 & MASK(CR0_PE))?"on":"off");
    printf("Paging Mode: %s \n",    (sysinfo->cr0 & MASK(CR0_PG))?"on":"off");
    printf("Ring: CPL=%d \n",       sysinfo->cpl);
    printf("================ \n");
    printf("GDT: base=0x%08X limit=0x%04X \n", sysinfo->gdt.base, sysinfo->gdt.limit);
    printf("IDT: base=0x%08X limit=0x%04X \n", sysinfo->idt.base, sysinfo->idt.limit);
    printf("LDTR: 0x%X\n",sysinfo->ldtr);
    printf("TR: 0x%X\n", sysinfo->tr);
}
// End For task 1

// For task 2
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
uint32 incr = 0;

void _interrupt pf_handler(void)
{
    __asm {
        //cli
        push eax
        push edx
        mov edx, cr2
        cmp edx, PF_ADDR        //"my" address
        jnz pf
        mov eax, my_ptr         //pde/pte corresponding to "my" unpresent address
        or dword ptr[eax], 1h   //restore P bit
        invlpg [eax]            //invalidate all paging caches for "my" address
        lea eax, incr           
        add [eax], 1            //inc c0. counter of "my" #PF
        jmp done
pf:
        pop edx
        pop eax
        // sub esp, 2
        push old_segment
        push old_offset
        retf
done:
        pop edx
        pop eax
        //sti
        add esp, 4
        iretd
    }
}

#define ADDR 0x1FC00000
#define MEMORY_ALLOCAE 8*1024*1024
#define OFFSET 4*1024*1024

void paging_task()
{
    int i;
    char* addr = (char*)ADDR;
    void* p = malloc(MEMORY_ALLOCAE);
    uint32 _p = (uint32)p;
    uint32 _p_aligned = (_p & 0xFFC00000) + OFFSET;
    uint32 _pd = _p_aligned + 0;
    PPTE pd = (PPTE)_pd;

    printf("Malloc 8Mb at 0x%08X-0x%08x, aligned at 0x%08X \n", _p, _p + MEMORY_ALLOCAE, _p_aligned);
    
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

    printf("Page directory adress: 0x%x %d\n", &_p_aligned, &_p_aligned);
    // printf("Read from 0x%08X = %x", addr, *addr);
}

void pf_test(PSYSINFO sysinfo)
{
    PIDTENTRY idt_table = (PIDTENTRY)sysinfo->idt.base;
    uint32 old_offset = idt_table[PF_EXCEPTION].offset_h << 16 | idt_table[PF_EXCEPTION].offset_l;
    uint16 old_segment = idt_table[PF_EXCEPTION].seg_sel;
    uint32 new_offset;
    uint16 new_segment;
    uint32 *addr = NULL;

    printf("MY PF counter: %d\n", incr);

    __asm {
        mov edx, offset pf_handler
        mov new_offset, edx
        mov ax, seg pf_handler
        mov new_segment, ax
    }

    printf("old: offset 0x%p segment 0x%p \n", old_offset, old_segment);
    printf("func: 0x%p \n", pf_handler);
    printf("offset: 0x%x segment: 0x%x \n", new_offset, new_segment);

    idt_set_gate(idt_table, PF_EXCEPTION, new_offset, new_segment,idt_table[PF_EXCEPTION].flags);

    addr = (uint32 *)PF_ADDR;
    printf("I am memory 0x%x\n", *addr); // to recover page
    //printf("I am memory 0x%x\n", *(addr + 8));
    //printf("I am memory 0x%x\n", *(addr)); // to see default page fault

    printf("MY PF: %d\n", incr);
}
// End For task 2

// For task 3

uint32 DF_RET_ADDR = 0;

void df_task()
{
    int i;
    char* addr = (char*)ADDR;
    void* p = malloc(MEMORY_ALLOCAE);
    uint32 _p = (uint32)p;
    uint32 _p_aligned = (_p & 0xFFC00000) + OFFSET;
    uint32 _pd = _p_aligned + 0;
    PPTE pd = (PPTE)_pd;

    printf("Malloc 8Mb at 0x%08X-0x%08x, aligned at 0x%08X \n", _p, _p + MEMORY_ALLOCAE, _p_aligned);
    
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

    printf("Page directory adress: 0x%x %d\n", &_p_aligned, &_p_aligned);
    // printf("Read from 0x%08X = %x", addr, *addr);
}

void _interrupt df_handler() {
    __asm {
        pop eax
        mov DF_RET_ADDR, eax
        push eax
    }
}

void _interrupt ud_handler() {
    
    // provoke an exception inside the exception handler

    __asm {
        pop ecx
        mov ecx, -1
        push ecx
        ret
    }
}


void rewrite_ud_exception(FILE* f, PSYSINFO sysinfo){
    PIDTENTRY idt_table = (PIDTENTRY)sysinfo->idt.base;
    // uint32 old_offset = idt_table[UD_EXCEPTION].offset_h << 16 | idt_table[UD_EXCEPTION].offset_l;
    // uint16 old_segment = idt_table[UD_EXCEPTION].seg_sel;
    uint32 new_offset;
    uint16 new_segment;

    __asm {
        mov edx, offset ud_handler
        mov new_offset, edx
        mov ax, seg ud_handler
        mov new_segment, ax
    }

    fprintf(f, "Setting #UD handler\n");
    
    idt_set_gate(idt_table, UD_EXCEPTION, (uint32)new_offset, new_segment, idt_table[UD_EXCEPTION].flags);
    
    fprintf(f, "#UD handler has been set\n");
}

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
    
    idt_set_gate(idt_table, DF_EXCEPTION, (uint32)new_offset, new_segment, idt_table[DF_EXCEPTION].flags);
    
    fprintf(f, "#DF handler has been set\n");
}
// End For task 3

// Tasks

void task_1(SYSINFO sysinfo) {
    printf(">> Task 1\n");
    
    fprint_tables(&sysinfo);
}

void task_2(SYSINFO sysinfo) {
    printf(">> Task 2\n");

    paging_task();
    pf_test(&sysinfo);
}

void task_3(SYSINFO sysinfo) {
    FILE* f = fopen("a:\\t3.txt", "w");
    printf(">> Task 3\n");

    //rewrite_ud_exception(f, &sysinfo);
    df_task();
    rewrite_df_exception(f, &sysinfo);

    fclose(f);

    // provoke an UD exception

    BSOD();
}

void task_4() {
    printf(">> Task 4\n");
}

// End of tasks
int main(int argc, char* argv[]) {
    SYSINFO sysinfo; 

    if (argc < 2) {
        printf(MSG_NOT_ENOUGH_ARGS);
        return 1;
    }

    // printf(MSG_TASK, argv[1]);

    // Get basic sysinfo

    memset(&sysinfo, 0, sizeof(sysinfo));
    get_sysinfo(&sysinfo);
    print_info(&sysinfo);

    // Decide which task to run

    if (strcmp(argv[1], "1") == 0) {
        task_1(sysinfo);
    } else
    if (strcmp(argv[1], "2") == 0) {
        task_2(sysinfo);
    } else 
    if (strcmp(argv[1], "3") == 0) {
        task_3(sysinfo);
    } else
    if (strcmp(argv[1], "4") == 0) {
        task_4(sysinfo);
    } else {
        printf(MSG_UNKNOWN_ARG, argv[1]);
    }

    BSOD();

    return 0;
}
