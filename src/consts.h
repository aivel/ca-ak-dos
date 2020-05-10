#define CR0_PE 0
#define CR0_PG 31
#define CR4_PSE 4
#define MASK(x) (1<<(x))
#define DB_EXCEPTION 1
#define UD_EXCEPTION 6
#define PF_EXCEPTION 14


typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned int uint32;


char* MSG_NOT_ENOUGH_ARGS = "Not enough arguments\nAvailable arguments:\n\n\t1 - for the first task\n\t2 - for the paging task\n\t3 - for the third task\n\t4 - for the APIC task";

char* MSG_TASK = ">> Task %s has been chosen\n";
char* MSG_UNKNOWN_ARG = ">> Unknown argument: %s\n";

// Structures

#pragma pack (push, 1)
typedef struct _DTR {
	uint16 limit;
	uint32 base;
	uint16 _padding;
} DTR, *PDTR;
#pragma pack (pop)

typedef struct _SYSINFO {
	uint32 cpl;
	uint32 cr0;
	DTR gdt;
	DTR idt;
	uint16 ldtr;
	uint16 tr;
} SYSINFO, *PSYSINFO;

// Helpers

void get_sysinfo(PSYSINFO sysinfo)
{
	uint32 _cpl = 0;
	uint32 _cr0 = 0;
	DTR* _gdt = &sysinfo->gdt;
	DTR* _idt = &sysinfo->idt;
	uint16* _ldtr = &sysinfo->ldtr;
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
