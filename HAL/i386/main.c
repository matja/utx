#include "../common.h"
#include "types.h"
#include "console.h"
#include "cpudetect.h"
#include "multiboot.h"
#include "console.h"

extern void *irq_handlers[];

static unsigned timer_ticks_low = 0, timer_ticks_high = 0;
static unsigned keyboard_irq_count = 0;
static unsigned irq_count[16];

#define SELECTOR_NULL        0x00
#define SELECTOR_TSS         0x08
#define SELECTOR_KERNEL_CODE 0x10
#define SELECTOR_KERNEL_DATA 0x18
#define SELECTOR_USER_CODE   0x20
#define SELECTOR_USER_DATA   0x28

#define IDT_TYPE_TRAP_GATE      0x8e00
#define IDT_TYPE_INTERRUPT_GATE 0x8f00
#define IDT_TYPE_TASK_GATE      0x8f00

extern void exception_handler_default(void);
extern void irq_handler_default(void);
extern void irq_handler_0(void);
extern void irq_handler_1(void);
extern void irq_handler_4(void);

extern unsigned char g_gdt[];
extern unsigned char g_idt[];

void irq_update_console(void)
{
	unsigned old_x, old_y, old_attribute;

	console_cursor_get(&old_x, &old_y);
	console_attribute_get(&old_attribute);

	console_cursor_set(0, 0);
	console_attribute_set(0x1f);
	console_printf("irq[0:%u 1:%u 2:%u 3:%u 4:%u]"
		,irq_count[0]
		,irq_count[1]
		,irq_count[2]
		,irq_count[3]
		,irq_count[4]
	);

	console_cursor_set(old_x, old_y);
	console_attribute_set(old_attribute);
}

void timer_irq(void)
{
	irq_count[0]++;
	irq_update_console();

	pic_eoi();
}

void keyboard_irq(void)
{
	char scancode = 0;

	irq_count[1]++;

	scancode = in8(0x60);
	console_printf("%u ", scancode&0xff);

	pic_eoi();
}

void ethernet_irq(void)
{
	irq_count[4]++;

	pic_eoi();
}

void gdt_set_gate(
	unsigned index, unsigned base, unsigned limit
	,unsigned access, unsigned granularity)
{
	uint8_t *e = ((uint8_t *)&g_gdt) + index*8;

	e[0] = limit & 0xff;
	e[1] = (limit >> 8) & 0xff;
	e[2] = base & 0xff;
	e[3] = (base >> 8) & 0xff;
	e[4] = (base >> 16) & 0xff;
	e[5] = access & 0xff;
	e[6] = granularity & 0xff;
	e[7] = (base >> 24) & 0xff;
}

extern uint8_t tss[];
extern uint8_t tss_end[];
extern uint32_t tss_ss0;
extern uint32_t tss_esp0;
extern uint32_t tss_iomap_base;

void gdt_init(void)
{
	unsigned base = 0, limit = 0xffffffff;

	gdt_set_gate(0, 0, 0, 0, 0); // null selector

	// TSS for calling syscalls from ring 3
	gdt_set_gate(1, tss, tss_end - tss - 1, 0x89, 0x40);
	tss_ss0 = 0x10;
	tss_esp0 = 0x200000;
	tss_iomap_base = 104;

	gdt_set_gate(2, base, limit, 0x9a, 0xcf); // kernel code
	gdt_set_gate(3, base, limit, 0x92, 0xcf); // kernel data
	gdt_set_gate(4, base, limit, 0xfa, 0xcf); // user code
	gdt_set_gate(5, base, limit, 0xf2, 0xcf); // user data

	gdt_flush();
	tss_flush();
}

void idt_set_gate(unsigned index, void (*handler)(void)
	,uint16_t selector, uint16_t type)
{
	unsigned char *e = (unsigned char *)g_idt + index*8;

	e[0] = (uint32_t)handler & 0xff;
	e[1] = ((uint32_t)handler >> 8) & 0xff;
	e[2] = selector & 0xff;
	e[3] = (selector >> 8) & 0xff;
	e[4] = type & 0xff;
	e[5] = (type >> 8) & 0xff;
	e[6] = ((uint32_t)handler >> 16) & 0xff;
	e[7] = ((uint32_t)handler >> 24) & 0xff;
}

void exception_handler_DE(void);
void exception_handler_DB(void);
void exception_handler_NMI(void);
void exception_handler_BP(void);
void exception_handler_OF(void);
void exception_handler_BR(void);
void exception_handler_UD(void);
void exception_handler_NM(void);
void exception_handler_DF(void);
void exception_handler_R0(void);
void exception_handler_TS(void);
void exception_handler_NP(void);
void exception_handler_SS(void);
void exception_handler_GP(void);
void exception_handler_PF(void);
void exception_handler_R1(void);
void exception_handler_MF(void);
void exception_handler_AC(void);
void exception_handler_MC(void);
void exception_handler_XM(void);

void idt_init(void)
{
	unsigned i = 0;

	struct ExceptionHandler {
		void (*handler)(void);
		unsigned idt_type;
	};

	static const struct ExceptionHandler eh[] = {
		{ exception_handler_DE, IDT_TYPE_TRAP_GATE }
		,{ exception_handler_DB, IDT_TYPE_INTERRUPT_GATE }
		,{ exception_handler_NMI, IDT_TYPE_INTERRUPT_GATE }
		,{ exception_handler_BP, IDT_TYPE_TRAP_GATE }
		,{ exception_handler_OF, IDT_TYPE_TRAP_GATE }
		,{ exception_handler_BR, IDT_TYPE_TRAP_GATE }
		,{ exception_handler_UD, IDT_TYPE_TRAP_GATE }
		,{ exception_handler_NM, IDT_TYPE_TRAP_GATE }
		,{ exception_handler_DF, IDT_TYPE_TASK_GATE }
		,{ exception_handler_R0, IDT_TYPE_TRAP_GATE }
		,{ exception_handler_TS, IDT_TYPE_TRAP_GATE }
		,{ exception_handler_NP, IDT_TYPE_TRAP_GATE }
		,{ exception_handler_SS, IDT_TYPE_TRAP_GATE }
		,{ exception_handler_GP, IDT_TYPE_TRAP_GATE }
		,{ exception_handler_PF, IDT_TYPE_TASK_GATE }
		,{ exception_handler_R1, IDT_TYPE_TRAP_GATE }
		,{ exception_handler_MF, IDT_TYPE_TRAP_GATE }
		,{ exception_handler_AC, IDT_TYPE_TRAP_GATE }
		,{ exception_handler_MC, IDT_TYPE_TRAP_GATE }
		,{ exception_handler_XM, IDT_TYPE_TRAP_GATE }
	};

	// set default exception handlers
	for (i=0; i < sizeof(eh)/sizeof(*eh); i++)
	{
		const struct ExceptionHandler *peh = eh + i;
		idt_set_gate(i, peh->handler, SELECTOR_KERNEL_CODE, peh->idt_type);
	}

	// set default IRQ handlers
	for (i=32; i<256; i++) {
		idt_set_gate(i, irq_handler_default
			,SELECTOR_KERNEL_CODE, IDT_TYPE_INTERRUPT_GATE
		);
		idt_set_gate(i, NULL, 0, 0);
	}

	// set specific IRQ handlers
	idt_set_gate(0x20, irq_handler_0, SELECTOR_KERNEL_CODE, IDT_TYPE_INTERRUPT_GATE);
	idt_set_gate(0x21, irq_handler_1, SELECTOR_KERNEL_CODE, IDT_TYPE_INTERRUPT_GATE);
	idt_set_gate(0x24, irq_handler_4, SELECTOR_KERNEL_CODE, IDT_TYPE_INTERRUPT_GATE);

	idt_flush();
}

void breakpoint(void)
{
	asm volatile("xchg %bx,%bx");
}

int main(void)
{
	gdt_init();
	idt_init();
	tss_init();

	//console_mode_set(80, 50, 0);
	console_clear();
	console_attribute_set(2);
	console_write("\nstarted " OS_NAME " i386 HAL\n");

	//cpu_detect();
	multiboot_print();
	//vbe3_scan();

	pit_init();
	pic_init();

	//pci_init();

	irq_handlers[0] = timer_irq;
	irq_handlers[1] = keyboard_irq;
	irq_handlers[4] = ethernet_irq;
	breakpoint();
	pci_dump_device();
	irq_enable();

	console_write("irq_wait ");
	while (1) {
		irq_wait();
	}

	return 0;
}

