
## Register Risc-v R32I

Name  |Mnemonic|Meaning
------|--------|---------------------------------------
x0    |zero    |always zero
x1    |ra      |return address
x2    |sp      |stack pointer
x3    |gp      |global pointer
x4    |tp      |thread pointer
x5    |t0      |temporary (alternative link register)
x6-7  |t1-2    |temporary register
x8    |s0/fp   |saved register / frame pointer
x9    |s1      |saved register
x10-11|a0-1    |function arguments and return value
x12-17|a2-7    |function arguments
x18-27|s2-11   |saved register
x28-31|t3-6    |temporary register


## Load / Store Instructions
Carrega o conteudo da memoria apontado por Src1 + Offset no registro Dest

    lw Dest, Offset(Src1)
        Dest = Mem[Reg[Src1] + Offset]

Armazenar na memoria apontada por Src1 + offset valor do registro Src2

    sw Src2, offset(Src1)
        Mem[Reg[Src1] + Offset] = Src2

```
; Operacao de Push
    lw a2, 4(sp)  ; carrega o conteudo da memoria apontado por sp + 4 no registro a2
    sw a0, 0(a2)  ; amazena na memoria apontada por a2+0 o conteudo do registrador a0

swap:            ; Swap dos valores armazenados na memoria apontados por a0 e a1
    lw t0, 0(a0) ; val_1 armazenado na memoria apontada por a0 e carregado no registro t0
    lw t1, 0(a1) ; val_2 armazenado na memoria apontada por a1 e carregado no registro t1
    sw t1, 0(a0) ; registro t1 e armazenado na memoria apontada em a0
    sw t0, 0(a1) ; retistro t0 e armazenado na memoria apontada em a1

```


## Aritmetric Operations


add Dest, Src1, Src2    ; Dest = Src1 + Src2    ; add
sub Dest, Src1, Src2    ; Dest = Src1 - Src2    ; sub
sll Dest, Src1, Src2    ; Dest = Src1 << Src2   ; inserts 0 in the lsb
sra Dest, Src1, Src2    ; Dest = Src1 >> Src2   ; shift arhit preserves sing bit
srl Dest, Src1, Src2    ; Dest = Src1 >> Src2   ; shift logical add 0 bits in msb

xor Dest, Src1, Src2    ; Dest = Src1 ^ Src2    ; or exclusive
and Dest, Src1, Src2    ; Dest = Src1 & Src2    ; add
or Dest, Src1, Src2     ; Dest = Src1 | Src2    ; add

addi Dest, Src1, imm    ; Dest = Src1 + imm     ; add


```c
int arith(int x, int y, int z) {

    int t1 = x + y;
    int t2 = z + t1;
    int t3 = x;
    int t4 = y * 48;
    int t5 = t5 + t4;
    int rval = t2 + t5;
    return rval;
}
```

```s
arith:
    add t1, a0, a1
    add t2, a2, t1
    add t4, a1, a1
    add t4, t4, a1
    slli t4, t4, 4
    add t5, a0, t4
    add a0, t2, t5
    ret 
```

# Stack

 - Pushing (stack decrementa a cada valor novo)
    addi sp, sp, -4 ; nova posicao do stack 
    sw t1, 0(sp)    ; salva conteudo de t1 no stack
 
 - Poping (stack incrementa apos pegar valor atual)
    lw t1, 0(sp)    ; carrega valor atual do stack no registro t1
    addi sp, sp, 4  ; stack volta para o topo
    

 - Calls
    jal ra, label ; Escreve (pc + 4) no registrador ra (contem o add de retorno) antes de saltar para o label
    

- Returns
    jalr x0, ra   ; escreve pc + 4  no registrador x0 e salta para o endereco apontado por ra


```c
int main() {
    return absadd(-1, 2);
}

int absadd(int x, int y) {
    int t0 = abs(x);
    int t1 = abs(y);
    return t0 + t1;
}

int abs(int v) {
    int mask = v >> 4 *8 -1;
    return (v+ mask) ^ mask;
}
```

```s
00400500 <main>:
  400500: addi sp, sp, -4   # proxima vaga do stack
  400504: sw ra, 0(sp)      # salva ret addr
  400508: addi a0, x0, -1   # arg0 = -1
  40050C: addi a1, x0, -2   # arg1 = 2
  400510: jal ra, absadd    # absadd(-1,2)
  400514: lw ra, 0(sp)      # restore ra
  400518: jalr x0, ra       # return
  
00400540 <absadd>:
  400540 addi sp, sp, -12   # aloca stack para 3 valores
  400544 sw ra, 8(sp)       # sava ret addr
  400548 sw s0, 4(sp)       # salva s0 reg
  40054C sw a1, 0(sp)       # save y
  40055C jal ra, abs        # abs(x)
  400560 add a0, a0, s0     # s0 = abs(x)
  400558 lw a0, 0(sp)       # a0 =y
  40055C jal ra, abs        # abs(y)
  400564 add a0, a0, s0     # a0 = |x| + |y|
  400568 lw s0, 4(sp)       # restore so reg
  40056c lw ra, 8(sp)       # restore ret addr
  40056C addi sp, sp, 12    # free stack
  400570 jalr x0, ra

00400570 <abs>:
  400570 sra t0, a0, 32     # mask = v >>31
  400574 add a0, a0, t0     # v + m
  400578 xor a0, a0, t0     # a0 = (v+m) ^ m
  40057C jalr x0, ra        # return a0

```




refs:
- https://msyksphinz-self.github.io/riscv-isadoc/html/rvi.html
- https://www.youtube.com/watch?v=PMzbRMsGpwU
- https://www.youtube.com/watch?v=QfXav1X7EP8
- https://www.youtube.com/watch?v=GFlkl2DKqiY
- https://ftp.gnu.org/old-gnu/Manuals/ld-2.9.1/html_chapter/ld_3.html
- https://blog.thea.codes/the-most-thoroughly-commented-linker-script/