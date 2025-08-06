#include <AsyncTCP.h>           // AsyncTCP library
#include <ESPAsyncWebServer.h>  // ESPAsyncWebServer library
#include <LittleFS.h>           // LittleFS library
#include <WebSocketsServer.h>   // WebSockets library
#include <WiFi.h>               // WiFi library

#include "mbedtls/base64.h"  //Base64 é uma forma de representar dados binários (como bytes ou ficheiros) em texto ASCII seguro.

/*
-----------------------------------------
VARIABLES & CONSTANTS
-----------------------------------------*/
// serial0 é a COM8 que vai ligar ao ESP32.Esta macro pode dar problemas porque
// na biblioteca HardwareSerial.h, como default aparece Serial, é necessário
// definir o Serial0 para remover o erro e funcionar corretamente.
#define PCEsp \
  Serial0  // If using ESP32-S2, write Serial0, if using lolin, write Serial in
           // the file `HardwareSerial.cpp`.
#define ESPMain \
  Serial1             // serial1 é a COM9 ou COM10 que vai ligar ao mainframe.
#define RxSerial1 18  // para uart nos pinos, é necessário iundicar os pinos.
#define TxSerial1 17

#define XON 0x11   // Represents the byte "DC1".
#define XOFF 0x13  // Represents the byte "DC3".

WebSocketsServer webSocket(81);  // Inicializa o servidor WebSocket na porta 81
AsyncWebServer server(80);       // Inicializa o servidor HTTP na porta 80

// Definições de pinos para LEDs RGB
#define redLED 41
#define greenLED 2
#define blueLED 1
// Canais PWM (0–15 disponíveis no ESP32-S2)
#define redChannel 0
#define greenChannel 1
#define blueChannel 2
// Resolução e frequência PWM
#define pwmFreq 5000
#define pwmResBits 8  // 8 bits = valores de 0 a 255

// Variaveis para armazenar a mensagem recebida da mainframe
const size_t MAX_MSG_SIZE = 300;
uint8_t rxBuffer[MAX_MSG_SIZE];
size_t rxLen = 0;
// typedef auto auxfunction_t;
// typedef static definitelyNotStatic_t;

/**
 * @brief Variáveis referentes ao Access Point e aos clientes.
 * @author M.V.
 * @date 2025-07-14
 */
namespace clientManagerMV {
// Configuração do Access Point (AP)
const char *ap_ssid = "Scorbot-ER VII AP";  // Nome do Access Point (SSID)
const char *ap_password = "123456789";      // Senha do Access Point (senha)

bool canSendCommands =
    true;  // If true a command can be sended to the mainframe, if false, the
           // command keep in waiting until XON is received.

constexpr int MAX_NOME_LENGTH =
    25;  // Máximo de caracteres para o nome do usuário.
constexpr const char ADMIN_PASSWORD[] = "1";  // Senha correta de Admin.

const uint8_t MAX_ADMIN = 1;  // Número máximo de administradores
uint8_t adminCount = 0;       // Contador atual de administradores conectados
uint8_t adminId =
    255;  // ID do administrador atual (255 = nenhum admin atribuído)

const uint8_t MAX_USER = 1;  // Número máximo permitido de utilizadores normais
uint8_t usercount = 0;       // Contador atual de utilizadores conectados
uint8_t userId = 255;  // ID do utilizador atual (255 = nenhum user atribuído)

struct Cliente {
  uint8_t id;                  // ID único para o usuário
  bool isAdmin;                // Se é ADM ou não
  bool isUser;                 // Se é USER ou não
  IPAddress ip;                // IP do cliente
  bool conectado;              // Status de conexão (conectado/desconectado)
  char nome[MAX_NOME_LENGTH];  // Nome do usuário armazenado como uma string de
                               // caracteres
  unsigned long startConnectionMillis;  // Momento em que se conectou
  unsigned long lastPingMillis;         // Momento do último ping enviado

  // Construtor para inicializar o cliente com valores padrão
  Cliente()
      : id(255),
        isAdmin(false),
        isUser(false),
        conectado(false),
        startConnectionMillis(0),
        lastPingMillis(0) {
    memset(nome, 0, MAX_NOME_LENGTH);  // Inicializar nome como uma string vazia
  }
};
Cliente clients[WEBSOCKETS_SERVER_CLIENT_MAX];  // Array para armazenar os
                                                // clientes conectados
}  // namespace clientManagerMV

/**
 * @brief Variáveis e funções referentes aos `ValoresDoRobo`, à lista de
 * comandos `commandBacklog` e à lista de mensagens `pendingMessages`.
 * @author M.V.
 * @date 2025-07-14
 */
namespace robotValuesMV {
struct ValoresDoRobo {
  int coordenadas[5];  // 5 * 4 bytes = 20 bytes
  int encoders[5];     // 5 * 4 bytes = 20 bytes
  uint16_t inState;    // 2 bytes
  uint16_t outState;   // 2 bytes

  ValoresDoRobo()
      : coordenadas{0, 0, 0, 0, 0},
        encoders{0, 0, 0, 0, 0},
        inState(0b0000000000000000),
        outState(0b0000000000000000) {}
};
ValoresDoRobo valoresAcessiveis;

bool isQueueFull(uint8_t queueStart, uint8_t queueEnd, uint8_t maxQueueSize) {
  return ((queueEnd + 1) % maxQueueSize) == queueStart;
}
bool isQueueEmpty(uint8_t queueStart, uint8_t queueEnd) {
  return queueStart == queueEnd;
}

// lista de comandos que precisam de ser armazenados para saber a que comando
// pertence a resposta
constexpr uint8_t MAX_NUMBER_OF_COMMANDS_IN_QUEUE = 4;
enum commandType { NONE = 0, SHOW_DIN = 1, SHOW_DOUT = 2 };
commandType commandBacklog[MAX_NUMBER_OF_COMMANDS_IN_QUEUE];
uint8_t queueStartCBL = 0;
uint8_t queueEndCBL = 0;

// Obtem o comando da lista commandBacklog mais recente, e remove-o da fila.
commandType obterProximoComandoDaFila() {
  if (isQueueEmpty(queueStartCBL, queueEndCBL)) return NONE;

  commandType cmd = commandBacklog[queueStartCBL];
  queueStartCBL = (queueStartCBL + 1) % MAX_NUMBER_OF_COMMANDS_IN_QUEUE;
  return cmd;
}

// fila de comandos para enviara para a mainframe ⚠️ remover esta fila pois o
// porblema já foi resolvido
constexpr size_t MAX_MSG_LENGTH =
    64;  // max caracteres por mensagem (inclui '\0')
constexpr uint8_t MAX_PENDING_MSGS = 15;  // capacidade da fila

char pendingMessages[MAX_PENDING_MSGS]
                    [MAX_MSG_LENGTH];  // fila de mensagens, 1º[numero maximo de
                                       // espaços disponiveis],2º [numero maximo
                                       // de bytes que pode conter]
uint8_t queueStartPM = 0;  // índice de leitura
uint8_t queueEndPM = 0;    // índice de escrita

void sendMessageSlowly(const char *msg) {
  for (int i = 0; i < MAX_MSG_LENGTH && msg[i] != '\0'; ++i) {
    ESPMain.write((uint8_t *)&msg[i], 1);
    PCEsp.printf("Caractere enviado: %c\n", msg[i]);
    delay(10);  // Espera Xms entre caracteres(mudado de 50ms para 10ms para ser
                // mais rápido)
  }
  PCEsp.printf("Mensagem completa enviada: %s\n", msg);
}
void sendNextPendingMessage() {
  if (isQueueEmpty(queueStartPM, queueEndPM)) return;

  const char *msg = pendingMessages[queueStartPM];
  sendMessageSlowly(msg);

  // Avança a fila após envio
  queueStartPM = (queueStartPM + 1) % MAX_PENDING_MSGS;
}

enum listType { CHAR_PM = 0, COMMAND_TYPES = 1 };
/**
 * @author M.V.
 * @date 2025-07-14
 * @brief Adiciona um comando ou mensagem na fila especifica. Esta função
 * verifica se a fila está cheia e, se não estiver, adiciona o comando ou a
 * mensagem na fila correta com base no tipo. Esta função é modular para listas
 * circulares.
 *
 * @param type O tipo da lista/fila onde será inserido:
 *             - Use CHAR_PM para a fila de mensagens pendentes (char[15][64])
 *             - Use COMMAND_TYPES para a fila de comandos (commandBacklog).
 * @param cmd O comando a ser adicionado na fila (somente usado se type ==
 * COMMAND_TYPES), caso não seja esse use
 * 'robotValuesMV::commandType::NONE',nunca nullptr.
 * @param mensagem Texto a ser adicionado à fila de mensagens (somente usado se
 * type == CHAR_PM), caso contrario use nullptr.
 * @param queueStart Referência ao índice de início da fila (usado para
 * verificação).
 * @param queueEnd Referência ao índice de fim da fila; será atualizado após a
 * inserção.
 * @param maxQueueSize Tamanho máximo da fila (usado no controle circular).
 * @example
 * Para a lista commandBacklog: adicionarComandoNaFila(SHOW_DIN, nullptr,
 * COMMAND_TYPES, queueStartCBL, queueEndCBL, MAX_NUMBER_OF_COMMANDS_IN_QUEUE);
 * Para a lista pendingMessages: adicionarComandoNaFila(NONE, "mensagem
 * recebida", CHAR_PM, queueStartPSC, queueEndPSC, MAX_PENDING_MSGS);
 * @note se for para a lista pendingMessages, é possivel colocar qualquer valor
 * em 'cmd' pois essa lista é intocável in case of CHAR_PM
 * @return true Se o item foi adicionado com sucesso. false Se a fila estava
 * cheia ou tipo inválido.
 */
bool adicionarComandoNaFila(listType type, commandType cmd, const char *msg,
                            uint8_t &queueStart, uint8_t &queueEnd,
                            uint8_t maxQueueSize) {
  if (isQueueFull(queueStart, queueEnd, maxQueueSize)) {
    PCEsp.println("[ERRO] Fila cheia.");
    return false;
  }

  switch (type) {
    case CHAR_PM:
      if (strlen(msg) >= MAX_MSG_LENGTH) {
        // Mensagem muito grande: envia imediatamente para o mainframe e não
        // adiciona à fila
        PCEsp.println(
            "[AVISO] Mensagem excedeu o limite e será enviada imediatamente, "
            "não vai para a fila de espera.");
        ESPMain.write((const uint8_t *)msg, strlen(msg));
        return true;
      }
      // Copiar só até MAX_MSG_LENGTH - 1 e forçar o '\0'
      strncpy(pendingMessages[queueEnd], msg, MAX_MSG_LENGTH - 1);
      pendingMessages[queueEnd][MAX_MSG_LENGTH - 1] =
          '\0';  // Garante terminação
      // Atualizar queueEnd também aqui!
      queueEnd = (queueEnd + 1) % maxQueueSize;
      break;

    case COMMAND_TYPES:
      commandBacklog[queueEnd] = cmd;
      queueEnd = (queueEnd + 1) % maxQueueSize;
      break;

    default:
      PCEsp.println("[ERRO] Tipo de fila desconhecido.");
      return false;
  }

  return true;
}

}  // namespace robotValuesMV

/**
 * @brief Contem as variaveis referentes ao tempo(millis).
 * @author M.V.
 * @date 2025-07-14
 *
 * @note A maioria é usada no `void loop()`.
 */
namespace variaveisMillisMV {
unsigned long agora;  // Variável para armazenar o tempo atual

const unsigned long MAX_SESSION_TIME =
    60 * 60 * 1000UL;                // 60 minutos em milissegundos
unsigned long lastCheckTimeout = 0;  // Última vez que a verificação foi feita
const unsigned long checkTimeoutInterval =
    17 * 60 * 1000UL;  // Intervalo de 17 minutos (1 200 000 ms)

unsigned long anteriorDebug =
    0;  // Variável para armazenar o tempo anterior do debug
const unsigned long intervaloDebug =
    500;  // Intervalo de 0,5 segundos para o debug

unsigned long lastPingTime = 0;  // Tempo do último envio de ping
unsigned long lastPingCheck = 0;
const unsigned long pingInterval =
    20000;  // Intervalo de 20 segundos para enviar pings
const unsigned long pongTimeout =
    10000;  // Tempo limite de 10 segundos para resposta de pong
bool pongPending[WEBSOCKETS_SERVER_CLIENT_MAX] = {
    false};  // Rastreamento de pongs pendentes

const unsigned long intervaloMainframe = 200;
unsigned long lastSerial1Read = 0;

const unsigned long intervaloEntreComandosPedentes = 1000;
unsigned long lastTimeCommandWasSended = 0;

unsigned long rxStartTime = 0;
const unsigned long RX_TIMEOUT = 300;  // ms (ajusta conforme necessário)

}  // namespace variaveisMillisMV

/*
-----------------------------------------
FUNCTION DECLARATIONS
-----------------------------------------*/

// void listarRedesWiFi();
// const char *traduzirEncriptacao(wifi_auth_mode_t tipo);

void ledRGB(uint8_t red, uint8_t green, uint8_t blue);
void desconectarCliente(uint8_t id, const char *motivo);
void verificarTimeouts();
void clientsTrullyConected();
void mostrarStatusClientes(uint8_t id, uint8_t target);
void connectToSTA(const char *ssid, const char *password);
bool startsWithIgnoreCase(const uint8_t *payload, size_t length,
                          const char *prefix);
void updadeADMlist();
void enviarTextoParaCliente(uint8_t id, const char *texto);
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                    size_t length);
void mostrarInfoRedeAtual(uint8_t target);
void debbug();
void mainframe();

/*
-----------------------------------------
FUNCTION DEFINITIONS
-----------------------------------------*/

/**
 * @author M.V.
 * @date 2025-07-14
 * @version 1.0
 * @brief Escolhe o melhor canal Wi-Fi (menos congestionado) entre 1 e 13.
 * Realiza múltiplas varreduras de redes Wi-Fi próximas e conta quantas redes
 * estão em cada canal. Retorna o canal com menor número de redes detectadas,
 * ideal para configurar o Access Point (AP). Caso não encontre nenhuma rede
 * após todas as tentativas, retorna o valor padrão (6).
 *
 * @param tentativasMax Número máximo de tentativas de varredura (default: 5).
 * @param intervaloMs Intervalo em milissegundos entre tentativas (default:
 * 500ms).
 * @return int Canal Wi-Fi recomendado (1 a 13).
 */
int escolherMelhorCanal(int tentativasMax = 5, int intervaloMs = 500) {
  int melhorCanal = 6;   // fallback neutro
  int channelUsage[13];  // uso por tentativa

  for (int tentativa = 0; tentativa < tentativasMax; tentativa++) {
    memset(channelUsage, 0, sizeof(channelUsage));  // limpar uso

    int n = WiFi.scanNetworks();
    if (n <= 0) {
      delay(intervaloMs);
      continue;
    }

    // Contar uso de canais
    for (int i = 0; i < n; i++) {
      int ch = WiFi.channel(i);
      if (ch >= 1 && ch <= 13) channelUsage[ch - 1]++;
    }

    // Encontrar canal com menos interferência
    int minUso = channelUsage[0];
    melhorCanal = 1;
    for (int i = 1; i < 13; i++) {
      if (channelUsage[i] < minUso) {
        minUso = channelUsage[i];
        melhorCanal = i + 1;
      }
    }
    return melhorCanal;  // saiu com dados válidos
  }
  // Não encontrou nenhuma rede após todas as tentativas
  return melhorCanal;
}

void ledRGB(uint8_t red, uint8_t green, uint8_t blue) {
  ledcWrite(redChannel, red);
  ledcWrite(greenChannel, green);
  ledcWrite(blueChannel, blue);
}

void setup() {  // put your setup code here, to run once:
  delay(6000);  // delay para ser possível ver os prints do setup, obrigatório
                // para debug

  // Configura canais PWM
  ledcSetup(redChannel, pwmFreq, pwmResBits);
  ledcSetup(greenChannel, pwmFreq, pwmResBits);
  ledcSetup(blueChannel, pwmFreq, pwmResBits);

  // Associa os canais aos pinos
  ledcAttachPin(redLED, redChannel);
  ledcAttachPin(greenLED, greenChannel);
  ledcAttachPin(blueLED, blueChannel);

  // luzes iniciais.
  for (int i = 0; i <= 7; i++) {
    switch (i) {
      case 0:
        ledRGB(150, 0, 0);
        break;  // Vermelho
      case 1:
        ledRGB(0, 150, 0);
        break;  // Verde
      case 2:
        ledRGB(0, 0, 155);
        break;  // Azul
      case 3:
        ledRGB(155, 155, 0);
        break;  // Amarelo
      case 4:
        ledRGB(155, 0, 155);
        break;  // Magenta
      case 5:
        ledRGB(0, 155, 155);
        break;  // Ciano
      case 6:
        ledRGB(155, 155, 155);
        break;  // Branco
      case 7:
        ledRGB(0, 0, 0);
        delay(200);
        break;  // Desliga tudo no fim
    }
    delay(400);
  }

  // Inicialização da Serial para depuração
  PCEsp.begin(115200);
  ESPMain.setRxBufferSize(MAX_MSG_SIZE + 30);  // Increase buffer size
  ESPMain.begin(
      9600, SERIAL_8N1, RxSerial1,
      TxSerial1);  // Configura a Serial1 para comunicação com o mainframe

  // Inicialize o sistema de arquivos
  if (!LittleFS.begin()) {
    while (true) {
      for (int i = 0; i < 3; i++) {
        ledRGB(140, 0, 0);
        delay(300);
        ledRGB(0, 0, 0);
        delay(300);
      }
    }
  }

  WiFi.mode(WIFI_STA);
  if (WiFi.disconnect()) {
    PCEsp.println(F("✅ Desconexão completa."));
  } else {
    PCEsp.println(F("❌ Erro ao desconectar."));
    // ESP.restart();  // Reinicia o ESP32 se não conseguir desconectar
  }

  int melhorCanal =
      escolherMelhorCanal();  // guarda o melhor canal para conexão
  PCEsp.printf("📶 Canal menos congestionado: %d\n", melhorCanal);

  WiFi.mode(WIFI_AP_STA);  // Configurar WiFi no modo STA e AP

  /*----------------------------
  Parte do Acess Point com canal automático
  ----------------------------*/
  WiFi.softAP(clientManagerMV::ap_ssid, clientManagerMV::ap_password,
              melhorCanal, 0, 4);  // inicia o AP

  PCEsp.printf("Access Point '%s' iniciado.\nIP do ESP32 (AP): %s\n",
               clientManagerMV::ap_ssid, WiFi.softAPIP().toString().c_str());
  /*----------------------------
  Parte do servidor HTTP
  ----------------------------*/
  // Página principal
  server.serveStatic("/", LittleFS, "/").setDefaultFile("html/index.html");
  // atualiza a página http://192.168.1.167/estado para cada requisição dos
  // viewers
  server.on("/estado", HTTP_GET, [](AsyncWebServerRequest *request) {
    char texto[80];      // coordenadas e encoders como string
    uint8_t ioBytes[4];  // binário dos IOs
    char base64IO[10];   // Base64 de 4 bytes = 8 caracteres (+ margem)
    size_t base64Len = 0;
    char respostaFinal[192];  // resposta total (texto + base64IO)

    const int *coordenadas = robotValuesMV::valoresAcessiveis
                                 .coordenadas;  // 5var * 4bytes = 20 bytes
    const int *encoders =
        robotValuesMV::valoresAcessiveis
            .encoders;  // 5var * 4bytes = 20 bytes  ao juntar os 2 dá 40 bytes
                        // | 40bytes * 8bits(1byte) = 320 bits
    uint16_t inState = robotValuesMV::valoresAcessiveis.inState;
    uint16_t outState =
        robotValuesMV::valoresAcessiveis
            .outState;  // 4bytes = 32bits, base64 gera saidas com multiplos de4
                        // então em vez de gerar 6 bytes, vai gerar 8bytes

    // Prepara o texto das coordenadas e encoders
    snprintf(
        texto, sizeof(texto),
        "%d,%d,%d,%d,%d;%d,%d,%d,%d,%d;",  // Total = 60 (números) + 10
                                           // (separadores) + 1 ('\0') = 71
                                           // bytes (pior caso possivel -99999)
        coordenadas[0], coordenadas[1], coordenadas[2], coordenadas[3],
        coordenadas[4], encoders[0], encoders[1], encoders[2], encoders[3],
        encoders[4]);

    // Prepara os 4 bytes binários
    ioBytes[0] = inState >> 8;
    ioBytes[1] = inState & 0xFF;
    ioBytes[2] = outState >> 8;
    ioBytes[3] = outState & 0xFF;
    // Codifica os IOs em Base64,
    int ret = mbedtls_base64_encode((unsigned char *)base64IO, sizeof(base64IO),
                                    &base64Len, ioBytes, 4);
    if (ret != 0) {
      request->send(500, "text/plain", "Erro na codificação Base64");
      return;
    }
    base64IO[base64Len] = '\0';  // garantir terminação nula
    // Concatena texto + base64IO na resposta final
    snprintf(respostaFinal, sizeof(respostaFinal), "%s%s", texto, base64IO);
    // Envia como texto plano
    request->send(200, "text/plain", respostaFinal);
  });
  // Responder ao pedido /get-file apenas via código JS
  server.on("/get-comandos", HTTP_GET, [](AsyncWebServerRequest *request) {
    File file = LittleFS.open("/comandos.md", "r");
    if (!file) {
      request->send(404, "text/plain", "Ficheiro não encontrado");
      return;
    }
    AsyncWebServerResponse *comandsFileResponse = request->beginResponse(
        "text/plain", file.size(),
        [file](uint8_t *buffer, size_t maxLen, size_t index) mutable -> size_t {
          return file.read(buffer, maxLen);
        });
    comandsFileResponse->addHeader("Content-Disposition",
                                   "attachment; filename=comandos.md");
    request->send(comandsFileResponse);
  });
  // Inicializa o servidor HTTP para o primeiro handshake
  server.begin();
  PCEsp.println(F("Servidor HTTP iniciado na porta 80."));
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(204);
  });  // 204 No Content, evita erro e ignora o favicon

  /*----------------------------
  Parte do servidor WEBSOCKET
  ----------------------------*/
  // Inicializa o WebSocket para comunicação em tempo real
  webSocket.begin();
  PCEsp.println(F("Servidor WebSocket iniciado na porta 81."));
  webSocket.onEvent(webSocketEvent);
}

// de vez em quando mede o tempo que demora cada função a ser executada, para
// ver se existe alguma que demora mais do que deve.
void loop() {  // put your main code here, to run repeatedly:
  variaveisMillisMV::agora = millis();

  webSocket.loop();

  if (variaveisMillisMV::agora - variaveisMillisMV::anteriorDebug >=
      variaveisMillisMV::intervaloDebug) {
    variaveisMillisMV::anteriorDebug = variaveisMillisMV::agora;
    debbug();
  }
  if (variaveisMillisMV::agora - variaveisMillisMV::lastCheckTimeout >=
      variaveisMillisMV::checkTimeoutInterval) {
    variaveisMillisMV::lastCheckTimeout = variaveisMillisMV::agora;
    verificarTimeouts();
  }
  if (variaveisMillisMV::agora - variaveisMillisMV::lastPingCheck >=
      variaveisMillisMV::pongTimeout) {
    variaveisMillisMV::lastPingCheck = variaveisMillisMV::agora;
    clientsTrullyConected();
  }
  if (variaveisMillisMV::agora - variaveisMillisMV::lastSerial1Read >=
      variaveisMillisMV::intervaloMainframe) {
    variaveisMillisMV::lastSerial1Read = variaveisMillisMV::agora;
    mainframe();
  }
  if (variaveisMillisMV::agora - variaveisMillisMV::lastTimeCommandWasSended >=
          variaveisMillisMV::intervaloEntreComandosPedentes &&
      clientManagerMV::canSendCommands) {
    variaveisMillisMV::lastTimeCommandWasSended = variaveisMillisMV::agora;
    robotValuesMV::sendNextPendingMessage();
  }
}

// debbug
void debbug() {
  if (PCEsp.available()) {
    String terminalPc = PCEsp.readStringUntil('\n');  // Lê até Enter
    terminalPc.trim();  // Remove espaços e quebras de linha

    PCEsp.print("Recebido do PC: ");
    PCEsp.println(terminalPc);
    ESPMain.print(terminalPc + "\r");

    if (terminalPc.equals("clientinfo")) {  // <-- aqui são aspas duplas!
      mostrarStatusClientes(255, 1);
    }
    if (terminalPc.equals("wifiinfo")) {
      // listarRedesWiFi();
      mostrarInfoRedeAtual(0);
    }
    if (terminalPc.equals("clearlist")) {
      for (int i = 0; i < robotValuesMV::MAX_NUMBER_OF_COMMANDS_IN_QUEUE; i++) {
        robotValuesMV::commandBacklog[i] = robotValuesMV::commandType::NONE;
      }
      PCEsp.println("[DEBUG] lista de espera limpa.");
    }

    // envia para o websocket
    webSocket.broadcastTXT(terminalPc);
  }
}

/**
 * @author M.V.
 * @date 2025-07-15
 * @version 1.0
 * @brief Lê bytes da porta serial ESPMain e cria strings de textos.
 *
 * @todo none
 * @bug none
 * @details
 * Esta função contém um loop que lê 1 byte de cada vez da serial e armazena os
 * dados em um buffer chamado `rxBuffer`. A função deteta o fluxo de controle
 * XON/XOFF, dependentdo do tipo, atualiza a variavel `canSendCommands` para
 * permitir ou bloquear o envio de comandos. Se receber um CR espera até receber
 * um LF, se durnate um intervalo de tempo não receber o LF, a mensagem é
 * processada, o mesmo se aparecer um LF. Se estiver à espera de um LF e
 * recebrer outro caracteres a mensagem é processada com o CR. Se só vier um LF,
 * a mensagem é processada. Se o buffer tiver algum dado e não aparecer uma
 * terminação, a mensagem é processada por timeout.
 */
void mainframe() {
  // PCEsp.println(F("Entrou na função mainframe()"));
  /**
   * @fn int parseIntFrom(const char* str, size_t start, size_t end)
   * @brief Converte uma substring de uma string em inteiro.
   * @param str A string original.
   * @param start Índice inicial (inclusive).
   * @param end Índice final (exclusive).
   * @return Valor inteiro da substring.
   */
  auto parseIntFrom = [](const char *str, size_t start, size_t end) -> int {
    char temp[16];
    size_t len = end - start;
    if (len >= sizeof(temp)) len = sizeof(temp) - 1;
    strncpy(temp, str + start, len);
    temp[len] = '\0';
    return atoi(temp);
  };
  /**
   * @fn void processMessage(uint8_t* rxBuffer, size_t rxLen)
   * @brief Processa uma mensagem completa recebida na porta serial.
   * @param rxBuffer Buffer contendo a mensagem recebida.
   * @param rxLen Comprimento da mensagem recebida.
   *
   * @details
   * Mostra a mensagem completa em ASCII para o PCEsp,converte caracteres
   * imprimiveis em '.' e ignora-os. Envia por websocket para o adm e o user (se
   * conectados). Mostra a mensagem em formato HEX no PCEsp. Procura um padrão
   * específico "1 -> 16:" para identificar estados digitais de IOs. Se
   * encontrado, converte os bits em um array de bools e armazena no IO
   * correspondente. Procura coordenadas e encoders na mensagem, se encontrados,
   * converte-os em inteiros e armazena nos arrays correspondentes.
   */
  auto processMessage = [&](uint8_t *rxBuffer, size_t rxLen) {
    // Print ASCII-safe version
    PCEsp.print("[SAFE ASCII] ");
    for (size_t i = 0; i < rxLen; ++i) {
      if (isprint(rxBuffer[i]))
        PCEsp.print((char)rxBuffer[i]);
      else
        PCEsp.print('.');
    }
    PCEsp.println();

    // Converte caracteres imprimíveis em uma string de texto
    char msgTxt[257];  // MAX_MSG_SIZE + 1 para '\0'
    size_t j = 0;
    for (size_t i = 0; i < rxLen && j < sizeof(msgTxt) - 1; ++i) {
      if (isprint(rxBuffer[i])) {
        msgTxt[j++] = rxBuffer[i];
      }
    }
    msgTxt[j] = '\0';

    // Envia para admin e user (se conectados)
    if (clientManagerMV::adminId != 255) {
      enviarTextoParaCliente(clientManagerMV::adminId, msgTxt);
    }
    if (clientManagerMV::userId != 255 &&
        clientManagerMV::userId != clientManagerMV::adminId) {
      enviarTextoParaCliente(clientManagerMV::userId, msgTxt);
    }

    // HEX debug
    PCEsp.print("[HEX]  ");
    for (size_t i = 0; i < rxLen; ++i) {
      if (rxBuffer[i] < 0x10) PCEsp.print('0');
      PCEsp.print(rxBuffer[i], HEX);
      PCEsp.print(' ');
    }
    PCEsp.println();

    // Pattern: "1 -> 16:"
    constexpr const char pattern[] =
        "1 -> 16:";  // Padrão inicial com as resposta dos estados dos IOs.
    constexpr size_t patternLen = sizeof(pattern) - 1;
    size_t headerIndex = SIZE_MAX;
    if (rxLen >= patternLen) {
      for (size_t i = 0; i <= rxLen - patternLen; ++i) {
        if (memcmp(&rxBuffer[i], pattern, patternLen) == 0) {
          headerIndex = i;
          break;
        }
      }
    }
    if (headerIndex != SIZE_MAX) {
      bool digitalStates[16] = {0};
      size_t bitIndex = 0;
      size_t i =
          headerIndex + patternLen;  // i representa a posição do primeiro bit
                                     // após o padrão "1 -> 16:"
      while (i < rxLen && (rxBuffer[i] == ' ' || rxBuffer[i] == '\t'))
        i++;  // primeiro loop para ignorar espaços vazios
      for (/*inicialização*/; i < rxLen && bitIndex < 16;
           ++i) {  // inicialização vazia porque já iniciou anteriormente
        if (rxBuffer[i] == '0' ||
            rxBuffer[i] == '1') {  // Verifica se é um bit válido
          digitalStates[bitIndex++] =
              (rxBuffer[i] == '1');  // Na expreção lógica é convertido '1' para
                                     // true e '0' para false
          while (i + 1 < rxLen && rxBuffer[i + 1] == ' ')
            i++;  // Ignora espaços após o bit
        }
      }
      if (bitIndex == 16) {  // se tiver 16 bits manda armazenar nas variaveis
                             // de estados do IO correto.
        robotValuesMV::commandType cmd = robotValuesMV::
            obterProximoComandoDaFila();  // Obtem o comando que deu origem à
                                          // resposta dos estados dos IOs.
        uint16_t value = 0;
        for (int i = 0; i < 16; i++) {
          value |= (digitalStates[i] ? 1 : 0)
                   << (15 - i);  // |= is a bitwise OR
        }
        if (cmd == robotValuesMV::SHOW_DIN) {
          robotValuesMV::valoresAcessiveis.inState = value;
          PCEsp.printf("[DEBUG] SHOW_DIN,values updated:  0x%04X | binário: ",
                       value);
        } else if (cmd == robotValuesMV::SHOW_DOUT) {
          robotValuesMV::valoresAcessiveis.outState = value;
          PCEsp.printf("[DEBUG] SHOW_DOUT,values updated:   0x%04X | binário: ",
                       value);
        }
        for (int i = 15; i >= 0; i--) PCEsp.print((value >> i) & 1);
        PCEsp.println();
      } else {
        PCEsp.println("[ERRO] Número inválido de bits recebidos.");
      }
      return;
    }

    // coordenates and encoders parttern
    size_t start = 0;
    while (start < rxLen && rxBuffer[start] == ' ')
      start++;  // Ignora espaços em branco no início do buffer

    if (start + 1 < rxLen && isdigit(rxBuffer[start]) &&
        rxBuffer[start + 1] == ':') {  // Verifica se a mensagem começa com um
                                       // dígito seguido de ':' (ex: "1:1234")
      PCEsp.println(
          "[DEBUG] Mensagem recebida possivelmente contém encoders e "
          "coordenadas.");
      size_t i = 0;

      while (i < rxLen) {  // Percorre todo o buffer recebido
        if (i + 1 < rxLen && isdigit(rxBuffer[i]) &&
            rxBuffer[i + 1] == ':') {  // Procura padrões do tipo "N:valor" (N =
                                       // dígito de 1 a 5)
          int encoderId =
              rxBuffer[i] - '0';  // Extrai o número do encoder (1 a 5)
          size_t numStart =
              i + 2;  // Posição inicial do valor numérico após ':'

          while (numStart < rxLen && rxBuffer[numStart] == ' ')
            numStart++;  // Ignora espaços após ':'
          size_t numEnd = numStart;

          // Avança até o próximo espaço ou fim do valor numérico ou \n ou \r
          while (numEnd < rxLen && rxBuffer[numEnd] != ' ' &&
                 rxBuffer[numEnd] != '\n' && rxBuffer[numEnd] != '\r')
            numEnd++;

          // Converte a substring para inteiro
          int val = parseIntFrom((const char *)rxBuffer, numStart, numEnd);
          if (encoderId >= 1 &&
              encoderId <= 5) {  // Se o encoderId é válido, guarda o valor no
                                 // array de encoders
            robotValuesMV::valoresAcessiveis.encoders[encoderId - 1] = val;
            // PCEsp.printf("[DEBUG] Encoder %d -> Valor guardado: %d\n",
            // encoderId, val);
          } else {
            // PCEsp.printf("[ERRO] ID de encoder inválido: %d\n", encoderId);
          }
          i = numEnd;  // Avança o índice para depois do valor lido
          continue;    // Continua procurando outros padrões no buffer
        }
        i++;  // Avança para o próximo caractere se não encontrou padrão
      }
      return;  // Sai da função após processar todos os encoders encontrados
    }

    // Coordinates: "X:..."
    if (start + 1 < rxLen &&
        (rxBuffer[start] == 'X' || rxBuffer[start] == 'Y' ||
         rxBuffer[start] == 'Z' || rxBuffer[start] == 'P' ||
         rxBuffer[start] == 'R') &&
        rxBuffer[start + 1] == ':') {
      // PCEsp.println("[DEBUG] Mensagem recebida possivelmente contém apenas
      // coordenadas.");
      size_t i = start;
      while (i < rxLen) {
        if (i + 1 < rxLen &&
            (rxBuffer[i] == 'X' || rxBuffer[i] == 'Y' || rxBuffer[i] == 'Z' ||
             rxBuffer[i] == 'P' || rxBuffer[i] == 'R') &&
            rxBuffer[i + 1] == ':') {
          char coord = rxBuffer[i];
          size_t numStart = i + 2;
          while (numStart < rxLen &&
                 (rxBuffer[numStart] == ' ' || rxBuffer[numStart] == '\t'))
            numStart++;
          if (numStart >= rxLen) break;
          size_t numEnd = numStart;
          if (numEnd < rxLen && rxBuffer[numEnd] == '-') numEnd++;
          while (numEnd < rxLen && rxBuffer[numEnd] >= '0' &&
                 rxBuffer[numEnd] <= '9')
            numEnd++;
          int coordValue =
              parseIntFrom((const char *)rxBuffer, numStart, numEnd);
          int index = -1;
          switch (coord) {
            case 'X':
              index = 0;
              break;
            case 'Y':
              index = 1;
              break;
            case 'Z':
              index = 2;
              break;
            case 'P':
              index = 3;
              break;
            case 'R':
              index = 4;
              break;
          }
          if (index != -1) {
            robotValuesMV::valoresAcessiveis.coordenadas[index] = coordValue;
            // PCEsp.printf("[DEBUG] Coordenada %c -> Valor guardado: %d\n",
            // coord, coordValue);
          }
          i = numEnd;
          while (i < rxLen && (rxBuffer[i] == ' ' || rxBuffer[i] == '\t')) i++;
          continue;
        }
        i++;
      }
      return;
    }
  };

  // ---------------------------
  // Main RX loop: lê bytes da porta serial ESPMain e processa mensagens
  // completas
  // ---------------------------
  static bool waitingForLF =
      false;  // True se o último byte foi \r e estamos à espera de \n

  // Lê todos os bytes disponíveis na UART e monta mensagens completas
  while (ESPMain.available()) {  // avaiable() retorna o número de bytes
                                 // disponíveis para leitura, quando não houver
                                 // mais bytes, retorna 0.
    int byteRead = ESPMain.read();  // Lê um byte da porta serial ESPMain,quando
                                    // não houver mais bytes, retorna -1.
    if (byteRead < 0)
      continue;  // Se não houver bytes disponíveis, read() retorna -1, então
                 // ignora
    if (rxLen == 0)
      variaveisMillisMV::rxStartTime =
          millis();  // Se o buffer não está vazio, marca o início da receção
                     // (para timeout)

    // Verifica se o byte lido corresponde a XON ou XOFF
    if (byteRead == XON) {
      clientManagerMV::canSendCommands = true;
      PCEsp.println("[DEBUG] XON recebido, comandos podem ser enviados.");
      continue;  // Não é necessário guardar no buffer
    }
    if (byteRead == XOFF) {
      clientManagerMV::canSendCommands = false;
      PCEsp.println(
          "[DEBUG] XOFF recebido, não envia comandos até receber XON.");
      continue;  // Não é necessário guardar no buffer
    }

    // Se houver espaço no buffer...
    if (rxLen < MAX_MSG_SIZE) {
      rxBuffer[rxLen++] = static_cast<uint8_t>(
          byteRead);  // converte int para uint8_t e adiciona o byte ao buffer.
    } else {          // Se o buffer encher, limpa e avisa
      PCEsp.println(
          "[WARN] Buffer excedido, aumenta no codigo os bytes do array. "
          "Limpo.");
      rxLen = 0;
      waitingForLF = false;
      continue;
    }

    // Se está à espera de LF após CR, verifica se o byte atual é \n
    if (waitingForLF) {
      waitingForLF = false;
      if (byteRead == '\n') {  // Mensagem termina com \r\n: processa tudo
        processMessage(rxBuffer, rxLen);
        rxLen = 0;
        continue;
      } else {  // Mensagem termina só com \r: processa até ao \r (incluindo)
        processMessage(rxBuffer, rxLen - 1);
        rxBuffer[0] = static_cast<uint8_t>(
            byteRead);  // O byte atual (não-\n) inicia nova mensagem
        rxLen = 1;
        // Não faz continue, pois pode ser outro terminador
      }
    }
    // Se o byte lido é \r, espera para ver se o próximo é \n
    if (byteRead == '\r') {
      waitingForLF = true;
      continue;
    }
    // Se o byte lido é \n (sem \r antes), processa normalmente
    if (byteRead == '\n') {
      processMessage(rxBuffer, rxLen);
      rxLen = 0;
      continue;
    }
  }

  // Fora do while: verifica timeout de receção
  if (rxLen > 0 &&
      (millis() - variaveisMillisMV::rxStartTime >
       variaveisMillisMV::RX_TIMEOUT)) {  // Timeout: processa o que tem no
                                          // buffer, mesmo sem terminação
    processMessage(rxBuffer, rxLen);
    rxLen = 0;
    waitingForLF = false;
  }
}

/**
 * @authors M.V. , Markus Sattler
 * @date 2025-07-14
 * @version 1.0
 * @brief Evento de tratamento da comunicação WebSocket.
 * Esta função é chamada sempre que ocorre um evento no WebSocket (conexão,
 * desconexão, mensagem de texto, pong, erro). Permite gerir clientes
 * conectados, processar comandos específicos (como nome de utilizador ou
 * palavra-passe de administrador), e responder conforme necessário.
 *
 * @param num ID do cliente WebSocket que gerou o evento.
 * @param type Tipo de evento WebSocket (conexão, mensagem, erro, etc.).
 * @param payload Ponteiro para os dados da mensagem recebida (se
 * aplicável),para tratar deste tipo é necessário ter uma terminação "/0".
 * @param length Tamanho da mensagem recebida (em bytes).
 *
 * @todo USar o bin para receber os programas provindos do website para assim
 * criar um modo para enviar programas para o robot.
 *
 * @see WStype_t in file WebSocketsServer.h
 */
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                    size_t length) {
  /**
   * @fn conectarCliente(uint8_t num, IPAddress ip)
   * @brief Função auxiliar para conectar um cliente WebSocket.
   *
   * @param num ID do cliente.
   * @param ip Endereço IP do cliente.
   */
  auto conectarCliente = [](uint8_t num, IPAddress ip) {
    if (num < WEBSOCKETS_SERVER_CLIENT_MAX) {
      clientManagerMV::clients[num].id = num;
      clientManagerMV::clients[num].ip = ip;
      clientManagerMV::clients[num].conectado = true;
      clientManagerMV::clients[num].startConnectionMillis = millis();
      updadeADMlist();  // Atualiza a lista do administrador, se houver
    }
  };

  /**
   * @fn handleWebSocketMessage(uint8_t num, uint8_t *payload, size_t length)
   * @brief Função auxiliar para processar mensagens WebSocket recebidas.
   *
   * @param num ID do cliente que enviou a mensagem.
   * @param payload Ponteiro para os dados da mensagem recebida.
   * @param length Tamanho da mensagem recebida (em bytes).
   *
   * @details
   *
   */
  auto handleWebSocketMessage = [](uint8_t num, uint8_t *payload,
                                   size_t length) {
    /**
     * @fn atualizarNomeCliente(uint8_t num, const char *novoNome)
     * @brief Actualiza o nome de um cliente na estrutura de clientes
     * conectados. Esta função verifica se o ID é válido, se o cliente está
     * conectado e se o ponteiro para o novo nome não é nulo. Em seguida, copia
     * o novo nome para a estrutura correspondente, garantindo que a string
     * esteja devidamente terminada.
     *
     * @param num ID do cliente .
     * @param novoNome Ponteiro para a nova string de nome a ser atribuída.
     */
    auto atualizarNomeCliente = [](uint8_t num, const char *novoNome) {
      if (num < WEBSOCKETS_SERVER_CLIENT_MAX &&
          clientManagerMV::clients[num].conectado && novoNome != nullptr) {
        strncpy(clientManagerMV::clients[num].nome, novoNome,
                clientManagerMV::MAX_NOME_LENGTH - 1);
        clientManagerMV::clients[num]
            .nome[clientManagerMV::MAX_NOME_LENGTH - 1] =
            '\0';         // Garante terminação
        updadeADMlist();  //  Atualiza a lista do administrador, se houver
        // PCEsp.printf("Nome atualizado para cliente %u: %s\n", num,
        // clientManagerMV::clients[num].nome);
        enviarTextoParaCliente(
            num, "NOMEATUALIZADO");  // Envia confirmação de nome atualizado
      } else {
        PCEsp.printf(
            "Erro ao atualizar nome: id=%u, conectado=%d, nome pointer=%p\n",
            num, clientManagerMV::clients[num].conectado, novoNome);
      }
    };

    using namespace clientManagerMV;
    const char *msg = (const char *)payload;  // Comando e argumentos

    // 1. Comandos de identificação
    if (length > 5 && strncmp(msg, "NOME:", 5) == 0) {
      atualizarNomeCliente(num, msg + 5);
      return;
    }
    if (length > 8 && strncmp(msg, "ADMPASS:", 8) == 0) {
      char senhaRecebida[32];
      strncpy(senhaRecebida, msg + 8, sizeof(senhaRecebida) - 1);
      senhaRecebida[31] = '\0';
      if (strcmp(senhaRecebida, ADMIN_PASSWORD) == 0 &&
          adminCount < MAX_ADMIN) {
        clients[num].isAdmin =
            true;       // Concede o status de administrador ao cliente actual
        adminCount++;   // Incrementa o contador de administradores ativos
        adminId = num;  // Guarda o ID do cliente que se tornou administrador
        enviarTextoParaCliente(num,
                               "ADMOK");    // Envia confirmação de sucesso na
                                            // autenticação como administrador
        mostrarStatusClientes(adminId, 2);  // Actualiza a interface ou terminal
                                            // com o novo estado dos clientes
      } else {
        enviarTextoParaCliente(num,
                               adminCount >= MAX_ADMIN ? "ADMFULL" : "ADMFAIL");
      }
      return;
    }

    // 2. Comandos só para admin
    if (clients[num].isAdmin) {
      if (length == 10 && strncmp(msg, "CLIENTINFO", 10) == 0) {
        mostrarStatusClientes(num, 2);
        return;
      }
      if (length > 9 && strncmp(msg, "KICKUSER:", 9) == 0) {
        uint8_t idCliente = atoi(msg + 9);
        if (idCliente < WEBSOCKETS_SERVER_CLIENT_MAX &&
            clients[idCliente].conectado) {
          desconectarCliente(idCliente, "KICKED");
          mostrarStatusClientes(num, 2);
          // PCEsp.printf("O cliente %u foi desconectado pelo admin %u.\n",
          // idCliente, num);
        } else {
          enviarTextoParaCliente(
              num, "POPUP: ID inválido ou cliente não conectado.");
        }
        return;
      }
      if (length > 8 && strncmp(msg, "SETUSER:", 8) == 0) {
        uint8_t idCliente = atoi(msg + 8);
        if (idCliente < WEBSOCKETS_SERVER_CLIENT_MAX &&
            clients[idCliente].conectado) {
          if (usercount < MAX_USER) {
            clients[idCliente].isUser = true;  // Define o cliente como usuário
            usercount++;         // Incrementa o contador de usuários
            userId = idCliente;  // Guarda o ID do cliente que se tornou usuário
            enviarTextoParaCliente(idCliente, "USEROK");
            mostrarStatusClientes(
                adminId, 2);  // Mostra o status atualizado dos clientes
            // PCEsp.printf("O cliente %u foi definido como USER pelo admin
            // %u.\n", idCliente, num);
          } else {
            enviarTextoParaCliente(num, "USERFULL");
          }
        } else {
          enviarTextoParaCliente(num,
                                 "POPUP:ID inválido ou cliente não conectado.");
        }
        return;
      }
      if (length > 11 && strncmp(msg, "REMOVEUSER:", 11) == 0) {
        uint8_t idCliente = atoi(msg + 11);
        if (idCliente < WEBSOCKETS_SERVER_CLIENT_MAX &&
            clients[idCliente].conectado && clients[idCliente].isUser) {
          clients[idCliente].isUser = false;
          usercount--;
          userId = 255;
          enviarTextoParaCliente(idCliente, "USERREMOVED");
          mostrarStatusClientes(num, 2);
        } else {
          enviarTextoParaCliente(
              num, "POPUP:O cliente não é um usuário ou ID inválido.");
        }
        return;
      }
      if (length > 7 && strncmp(msg, "NEWSTA:", 7) == 0) {
        const char *data = msg + 7;
        const char *sep = strstr(data, "||");
        if (sep != NULL) {
          size_t ssid_len = sep - data;
          size_t pass_len = strlen(sep + 2);
          if (ssid_len < 64 && pass_len < 64) {
            char ssidSafe[64], passSafe[64];
            memcpy(ssidSafe, data, ssid_len);
            ssidSafe[ssid_len] = '\0';
            memcpy(passSafe, sep + 2, pass_len);
            passSafe[pass_len] = '\0';
            connectToSTA(ssidSafe, passSafe);
          } else {
            enviarTextoParaCliente(num,
                                   "POPUP: SSID ou password demasiado longos.");
          }
        } else {
          enviarTextoParaCliente(num, "WARNIG: Separador '||' não encontrado.");
        }
        return;
      }
      if (length == 11 && strncmp(msg, "GETREDEINFO", 11) == 0) {
        mostrarInfoRedeAtual(1);
        return;
      }
    }

    // 3. Mensagem comum
    PCEsp.printf("[Cliente %u] Mensagem: %s\n", num, msg);
    if (adminId != 255 &&
        !clients[num].isAdmin) {  // Evia a mensagem do USER para o ADM.
      char buffer[256];
      snprintf(buffer, sizeof(buffer), "SYS@[Cliente %u] %s", num, msg);
      enviarTextoParaCliente(adminId, buffer);
    }
    if (clients[num].isUser || clients[num].isAdmin) {
      if (startsWithIgnoreCase(payload, length, "show din")) {
        robotValuesMV::adicionarComandoNaFila(
            robotValuesMV::listType::COMMAND_TYPES, robotValuesMV::SHOW_DIN,
            nullptr, robotValuesMV::queueStartCBL, robotValuesMV::queueEndCBL,
            robotValuesMV::MAX_NUMBER_OF_COMMANDS_IN_QUEUE);
      } else if (startsWithIgnoreCase(payload, length, "show dout")) {
        robotValuesMV::adicionarComandoNaFila(
            robotValuesMV::listType::COMMAND_TYPES, robotValuesMV::SHOW_DOUT,
            nullptr, robotValuesMV::queueStartCBL, robotValuesMV::queueEndCBL,
            robotValuesMV::MAX_NUMBER_OF_COMMANDS_IN_QUEUE);
      }
      robotValuesMV::adicionarComandoNaFila(
          robotValuesMV::listType::CHAR_PM, robotValuesMV::commandType::NONE,
          msg, robotValuesMV::queueStartPM, robotValuesMV::queueEndPM,
          robotValuesMV::MAX_PENDING_MSGS);
      /* isto é para ignorar a lista de pendingmensages
      ESPMain.write((const uint8_t *)payload, strnlen((const char *)payload,
      length));// Envia a mensagem recebida para o mainframe (ESPMain) no
      formato ASCII PCEsp.print("enviado para o mainframe (HEX): ");for (size_t
      i = 0; i < length; ++i) {if (payload[i] < 0x10)          PCEsp.print('0');
      // Adiciona zero à esquerda para valores < 0x10 PCEsp.print(payload[i],
      HEX);        PCEsp.print(' ');      }      PCEsp.println();*/
    }
  };

  // webSocket.sendPing(num);// Envia um ping ao conectar(não necessita)
  switch (type) {
    case WStype_ERROR:
      PCEsp.println("Erro na comunicação WebSocket!");
      break;
    case WStype_DISCONNECTED:
      desconectarCliente(num, "MANUAL");
      break;
    case WStype_CONNECTED:
      conectarCliente(num, webSocket.remoteIP(num));
      break;
    case WStype_TEXT:
      handleWebSocketMessage(num, payload, length);
      break;  // no maximo aguenta com (15.360 bytes) de uma vez [#define
              // WEBSOCKETS_MAX_DATA_SIZE (15 * 1024)]
    case WStype_BIN:
      PCEsp.print("Mensagem binária recebida com tamanho: ");
      PCEsp.println(length);
      break;
    case WStype_FRAGMENT_TEXT_START:
      PCEsp.println(
          "Início de fragmento de texto recebido.(parte inicial de um pedaço "
          "de texto)");
      break;
    case WStype_FRAGMENT_BIN_START:
      PCEsp.println(
          "Início de fragmento binário recebido.(parte inicial de um pedaço "
          "binário)");
      break;
    case WStype_FRAGMENT:
      PCEsp.println("Fragmento recebido.(parte intermediária de um pedaço)");
      break;
    case WStype_FRAGMENT_FIN:
      PCEsp.println("Fragmento final recebido.(parte final de um pedaço)");
      break;
    case WStype_PING:
      PCEsp.printf("Recebido PING do cliente: %u\n", num);
      break;
    case WStype_PONG:
      PCEsp.printf("Recebido PONG do cliente: %u\n",
                   num);  // Resposta de PONG recebida
      variaveisMillisMV::pongPending[num] =
          false;  // Marca o ping como respondido
      break;
    default:
      PCEsp.printf("Tipo de evento desconhecido: %u\n", type);
      break;
  }
}

/**
 * @brief Envia uma mensagem de texto para um cliente específico. Verifica se o
 * cliente está conectado e se o ponteiro da mensagem é válido antes de enviar a
 * mensagem via WebSocket.
 * @param id ID do cliente .
 * @param texto Mensagem a ser enviada (string terminada em nulo).
 */
void enviarTextoParaCliente(uint8_t id, const char *texto) {
  if (clientManagerMV::clients[id].conectado && texto) {
    webSocket.sendTXT(id, texto);
  }
}

// ativa a função que cria a tabela com os dados dos clientes, verifica se há um
// ADM
void updadeADMlist() {
  if (clientManagerMV::adminId != 255) {
    mostrarStatusClientes(clientManagerMV::adminId, 2);
  }
}

/**
 * @author M.V.
 * @date 2025-07-14
 * @version 1.0
 * @brief Verifica se o conteúdo do array `payload`(de tamanho 'length') começa
 * com a string `prefix`, ignorando maiúsculas/minúsculas.
 *
 * @return `True` se corresponder, `False` caso contrário.
 */
bool startsWithIgnoreCase(const uint8_t *payload, size_t length,
                          const char *prefix) {
  for (size_t i = 0; prefix[i] != '\0';
       ++i) {  // Itera sobre cada carácter do prefixo
    if (i >= length)
      return false;  // Se o prefixo for maior que o payload, não pode haver
                     // correspondência

    // Lê os caracteres correspondentes do payload e do prefixo
    char c1 = payload[i];
    char c2 = prefix[i];

    // Converte ambos para minúsculas manualmente (mais leve que tolower())
    if (c1 >= 'A' && c1 <= 'Z') c1 += 'a' - 'A';
    if (c2 >= 'A' && c2 <= 'Z') c2 += 'a' - 'A';
    if (c1 != c2)
      return false;  // Se os caracteres diferirem, a string não começa com o
                     // prefixo
  }
  return true;  // Todos os caracteres do prefixo coincidem (ignorando
                // maiúsculas/minúsculas)
}

/**
 * @author M.V.
 * @date 2025-07-14
 * @version 1.0
 * @brief Desconecta um cliente e envia o motivo da desconexão. Esta função
 * desconecta o cliente especificado, envia uma mensagem com o motivo da
 * desconexão, encerra a conexão WebSocket e limpa os dados do cliente.
 *
 * @param id ID do cliente .
 * @param reason Motivo da desconexão (string terminada em nulo).
 */
void desconectarCliente(uint8_t id, const char *reason) {
  if (id < WEBSOCKETS_SERVER_CLIENT_MAX) {
    // Se o cliente era administrador, decrementa o contador
    if (clientManagerMV::clients[id].isAdmin &&
        clientManagerMV::adminCount > 0) {
      clientManagerMV::adminCount--;
      clientManagerMV::adminId = 255;  // Nenhum administrador conectado
      PCEsp.printf("AdminCount decrementado. Novo valor: %u\n",
                   clientManagerMV::adminCount);
    }
    // se o cliente era utilizador, decrementa o contador
    if (clientManagerMV::clients[id].isUser && clientManagerMV::usercount > 0) {
      clientManagerMV::usercount--;
      clientManagerMV::userId = 255;  // Nenhum usuário conectado
      PCEsp.printf("UserCount decrementado. Novo valor: %u\n",
                   clientManagerMV::usercount);
    }

    // Envia mensagem com o motivo da desconexão (se ainda está ligado)
    if (webSocket.remoteIP(id)[0] != 0) {
      const char *prefixo = "DISCONNECTEDBY";
      char mensagem[25];
      snprintf(mensagem, sizeof(mensagem), "%s%s", prefixo, reason);
      enviarTextoParaCliente(id, mensagem);
    }
    PCEsp.printf("O cliente %u foi desconectado pelo motivo: %s\n", id,
                 reason);  // Exibe a mensagem de desconexão no console

    webSocket.disconnect(id);  // Encerra a conexão WebSocket do cliente
    delay(5);  // Espera 1 ciclo para garantir encerramento (opcional, mas pode
               // prevenir race conditions)

    // Reinicializa a estrutura do cliente
    clientManagerMV::clients[id] = clientManagerMV::Cliente();
    variaveisMillisMV::pongPending[id] = false;

    updadeADMlist();  // Atualiza a lista do administrador, se houver
  }
}

/**
 * @author M.V.
 * @date 2025-07-14
 * @version 1.0
 * @brief Função para verificar e desconectar clientes que ultrapassaram o tempo
 * máximo de sessão. Esta função verifica periodicamente se algum cliente
 * conectado excedeu o tempo máximo de sessão. Se isso ocorrer, o cliente é
 * desconectado automaticamente com o motivo "TIMEOUT".
 *
 * @note A função usa millis() para monitorar o tempo desde o início do
 * programa.
 * @see desconectarCliente()
 */
void verificarTimeouts() {
  for (int i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++) {
    if (clientManagerMV::clients[i].conectado) {
      // Verifica se o tempo de conexão ultrapassou o limite máximo
      if (variaveisMillisMV::agora -
              clientManagerMV::clients[i].startConnectionMillis >
          variaveisMillisMV::MAX_SESSION_TIME) {
        // PCEsp.printf("O cliente %u atingiu o tempo máximo de sessão.\n", i);
        desconectarCliente(
            i, "TIMEOUT");  // Desconecta o cliente após o tempo limite
      }
    }
  }
}

/**
 * @author M.V.
 * @date 2025-07-14
 * @version 1.0
 * @brief Verifica se os clientes estão realmente conectados via ping. Envia
 * pings a cada 20 segundos para clientes conectados. Se não houver resposta
 * (pong) em até 10 segundos, o cliente é desconectado com o motivo "NO_PONG".
 *
 * @details
 * - Atualiza `lastPingTime` e `lastPingMillis` de cada cliente.
 * - Marca pings pendentes com `pongPending[]`.
 * @note Chamado a cada 10s no loop principal. Não deve ser executado
 * diretamente no loop.
 * @see webSocket.sendPing(), desconectarCliente()
 */
void clientsTrullyConected() {
  // Envia pings a cada 20 segundos
  if (variaveisMillisMV::agora - variaveisMillisMV::lastPingTime >=
      variaveisMillisMV::pingInterval) {
    variaveisMillisMV::lastPingTime = variaveisMillisMV::agora;
    for (int i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++) {
      if (clientManagerMV::clients[i].conectado) {
        webSocket.sendPing(i);                     // Envia ping para o cliente
        variaveisMillisMV::pongPending[i] = true;  // Marca o ping como pendente
        clientManagerMV::clients[i].lastPingMillis =
            variaveisMillisMV::agora;  // Atualiza o tempo do último ping
      }
    }
  }

  // Verifica se algum cliente não respondeu ao ping dentro de 10 segundos
  for (int i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++) {
    if (clientManagerMV::clients[i].conectado &&
        variaveisMillisMV::pongPending[i] &&
        (variaveisMillisMV::agora - clientManagerMV::clients[i].lastPingMillis >
         variaveisMillisMV::pongTimeout)) {
      // PCEsp.printf("Cliente %u não respondeu ao ping. Desconectando...\n",
      // i);
      desconectarCliente(i, "NO_PONG");           // Desconecta o cliente
      variaveisMillisMV::pongPending[i] = false;  // Reseta o estado do ping
    }
  }
}

/**
 * @author M.V.
 * @date 2025-07-14
 * @version 1.0
 * @brief Mostra o status de todos os clientes conectados.
 *
 * @param id ID do cliente (utilizado apenas quando o tipo de destino é 2, para
 * enviar a mensagem via WebSocket).
 * @param target Tipo de destino:
 * @p 1: Envia o status para o Serial PC.
 * @p 2: Envia o status para o administrador via WebSocket.
 * @example
 *  `mostrarStatusClientes(255, 1);`  // Envia para o Serial PC
 *  `mostrarStatusClientes(1, 2);`    // Envia para o cliente ID 1 via WebSocket
 */
void mostrarStatusClientes(uint8_t id, uint8_t target) {
  using namespace clientManagerMV;
  constexpr size_t MAX_CLIENTS = WEBSOCKETS_SERVER_CLIENT_MAX;
  constexpr size_t CLIENT_INFO_LEN = 85;
  constexpr size_t BUFFER_TOTAL = 680;

  /**
   * @brief Formata o status de um cliente em uma string.
   * @param client Referência constante para o cliente a ser formatado. Usar
   * referência garante acesso eficiente e evita a necessidade de checar
   * ponteiro nulo.
   * @param buffer Buffer de destino onde a string formatada será armazenada.
   * @param bufferSize Tamanho máximo do buffer fornecido (inclui o caractere
   * nulo).
   * @details
   * A string gerada inclui: ID, IP, status de admin/user, nome e tempo de
   * conexão (hh:mm:ss). Exemplo de saída:
   * "ID:1|192.168.1.10|Sim|Não|João|00:12:34\n"
   */
  auto formatarStatusCliente = [](const clientManagerMV::Cliente &client,
                                  char *buffer, size_t bufferSize) {
    unsigned long tempoConexao = millis() - client.startConnectionMillis;
    unsigned long horas = tempoConexao / 3600000;
    unsigned long minutos = (tempoConexao % 3600000) / 60000;
    unsigned long segundos = (tempoConexao % 60000) / 1000;
    snprintf(buffer, bufferSize, "ID:%u|%s|%s|%s|%s|%02lu:%02lu:%02lu\n",
             client.id, client.ip.toString().c_str(),
             client.isAdmin ? "Sim" : "Não", client.isUser ? "Sim" : "Não",
             client.nome, horas, minutos, segundos);
  };

  switch (target) {
    case 1: {  // Serial PC
      PCEsp.println(F("---- Status dos Clientes ----"));
      PCEsp.println(F("ID | IP | Admin | User | Nome | Conectado à:"));
      for (uint8_t i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].conectado) continue;
        char linha[CLIENT_INFO_LEN];
        formatarStatusCliente(clients[i], linha, sizeof(linha));
        PCEsp.print(linha);
      }
      PCEsp.println(F("-------------------------------"));
      break;
    }
    case 2: {  // Enviar para ADM
      char mensagemTotal[BUFFER_TOTAL] = {0};
      size_t mensagemLen = 0;

      for (uint8_t i = 0; i < MAX_CLIENTS; i++) {
        ledRGB(0, 0, 100);
        if (!clients[i].conectado) {
          ledRGB(0, 0, 0);
          continue;
        }

        char clienteInfo[CLIENT_INFO_LEN];
        formatarStatusCliente(clients[i], clienteInfo, sizeof(clienteInfo));
        size_t infoLen = strlen(clienteInfo);
        if (mensagemLen + infoLen >= BUFFER_TOTAL) {
          enviarTextoParaCliente(id, mensagemTotal);
          mensagemLen = 0;
          mensagemTotal[0] = '\0';
        }
        memcpy(mensagemTotal + mensagemLen, clienteInfo, infoLen);
        mensagemLen += infoLen;
        mensagemTotal[mensagemLen] = '\0';
        ledRGB(0, 0, 0);
      }
      if (mensagemLen > 0) {
        enviarTextoParaCliente(id, mensagemTotal);
      }
      break;
    }
  }
}

/**
 * @author M.V.
 * @date 2025-07-14
 * @version 1.0
 * @brief Serve para conectar a uma rede sta.
 *
 * @attention Esta função ao ser chamada vai parar o servidor.
 * @param ssid Nome da rede.
 * @param password Password da rede.
 * @note HotFix: Se não conseguir ligar à rede, o servidor reinicia e volta a
 * funcionar normalmente.
 *
 * @bug Se não conseguir ligar à rede sta, o servidor deixa de responder aos
 * clientes por wireless.
 * @details
 *  Se conseguir ligar à rede, o servidor continua a funcionar normalmente e os
 * clientes conectados ao AP continuam a funcionar.
 */
void connectToSTA(const char *ssid, const char *password) {
  if (!ssid || !password) {
    enviarTextoParaCliente(clientManagerMV::adminId,
                           "WARNING:SSID ou password nulos.");
    return;
  }
  // Transfere os dados dos ponteiros para arrays seguros.
  char ssidSafe[64], passSafe[64];
  strncpy(ssidSafe, ssid, sizeof(ssidSafe) - 1);
  ssidSafe[sizeof(ssidSafe) - 1] = '\0';
  strncpy(passSafe, password, sizeof(passSafe) - 1);
  passSafe[sizeof(passSafe) - 1] = '\0';

  WiFi.mode(WIFI_AP_STA);         //  modo AP+STA
  WiFi.disconnect(false, false);  // Só desconecta STA, nunca o AP!
  delay(100);
  WiFi.begin(ssidSafe, passSafe);  // inicia a conexão com a rede WiFi STA

  PCEsp.printf("A ligar à nova rede STA: %s ...\n", ssidSafe);
  enviarTextoParaCliente(clientManagerMV::adminId,
                         "POPUP:ESP:A ligar à nova rede STA...");
  enviarTextoParaCliente(
      clientManagerMV::adminId,
      "POPUP:As informações de status irão aparecer na DevTool desta página.");

  uint32_t start = millis();
  bool connected = false;
  // O tempo máximo de espera no while deve ser menor que
  // variaveisMillisMV::pongTimeout.(para não desconectar clientes por falta de
  // pong)
  while (WiFi.status() != WL_CONNECTED &&
         millis() - start < (variaveisMillisMV::pongTimeout - 4500UL)) {
    delay(480);
    yield();
    PCEsp.print(".");
  }
  connected =
      (WiFi.status() == WL_CONNECTED);  // obtem o valor bool de uma expressão.

  if (connected) {
    ledRGB(0, 50, 0);
    char avisoLigacaoSTA[128];
    snprintf(avisoLigacaoSTA, sizeof(avisoLigacaoSTA),
             "WARNING: Conectado a %s com sucesso! IP (STA): %s",
             WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
    PCEsp.println(avisoLigacaoSTA);
    enviarTextoParaCliente(clientManagerMV::adminId, avisoLigacaoSTA);
  }
  if (!connected) {
    PCEsp.println(F("\n❌Timeout excedido para conexão à rede STA."));
    enviarTextoParaCliente(
        clientManagerMV::adminId,
        "WARNING:Não foi possível ligar à rede.O servidor vai reniciar, "
        "problemas com a rede serão resolvidos.  ");
    ledRGB(50, 50, 0);
    PCEsp.println(F("Reiniciando ESP para restaurar o AP ..."));
    delay(1000);
    ESP.restart();
  }
}

/**
 * @author M.V.
 * @date 2025-07-14
 * @version 1.0
 * @brief Manda infromação sobre os modos de rede do ESP.
 *
 * @param target (0 = Serial | 1 = WebSocket) */
void mostrarInfoRedeAtual(uint8_t target) {
  switch (target) {
    case 0: {  // Serial
      PCEsp.println(F("📶 Informações da ligação de rede actual:\n"));
      if (WiFi.getMode() & WIFI_STA && WiFi.isConnected()) {
        PCEsp.println(F("🟢 Ligado como STA (cliente):"));
        PCEsp.printf("    SSID: %s\n", WiFi.SSID().c_str());
        PCEsp.printf("    BSSID: %s\n", WiFi.BSSIDstr().c_str());
        PCEsp.printf("    RSSI: %d dBm\n", WiFi.RSSI());
        PCEsp.printf("    Canal: %d\n", WiFi.channel());
        PCEsp.printf("    IP Local: %s\n", WiFi.localIP().toString().c_str());
        PCEsp.printf("    Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
        PCEsp.printf("    DNS: %s\n", WiFi.dnsIP().toString().c_str());
        PCEsp.printf("    MAC do ESP: %s\n", WiFi.macAddress().c_str());
      } else {
        PCEsp.println(F("🔴 Não está ligado a nenhuma rede como STA."));
      }

      if (WiFi.getMode() & WIFI_AP) {
        PCEsp.println(F("\n📡 A funcionar como ponto de acesso (AP):"));
        PCEsp.printf("    SSID AP: %s\n", WiFi.softAPSSID().c_str());
        PCEsp.printf("    IP do AP: %s\n", WiFi.softAPIP().toString().c_str());
        PCEsp.printf("    MAC AP: %s\n", WiFi.softAPmacAddress().c_str());
      }
      break;
    }
    case 1: {  // WebSocket
      char infoWiFi[400];
      infoWiFi[0] = '\0';

      if (WiFi.getMode() & WIFI_STA && WiFi.isConnected()) {
        snprintf(infoWiFi + strlen(infoWiFi),
                 sizeof(infoWiFi) - strlen(infoWiFi),
                 "WARNING: Ligado como STA (cliente)\nSSID: %s\nBSSID: "
                 "%s\nRSSI: %d dBm\nCanal: %d\nIP Local: %s\nGateway: %s\nDNS: "
                 "%s\nMAC do ESP: %s\n",
                 WiFi.SSID().c_str(), WiFi.BSSIDstr().c_str(), WiFi.RSSI(),
                 WiFi.channel(), WiFi.localIP().toString().c_str(),
                 WiFi.gatewayIP().toString().c_str(),
                 WiFi.dnsIP().toString().c_str(), WiFi.macAddress().c_str());
      } else {
        snprintf(infoWiFi + strlen(infoWiFi),
                 sizeof(infoWiFi) - strlen(infoWiFi),
                 "WARNING: Não está ligado a nenhuma rede como STA.\n");
      }

      if (WiFi.getMode() & WIFI_AP) {
        snprintf(infoWiFi + strlen(infoWiFi),
                 sizeof(infoWiFi) - strlen(infoWiFi),
                 "WARNING: A funcionar como ponto de acesso (AP)\nSSID AP: "
                 "%s\nIP do AP: %s\nMAC AP: %s\n",
                 WiFi.softAPSSID().c_str(), WiFi.softAPIP().toString().c_str(),
                 WiFi.softAPmacAddress().c_str());
      }

      enviarTextoParaCliente(clientManagerMV::adminId, infoWiFi);
      break;
    }
  }
}
/*
void listarRedesWiFi() {
  PCEsp.println(F("🔍 A procurar redes Wi-Fi próximas..."));
  int numRedes = WiFi.scanNetworks(false, true);  // async=false,
show_hidden=true

  if (numRedes == 0) {
    PCEsp.println(F("❌ Nenhuma rede encontrada."));
  }
  else {
    PCEsp.printf("✅ %d redes encontradas:\n\n", numRedes);

    for (int i = 0; i < numRedes; i++) {
      String ssid = WiFi.SSID(i);
      String bssid = WiFi.BSSIDstr(i);
      int rssi = WiFi.RSSI(i);
      int canal = WiFi.channel(i);
      wifi_auth_mode_t auth = WiFi.encryptionType(i);

      // Banda e frequência
      const char *banda = (canal > 13) ? "5 GHz" : "2.4 GHz";
      int freqMHz = (canal <= 14) ? (2407 + canal * 5) : (5000 + canal * 5);

      PCEsp.printf("📡 [%d] SSID: %s\n", i + 1, ssid.c_str());
      PCEsp.printf("    BSSID: %s\n", bssid.c_str());
      PCEsp.printf("    RSSI: %d dBm\n", rssi);
      PCEsp.printf("    Canal: %d (%s)\n", canal, banda);
      PCEsp.printf("    Frequência: %d MHz\n", freqMHz);
      PCEsp.printf("    Encriptação: %s\n\n", traduzirEncriptacao(auth));
    }
  }
  WiFi.scanDelete();
}
const char *traduzirEncriptacao(wifi_auth_mode_t tipo) {
  switch (tipo) {
    case WIFI_AUTH_OPEN:
      return "Nenhuma";
    case WIFI_AUTH_WEP:
      return "WEP";
    case WIFI_AUTH_WPA_PSK:
      return "WPA";
    case WIFI_AUTH_WPA2_PSK:
      return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK:
      return "WPA/WPA2";
    case WIFI_AUTH_WPA2_ENTERPRISE:
      return "WPA2 Enterprise";
    case WIFI_AUTH_WPA3_PSK:
      return "WPA3";
    case WIFI_AUTH_WPA2_WPA3_PSK:
      return "WPA2/WPA3";
    case WIFI_AUTH_WAPI_PSK:
      return "WAPI";
    default:
      return "Desconhecido";
  }
}
*/

// mudar o tema do VS Code faz CTRL+K logo a seguir CTRL+T
/*
Tipos básicos em C/C++ (tamanhos e intervalos típicos):

int8_t       : 1 byte            : -128 a 127                  (inteiro com
sinal 8 bits) uint8_t      : 1 byte            : 0 a 255 (inteiro sem sinal 8
bits) char         : 1 byte            : -128 a 127 (signed) ou 0 a 255
(unsigned) dependendo do compilador int16_t      : 2 bytes           : -32.768
a 32.767            (inteiro com sinal 16 bits) uint16_t     : 2 bytes : 0
a 65.535                  (inteiro sem sinal 16 bits) short        : 2 bytes :
geralmente igual a int16_t int          : 4 bytes           : -2.147.483.648
a 2.147.483.647 (inteiro com sinal 32 bits) unsigned int : 4 bytes           : 0
a 4.294.967.295           (inteiro sem sinal 32 bits) long         : 4 ou 8
bytes (depende da plataforma) int64_t      : 8 bytes           :
-9.223.372.036.854.775.808 a 9.223.372.036.854.775.807 (inteiro com sinal 64
bits) uint64_t     : 8 bytes           : 0 a 18.446.744.073.709.551.615 (inteiro
sem sinal 64 bits) float        : 4 bytes           : ± ~3.4×10³⁸ (6-7 dígitos
de precisão) (ponto flutuante simples) double       : 8 bytes           : ±
~1.8×10³⁰⁸ (15 dígitos de precisão)   (ponto flutuante dupla) long double  : 8,
12 ou 16 bytes (depende da plataforma e compilador) (precisão estendida) bool :
1 byte            : true (1) ou false (0)        (valor lógico) size_t       : 4
ou 8 bytes (depende da arquitetura)  (tipo para tamanhos e índices, sempre
positivo)

Observações:
- O tamanho exato pode variar conforme arquitetura e compilador.
- Tipos fixos como int8_t, uint16_t etc. garantem tamanho definido, incluídos em
<stdint.h> ou <cstdint>.
- Use unsigned para valores sempre positivos.
- Para tamanhos e índices, prefira usar size_t.

"volatile"
A palavra-chave volatile informa ao compilador que a variável pode ser
modificada a qualquer momento fora do fluxo do programa atual, por exemplo: por
uma interrupção (ISR); por uma thread diferente (em sistemas com RTOS); por
acesso direto ao hardware (registradores); por DMA (acesso direto à memória).
Quando uma variável é volatile, o compilador não pode otimizá-la (por exemplo,
armazenar o valor em registradores ou assumir que não muda entre leituras
consecutivas).
*/