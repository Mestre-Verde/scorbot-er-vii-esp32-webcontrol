// ❕this file still remains here because it is easier to program here than in index.html❕

let isADM = false;
let isUSER = false;
let bc = new BroadcastChannel("terminal_channel"); // Communication channel between tabs

// Place for function definitions
const functions = {
  popup() { },
  GETADM(number) { },
}

/*-------------------------------
auxiliary functions.
-------------------------------*/
/** Download a file with commands */
function downloadCommands() {
  if (!websocket.isWebSocketOpen()) { return; }

  if (hasRole()) { // ADM ou USER
    fetch("/get-comandos")
      .then((response) => {
        if (!response.ok) throw new Error("Resposta inválida");
        return response.blob();
      })
      .then((blob) => {
        const url = URL.createObjectURL(blob);
        const a = document.createElement("a");
        a.href = url;
        a.download = "comandos.md";
        a.click();
        URL.revokeObjectURL(url);
      })
      .catch((error) => {
        console.error("Erro ao obter comandos.md:", error);
        popup("Erro ao descarregar o ficheiro.");
      });
  }
}

/**
 * @brief Define client's rule.
 *
 * @param {'ADM'|'USER'|'R_USER'|null} role - The rule to apply. Use null to remove any rule.
 * @example
 * setRole('ADM'); // Update the corrent rule to ADM
 * setRole(null);  // Remove the rules
 */
function setRole(role) {
  switch (role) {
    case "ADM": isADM = true; break;
    case "USER": isUSER = true; break;
    case "R_USER": isUSER = false; break;
    case null:
      isADM = false;
      isUSER = false;
      break;
  }
}

/**
 * @brief Check if the client have a rule(ADMIN ou USER).
 * @returns {boolean}
 */
function hasRole() { if (isADM || isUSER) { return true; } else { popup("Não tens permissão para aceder a essa função."); return false; } }

function sendNewSTA(ssidInput, passwordInput) {
  if (!websocket.isWebSocketOpen()) { return; }

  const ssid = ssidInput.value.trim();
  const password = passwordInput.value.trim();

  if (hasRole()) {
    if (!ssid || !password) {
      popup("Não pode enviar com um campo vazio.");
      return;
    }
    if (ssid.length < 3 || password.length < 6) {
      popup("SSID deve ter pelo menos 3 caracteres e a password pelo menos 6.");
      return;
    }
    const message = `NEWSTA:${ssid}||${password}`;
    websocket.socket.send(message);
    ssidInput.value = "";
    passwordInput.value = "";
  }
}

/**Function to send the name of the client */
function sendName() {
  const clientName = document.getElementById("clientName");
  if (!clientName) {
    popup("Erro: não foi possível encontrar o campo de nome.");
    return;
  }
  if (!websocket.isWebSocketOpen()) { return; }
  const name = clientName.value.trim();
  if (name.length === 0) {
    popup("Por favor, insira o seu nome.");
    clientName.focus();
    return;
  }

  websocket.socket.send(`NOME:${name}`);
  popup("Nome enviado.");

  clientName.value = "";
}

/** Function to send the password to get the ADM rule */
function GETADM(string) {
  if (!websocket.isWebSocketOpen()) { return; }
  const ADMPassword = document.getElementById("ADMPassword");
  switch (string) {
    case 'send':
      const ADMPasswordTrim = ADMPassword.value.trim();
      if (ADMPasswordTrim.length === 0) {
        popup("Password inválida. Por favor, introduza a password de administrador.");
        return;
      }
      websocket.socket.send("ADMPASS:" + ADMPasswordTrim);
      break;
    case 'ok':
      setRole("ADM");
      popup("Acesso de administrador concedido.");
      document.getElementById("admin-environment").style.display = "block";
      if (ADMPassword) ADMPassword.value = "";
      break;
    case 'fail':
      popup("Palavra-passe de administrador incorreta.");
      if (ADMPassword) ADMPassword.value = "";
      break;
    case 'full':
      popup("Já existe 1 administrador.");
      if (ADMPassword) ADMPassword.value = "";
      break;
    default:
      console.log("Erro ao defenir 'number' na function GETADM()");
      break;
  }

}

/** Send the string "CLIENTINFO" to the ESP for it to respond with the client information */
function updateClientTable() {
  const command = "CLIENTINFO";
  if (!websocket.isWebSocketOpen()) { return; }
  websocket.socket.send(command);
  console.log("Comando CLIENTINFO enviado para o ESP.");
}

/*-------------------------------
HTML messing functions
-------------------------------*/
// Update the WebSocket state in the websocket-panel(html).
function updateConnectionState() {
  const connectionStateElement = document.getElementById("connection-state");
  if (!websocket.socket || websocket.socket.readyState === WebSocket.CLOSED) {
    connectionStateElement.textContent = "Sem ligação.";
    return;
  }
  switch (websocket.socket.readyState) {
    case WebSocket.CONNECTING:
      connectionStateElement.textContent = "A conectar...";
      break;
    case WebSocket.OPEN:
      connectionStateElement.textContent = "Ligação aberta.";
      break;
    case WebSocket.CLOSING:
      connectionStateElement.textContent = "A fechar...";
      break;
    case WebSocket.CLOSED:
      connectionStateElement.textContent = "Ligação fechada.";
      break;
    default:
      connectionStateElement.textContent = "Estado desconhecido.";
  }
}

/**
 * @brief  Define the state of a LED in the mainframe. Turn on or off a specific LED of the input or output.
 * @param {string} ioName - O nome do IO (ex: "in1", "out2").
 * @param {boolean} isActive - true para ativar o LED, false para desativar.
 * @example
 * setLedState('in1', true); // Turn On the LED "in1".
 * setLedState('out2', false); //  Turn Off the LED "out2".
 * */
function setLedState(ioName, isActive) {
  const led = document.querySelector(`.led[data-io="${ioName}"]`);
  if (!led) return;
  if (isActive) {
    led.classList.add("active");
  } else {
    led.classList.remove("active");
  }
}

// Function to show the WebSocket container.Displays the WebSocket connection panel in the interface.
function showConnectionPanel() { document.getElementById("websocket-container").classList.remove("hidden"); }
//Function to hide the WebSocket container. It hides the WebSocket connection panel in the interface.
function closeConnectionPanel() { document.getElementById("websocket-container").classList.add("hidden"); }

/*-------------------------------
Body related functions
-------------------------------*/
// Main title animation
const text = "⚙️Controlo do Robô Scorbot-ER VII⚙️ ";
const title = document.querySelector("#title");
let letterIndex = 0;
let deleted = false;
function writeMainTitle() {
  if (!deleted) {
    if (letterIndex <= text.length) {
      title.textContent = text.substring(0, letterIndex);
      letterIndex++;
      setTimeout(writeMainTitle, 150);
    } else {
      setTimeout(() => {
        deleted = true;
        writeMainTitle();
      }, 10000);
    }
  } else {
    if (letterIndex >= 0) {
      title.textContent = text.substring(0, letterIndex);
      letterIndex--;
      setTimeout(writeMainTitle, 100);
    } else {
      deleted = false;
      setTimeout(writeMainTitle, 3000);
    }
  }
}

const LedHrModule = {
  leds: [],

  setup: function () {
    this.leds = document.querySelectorAll(".ledT");
  },

  loop: function () {
    this.leds.forEach(led => {
      const isOn = led.classList.contains("on");
      if (Math.random() > 0.5) {
        led.classList.toggle("on", !isOn);
      }
    });
  }
};

/**
 * @brief Displays a popup message in the upper right corner of the screen.
 * @detail This function creates a temporary visual element to display feedback messages to the user,
 *  such as errors, information, or notifications. Popups disappear automatically after 10 seconds.
 *  Only 3 popups can show at a time,if a new is created then it will overwrite the old popup.
 * @param {string} message - The text to show in the popup.
 * @example
 * popup("Conexão estabelecida com sucesso.");
 * popup(`Erro no WebSocket: ${error}`);
 * @note This function had the goal to replace the `alert()` function.
 * @see
 * - A classe CSS `.popup`.
 * - A classe CSS `.hide`.
 */
function popup(message) {
  // Check if there is already 3 popups
  const existingPopups = document.querySelectorAll(".popup");
  if (existingPopups.length >= 3) {
    existingPopups[0].remove();
  }

  // Add a new popup
  const popupElement = document.createElement("div");
  popupElement.className = "popup";
  popupElement.textContent = message;

  // Add the popup in the boby of the index.html
  document.body.appendChild(popupElement);

  // Recalculate the positions of all popups.
  const updatedPopups = document.querySelectorAll(".popup");
  updatedPopups.forEach((popup, index) => {
    popup.style.top = `${20 + index * 60}px`; // 20px initial margin + 60px per popup
  });

  // Remove the popup after 10 seconds
  setTimeout(() => {
    popupElement.classList.add("hide");
    setTimeout(() => {
      popupElement.remove();
      // RRecalculate again the positions
      const remainingPopups = document.querySelectorAll(".popup");
      remainingPopups.forEach((popup, index) => {
        popup.style.top = `${20 + index * 60}px`;
      });
    }, 500); // Time for the animations of removing the popup.
  }, 10000);
}
/*-------------------------------
footer
-------------------------------*/
async function updateFooterRepoInfo() {
  const repoOwner = "Mestre-Verde";
  const repoName = "scorbot-er-vii-esp32-webcontrol";
  try {
    // 1. Last release 
    const releaseRes = await fetch(`https://api.github.com/repos/${repoOwner}/${repoName}/releases/latest`);
    if (releaseRes.ok) {
      const releaseData = await releaseRes.json();
      document.getElementById("projectVersion").innerText = releaseData.tag_name;
    }
    // 2. Last branch main committed
    const commitRes = await fetch(`https://api.github.com/repos/${repoOwner}/${repoName}/commits/main`);
    if (commitRes.ok) {
      const commitData = await commitRes.json();
      const date = new Date(commitData.commit.committer.date);
      document.getElementById("lastProjectUpdate").innerText = date.toLocaleDateString();
    }
  } catch (err) {
    console.error("Erro ao buscar informações do repositório:", err);
  }
}

const robotValuesHTTP = {
  autoUpdateInterval: null,//  Var to store the update interval ID

  /** function to change the style of the button and to set the requisition interval */
  toggleAutoUpdate(updateInterval = 5000) {
    const button = document.getElementById("toggleAutoUpdate");
    if (this.autoUpdateInterval !== null) {
      clearInterval(this.autoUpdateInterval);
      this.autoUpdateInterval = null;
      button.textContent = "Tabelas & I/O 🔄️";
      popup("Atualização automática desativada.");
    } else {
      this.autoUpdateInterval = setInterval(() => {
        this.downloadValuesFromHTTP();
      }, updateInterval);
      button.textContent = "Tabelas & I/O 🛑";
      popup("Atualização automática ativada.");
    }
  },

  /** Download the values from the HTTP page /estado.*/
  async downloadValuesFromHTTP() {
    try {
      const stateAnswer = await fetch("/estado");
      if (!stateAnswer.ok) throw new Error("Erro ao obter dados");

      const rawText = await stateAnswer.text();
      const parts = rawText.split(";");
      if (parts.length < 3) throw new Error("Resposta inválida do servidor");

      const coordenadas = parts[0].split(",").map(Number);
      const encoders = parts[1].split(",").map(Number);
      const base64IO = parts[2];

      this.updateCoordinateTable(coordenadas);
      this.updateEncoderTable(encoders);

      // Decodificar os 4 bytes dos IOs
      const ioData = atob(base64IO);
      if (ioData.length !== 4) throw new Error("Dados binários inválidos");

      const inState = (ioData.charCodeAt(0) << 8) | ioData.charCodeAt(1);
      const outState = (ioData.charCodeAt(2) << 8) | ioData.charCodeAt(3);

      const inStateArray = Array.from({ length: 16 }, (_, i) => (inState >> (15 - i)) & 1);
      const outStateArray = Array.from({ length: 16 }, (_, i) => (outState >> (15 - i)) & 1);

      this.updateInputState(inStateArray);
      this.updateOutputState(outStateArray);
    }
    catch (err) {
      popup(`Erro: ${err.message}`);
    }
  },

  // Update the LED input state in the mainframe.
  updateInputState(inStateArray) {
    for (let i = 0; i < inStateArray.length; i++) {
      const ioName = `in${i + 1}`; // "in1", "in2", ..., "in16"
      const isActive = inStateArray[i] === 1;
      setLedState(ioName, isActive);
    }
  },
  // Update the LED output state in the mainframe.
  updateOutputState(outStateArray) {
    for (let i = 0; i < outStateArray.length; i++) {
      const ioName = `out${i + 1}`; // "out1", "out2", ..., "out16"
      const isActive = outStateArray[i] === 1;
      setLedState(ioName, isActive);
    }
  },

  // Flash animation for the values that are diferent from the previous one.
  updateCellIfChanged(cell, newValue) {
    if (cell.textContent !== String(newValue)) {
      cell.textContent = newValue;
      cell.classList.add("updated");
      setTimeout(() => cell.classList.remove("updated"), 500);
    }
  },

  // Function to update the values of the coordenates.
  updateCoordinateTable(values) {
    const table = document.getElementById("coordinateValues");
    if (!table) {
      console.error("Tabela de coordenadas não encontrada no DOM.");
      return;
    }
    const row = table.querySelector("tbody tr");
    if (row) {
      values.forEach((valor, index) => {
        this.updateCellIfChanged(row.cells[index], valor);
      });
    }
  },

  // Function to update the values of the encoders.
  updateEncoderTable(values) {
    const table = document.getElementById("encoderValues");
    if (!table) {
      console.error("Tabela de encoders não encontrada no DOM.");
      return;
    }
    const row = table.querySelector("tbody tr");
    if (row) {
      values.forEach((valor, index) => {
        this.updateCellIfChanged(row.cells[index], valor);
      });
    }

  },
};

const ADMClientTable = {
  /**
 * Processes the list of clients received from the server.
 * @param {string} clientInfo - The message containing client data.
 */
  processClientInfo(clientInfo) {
    const clientRows = clientInfo.trim().split("\n");
    for (let i = 0; i < 8; i++) {
      this.clearRow(i);
    }
    clientRows.forEach((row) => {
      const parts = row.split("|");
      const client = {
        id: parseInt(parts[0].split(":")[1]),
        ip: parts[1],
        admin: parts[2],
        user: parts[3],
        name: parts[4],
        connectionTime: parts[5],
      };
      this.updateRow(client, client.id);
    });
  },

  /**
  * Clears the data of a specific row in the table.
  * @param {number} index - The index of the row to clear (0 to 7).
  */
  clearRow(index) {
    document.getElementById(`cliente-${index}-id`).textContent = "";
    document.getElementById(`cliente-${index}-ip`).textContent = "";
    document.getElementById(`cliente-${index}-admin`).textContent = "";
    document.getElementById(`cliente-${index}-user`).textContent = "";
    document.getElementById(`cliente-${index}-name`).textContent = "";
    document.getElementById(`cliente-${index}-connectionTime`).textContent = "";
  },

  /**
   * Updates the data of a client in the table.
   * @param {Object} client - Object containing the client's data.
   * @param {number} index - The index of the row in the table.
   */
  updateRow(cliente, index) {
    if (index < 0 || index >= 8) {
      popup(`Índice inválido: ${index}. Deve estar entre 0 e 7.`);
      return;
    }
    document.getElementById(`cliente-${index}-id`).textContent = cliente.id;
    document.getElementById(`cliente-${index}-ip`).textContent = cliente.ip;
    document.getElementById(`cliente-${index}-admin`).textContent = cliente.admin;
    document.getElementById(`cliente-${index}-user`).textContent = cliente.user;
    document.getElementById(`cliente-${index}-name`).textContent = cliente.name;
    document.getElementById(`cliente-${index}-connectionTime`).textContent = cliente.connectionTime;
  },

  /**
   * Called to define a client as a USER.
   * @param {number} clienteIndex
   */
  setUser(clienteIndex) {
    if (!websocket.isWebSocketOpen()) return;
    if (isADM) {
      const command = `SETUSER:${clienteIndex}`;
      websocket.socket.send(command);
      console.log(`Comando SETUSER enviado para o cliente ${clienteIndex}`);
    }
    else {
      popup("Apenas o ADM pode adicionar cargos.");
    }
  },

  /**
   * Called to remove the USER role from a client
   * @param {number} clienteIndex
   */
  removeUser(clienteIndex) {
    if (!websocket.isWebSocketOpen()) return;
    if (isADM) {
      const command = `REMOVEUSER:${clienteIndex}`;
      const button = document.getElementById(`btn-remove-user-${clienteIndex}`);

      if (button) button.disabled = true;
      websocket.socket.send(command);

      setTimeout(() => {
        if (button) button.disabled = false;
      }, 2000);
    }
    else {
      popup("Apenas o ADM pode remover cargos.");
    }

  },

  /**
   * Disconect a client.
   * @param {number} clienteIndex
   */
  disconnectClient(clienteIndex) {
    if (!websocket.isWebSocketOpen()) return;
    if (isADM) {
      const command = `KICKUSER:${clienteIndex}`;
      websocket.socket.send(command);
    }
    else {
      popup("Apenas o ADM pode desconectar clientes.");
    }
  }
};



const websocket = {
  socket: null,
  terminalWindow: null,

  /**Bool function to know if the websocket is active. True if connected, false if not.*/
  isWebSocketOpen(showpopup = true) {
    if (!this.socket || this.socket.readyState !== WebSocket.OPEN) {
      if (showpopup) popup("Não há um Websocket ativo. Verifique o estado da ligação");
      return false;
    }
    else { return true; }
  },

  /**
   * @brief Webscoket Main function, this functio deals with websocket events.
   * @detail
   * Treated Events:
   * - socket.onopen: executed when a connection is execute sucessefully.
   * - socket.onclose: executed when a connection is closed by either the client or the server.
   * - socket.onerror: executed when occur an error.
   * - socket.onmessage: executed when an mensage arrive.
   */
  WebSocketEvent() {
    const buttonConectWebsocket = document.getElementById("buttonConectWebsocket");

    buttonConectWebsocket.disabled = true; // Disable immedeately
    setTimeout(() => { buttonConectWebsocket.disabled = false; }, 2500);

    if (this.isWebSocketOpen(showpopup = false)) { popup("Já conectado ao WebSocket."); return; }

    this.socket = new WebSocket("ws://" + window.location.hostname + ":81");
    this.socket.onopen = () => { popup("Handshake com o Servidor iniciado."); };
    this.socket.onclose = () => {
      popup("Conexão WebSocket fechada.");
      this.socket = null;
      if (isADM) {
        for (let i = 0; i < 8; i++) {
          ADMClientTable.clearRow(i);
        }
      }
      document.getElementById("admin-environment").style.display = "none";
      setRole(null);
      updateConnectionState();
      if (this.terminalWindow && !this.terminalWindow.closed) { this.terminalWindow.close(); }
    };
    this.socket.onerror = (error) => { popup(`Erro no WebSocket: ${error}`); };
    this.socket.onmessage = this.handleMessage;
  },

  /** @brief Function to disconect a client from the server.*/
  WebSocketDisconnect() {
    if (this.socket) {
      popup("A desconectar...");
      this.socket.close();
      updateConnectionState();
    } else {
      popup("Nenhuma conexão ativa");
      updateConnectionState();
    }
  },

  /**Function to receive data from the terminal.*/
  bcOnMessage(event) {
    switch (event.data.type) {
      case "input-command": {
        const bccommand = event.data.command;
        if (this.isWebSocketOpen()) {
          this.socket.send(bccommand);
        }
        break;
      }
    }
  },

  /**
   * @brief Deal with the messages received from the server through websocket.
   * @param {MessageEvent} event - WebSocket event that contains the message.
   */
  handleMessage(event) {
    const message = event.data;
    let i = ""; // temporary stack var
    //console.log("[DEBUG]Mensagem recebida:", message); // Log para depuração

    if (message.startsWith("POPUP:")) { popup(message.substring(6)); }
    else if (message.startsWith("ID:")) { ADMClientTable.processClientInfo(message); }
    else if (message.startsWith("WARNING:")) {
      console.log(message.substring(8));
      popup("Chegou um WARNING.(F12)->Consola");
    }
    else if (message === "NOMEATUALIZADO") { popup("Nome atrualizado com sucesso!"); }
    else if (message === "DISCONNECTEDBYTIMEOUT") { popup("A sua conexão foi fechada devido ao timeout."); }
    else if (message === "DISCONNECTEDBYKICKED") { popup("Você foi desconectado pelo administrador."); }
    else if (message === "DISCONNECTEDBYNO_PONG") { popup("A tua ligação Websocket tem um problema de handshake."); }
    else if (message.startsWith("ADM")) {
      i = message.substring(3);
      if (i === "OK") { GETADM('ok'); }
      else if (i === "FAIL") { GETADM('fail') }
      else if (i === "FULL") { GETADM('full') }
    }
    else if (message.startsWith("USER")) {
      i = message.substring(4);
      if (i === "OK") {
        popup("Acesso de utilizador concedido.");
        setRole("USER");
      }
      else if (i === "FULL") { popup("Já existe 1 utilizador."); }
      else if (i === "REMOVED") {
        popup("Foi-te removido o cargo de USER.");
        setRole("R_USER");
        if (this.terminalWindow && !this.terminalWindow.closed && !isADM) {
          terminalWindow.close();
        }
      }
    }
    else if (message.startsWith("SYS@")) { bc.postMessage({ type: "system-message", data: message }); }
    else {
      // Forwards the menssage via BroadcastChannel to the terminal
      console.log("ESP-message");
      bc.postMessage({ type: "ESP-message", data: message });
    }
  }
};

//---------------------------------------------------------------------------------------------------

function setup() {
  updateFooterRepoInfo();// Update the repository information in the footer

  bc.onmessage = (event) => websocket.bcOnMessage(event);

  (document.getElementById("sendTheNewSSID")).addEventListener("click", (e) => {
    e.preventDefault();
    ssidInput = document.getElementById("ssid-name");
    passwordInput = document.getElementById("ssid-password");
    sendNewSTA(ssidInput, passwordInput);
  });

  // Click button event to open a new page with the terminal.
  (document.getElementById("toggleTerminal")).addEventListener("click", () => {
    if (!websocket.isWebSocketOpen()) { return; }
    if (hasRole()) {
      localStorage.setItem("isADM", isADM);
      localStorage.setItem("isUSER", isUSER);
      websocket.terminalWindow = window.open("/html/terminal.html", "_blank");
    }
  });

  // Button to show/hide the D-PAD 
  (document.getElementById("toggle-d-pad")).addEventListener("click", () => {
    const controlsSection = document.getElementById("controls-section");
    const buttonArows = document.getElementById("toggle-d-pad");
    controlsSection.classList.toggle("hidden");

    if (controlsSection.classList.contains("hidden")) {
      buttonArows.innerText = "🎮 SHOW";
    } else {
      buttonArows.innerText = "🎮 HIDE"
      controlsSection.scrollIntoView({ behavior: "smooth", block: "start" });
    }
  });

  // Button to update the table of robot values.
  (document.getElementById("toggleAutoUpdate")).addEventListener("click", () => {
    robotValuesHTTP.toggleAutoUpdate();
  });

  // Button to get the network status from the server.
  (document.getElementById("getNetInfo")).addEventListener("click", () => {
    if (!websocket.isWebSocketOpen) { return; }
    if (hasRole()) {
      const comando = "GETREDEINFO";
      websocket.socket.send(comando);
      console.log("Comando enviado:", comando);
    }
  });

}

function loop() {
  writeMainTitle();// Function responsible for the main title animation  

  setInterval(updateConnectionState, 500);

  setInterval(() => {// update the time in the footer every second
    const now = new Date();
    const timeStr = now.toLocaleTimeString("pt-PT");// you can remove the argument for so the js assume your local time ;)
    document.getElementById("footer-clock").textContent = timeStr;
  }, 1000);

}

function main() {
  setup();
  loop();

  LedHrModule.setup();// Initialize LEDs

  setInterval(() => LedHrModule.loop(), 1000);
}

document.addEventListener("DOMContentLoaded", main);