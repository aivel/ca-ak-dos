#include "DefStructs.c"

void fprint_descripor(FILE* f, PDESCRIPTOR d)
{
    fprintf(f, "\tVALUE=0x%08X-%08X PRESENT=%s \n", d->raw.high, d->raw.low, d->desc.p ? "yes":"no");
    if (d->desc.p) {
        fprintf(f, "\tBASE=0x%08X LIMIT=0x%08X \n", BASE_FROM_DESCRIPTOR(d), LIMIT_FROM_DESCRIPTOR(d));
        fprintf(f, "\tRING=%d TYPE=0x%X SYSTEM=%s DB=%s\n", d->desc.dpl, d->desc.type, d->desc.s ? "segment":"system", d->desc.db ? "32bit":"16bit");
    }
    //TODO we should check s bit before
    //TODO we should print type in textual form
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
        printf("ERROR: cannot fopen gdt_dump \n");
    } else {
        fprint_desctable(idt_dump, (uint32*)sysinfo->idt.base, sysinfo->idt.limit);
    }
	
	//print LDT
	ldt_dump =  fopen("a:\\ldt_dump.txt","w");	
	if (0 == ldt_dump) {
        printf("ERROR: cannot fopen gdt_dump \n");
    } else {
		d.raw.low = sysinfo->gdt[((sysinfo->ldtr)>>3)*2];
		d.raw.high = sysinfo->gdt[((sysinfo->ldtr)>>3)*2+1];
		fprint_descripor(ldt_dump,&d);
        //fprint_desctable(, (uint32*)sysinfo->idt.base, sysinfo->idt.limit);
    }
	
	//print TSS
	tss_dump =  fopen("a:\\tss_dump.txt","w");	
	if (0 == tss_dump) {
        printf("ERROR: cannot fopen gdt_dump \n");
    } else {
        //fprint_desctable(, (uint32*)sysinfo->idt.base, sysinfo->idt.limit);
    }


    fclose(gdt_dump);
    fclose(idt_dump);
	fclose(ldt_dump);
	fclose(tss_dump);
}


void main()
{
    SYSINFO sysinfo;

    memset(&sysinfo, 0, sizeof(sysinfo));
    get_sysinfo(&sysinfo);

    
	printf("TASK 1\n");
    printf("Protected Mode: %s \n", (sysinfo.cr0 & MASK(CR0_PE))?"on":"off");
    printf("Paging Mode: %s \n",    (sysinfo.cr0 & MASK(CR0_PG))?"on":"off");
    printf("Ring: CPL=%d \n",       sysinfo.cpl);
    printf("================ \n");
    printf("GDT: base=0x%08X limit=0x%04X \n", sysinfo.gdt.base, sysinfo.gdt.limit);
    printf("IDT: base=0x%08X limit=0x%04X \n", sysinfo.idt.base, sysinfo.idt.limit);
	printf("LDTR: 0x%X\n",sysinfo.ldtr);
	printf("TR: 0x%X\n", sysinfo.tr);

    fprint_tables(&sysinfo);

   // paging_task();
   // pf_test(&sysinfo);

    /*
    __asm {
        xor ax,ax
        mov cs,ax
    }
    */
}
