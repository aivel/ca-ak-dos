#include "DefStructs.c"

char* code_data_descr_type_decode(uint8 type)
{
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

char* system_descr_type_decode(uint8 type)
{
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

void fprint_descripor(FILE* f, PDESCRIPTOR d)
{
    fprintf(f, "\tVALUE=0x%08X-%08X PRESENT=[%s] \n", d->raw.high, d->raw.low, d->desc.p ? "yes":"no");
    if (d->desc.p) {
        fprintf(f, "\tBASE=0x%08X LIMIT=0x%08X \n", BASE_FROM_DESCRIPTOR(d), LIMIT_FROM_DESCRIPTOR(d));
        fprintf(f, "\tRING=%d TYPE=[%s] SYSTEM=[%s] DB=[%s]\n", d->desc.dpl, d->desc.s ? code_data_descr_type_decode(d->desc.type) : system_descr_type_decode(d->desc.type), d->desc.s ? "segment":"system", d->desc.db ? "32bit":"16bit");
    }
}

void fprint_desctable(FILE* f, uint32* base, uint32 limit)
{
    int i;
    for(i=0;;i++) { //i is an index in the array of 64bit descriptors
        DESCRIPTOR d;
        if (i*8 > limit) break;
        fprintf(f, "element %d (selector = %04X): \n", i, i<<3);
        d.raw.low = base[i*2];
        d.raw.high = base[i*2+1];
        fprint_descripor(f, &d);
    }
}

void fprint_tables(PSYSINFO sysinfo)
{
    FILE* gdt_dump;
    FILE* idt_dump;
    FILE* ldt_dump;
    FILE* tss_dump;
	DESCRIPTOR d;
	
	
    //print GDT
    gdt_dump = fopen("a:\\gdt_dump.txt","w");
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
		}
        //fprint_desctable(, (uint32*)sysinfo->idt.base, sysinfo->idt.limit);
    }
	
	//print TSS
	tss_dump =  fopen("a:\\tss_dump.txt","w");	
	if (0 == tss_dump) {
        printf("ERROR: cannot fopen tss_dump \n");
    } else {
		PTSS tss;
		uint32* base;
		 base = (uint32*)(sysinfo->gdt.base);
		base = base+((sysinfo->tr)>>3)*2;
		tss = (PTSS)base;
			
        fprintf(tss_dump,"%08X\n", tss->link);
        fprintf(tss_dump,"%08X\n",  tss->esp0);
        fprintf(tss_dump,"%08X\n",  tss->ss0);
        fprintf(tss_dump,"%08X\n",  tss->esp1);
        fprintf(tss_dump,"%08X\n",  tss->ss1);
        fprintf(tss_dump,"%08X\n",  tss->esp2);
        fprintf(tss_dump,"%08X\n",  tss->ss2);
        fprintf(tss_dump,"%08X\n",  tss->CR3);
        fprintf(tss_dump,"%08X\n",  tss->EIP);
        fprintf(tss_dump,"%08X\n",  tss->EFLAGS);
        fprintf(tss_dump,"%08X\n",  tss->EAX);
        fprintf(tss_dump,"%08X\n",  tss->ECX);
        fprintf(tss_dump,"%08X\n",  tss->EDX);
        fprintf(tss_dump,"%08X\n",  tss->EBX);
        fprintf(tss_dump,"%08X\n",  tss->ESP);
        fprintf(tss_dump,"%08X\n",  tss->EBP);
        fprintf(tss_dump,"%08X\n",  tss->ESI);
        fprintf(tss_dump,"%08X\n",  tss->EDI);
        fprintf(tss_dump,"%08X\n",  tss->ES);
        fprintf(tss_dump,"%08X\n",  tss->CS);
        fprintf(tss_dump,"%08X\n",  tss->SS);
        fprintf(tss_dump,"%08X\n",  tss->DS);
        fprintf(tss_dump,"%08X\n",  tss->FS);
        fprintf(tss_dump,"%08X\n",  tss->GS);
        fprintf(tss_dump,"%08X\n",  tss->LDTR);
        fprintf(tss_dump,"%08X\n",  tss->_IOmap);
	


		//printf("");
        //fprint_desctable(, (uint32*)sysinfo->idt.base, sysinfo->idt.limit);
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

void main()
{
    SYSINFO sysinfo;

    memset(&sysinfo, 0, sizeof(sysinfo));
    get_sysinfo(&sysinfo);

    print_info(&sysinfo);
    
    fprint_tables(&sysinfo);
}
