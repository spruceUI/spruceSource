/*
 * rumble - fire a force feedback effect on an evdev device.
 *
 *     rumble <event-device> <intensity> <duration-ms>
 *
 * e.g.  rumble /dev/input/event1 0xFFFF 50
 *
 * Written for the Anbernic RG XX line, whose motor is NOT behind a GPIO. There
 * is no /sys/class/motor, no timed_output and no exported gpio on those
 * devices - the pad itself ("ANBERNIC-keys") advertises EV_FF, and the motor is
 * driven by uploading an FF_RUMBLE effect and playing it. That is the same
 * approach the GKD Pixel2 uses, and this keeps the argument order its stock
 * "rumble" binary uses so the shell side is identical on both.
 *
 * Freestanding on purpose: no libc, raw syscalls, static, no interpreter and no
 * NEEDED entries. spruce ships one binary to devices with quite different glibc
 * versions, and the build box has no aarch64 sysroot - this sidesteps both.
 *
 * The effect lives only as long as the file descriptor: closing it removes the
 * effect and stops the motor. So this sleeps for the requested duration before
 * exiting rather than firing and quitting, which would cut the buzz short.
 */

#define AT_FDCWD        -100
#define O_RDWR          2

#define SYS_ioctl       29
#define SYS_openat      56
#define SYS_close       57
#define SYS_write       64
#define SYS_exit_group  94
#define SYS_nanosleep   101

#define EV_FF           0x15
#define FF_RUMBLE       0x50

/*
 * _IOW('E', 0x80, struct ff_effect) with sizeof(struct ff_effect) == 48.
 * 48 because the union is pointer aligned - ff_periodic_effect holds a
 * __user pointer - so it starts at offset 16 and is itself 32 bytes. Both
 * numbers were confirmed against a live kernel before this was written.
 */
#define EVIOCSFF        0x40304580
#define FF_EFFECT_SIZE  48
#define FF_UNION_OFFSET 16

typedef unsigned short u16;
typedef signed short   s16;
typedef unsigned int   u32;
typedef signed int     s32;
typedef unsigned long  u64;

static long sys3(long nr, long a, long b, long c)
{
	register long x8 __asm__("x8") = nr;
	register long x0 __asm__("x0") = a;
	register long x1 __asm__("x1") = b;
	register long x2 __asm__("x2") = c;
	__asm__ volatile("svc #0" : "+r"(x0) : "r"(x1), "r"(x2), "r"(x8) : "memory", "cc");
	return x0;
}

static long sys4(long nr, long a, long b, long c, long d)
{
	register long x8 __asm__("x8") = nr;
	register long x0 __asm__("x0") = a;
	register long x1 __asm__("x1") = b;
	register long x2 __asm__("x2") = c;
	register long x3 __asm__("x3") = d;
	__asm__ volatile("svc #0" : "+r"(x0) : "r"(x1), "r"(x2), "r"(x3), "r"(x8) : "memory", "cc");
	return x0;
}

static void put(const char *s)
{
	long n = 0;
	while (s[n])
		n++;
	sys3(SYS_write, 2, (long)s, n);          /* stderr */
}

static void die(const char *msg, int code)
{
	put(msg);
	sys3(SYS_exit_group, code, 0, 0);
	__builtin_unreachable();
}

/* Accepts decimal, or hex when prefixed 0x - the shell side passes 0xFFFF. */
static long parse_num(const char *s)
{
	long v = 0;

	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
		s += 2;
		if (!*s)
			return -1;
		for (; *s; s++) {
			char c = *s;
			if (c >= '0' && c <= '9')      v = v * 16 + (c - '0');
			else if (c >= 'a' && c <= 'f') v = v * 16 + (c - 'a' + 10);
			else if (c >= 'A' && c <= 'F') v = v * 16 + (c - 'A' + 10);
			else return -1;
		}
		return v;
	}

	if (!*s)
		return -1;
	for (; *s; s++) {
		if (*s < '0' || *s > '9')
			return -1;
		v = v * 10 + (*s - '0');
	}
	return v;
}

static void store16(unsigned char *p, int off, u16 v)
{
	p[off]     = (unsigned char)(v & 0xff);
	p[off + 1] = (unsigned char)((v >> 8) & 0xff);
}

void rumble_main(u64 *stack)
{
	long argc = (long)stack[0];
	char **argv = (char **)&stack[1];
	unsigned char effect[FF_EFFECT_SIZE];
	unsigned char ev[24];
	long fd, rc, i, strength, ms;
	s16 id;

	if (argc < 4)
		die("usage: rumble <event-device> <intensity> <duration-ms>\n", 2);

	strength = parse_num(argv[2]);
	ms = parse_num(argv[3]);
	if (strength < 0 || strength > 0xFFFF)
		die("rumble: intensity must be 0..65535 (decimal or 0x hex)\n", 2);
	if (ms < 0 || ms > 60000)
		die("rumble: duration must be 0..60000 ms\n", 2);

	fd = sys4(SYS_openat, AT_FDCWD, (long)argv[1], O_RDWR, 0);
	if (fd < 0)
		die("rumble: cannot open device\n", 1);

	for (i = 0; i < FF_EFFECT_SIZE; i++)
		effect[i] = 0;
	store16(effect, 0, FF_RUMBLE);                       /* type              */
	store16(effect, 2, 0xFFFF);                          /* id -1 = allocate  */
	store16(effect, 10, (u16)ms);                        /* replay.length     */
	store16(effect, FF_UNION_OFFSET, (u16)strength);     /* strong_magnitude  */
	store16(effect, FF_UNION_OFFSET + 2, (u16)strength); /* weak_magnitude    */

	rc = sys3(SYS_ioctl, fd, EVIOCSFF, (long)effect);
	if (rc < 0) {
		sys3(SYS_close, fd, 0, 0);
		die("rumble: device rejected the effect (no force feedback?)\n", 1);
	}

	/* The kernel writes the allocated id back into the struct. */
	id = (s16)(effect[2] | (effect[3] << 8));

	/* input_event: struct timeval (2 x 8 bytes), then type, code, value. */
	for (i = 0; i < 24; i++)
		ev[i] = 0;
	store16(ev, 16, EV_FF);
	store16(ev, 18, (u16)id);
	ev[20] = 1;                                          /* play once         */

	rc = sys3(SYS_write, fd, (long)ev, 24);
	if (rc < 0) {
		sys3(SYS_close, fd, 0, 0);
		die("rumble: could not start the effect\n", 1);
	}

	/*
	 * Hold the descriptor open while it plays. Closing removes the effect,
	 * so returning immediately would silence the motor almost at once.
	 */
	{
		u64 ts[2];
		ts[0] = (u64)(ms / 1000);
		ts[1] = (u64)((ms % 1000) * 1000000L);
		sys3(SYS_nanosleep, (long)ts, 0, 0);
	}

	sys3(SYS_close, fd, 0, 0);
	sys3(SYS_exit_group, 0, 0, 0);
	__builtin_unreachable();
}

/*
 * No libc, so no startup code either. At process entry sp points at argc with
 * argv immediately after, which is all rumble_main needs.
 */
__asm__(
	".global _start\n"
	"_start:\n"
	"	mov	x0, sp\n"
	"	b	rumble_main\n"
);
