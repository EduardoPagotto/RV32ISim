
pgm.o:	file format elf32-littleriscv

Disassembly of section .text:

00000000 <_start>:
       0: 13 71 01 00  	andi	sp, sp, 0
       4: 93 f2 02 00  	andi	t0, t0, 0
       8: 13 73 03 00  	andi	t1, t1, 0
       c: 93 f3 03 00  	andi	t2, t2, 0
      10: 13 7e 0e 00  	andi	t3, t3, 0
      14: 93 fe 0e 00  	andi	t4, t4, 0
      18: 13 7f 0f 00  	andi	t5, t5, 0
      1c: 93 ff 0f 00  	andi	t6, t6, 0
      20: 93 f4 04 00  	andi	s1, s1, 0
      24: 13 79 09 00  	andi	s2, s2, 0
      28: 93 f9 09 00  	andi	s3, s3, 0

0000002c <.Lpcrel_hi0>:
      2c: 17 01 00 00  	auipc	sp, 0
      30: 13 01 41 3a  	addi	sp, sp, 932

00000034 <.Lpcrel_hi1>:
      34: 97 04 00 00  	auipc	s1, 0
      38: 93 84 c4 1c  	addi	s1, s1, 460
      3c: 03 a3 04 00  	lw	t1, 0(s1)

00000040 <.Lpcrel_hi2>:
      40: 17 09 00 00  	auipc	s2, 0
      44: 13 09 09 21  	addi	s2, s2, 528
      48: 23 20 09 00  	sw	zero, 0(s2)

0000004c <.Lpcrel_hi3>:
      4c: 97 09 00 00  	auipc	s3, 0
      50: 93 89 49 20  	addi	s3, s3, 516
      54: 23 a0 09 00  	sw	zero, 0(s3)
      58: 73 00 00 00  	ecall	
      5c: ef 00 80 00  	jal	0x64 <vvv>

00000060 <halt>:
      60: 6f 00 00 00  	j	0x60 <halt>

00000064 <vvv>:
      64: 13 00 00 00  	nop
      68: 13 00 00 00  	nop
      6c: 13 00 00 00  	nop
      70: 13 00 00 00  	nop
      74: 67 80 00 00  	ret

Disassembly of section .rodata:

00000078 <msg_erro>:
      78: 65 72        	<unknown>
      7a: 72 6f        	<unknown>
      7c: 20 66        	<unknown>
      7e: 61 74        	<unknown>
      80: 61 6c        	<unknown>

00000082 <msg_erroc>:
      82: 65 72        	<unknown>
      84: 72 6f        	<unknown>
      86: 20 66        	<unknown>
      88: 61 74        	<unknown>
      8a: 61 6c        	<unknown>
      8c: 0a 00        	<unknown>
      8e: 00 00        	<unknown>

Disassembly of section .data:

00000200 <seque4b>:
     200: 33 22 11 00  	slt	tp, sp, ra

00000204 <helloworld>:
     204: 4f 6c 61 20  	<unknown>
     208: 4d 75        	<unknown>
     20a: 6e 64        	<unknown>
     20c: 6f 00 00 00  	j	0x20c <helloworld+0x8>

0000020d <buffer>:
		...
     24d: 00 00        	<unknown>
     24f: 00           	<unknown>

Disassembly of section .bss:

00000250 <value2>:
     250: 4c 69        	<unknown>
     252: 6e 6b        	<unknown>
     254: 65 72        	<unknown>
     256: 3a 20        	<unknown>
     258: 55 62        	<unknown>
     25a: 75 6e        	<unknown>
     25c: 74 75        	<unknown>
     25e: 20 4c        	<unknown>
     260: 4c 44        	<unknown>
     262: 20 31        	<unknown>
     264: 34 2e        	<unknown>
     266: 30 2e        	<unknown>
     268: 30 00        	<unknown>
     26a: 00 00        	<unknown>

Disassembly of section .stack:

0000026c <_sstack>:
...

Disassembly of section .symtab:

00000000 <.symtab>:
		...
      10: 01 00        	<unknown>
      12: 00 00        	<unknown>
      14: 2c 00        	<unknown>
		...
      1e: 01 00        	<unknown>
      20: 0d 00        	<unknown>
      22: 00 00        	<unknown>
      24: 34 00        	<unknown>
		...
      2e: 01 00        	<unknown>
      30: 19 00        	<unknown>
      32: 00 00        	<unknown>
      34: 00 02        	<unknown>
		...
      3e: 03 00 21 00  	lb	zero, 2(sp)
      42: 00 00        	<unknown>
      44: 40 00        	<unknown>
		...
      4e: 01 00        	<unknown>
      50: 2d 00        	<unknown>
      52: 00 00        	<unknown>
      54: 50 02        	<unknown>
		...
      5e: 04 00        	<unknown>
      60: 34 00        	<unknown>
      62: 00 00        	<unknown>
      64: 4c 00        	<unknown>
		...
      6e: 01 00        	<unknown>
      70: 40 00        	<unknown>
      72: 00 00        	<unknown>
      74: 50 02        	<unknown>
		...
      7e: 04 00        	<unknown>
      80: 47 00 00 00  	<unknown>
      84: 64 00        	<unknown>
		...
      8e: 01 00        	<unknown>
      90: 4b 00 00 00  	<unknown>
      94: 60 00        	<unknown>
		...
      9e: 01 00        	<unknown>
      a0: 50 00        	<unknown>
      a2: 00 00        	<unknown>
      a4: 78 00        	<unknown>
		...
      ae: 02 00        	<unknown>
      b0: 59 00        	<unknown>
      b2: 00 00        	<unknown>
      b4: 82 00        	<unknown>
		...
      be: 02 00        	<unknown>
      c0: 63 00 00 00  	beqz	zero, 0xc0 <msg_erroc+0x3e>
      c4: 04 02        	<unknown>
		...
      ce: 03 00 6e 00  	lb	zero, 6(t3)
      d2: 00 00        	<unknown>
      d4: 0d 02        	<unknown>
		...
      de: 03 00 75 00  	lb	zero, 7(a0)
      e2: 00 00        	<unknown>
      e4: 50 02        	<unknown>
		...
      ee: 04 00        	<unknown>
      f0: 7a 00        	<unknown>
		...
      fa: 00 00        	<unknown>
      fc: 10 00        	<unknown>
      fe: 01 00        	<unknown>
     100: 81 00        	<unknown>
     102: 00 00        	<unknown>
     104: d0 03        	<unknown>
     106: 00 00        	<unknown>
     108: 00 00        	<unknown>
     10a: 00 00        	<unknown>
     10c: 10 00        	<unknown>
     10e: 05 00        	<unknown>
     110: 89 00        	<unknown>
		...
     11a: 00 00        	<unknown>
     11c: 10 00        	<unknown>
     11e: f1 ff        	<unknown>
     120: 9c 00        	<unknown>
     122: 00 00        	<unknown>
     124: 00 02        	<unknown>
     126: 00 00        	<unknown>
     128: 00 00        	<unknown>
     12a: 00 00        	<unknown>
     12c: 10 00        	<unknown>
     12e: f1 ff        	<unknown>
     130: ae 00        	<unknown>
     132: 00 00        	<unknown>
     134: 00 02        	<unknown>
     136: 00 00        	<unknown>
     138: 00 00        	<unknown>
     13a: 00 00        	<unknown>
     13c: 10 00        	<unknown>
     13e: f1 ff        	<unknown>
     140: c1 00        	<unknown>
     142: 00 00        	<unknown>
     144: 00 02        	<unknown>
     146: 00 00        	<unknown>
     148: 00 00        	<unknown>
     14a: 00 00        	<unknown>
     14c: 10 00        	<unknown>
     14e: f1 ff        	<unknown>
     150: d3 00 00 00  	<unknown>
     154: 6c 02        	<unknown>
     156: 00 00        	<unknown>
     158: 00 00        	<unknown>
     15a: 00 00        	<unknown>
     15c: 10 00        	<unknown>
     15e: 05 00        	<unknown>
     160: db 00 00 00  	<unknown>
     164: d0 03        	<unknown>
     166: 00 00        	<unknown>
     168: 00 00        	<unknown>
     16a: 00 00        	<unknown>
     16c: 10 00        	<unknown>
     16e: 05 00        	<unknown>

Disassembly of section .shstrtab:

00000000 <.shstrtab>:
       0: 00 2e        	<unknown>
       2: 74 65        	<unknown>
       4: 78 74        	<unknown>
       6: 00 2e        	<unknown>
       8: 72 6f        	<unknown>
       a: 64 61        	<unknown>
       c: 74 61        	<unknown>
       e: 00 2e        	<unknown>
      10: 64 61        	<unknown>
      12: 74 61        	<unknown>
      14: 00 2e        	<unknown>
      16: 62 73        	<unknown>
      18: 73 00 2e 73  	<unknown>
      1c: 74 61        	<unknown>
      1e: 63 6b 00 2e  	bltu	zero, zero, 0x314 <_sstack+0xa8>
      22: 73 79 6d 74  	csrrci	s2, 1862, 26
      26: 61 62        	<unknown>
      28: 00 2e        	<unknown>
      2a: 73 68 73 74  	csrrsi	a6, mseccfg, 6
      2e: 72 74        	<unknown>
      30: 61 62        	<unknown>
      32: 00 2e        	<unknown>
      34: 73 74 72 74  	csrrci	s0, mseccfg, 4
      38: 61 62        	<unknown>
      3a: 00           	<unknown>

Disassembly of section .strtab:

00000000 <.strtab>:
       0: 00 2e        	<unknown>
       2: 4c 70        	<unknown>
       4: 63 72 65 6c  	bgeu	a0, t1, 0x6c8 <_estack+0x2f8>
       8: 5f 68 69 30  	<unknown>
       c: 00 2e        	<unknown>
       e: 4c 70        	<unknown>
      10: 63 72 65 6c  	bgeu	a0, t1, 0x6d4 <_estack+0x304>
      14: 5f 68 69 31  	<unknown>
      18: 00 73        	<unknown>
      1a: 65 71        	<unknown>
      1c: 75 65        	<unknown>
      1e: 34 62        	<unknown>
      20: 00 2e        	<unknown>
      22: 4c 70        	<unknown>
      24: 63 72 65 6c  	bgeu	a0, t1, 0x6e8 <_estack+0x318>
      28: 5f 68 69 32  	<unknown>
      2c: 00 76        	<unknown>
      2e: 61 6c        	<unknown>
      30: 75 65        	<unknown>
      32: 31 00        	<unknown>
      34: 2e 4c        	<unknown>
      36: 70 63        	<unknown>
      38: 72 65        	<unknown>
      3a: 6c 5f        	<unknown>
      3c: 68 69        	<unknown>
      3e: 33 00 76 61  	<unknown>
      42: 6c 75        	<unknown>
      44: 65 32        	<unknown>
      46: 00 76        	<unknown>
      48: 76 76        	<unknown>
      4a: 00 68        	<unknown>
      4c: 61 6c        	<unknown>
      4e: 74 00        	<unknown>
      50: 6d 73        	<unknown>
      52: 67 5f 65 72  	<unknown>
      56: 72 6f        	<unknown>
      58: 00 6d        	<unknown>
      5a: 73 67 5f 65  	csrrsi	a4, 1621, 30
      5e: 72 72        	<unknown>
      60: 6f 63 00 68  	jal	t1, 0x66e0 <_estack+0x6310>
      64: 65 6c        	<unknown>
      66: 6c 6f        	<unknown>
      68: 77 6f 72 6c  	<unknown>
      6c: 64 00        	<unknown>
      6e: 62 75        	<unknown>
      70: 66 66        	<unknown>
      72: 65 72        	<unknown>
      74: 00 5f        	<unknown>
      76: 65 6e        	<unknown>
      78: 64 00        	<unknown>
      7a: 5f 73 74 61  	<unknown>
      7e: 72 74        	<unknown>
      80: 00 5f        	<unknown>
      82: 65 73        	<unknown>
      84: 74 61        	<unknown>
      86: 63 6b 00 5f  	bltu	zero, a6, 0x67c <_estack+0x2ac>
      8a: 5f 73 74 61  	<unknown>
      8e: 72 74        	<unknown>
      90: 5f 6d 65 6d  	<unknown>
      94: 6f 72 79 5f  	jal	tp, 0x97e8a <_estack+0x97aba>
      98: 72 6f        	<unknown>
      9a: 6d 00        	<unknown>
      9c: 5f 5f 73 69  	<unknown>
      a0: 7a 65        	<unknown>
      a2: 5f 6d 65 6d  	<unknown>
      a6: 6f 72 79 5f  	jal	tp, 0x97e9c <_estack+0x97acc>
      aa: 72 6f        	<unknown>
      ac: 6d 00        	<unknown>
      ae: 5f 5f 73 74  	<unknown>
      b2: 61 72        	<unknown>
      b4: 74 5f        	<unknown>
      b6: 6d 65        	<unknown>
      b8: 6d 6f        	<unknown>
      ba: 72 79        	<unknown>
      bc: 5f 72 61 6d  	<unknown>
      c0: 00 5f        	<unknown>
      c2: 5f 73 69 7a  	<unknown>
      c6: 65 5f        	<unknown>
      c8: 6d 65        	<unknown>
      ca: 6d 6f        	<unknown>
      cc: 72 79        	<unknown>
      ce: 5f 72 61 6d  	<unknown>
      d2: 00 5f        	<unknown>
      d4: 73 73 74 61  	csrrci	t1, 1559, 8
      d8: 63 6b 00 5f  	bltu	zero, a6, 0x6ce <_estack+0x2fe>
      dc: 65 6e        	<unknown>
      de: 64 00        	<unknown>
