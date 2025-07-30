# --------------------------
# Bibliotecas
# --------------------------
import serial                   # Biblioteca para comunicação serial (envio e recebimento de dados via portas COM)
import serial.tools.list_ports  # Módulo da biblioteca serial para listar as portas COM disponíveis no sistema
import threading                # Biblioteca para criar e gerenciar threads (execução de múltiplas tarefas em paralelo)
import time                     # Biblioteca para manipular pausas e medir tempo (delays e timestamps)
import tkinter as tk            # Biblioteca para criar interfaces gráficas (GUI) em Python
import re                       # Biblioteca para manipulação e busca de padrões em strings usando expressões regulares

# --------------------------
# Configurações e Variáveis
# --------------------------
ser = None                      # Objeto da porta serial
command = None                  # variavel global para armazenar o comando
resposta = None                 # variavel global para armazenar a resposta da mainframe
# --------------------------
encoder_labels = []             # Lista para armazenar os widgets dos encoders (8 no total)
coordenadas_label = []                # Lista para armazenar os widgets dos eixos 
# --------------------------
thread_enco = False             # Objeto da thread do encoder
thread_coor = False             # Objeto da thread das coordenadas
#command_sended = False          # variavel de estratégia semafro


# --------------------------
# Funções de exibição de prints
# --------------------------
def print_recebido(mensagem):   #mensagem de Rx
    """Exibe a mensagem no terminal gráfico e no terminal Python., prefixadas com 'Rx:'."""

    terminal.insert(tk.END, f"Rx: {mensagem}\n")  # Exibe no terminal gráfico
    terminal.see(tk.END)  # Faz scroll para o final do terminal
def print_sistem(mensagem):     #mensagem internas python
    """Exibe mensagens do sistema no terminal gráfico e no terminal Python, prefixadas com 'SYSTEM:'."""
    mensagem_formatada = f"SYSTEM: {mensagem}"  # Adiciona o prefixo SYSTEM:
    
    try:
        terminal.insert(tk.END, f"{mensagem_formatada}\n")  # Exibe no terminal gráfico
        terminal.see(tk.END)  # Faz scroll para o final do terminal
    except NameError:
        # Se o widget terminal não foi definido ainda, apenas imprime no terminal padrão
        print(mensagem_formatada)
        return
    
    print(mensagem_formatada)  # Exibe no terminal Python
def print_enviado(mensagem):    #mensagem de Tx
    """Exibe a mensagem no terminal gráfico e no terminal Python., prefixadas com '> '."""

    terminal.insert(tk.END, f"> {mensagem}\n")  # Exibe no terminal gráfico
    terminal.see(tk.END)  # Faz scroll para o final do terminal


# ----------------------------------------------------------
# Funções para abrir a porta serial e iniciar a comunicação
# ----------------------------------------------------------
def encontrar_porta_com():    # OBTENÇÃO DA PORTA COMX
    """Encontra a primeira porta COM ativa."""
    portas = serial.tools.list_ports.comports()
    
    if not portas:
        print_sistem("Não foi encontrada uma porta COM") # se não encontrar só manda isto
        return 
    
    COM_PORT = portas[0].device  # Se encontrou, usa a primeira porta COM disponível
    print_sistem(f"Foi encontrada a porta serial {COM_PORT}")
    
    return COM_PORT   
def abrir_conexao_serial():   # Chama a função |encontrar_porta_com()| para encontrar a porta COM ativa
    #Setup da conexão serial
    global ser  # Permite que ser seja acessado globalmente

    # Detectar a porta COM ativa
    COM_PORT = encontrar_porta_com() # Chama a função para encontrar a porta COM ativa
    # Se não houver porta COM ativa, desconecta
    if not COM_PORT:
        print_sistem("Tende outra vez.")
        return None
    
    # Configure serial connection
    ser = serial.Serial(    
        port=COM_PORT,         
        baudrate=9600,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        timeout=0.3,           # Set timeout for read operations
        xonxoff=True,        # Enable software flow control
        rtscts=False         # Disable hardware flow control
    )

    print_sistem(f"Conectado à porta {COM_PORT}.")

    if ser.is_open:  # Confirma se a porta serial foi aberta corretamente
        print_sistem(f"Porta {COM_PORT} aberta com sucesso!")
    else:
        print_sistem(f"Existe um problema na porta serial {COM_PORT}.")
        return None
    
    # Start RX thread
    threading.Thread(target=Rx, args=(ser,), daemon=True).start()


#-----------------------
# Comandos à parte
#-----------------------
def show_enco(ser): # Mostra os valores recebidos do comando SHOW ENCO
    """Captura e exibe valores dos encoders na interface gráfica."""
    global thread_enco
    global resposta
    global encoder_labels

    padrao_enco = re.compile(r"(-?\d{5,6})")  # Aceita números de 5 ou 6 dígitos(positivos e negativos)
    valores_de_encoder_repetido = ["0"] * 8  # Inicializa com 8 valores padrão

    while thread_enco:
        if resposta:  # Garante que há dados na variável
            valores_de_encoder = padrao_enco.findall(resposta)  # Busca padrões dentro da string
            
            if valores_de_encoder != valores_de_encoder_repetido and len(valores_de_encoder) == 8:  # Garante 8 valores válidos antes de atualizar
                for i in range(8):
                    encoder_labels[i].config(text=valores_de_encoder[i])  # Atualiza a interface gráfica
                
                #print_sistem(f"Valores dos encoders atualizados: {valores}")
                valores_de_encoder_repetido = valores_de_encoder  # Agora sim, atualiza a variável após a comparação
        time.sleep(0.4)  # Pequeno delay para evitar processamento excessivo

    print_sistem("Thread 'show enco' encerrada.")  # Mensagem quando a thread termina
def listpv_position(ser):
    """Thread para enviar o comando listpv position e capturar a resposta."""
    global thread_coor
    global resposta
    global coordenadas_label  # Lista de labels da GUI

    coordenadas = ["0"] * 10  # Lista temporária para armazenar os valores lidos

    # Ajuste no padrão da Linha 1
    padrao_linha1 = re.compile(
        r"1:\s*(-?\d+)\s*2:\s*(-?\d+)\s*3:\s*(-?\d+)\s*4:\s*(-?\d+)\s*5:\s*(-?\d+)\s*[\r\n]*"
    )
    padrao_linha2 = re.compile(
        r"X:\s*(-?\d+)\s*Y:\s*(-?\d+)\s*Z:\s*(-?\d+)\s*P:\s*(-?\d+)\s*R:\s*(-?\d+)"
    )

    while thread_coor:
        ser.write(("LISTPV POSITION" + "\r").encode()) 
        time.sleep(0.3)
        
        # Variáveis temporárias para armazenar os dados lidos
        linha1 = None
        linha2 = None

        for _ in range(2):  # Tenta ler até 2 vezes,O underscore (_) é usado como
                            #uma variável descartável, indicando que o valor de cada 
                            # iteração não será utilizado dentro do loop.
            if resposta:  
                if not linha1:  # Se a linha 1 ainda não foi encontrada, tenta buscar
                    match1 = padrao_linha1.search(resposta)
                    if match1:
                        linha1 = list(match1.groups())  # Armazena os valores encontrados

                if not linha2:  # Se a linha 2 ainda não foi encontrada, tenta buscar
                    match2 = padrao_linha2.search(resposta)
                    if match2:
                        linha2 = list(match2.groups())  # Armazena os valores encontrados

            if linha1 and linha2:  
                break  # Sai do loop assim que ambas as linhas forem encontradas

            time.sleep(0.1)  # Pequeno delay antes da segunda tentativa

        # Se encontramos alguma das linhas, atualizamos os valores
        if linha1:
            coordenadas[:5] = linha1  # Atualiza os primeiros 5 valores
        if linha2:
            coordenadas[5:] = linha2  # Atualiza os últimos 5 valores

        # Atualiza os labels da interface gráfica
        for i in range(10):
            coordenadas_label[i].config(text=coordenadas[i])

        time.sleep(0.1)  # Pequeno delay antes da próxima iteração

    print_sistem("Thread 'listpv position' encerrada.")  # Mensagem quando a thread termina
  

#-------------------------
# Funções de comunicação
#-------------------------
def Tx(event=None):
    """ Para envio de comandos pelo usuário. """
    global ser  # Usa a variável global ser
    global command  # Usa a variável para armazenar o comando
    global thread_enco, thread_coor  # Usa as variáveis para controlar as threads
    #global command_sended

    # Obter o comando da entrada de texto (campo 'entrada')
    command = entrada.get("1.0", tk.END).strip()  # Obter o texto da caixa de entrada e remover espaços extras
    print_enviado(command)

    if command.lower() == 'exit':
        print_sistem("A encerrar a conexão...") 
        ser.close() # Fecha a conexão serial
        janela.quit()  # Fechar a interface gráfica
    
    elif command.lower() == 'stop enco':
        print_sistem("Sending Ctrl+C...")  # Envia Ctrl+C
        ser.write(b'\x03')                 # Send CTRL+C signal

        #command_sended = True
        thread_enco = False                #parar a thread do SHOW ENCO
        
    elif command.lower() == 'show enco':
        if not thread_enco:  # Se a thread ainda não estiver ativa
            ser.write((command + "\r").encode())  # Enviar comando com terminação CR
            thread_enco = True
            threading.Thread(target=show_enco, args=(ser,), daemon=True).start()
            print_sistem("Thread 'show enco' iniciada.")
        else:
            print_sistem("Thread 'show enco' já está em execução.")
        
    elif command.lower() == 'off coor':
        thread_coor = False

    elif command.lower() == 'listpv position':
        if not thread_coor:
            thread_coor = True
            threading.Thread(target=listpv_position, args=(ser,), daemon=True).start()
            print_sistem("Thread 'listpv position' iniciada.")
        else:
            print_sistem("Thread 'listpv position' já está em execução.")

    else:
        commandWithCR = command + "\r"              # String com CR
        command_bytes = commandWithCR.encode()      # Converte para bytes
        ser.write(command_bytes)                     # Envia para a serial
        command_hex = command_bytes.hex().upper()   # Converte bytes para HEX
        print("Comando em HEX:", command_hex)


        #command_sended = True
        #print_sistem(command_sended)
    
    # Limpar a caixa de texto após enviar o comando
    entrada.delete("1.0", tk.END) 
def Tx_manual(comando):
    """Função para enviar comandos ao clicar nos botões da interface grafica"""
    entrada.delete("1.0", tk.END)
    entrada.insert("1.0", comando)
    print(f"Comando enviado: {comando}")  # Debug
    Tx()
def Rx(ser):
    """Função para ler dados da porta serial e exibir no terminal gráfico. Em HEX no terminal python."""
    global command_sended  # Usa a variável global command_sended
    global resposta # para a show enco saber quando a sequencia de dados foi recebida

    print_sistem("A ler Rx.")

    while True:  # Loop contínuo para ler dados
        #if command_sended:  # Só lê quando command_sended for True
            if ser.in_waiting > 0:  # Verifica se há dados para ler
                resposta = ser.readline().decode(errors='ignore')#.strip()  # Lê a linha da porta serial até encontrar "\n"

                #Os valores recebidos de SHOW ENCO são separaos por um "CR"
                if "\r" in resposta:  # Verifica se a resposta contém "CR" (0D em hexadecimal)
                    print_recebido(resposta.strip())  # Exibe a resposta sem espaços extras
                # Exibe também a resposta em formato hexadecimal
                resposta_bytes = resposta.encode()  # Converte a resposta para bytes
                resposta_hex = resposta_bytes.hex()  # Converte os bytes para hexadecimal
                print("Resposta em Hexadecimal:", resposta_hex.upper())  # Exibe a resposta em hex
            
            #command_sended = False  # Reseta o comando enviado para a próxima vez que um comando for dado
            #print_sistem(command_sended)
            #print_sistem("terminou de ler")
            time.sleep(0.1)  # Aguarda um pouco antes de ler novamente (evita alto consumo de CPU)


#-------------------------------
#INTERFACE GRAFICA
#-------------------------------
janela = tk.Tk()
janela.title("Interface do Robô")                           # Define o título da janela
janela.geometry("1400x650")                                 # Define o tamanho da janela
janela.configure(bg="#333333")                              # Cor de fundo
#janela.iconbitmap("icon\\minecraft.ico")                         # Ícone da janela

# Caixa de texto superior (amarela) para envio de comandos
entrada = tk.Text(janela, height=2, bg="khaki", fg="black", font=("Arial", 12))
entrada.pack(fill="x", padx=10, pady=5)
entrada.bind("<Return>", Tx)  # Associa a tecla Enter ao envio do comando

# Terminal (cinza) onde os comandos enviados são exibidos
terminal = tk.Text(janela, bg="gray", fg="black", font=("Courier", 12))
terminal.pack(side="left", expand=True, fill="both", padx=10, pady=5)

# Frame que contém os botões, coordenadas e valores dos encoders
frame_direita = tk.Frame(janela, bg="#333333")
frame_direita.pack(side="right", padx=20, pady=20, fill="y")

#-------------------------------------------------------------------------------------------------------------------------------------------------------------
# Seção ATALHOS 

tk.Label(frame_direita, text="ATALHOS", fg="white", bg="#333333", font=("Arial", 14, "bold")).pack(pady=10)
# Frame dos botões
frame_botoes = tk.Frame(frame_direita, bg="#333333")
frame_botoes.pack(pady=5, anchor="center")

# Criando os botões na primeira linha

btn_custom_LISTPV_POSITION = tk.Button(frame_botoes, text="LISTPV POSIT", bg="blue",fg="white", font=("Arial", 12), width=12, command=lambda: Tx_manual("LISTPV POSITION"))
btn_custom_LISTPV_POSITION.grid(row=0, column=0, padx=10, pady=5)

btn_custom_off = tk.Button(frame_botoes, text="STOP LISTPV", bg="pink", fg="black", font=("Arial", 12), width=12,command=lambda: Tx_manual("off coor"))
btn_custom_off.grid(row=0, column=1, padx=10, pady=5)  

btn_custom_SHOW_ENCO = tk.Button(frame_botoes, text="SHOW ENCO", bg="green", font=("Arial", 12), width=12, command=lambda: Tx_manual("SHOW ENCO"))
btn_custom_SHOW_ENCO.grid(row=0, column=2, padx=10, pady=5)

btn_custom_SE = tk.Button(frame_botoes, text="STOP ENCO", bg="pink", font=("Arial", 12), width=12, command=lambda: Tx_manual("STOP ENCO"))
btn_custom_SE.grid(row=0, column=3, padx=10, pady=5)

# Botão centralizado abaixo
btn_custom_A = tk.Button(frame_botoes, text="STOP ROBOT", bg="red", fg="white", font=("Arial", 12), width=12, command=lambda: Tx_manual("A"))
btn_custom_A.grid(row=1, column=0, columnspan=4, pady=10)# Usa columnspan=4 para centralizar

# --------------------------------------------------------------------------------------------------------------------------------------------------------
# Seção Coordenadas do Robô

tk.Label(frame_direita, text="1º-valores encodres; 2º coordenadas do robô", fg="white", bg="#333333", font=("Arial", 14, "bold")).pack()
frame_coordenadas = tk.Frame(frame_direita, bg="#333333")
frame_coordenadas.pack(pady=5)

# Criando as caixas para coordenadas (X, Y, Z, P, R)
nomes_coordenadas = ["1", "2", "3", "4", "5", "X", "Y", "Z", "P", "R"]
for i, nome in enumerate(nomes_coordenadas):
    tk.Label(frame_coordenadas, text=nome, fg="white", bg="#333333", font=("Arial", 10)).grid(row=i // 5 * 2, column=i % 5, padx=5, pady=2)
    
    label = tk.Label(frame_coordenadas, text="0", bg="lightgreen", width=10, height=2)
    label.grid(row=(i // 5) * 2 + 1, column=i % 5, padx=5, pady=5)
    coordenadas_label.append(label)  # Adiciona o label na lista


# Espaço entre seções
tk.Label(frame_direita, text="", bg="#333333", height=1).pack()

# --------------------------------------------------------------------------------------------------------------------------------------------------------
# Seção Valores dos Encoders

tk.Label(frame_direita, text="Valores de todos os encoders", fg="white", bg="#333333", font=("Arial", 14, "bold")).pack(pady=10)
frame_encoders = tk.Frame(frame_direita, bg="#333333")
frame_encoders.pack(pady=5)

# Criando as caixas para os valores dos encoders (8 eixos)
for i in range(1, 9):
    row, col = divmod(i - 1, 4)  # Calcula a posição na grade (4 colunas)
    tk.Label(frame_encoders, text=f"Eixo {i}", fg="white", bg="#333333", font=("Arial", 10))\
        .grid(row=row * 2, column=col, padx=5, pady=2)
    label = tk.Label(frame_encoders, text="0", bg="lightgreen", width=10, height=2)
    label.grid(row=row * 2 + 1, column=col, padx=5, pady=5)
    encoder_labels.append(label)

# Focar automaticamente na caixa de entrada para facilitar o uso
entrada.focus()
def main():
    # Inicia a interface gráfica
    janela.after(1000, abrir_conexao_serial)  # Inicia a conexão serial após 1000 ms
    janela.mainloop()  # Inicia o loop principal da interface gráfica

if __name__ == "__main__":
    main()