/*
 * Copyright (c) 2003, Bernard Leach (leachbj@bouncycastle.org)
 */

#ifndef __ASM_ARCH_KEYBOARD_H__
#define __ASM_ARCH_KEYBOARD_H__

extern void sansakb_init_hw(void);

/* drivers/char/keyboard.c */
#define kbd_setkeycode(sc,kc)		(-EINVAL)
#define kbd_getkeycode(sc)		(-EINVAL)
#define kbd_translate(sc,kcp,rm)	({ *(kcp) = (sc); 1; })
#define kbd_unexpected_up(kc)		(0200)
#define kbd_leds(leds)
#ifdef CONFIG_KB_SANSA
#define kbd_init_hw()			sansakb_init_hw()
#else
#define kbd_init_hw()
#endif
#define kbd_enable_irq()
#define kbd_disable_irq()

#endif

