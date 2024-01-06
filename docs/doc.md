
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