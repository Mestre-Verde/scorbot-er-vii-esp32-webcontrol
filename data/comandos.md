<!-- | Função                    | Atalho (Windows/Linux)      | Observações                                   |
| ------------------------- | --------------------------- | --------------------------------------------- |
| **Visualizar Preview**    | `Ctrl + Shift + V`          | Abre o Markdown preview                       |
| **Abrir preview ao lado** | `Ctrl + K` depois `V`       | Mostra lado a lado                            |
| **Negrito** (`**texto**`) | `Ctrl + B` *(com extensão)* | Atalho típico ativado pela extensão           |
| **Itálico** (`*texto*`)   | `Ctrl + I` *(com extensão)* | Idem                                          |
| **Lista com marcadores**  | `Ctrl + Shift + L`          | Cria `- item`                                 |
| **Lista numerada**        | `Ctrl + Shift + O`          | Cria `1. item`                                |

 -->
Comandos.md | By Mestre Verde \
Wed Jul 30 21:48:36 2025 +0100 

# COMANDOS DE MANUSEAMENTO DA PÁGINA WEB

## Página principal
>In development.

## Terminal Retro

1. Se estiver no terminal (fora da caixa de texto), clique 2 vezes na tecla "espaço" para parar imediatamente o robo e programas a decorrer.

# COMANDOS ACL

### Descrição

- n » nº do eixo
- ```{ }``` As chaves delimitam uma lista a partir da qual deve escolher um item.
- ```[ ]``` As chaves retas delimitam itens opcionais.
    >Note, no entanto, que o formato ACL exige parênteses rectos em torno dos índices
de vetores de posição, matrizes de variáveis e entradas/saídas.
- A private variable is recognized only by the program it which it is defined.
- A global variable can be used by any user program.

## MANUAL MODE

- Enter MANUAL MODE: m or ~
- Exit MANUAL MODE: m or ~

- Change to JOINT MODE: j
- Change to XYZ MODE: x

- ENABLE CONTROL: c
- DISABLE CONTROL: f

- Set speed: s(write the value 1-100 and enter)

#### In Joint mode, the keys produce the following movements:

| frente | trás | descrição                                |
| ------ | ---- | ---------------------------------------- |
| 1      | Q    | Move axis 1 (base)                       |
| 2      | W    | Move axis 2 (shoulder)                   |
| 3      | E    | Move axis 3 (elbow)                      |
| 4      | R    | Move axis 4 (wrist pitch)                |
| 5      | T    | Move axis 5 (wrist roll)                 |
| 6      | Y    | Closes/Opens electrical gripper (axis 6) |

#### In XYZ mode the following changes in manual movement occur:

| frente | trás | descrição                            |
| ------ | ---- | ------------------------------------ |
| 1      | Q    | TCP moves along X+ and X– axes.      |
| 2      | W    | TCP moves along Y+ and Y– axes.      |
| 3      | E    | TCP moves along Z+ and Z– axes.      |
| 4      | R    | Pitch moves; TCP maintains position. |

All other movements are the same as in Joint mode.

## COMANDOS DIRETOS

```DO HELP``` - Provides on-line help for EDIT commands.

```A``` , ```<Ctrl>+A``` - Aborta imediatamente todos os programas e movimentos dos eixos.

```A (nome_programa)``` - Aborta o programa especificado.

```TEST``` - Executa o diagnóstico interno do controlador e I/Os.

```VER``` - Mostra a versão da EPROM ACL.

```DIR```- Displays a list of all current user programs:
  1. The first colum is the name of the program;
  2. Secund colum is the program valididy;
  3. Third colum is the program identity number:This is a controller assigned program number; this is the number you need to use for accessing programs from the teach pendant;
  4. last colum is the program execution priority.

```DO (comando de edição)``` - Este comando permite a execução de comandos edit em modo direto.

```LISTPV POSITION```- Mostra coordenadas e valores dos encoders.

```FREE``` - Displays a list of the available memory in user RAM:
  1. Available program lines;
  2. Available variables;
  3. Available points of group A;
  4. Available points of group B;
  5. Available points of group C;
  6. Available bytes for comments.
 
```ECHO / NOECHO``` - Ativa ou desativa a exibição de caracteres no ecrã.

```SHOW SPEED``` - Mostra a velocidade atual.

```SHOW DAC (axis[n])``` -  Mostra os valores DAC de um eixo.

```SHOW ENCO``` - Mostra os valores dos encoders de 500ms em 500ms.

```SHOW DIN``` - Mostra o estado das entradas.

```SHOW DOUT``` - Mostra o estado das saídas.

```DISABLE {IN|OUT} (n)``` - Remove o controlo do controlador sobre uma entrada/saida.

```ENABLE {IN|OUT} (n)``` - Dá ao controlador o controlo sobre uma entrada/saida.

```FORCE {IN|OUT} (n) {0/1}``` - Força o estado em uma entrada/saida que foi **desativada**.

```DISABLE ?``` - Mostra os I/Os que estão desativados

```FORCE ?``` - Mostra os I/Os que Foram forçados

```CON ``` - Ativa os eixos do braço robótico.

```CON[A/B]``` - Ativa os eixos de 1 grupo.

```CON (n)``` - Activa o controlo de um eixo específico no grupo C.

```COFF``` - Desativa os eixos do braço robótico.

```COFF[A/B]``` - Desativa os eixosd e 1 grupo.

```COFF (n)``` - DEsativa o controlo de um eixo especifico do gruppo C.

```LIST```, ```LIST [prog]```
- Lista todos os programas existentes na USER RAM. Ao colocar o nome de um programa depois do coamndo somente esse programa vai ser mostrado.
- When using the LIST command to view program lines, the commands ENDFOR, ENDIF and ELSE are followed by the line number of the corresponding FOR and IF commands.

```CONFIG ?``` - Displays the current controller configuration.

```CLR (n)``` , ```CLR *``` - Reinicializa encoders (um(n) ou todos(*)).⚠️

```TON (n)``` , ```TOFF (n) ``` - Ativa/desativa a proteção térmica dos motores.⚠️

```LISTVAR``` - Lista todas as variaveis de utilizador e de sistema.

---

### YET TO TEST

COPY (prog1) (prog2)

- Copia o programa existente 1 para o programa existente 2.

EMPTY (prog)

- Limpa um programa deixando o vazio mas valido. As variaveis privadas defenidas tambem são eleminadas.

HERER (pos2)

- Records the offset values of pos2, relative to the current position, in joint (encoder) values.
- You must enter the offset values, as shown in the example below. Pos2 will always be relative to the current position.

INSERT &pvect[n]  &pvect is a position vector; n is the index of one of the positions in the vector.
- Guarda as coordenadas da posição atual do braço(como o comando HERE ou o HEREC) na posição "n" do vetor.
- Todas as posições acima de &pvect são movidas (n+1) até ser erncontrada uma posição vazia. Se não houver uma posição vazia é mostrada uma mensagem de erro e o comando é cancelado.

LISTP - Apresenta uma lista de todas as posições definidas com nomes alfanuméricos e o grupo a que estão dedicadas. As posições com nomes numéricos não são listadas.

LISTPV (pos) - Mostra os valores de encoders e coordenadas de uma posição(pos).

---

## COMANDOS DE EDIÇÃO

```EDIT (prog)``` - Activates the EDIT mode and calls up a user program named prog. If prog is not found, the system automatically creates a new program of that name.

```END``` - The system automatically writes END as the last line of a program. The system automatically writes (END) at the end of a listing, END is not a user command.

```EXIT``` - Este comando serve para sair do modo EDIT. Ao enviar este comando o controlador vai verificar se o prograam é valido.

### YET TO TEST

L (line1) (line2)
- Mostra as linhas especificas de um programa. Sendo que é mostrado da linha defenida "line1" até à linha defenida "line2".

```DEL``` - Ao enviar este comando a linha anterior do programa a ser editado vai ser eliminada.

```DEFINE (var1) [var2 ... var12]``` - Cria uma ou mais variaveis de sistema.

```DELAY (var)``` - Cria um delay durante o programa. Em que **var=1 é igual a 0,01segundos**.

```DIM (var[n])``` - Cria um array de variaveis privadas em que var é o nome e n o numero maximo do array.

```DIMG (var[n])```

- Cria um array de variaveis globais em que var é o nome e n o numero maximo do array.

```DIMP{A/B} (pvect[n])```, ```DIMPC (pvect[n]) (axis)```  - Cria um vetor que contem n posições. O primeiro caracter do nome de um vetor deve ser uma letra ou "&".

```GOSUB (prog)``` - Executa um programa existente dentro do programa principal.

```GOTO (labeln)``` - Salta para a linha a seguir à labeln correspondente se existir. "labeln" is any number, 0 <= n <= 9999.

LABEL (labeln) -  labeln is any number, 0 ≤ labeln ≤ 9999.
- Marca o início de uma sub-rotina de programa que é executada quando é dado o comando GOTO com o memso numero do "labeln".

```FOR (var1=var2) TO (var3)``` - Executes a subroutine for all values of var1, beginning with var2 and ending with var3 (até ENDFOR).

```ENDFOR``` - É necessário colocar este argumento no final de um "for" para assim a máquina saber que o loop "for" terminou.

```IF (var1) (oper) (var2)``` var1 is a variable; var2 is a variable or constant; oper can be: <, >, =, <=, >=, < >.
 - Se a operação lógica a seguir de IF for verdadeira, o que está depois do If é executado, caso contrário o bloco é ignorado e passado à frente.

```ELSE``` - Este comando vem a seguir do IF (não é obrigatório), caso a condição do IF seja falsa, o seu else vai ser executado.

```ENDIF``` - É necessário colocar no final do "if" sem else e no final do com "else" para dizer à maquina que essa lógica terminou.

MOVED (pos) [duration] - Este comando, ao contrario do MOVE, garante que as operações em um programa seja sequenciais. Estes comandos são armazenados no buffer de movimento quando o anterior tiver sido completamente executado.
- Este comando termina somente quando chegar à posição dentro da precisão especificada, não interessa o tempo que leve a chegar mesmo que a "duration" tenha sido defenida.
- Se for necessário um maior rigor no comprimento do tempo , use o coamndo EXACT OFF antes deste comando.


## COMANDOS MISTOS (DIRETOS & EDIÇÃO)

```SPEED [1-100]``` - Define a velocidade dos movimentos dos eixos.

```HOME``` - Procura a posição “home” dos eixos.

```HOME (n)``` - Procura a posição home do eixo n.

```HHOME (n)``` - Procura a posição "Hardstop" do eixo n.⚠️

```SET OUT[n]={0/1}``` - Define o estado de uma saída.

```HELP``` - Mostra todos os comandos disponíveis. Depois deste comando ser enviado, caso envie um comando , o controlador irá responder com a explicação do mesmo, para sair basta enviar um <enter>.

```RUN prog``` -Runs the specified program.

```CLRBUF``` - Limpa o buffer dos movimentos de cada eixo

```CLRBUF [A/B]``` - Limpa o buffer de movimento de um grupo inteiro

```CLRBUF (n)``` (n is an axis in group C) - Limpa o buffer de movimento de um eixo especifico do grupo C.

```SET ANOUT[n]=[(-5000) <= DAC <= 5000]``` - Disables servo control of a specific axis and sets the DAC value for a specific axis. 
  >Use with caution. May damage motor.
 
```MOVE (pos)``` - Move o robô para um posição especifica com a velocidade anteriormente defenida pelo comando SPEED.

### YET TO TEST
EXACT {A/B/C} / EXACT OFF{A/B/C}

- Determines the accuracy of the commands which are used for sequential execution of operations in a program: MOVED MOVESD MOVELD MOVECD SPLINED
  When a movement command (with D suffix) is executed in EXACT mode, the axes reach the target position accurately (within a given position error tolerance).

GLOBAL (var1) [var2 ... var12]

- Cria uma ou mais variaveis globais.

CONTINUE (prog)

- Resumes execution of program prog from the point where it was previously suspended by the SUSPEND command.

DEFP (pos) or DEFPA (pos)
DEFPB (pos)
DEFPC (pos) (n)

- Define uma posição para um grupo especifico, cria uma variavel (pos) para armazenar as coordenadas.

DELP (pos)
DELP (pvect) pvect é um vetor

- Elimina uma posição criada por o comando DEFP.

DELVAR (var)

- Elimina uma variavel ou array de variaveis da RAM, no modo direto só elimina variaveis globais. Em edit elimina variaveis privadas, mas pode eliminar globais caso não haja uma privada com o mesmo nome.

HERE (pos) pos is a position for any axis group.
HEREC (pos) pos is a robot (group A) position.

- Records an absolute position, according to the current location of the axes. HERE for joint and HEREC for cartesian.

HERER (pos2) (pos1) pos2 and pos1 are defined for the same group.

- Records the offset values of pos2, relative to pos1, in joint values. Pos1 must be recorded before this command can be entered.
- Pos2 will always be relative to pos1, moving along with and maintaining its offset whenever pos1 is moved.

DELVAR (var) ⚠️

- Deletes variable from user RAM.

JAW var [duration] 
- "var" é o tamanho da abertura do gripper, é defenido em percentagem (0 a 100). "duration" é defenido em centenas de segundos(100 = 1s).
- É recomendado o uso dos comandos OPEN/CLOSE pois são mais seguros.
 >Warning! Be sure you select a proper value for the gripper opening. An incorrect size will cause constant and excessive power to motor, and may damage the motor. 


```MOVE (pos) [duration]``` - Move o robô para uma posição dentro de um tempo defenido em centenas de segundos(100 = 1s).
- O programa que emite o comando MOVE não espera que a operação seja concluída e continua independentemente do momento em que o comando MOVE é executado. Este comando deposita quando executado é colocado em um buffer de movimento.
  > Tenha cuidado no planenamento sequencial do movimento do braço ao usar este comando.










## Vamos em MOVEC