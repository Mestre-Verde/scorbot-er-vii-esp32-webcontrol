import serial

# Configuração da porta serial
porta = "COM4"  # Altere para a porta correta no seu PC
baudrate = 9600

try:
    with serial.Serial(porta, baudrate, bytesize=8, stopbits=1, parity='N', xonxoff=True, timeout=1) as ser:
        print("Conectado ao Arduino. Digite 'ON' para ligar o LED, 'OFF' para desligar ou 'EXIT' para sair.")

        while True:
            comando = input("Digite o comando: ").strip().upper()

            if comando :
                ser.write((comando + "\n").encode())  # Envia o comando para o Arduino
                resposta = ser.readline().decode().strip()  # Lê a resposta do Arduino
                print("Arduino:", resposta)

            elif comando == "EXIT":
                print("Encerrando a comunicação...")
                break

            else:
                print("Comando inválido!")

except serial.SerialException as e:
    print(f"Erro ao abrir a porta serial: {e}")

