
const unsigned
	io_port_pic_master_command	= 0x20,
	io_port_pic_master_data	= 0x21,
	io_port_pic_slave_command	= 0xa0,
	io_port_pic_slave_data	= 0xa1;

const unsigned
	io_cmd_pic_eoi	= 0x20;

// Acknowledge PIC-generated IRQ by sending EOI command to PIC.
// This allows the PIC to generate more IRQs.
void pic_eoi(void)
{
	out8(io_port_pic_master_command, io_cmd_pic_eoi);
}

void pic_set_mask(unsigned master_mask, unsigned slave_mask)
{
	out8(io_port_pic_master_data, master_mask);
	out8(io_port_pic_slave_data, slave_mask);
}

void pic_disable(void)
{
	pic_set_mask(0xff, 0xff);
}

void pic_enable(void)
{
	pic_set_mask(0x00, 0x00);
}

void pic_init(void)
{
	// Remap PIC IRQ 0-15 to IDT 0x20-0x2f so that they don't interfere with
	// CPU-generated exceptions.

	static const unsigned
		master_offset = 0x20,
		slave_offset = 0x28;

	static const unsigned
		ICW1_INIT = 0x10, // initialise
		ICW1_ICW4 = 0x01, // ICW4 (not) needed
		ICW4_8086 = 0x01; // 8086/88 (MCS-80/85) mode

	unsigned master_mask = in8(io_port_pic_master_data);
	unsigned slave_mask = in8(io_port_pic_slave_data);

	out8(io_port_pic_master_command, ICW1_INIT + ICW1_ICW4);
	out8(io_port_pic_slave_command, ICW1_INIT + ICW1_ICW4);
	out8(io_port_pic_master_data, master_offset);
	out8(io_port_pic_slave_data, slave_offset);
	out8(io_port_pic_master_data, 0x04); // master has slave PIC at IRQ2
	out8(io_port_pic_slave_data, 0x02); // slave PIC is cascade at IRQ2
	out8(io_port_pic_master_data, ICW4_8086);
	out8(io_port_pic_slave_data, ICW4_8086);

	pic_enable();
}

