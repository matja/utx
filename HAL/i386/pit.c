
#include "pit.h"
#include "types.h"

static const unsigned
	IO_PORT_PIT_CHANNEL_0 = 0x40
	,IO_PORT_PIT_CHANNEL_1 = 0x41
	,IO_PORT_PIT_CHANNEL_2 = 0x42
	,IO_PORT_PIT_MODE = 0x43;

static const unsigned
	IO_CMD_PIC_MODE_OPERATION_MASK = 0x0e;

#define NTSC_FREQUENCY 3579545
#define PIT_FREQUENCY (NTSC_FREQUENCY/3)

void pit_reload_set(uint16_t reload)
{
	out8(IO_PORT_PIT_MODE, 0x34); // channel 0, lo/hi, rate gen
	out8(IO_PORT_PIT_CHANNEL_0, reload & 0xff);
	out8(IO_PORT_PIT_CHANNEL_0, (reload >> 8) & 0xff);
}

void pit_frequency_set_hz(unsigned frequency_hz)
{
	uint32_t reload = PIT_FREQUENCY/frequency_hz;
	if (reload > 65535) {
		console_printf("%s: frequency (%u) too low for PIT reload timer\n", __func__, frequency_hz);
		return;
	}
	pit_reload_set(reload);
}

void pit_period_set_us(unsigned period_us)
{
	static const uint32_t period_multiplier = 1e6; // 1e6 for microseconds
	uint32_t reload = ((uint64_t)period_us * PIT_FREQUENCY) / period_multiplier;
	if (reload > 65535) {
		console_printf("%s: period (%u) too long for PIT reload timer\n", __func__, period_us);
		return;
	}
}

int pit_init(void)
{
	pit_frequency_set_hz(100);
	return 1;
}

