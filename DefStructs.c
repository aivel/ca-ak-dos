#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define CR0_PE 0
#define CR0_PG 31
#define CR4_PSE 4
#define MASK(x) (1<<(x))
#define PF_EXCEPTION 14

typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned int uint32;

#pragma pack (push, 1)
typedef struct _DTR {
    uint16 limit;
    uint32 base;
    uint16 _padding;
} DTR, *PDTR;

typedef struct _TSS{
	uint16 link;
	uint32 esp0;
	uint16 ss0;
	uint32 esp1;
	uint16 ss1;
	uint32 esp2;
	uint16 ss2;
	uint32 CR3;
	uint32 EIP;
	uint32 EFLAGS;
	uint32 EAX;
	uint32 ECX;
	uint32 EDX;
	uint32 EBX;
	uint32 ESP;
	uint32 EBP;
	uint32 ESI;
	uint32 EDI;
	uint16 ES;
	uint16 CS;
	uint16 SS;
	uint16 DS;
	uint16 FS;
	uint16 GS;
	uint16 LDTR;
	uint16 _IOmap;
} TSS, *PTSS;

typedef union _DESCRIPTOR {
    struct {
        uint32 low;
        uint32 high;
    } raw;
    struct {
        //3A.figure 3-8
        uint16 limit_low;
        uint16 base_low;
        uint8  base_mid;
        uint8  type:4;
        uint8  s:1;
        uint8  dpl:2;
        uint8  p:1;
        uint8 limit_high:4;
        uint8 avl:1;
        uint8 rsrvd:1;      //L bit only in 64bit
        uint8 db:1;
        uint8 g:1;
        uint8 base_high;
    } desc;
} DESCRIPTOR, *PDESCRIPTOR;

typedef union _PTE {
    uint32 raw;
    struct {
        uint32 p:1;
        uint32 rw:1;
        uint32 us:1;
        uint32 xx:4; //PCD,PWT,A,D
        uint32 ps:1;
        uint32 g:1;
        uint32 avl:3;
        uint32 pfn:20;
    };
} PTE, *PPTE;
#pragma pack (pop)

#define PTE_TRIVIAL_SELFMAP     0x007  //               //present read-write user 4Kb
#define PTE_TRIVIAL_LARGE       0x087  //0000 1000 0111 //present read-write user 4Mb
#define PTE_TRIVIAL_NONPRESENT  0xBA4  //---- ---- ---0
#define PTE_TRIVIAL_FAULTONCE   0x086  //same as PTE_TRIVIAL_LARGE but non-present

#define BASE_FROM_DESCRIPTOR(x) ((x->desc.base_low) | (x->desc.base_mid << 16) | (x->desc.base_high << 24))
#define LIMIT_FROM_DESCRIPTOR(x) (((x->desc.limit_low) | (x->desc.limit_high << 16)) << (x->desc.g ? 12 : 0))

typedef struct _SYSINFO {
    uint32 cpl;
    uint32 cr0;
    DTR gdt;
    DTR idt;
    uint16 ldtr;
    uint16 tr;
} SYSINFO, *PSYSINFO;

typedef struct _IDTENTRY {
    uint16 offset_l;
    uint16 seg_sel;
    uint8  zero;
    uint8  flags;
    uint16 offset_h;
} IDTENTRY, *PIDTENTRY;

void get_sysinfo(PSYSINFO sysinfo)
{
    uint32 _cpl = 0;
    uint32 _cr0 = 0;
    DTR* _gdt = &sysinfo->gdt;
    DTR* _idt = &sysinfo->idt;
	uint16* _ldtr = &sysinfo->ldtr;;
    uint16* _tr = &sysinfo->tr;
    __asm {
        //read cpl as code selector RPL (see 3A.X.Y)
        mov ax,cs
        and eax, 3
        mov _cpl, eax
        //store cr0 (see 3A.2.5 for bits)
        mov eax, cr0
        mov _cr0, eax
        //store gdt/idt (see 3A.X.Y)
        mov eax, _gdt
        sgdt [eax]
        mov eax, _idt
        sidt [eax]
		//store ldtr
		mov eax, _ldtr
		sldt [eax]
		//store tr
		mov eax, _tr 
		str [eax]

    }
    sysinfo->cpl = _cpl;
    sysinfo->cr0 = _cr0;
}

void idt_set_gate(PIDTENTRY idt, uint8 num, uint32 offset, uint16 seg_sel, uint8 flags) {
    idt[num].offset_l = offset & 0xFFFF;
    idt[num].offset_h = (offset >> 16) & 0xFFFF;
    idt[num].seg_sel = seg_sel;
    idt[num].zero = 0;
    idt[num].flags = flags;
}
