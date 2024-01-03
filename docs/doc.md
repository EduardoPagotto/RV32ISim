
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


csr

MRO -> machine read only


# CSRRW: CSR read write
escrita                                        leitura 
rs1 ------------------->  CSR ---------------> rd (retorno valor)
rs1 ------------------->  CSR ---------x-----> rd(0) (Sem leitura ,retorna 0)
rs1(0)--(escreve 0)---->  CSR ---------------> rd                                              

# CSRRWI: immediate (5 bit only, extended 0 -> 000 00000000 00011111)
escrita                                        leitura 
imm ------------------->  CSR ---------------> rd (retorno valor)
imm ------------------->  CSR ---------x-----> rd(0) (Sem leitura ,retorna 0)

# CSRRS: Read and set bits                                         
escrita                                        leitura 
rs1 ------------------->  CSR ---------------> rd (retorno sempre valor)
rs1(0) ---------------->  CSR ---------------> rd (retorno sempre valor) sem alteracao no CSR
rs1 contem a mascara de bits: bits 1 seta os bits do CSR, os bit 0 deixam o csr inalterado!

# CSRRSI: Read and set bits immediate (lower 5 bit como mascara)
imm ------------------->  CSR ---------------> rd (retorno valor)
imm(0) ---------------->  CSR ---------------> rd (retorno sempre valor) sem alteracao no CSR
imm contem a mascara de bits: bits 1 seta os bits do CSR, os bit 0 deixam o csr inalterado!

# CSRRC: Read and clear bits
escrita                                        leitura 
rs1 ------------------->  CSR ---------------> rd (retorno sempre valor)
rs1(0) ---------------->  CSR ---------------> rd (retorno sempre valor) sem alteracao no CSR
rs1 contem a mascara de bits: bits 1 limpa os bits do CSR, os bit 0 deixam o csr inalterado!


# CSRRCI: Read and clear bits immediate (lower 5 bit como mascara)
escrita                                        leitura 
imm ------------------->  CSR ---------------> rd (retorno valor)
imm(0) ---------------->  CSR ---------------> rd (retorno sempre valor) sem alteracao no CSR
imm contem a mascara de bits: bits 1 limpa os bits do CSR, os bit 0 deixam o csr inalterado!

# pseudo
CSRR (CSRRS x0, x1)
CSRW (CSRRW x1, x0)
CSRI (CSRRW imm, x0)


# interrupt
MEPC: Quando um exception ou uma interrupcao acontece contem o PC da instrução interrompida, quando terminar usar este valor +4 para a proxima instrucao!!
MCAUSE: lista de valores com o motivo do TRAP (interrupcao/excessao)
MTVEC: tabela de vetores apontam para tipos diferentes de interrupcoes baseados na tabela de interrupcoes
MSTATUS: permite habilitar e desabilitar interrupts
MTVAL: detalhe do tipo de tra que ocorreu, como o address de uma leitura mal alinhada

12 |11 | R/W Acessibilidade
-----------------------------
0  |0  | Standart RW 
0  |1  | Standart RW
1  |0  | Custom RW
1  |1  | Custom RO

10 | 09 | Nivel de Privilegio
-----------------------------
0  | 0  | User
0  | 1  | Supervisor
1  | 0  | Hypervisior
1  | 1  | Machine


11 |10 |09 |08 |07 |06 |05 |04 |03 |02 |01 |00 |Tipo
---|---|---|---|---|---|---|---|---|---|---|---|---------------------------
0  |0  |X  |X  |X  |X  |X  |X  |X  |X  |X  |X  |Acessibilidade Standart RW 
0  |1  |X  |X  |X  |X  |X  |X  |X  |X  |X  |X  |Acessibilidade Standart RW
1  |0  |X  |X  |X  |X  |X  |X  |X  |X  |X  |X  |Acessibilidade Custom RW
1  |1  |X  |X  |X  |X  |X  |X  |X  |X  |X  |X  |Acessibilidade Custom RO
X  |X  |0  |0  |X  |X  |X  |X  |X  |X  |X  |X  |Nivel user
X  |X  |0  |1  |X  |X  |X  |X  |X  |X  |X  |X  |Nivel Supervisor
X  |X  |1  |0  |X  |X  |X  |X  |X  |X  |X  |X  |Nivel Hypervisior
X  |X  |1  |1  |X  |X  |X  |X  |X  |X  |X  |X  |Nivel Machine



MSTATUS
 12 | 11 |10 |09 |08 |07 |06 |05 |04 |03 |02 |01 |00 |Tipo
--- |-- -|---|---|---|---|---|---|---|---|---|---|---|------------------------
    |    |   |   |   |   |   |   | X |   |   |   |   | MIE  Determina se irq esta habilitadas 1
    |    |   |   | X |   |   |   |   |   |   |   |   | MPIE Amrazena o MIE anterior (usara para restaurar MIE apos o MRET)
 X  | X  |   |   |   |   |   |   |   |   |   |   |   | MPP Determina o nivel de privilegio a ser definido quando a inst de ret de irq ocorer
    |    |   |   |   |   |   |   |   |   |   |   |
    |    |   |   |   |   |   |   |   |   |   |   |

MTVEC
31:2 | 1:0 |
-----|-----|-------
 X:X |     | BASE Garda o Endereco de interrupcao  [31:2] + o2
     | XX  | MODE 0 endereco direto, 1 endereco somado com 4x o valor da causa 


# controle de fluxo
## IF-THEN
```c
if (x>=10) {
    y = x;
}
```
```s
    ...
    #x em a3 e y em a4
    LI      t1, 10
    BLT     a3, t1, pula # pula se a3 < 10
    mv      a4, a3
pula:
    ...
```
## IF-THEN-ELSE
```c
if (x>=10) {
    y = y +1;
} else {
    y = x;
}
```
```s
    ...
    # x em a1 e y em a2
    LI      t3, 10
    BLT     a1, t3, else
    ADDI    a2, a2, 1
    J cont
else:
    MV a2, a1
cont:
    ...
```
## IF com AND
```c
    ...
    if ((x >= 10)&&(y < 20)) {
        x = y;
    }
    ...
```
```s
    # x em a1 e y em a2
    LI      t1, 10
    BLT     a1, t1, pula
    LI      t1, 20
    BGE     a2, t1, pula
    MV      a1, a2
pula:
    ....
```
## WHILE
```c
    i = 0;
    while(i < 20) {
        y = y + 3;
        i = i + 1;
    }
```
```s
    #i em a1 e y em a2
    LI a1, 0
enquanto:
    LI t1, 20
    BGE a1, t1, cont
    ADDI a2, a2, 3
    ADDI a1, a1, 1
    J enquanto
cont:
    ....
```

## FOR
```c
    for (i = 0; i < 10; i++) {
        y = y + 2;
    }
```
```s
    # i em a1 e y em a2
    LI a1, 0
for:
    LI t1, 10
    BGE a1, t1, cont
    ADDI a2, a2, 2
    ADDI a1, a1, 1
    J for
cont:
    ...

```
## DO_WHILE
```c
    ...
    i=0;
    do {

    } while (i < 10);
    ...
```
```s
    # i em a1 e y em a2
    LI a1, 0
    LI t1, 10
do:
    ADDI a2, a2, 2;
    ADDI a1, a1, 1;
    BLT a1, t1, do
cont:
    ...

```
## chamada de funcao
```c
int soma(int a, int b){
    return a + b;
}

    ...
    i = soma(2, 3);
    ...
```
```s
    # i em a3
    # parametros em a0 e a1
    # retorno em a0
    LI  a0, 2
    LI  a1, 3
    JAL soma
    mv a3, a0
    ...
    ...
    # parametros em a0, e a1
    # retorno em a0
    ...
soma:
    ADD a0, a0, 1
    RET
```

## Salcom com JAL - RET
```s
    ...
    LI  a0, 127
    JAL hash
    addi a1, a2, 1
    ...

    # Retorna o valor da funcao hash em a0
hash:
    ANDI a1, a0, 63 # a1 = a0 & 0x3F
    SRLI a0, a0, 6  # a0 = a0 >> 6
    AND a0, a1, a0  # a0 = a1 & a0
    ret             # JALR x0, ra, 0
```

# Memoria exemplo 

Address|Main Memory|ussed
:-----:|:---------:|:--------------:
 0xfff |Stack      |Grow Top-down    
  ...  |free space |  
  ...  |Free space |    
 0x200 |Heap       |Grow botton-up
 0x130 |.bss       |Statica data
 0x120 |.data      |Statica data
 0x100 |.rodata    |Statica data
 0x000 |.text      |Code



refs:
- https://msyksphinz-self.github.io/riscv-isadoc/html/rvi.html
- https://www.youtube.com/watch?v=PMzbRMsGpwU
- https://www.youtube.com/watch?v=QfXav1X7EP8
- https://www.youtube.com/watch?v=GFlkl2DKqiY
- https://ftp.gnu.org/old-gnu/Manuals/ld-2.9.1/html_chapter/ld_3.html
- https://blog.thea.codes/the-most-thoroughly-commented-linker-script/