#define LED_PIN 13  // LED interno do Arduino Mega

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);

  // Aguarda a abertura da conexão serial
  while (!Serial) {
    ;  // Espera a inicialização da serial
  }

  Serial.println("Arduino pronto! Digite ON ou OFF.");
}

void loop() {
  if (Serial.available() > 0) {  // Só processa se houver dados disponíveis
    String comando = Serial.readStringUntil('\n');  // Ler até nova linha
    comando.trim();  // Remove espaços e caracteres invisíveis

    if (comando == "ON") {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED ligado!");
    } else if (comando == "OFF") {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED desligado!");
    } else {
      Serial.println("Comando inválido! Use ON ou OFF.");
    }
  }
}