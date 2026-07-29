#define ESPMain Serial1 // serial1 é a que vai ligar à mainframe.

#define XON 0x11  // Represents the byte "DC1".
#define XOFF 0x13 // Represents the byte "DC3".

#define RxSerial1 18 // para uart nos pinos, é necessário iundicar os pinos.
#define TxSerial1 17

// Control Var, is connceted with the XON\XOFF protocoll (XON -> var = 1,XOFF -> var = 0)
bool canSendCommands = true;
