#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "DefStructs.c"

void idt_set_gate(PIDTENTRY idt, uint8 num, uint32 offset, uint16 seg_sel, uint8 flags) {
    idt[num].offset_l = offset & 0xFFFF;
    idt[num].offset_h = (offset >> 16) & 0xFFFF;
    idt[num].seg_sel = seg_sel;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

//TODO setup proper page addr & its pte addr
uint32 PF_ADDR = 0x1FC00004; 
uint32 my_ptr =  0xF03C01FC;
uint32 incr = 0;

void __declspec( naked ) pf_handler(void)
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
    printf("malloc 8Mb at 0x%08X-0x%08x, aligned at 0x%08X \n", _p, _p+MEMORY_ALLOCAE, _p_aligned);
    //trivial mapping
    for (i=0;i<1024;i++) {
        pd[i].raw = i*0x400000;
        pd[i].raw |= (i<512) ? PTE_TRIVIAL_LARGE : PTE_TRIVIAL_NONPRESENT;
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
	printf("Page directory adress: 0x%x %d\n",&_p_aligned, &_p_aligned);
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

    idt_set_gate(idt_table,PF_EXCEPTION, new_offset, new_segment,idt_table[PF_EXCEPTION].flags);

	addr = (uint32 *)PF_ADDR;
    printf("I am memory 0x%x\n", *addr); // to recover page
	//printf("I am memory 0x%x\n", *(addr + 8));
  	//printf("I am memory 0x%x\n", *(addr)); // to see default page fault

    printf("MY PF: %d\n", incr);
}

void main()
{
    SYSINFO sysinfo;

    memset(&sysinfo, 0, sizeof(sysinfo));
    get_sysinfo(&sysinfo);

   paging_task();
   pf_test(&sysinfo);

    /*
    __asm {
        xor ax,ax
        mov cs,ax
    }
    */
}

void hello(){
	printf("Hello, world!");
}
