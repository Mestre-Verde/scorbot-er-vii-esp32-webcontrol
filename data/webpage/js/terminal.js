const bc = new BroadcastChannel("terminal_channel");
const isADM = JSON.parse(localStorage.getItem("isADM") || "false");
const isUSER = JSON.parse(localStorage.getItem("isUSER") || "false");
let emergencyStep = 0;

// Place for function definitions
const functions = {}


/*-------------------------------
Auxiliary functions.
-------------------------------*/
/** Bool function to check if client have a role. */
function hasRole() {
  if (isADM || isUSER) {
    console.log("[DEBUG] Utilizador tem cargo:", isADM ? "ADM" : "USER");
    return true;
  }
  else {
    console.warn("[DEBUG] Utilizador sem cargo - bloqueado");
    showBlockedCommandPopup("🚫 Precisas de um cargo para enviar comandos!", 4);
    return false;
  }
}

/**
 * @brief Function to show a popup in case of a blocked command or an emergency sequence.
 * @param {string} text - The text to show in the popup.
 * @param {number} time - The time in secunds that it will be visible.
 */
function showBlockedCommandPopup(text, time) {
  const popup = document.getElementById("blockedCommandPopup");
  const content = popup.querySelector(".popup-content");
  const overlay = document.getElementById("popupOverlay");
  const timeInMilisenconds = time * 1000;

  content.textContent = text;

  // Mostra popup e fundo
  popup.classList.remove("hidden");
  popup.classList.add("show");
  overlay.classList.add("show");

  setTimeout(() => {
    popup.classList.remove("show");
    popup.classList.add("hidden");
    overlay.classList.remove("show");
  }, timeInMilisenconds);
}


const broadcastDataBridge = {
  input: document.getElementById("commandInput"),
  output: document.getElementById("output"),

  /** Handle data coming from the broadcast channel */
  bcOnMessage(event) {
    const { type, data } = event.data;

    if (type === "ESP-message") {
      const message = typeof data === "string" ? data.trim() : JSON.stringify(data);
      this.output.innerText += `»» ${message}\n`;
      this.output.scrollTop = this.output.scrollHeight;
    }

    else if (type === "system-message") {
      let message = typeof data === "string" ? data.trim() : JSON.stringify(data);
      if (message.startsWith("SYS@")) { message = message.slice(4); }
      this.output.innerText += `SYS_> ${message}\n`;
      this.output.scrollTop = this.output.scrollHeight;
    }
    else {
      this.output.innerText += `SYS_> [Mensagem desconhecida: ${type}]\n`;
      this.output.scrollTop = this.output.scrollHeight;
    }
  },

  /**
   *  @brief Function to check if the command sended is fine. It allow only clients with rule  to send.
   *  @param {string} command - Command to be processed in the filter.
   */
  CommandFilter(command) {
    if (!hasRole()) { return; }
    const blacklist = new Set([
      "clr",
      "toff",
      "config"
    ]);

    if (isUSER && !isADM) {
      const cmdLower = command.trim().toLowerCase();
      for (const item of blacklist) {
        if (cmdLower.startsWith(item)) {
          showBlockedCommandPopup("🚫 Não tem permissão para enviar esse comando!", 3);
          return;
        }
      }
    }
    broadcastDataBridge.sendToIndex(command);
  },

  /**
   * @brief Send the command via BroadcastChannel.
   * @param {string} command - the command to be sended.
   */
  sendToIndex(command) {
    if (!hasRole()) { return; }
    message = {
      type: "input-command",
      command: command + "\r",
    };
    bc.postMessage(message);
    this.output.innerText += `«« ${command}\n`;
    this.output.scrollTop = this.output.scrollHeight;
  }
}


const shortcutsModule = {
  toggleShortcuts: document.getElementById("toggleShortcuts"),
  shortcuts: document.getElementById("shortcuts"),
  visibleShortcuts: true,

  showingCoodenates: false,
  ListDelayToSend: null,
  LISTPV_POSITION_delay: 2500,
  terminalPrefixes: `\n- Comando enviado:            «« comando\n- Mensagem recebida (WS):     »» resposta\n- Mensagem de sistema:        SYS_> descrição\n`,

  /**
   * @param {string} command - The data inside the shortcut button.
   */
  shortcutCommand(command) {
    const buttonToggleCoordenadas = document.getElementById("toggle-coordenates");

    switch (command) {
      case "HELP": broadcastDataBridge.sendToIndex(command); this.showInOutput(this.terminalPrefixes); break;
      case "A": broadcastDataBridge.sendToIndex(command); break;
      case "CTRL+C": broadcastDataBridge.sendToIndex("\x03"); this.showInOutput("CTRL+C enviado."); break;
      case "CON": broadcastDataBridge.sendToIndex(command); break;
      case "COFF": broadcastDataBridge.sendToIndex(command); break;
      case "LISTPV POSITION":
        if (!this.showingCoodenates) {
          this.showingCoodenates = true;
          this.ListDelayToSend = setInterval(() => {
            if (!hasRole()) { return; }
            broadcastDataBridge.sendToIndex("LISTPV POSITION");
          }, this.LISTPV_POSITION_delay);
          var delay = this.LISTPV_POSITION_delay / 1000;
          var text = `O comando LISTPV POSITION vai ser enviado a cada ${delay} segundo${delay !== 1 ? "s" : ""}.`;
          this.showInOutput(text);

          if (buttonToggleCoordenadas) {
            buttonToggleCoordenadas.textContent = "🔴 Parar coordenadas";
            buttonToggleCoordenadas.setAttribute("data-comando", "STOP LISTPV");
            buttonToggleCoordenadas.setAttribute("title", "Parar envio de coordenadas");
          }
        }
        break;
      case "STOP LISTPV":
        if (this.showingCoodenates) {
          this.showingCoodenates = false;
          clearInterval(this.ListDelayToSend);
          this.ListDelayToSend = null;
          this.showInOutput("O comando LISTPV POSITION parou de ser enviado.");
          if (buttonToggleCoordenadas) {
            buttonToggleCoordenadas.textContent = "🟢 Mostrar coordenadas";
            buttonToggleCoordenadas.setAttribute("data-comando", "LISTPV POSITION");
            buttonToggleCoordenadas.setAttribute("title", "Envia LISTPV POSITION para a minframe repetidamente");
          }
        }
        break;
      case "ECHO": broadcastDataBridge.sendToIndex(command); break;
      case "NOECHO": broadcastDataBridge.sendToIndex(command); break;
      case "clearOutputHTML": broadcastDataBridge.output.innerText = ""; break;
      default: this.showInOutput(`Comando desconhecido: ${command}`); break;
    }

  },

  showInOutput(text) {
    broadcastDataBridge.output.innerText += `SYS_> ${text}\n`;
    broadcastDataBridge.output.scrollTop = broadcastDataBridge.output.scrollHeight;
  }
}


const IOsModule = {
  switchIO: document.getElementById('switchIO'),
  ioBlocks: document.querySelectorAll('.io-block'),
  toggleIOsBtn: document.getElementById("toggleIOsBtn"),
  iosVisivel: false,

  /**Function to set listeners in all IOs buttons.*/
  addIOsButtonListeners() {
    this.ioBlocks.forEach(ioBlock => {
      const numIO = ioBlock.getAttribute('numIO');
      const buttons = ioBlock.querySelectorAll('button');

      buttons.forEach(button => {
        button.addEventListener('click', () => {
          const isOut = switchIO.classList.contains('active');
          const tipo = isOut ? 'OUT' : 'IN';
          const text = button.textContent;
          if (!hasRole()) return;
          if (text.includes('Ligar')) {
            broadcastDataBridge.sendToIndex(`disable ${tipo.toLowerCase()} ${numIO}`);
            broadcastDataBridge.sendToIndex(`force ${tipo.toLowerCase()} ${numIO} 1`);
          } else if (text.includes('Desligar')) {
            broadcastDataBridge.sendToIndex(`disable ${tipo.toLowerCase()} ${numIO}`);
            broadcastDataBridge.sendToIndex(`force ${tipo.toLowerCase()} ${numIO} 0`);
          } else if (text.includes('Reativar')) {
            broadcastDataBridge.sendToIndex(`enable ${tipo.toLowerCase()} ${numIO}`);// Reativa o I/O (retoma controlo à mainframe)
          }
        });
      });
    });
  },

  /** @brief Rewrite the text in the IOs section(output/input) */
  updateIOsText() {
    const isActive = switchIO.classList.contains('active'); // Verifica o modo atual
    const label = switchIO.querySelector('.labelIO'); // Rótulo principal do botão de modo
    label.textContent = isActive ? 'Saídas' : 'Entradas'; // Atualiza o texto do rótulo principal

    // Atualiza cada bloco de I/O
    this.ioBlocks.forEach(ioBlock => {
      const numIO = ioBlock.getAttribute('numIO'); // Número do I/O
      const lbl = ioBlock.querySelector('.labelIO'); // Rótulo do bloco
      lbl.textContent = (isActive ? 'OUT ' : 'IN ') + numIO; // Atualiza o texto do rótulo do bloco

      const buttons = ioBlock.querySelectorAll('button'); // Botões do bloco
      buttons.forEach(button => {
        // Atualiza o texto de cada botão conforme o modo
        if (button.textContent.includes('Ligar')) {
          button.textContent = `Ligar ${(isActive ? 'OUT' : 'IN')}`;
        } else if (button.textContent.includes('Desligar')) {
          button.textContent = `Desligar ${(isActive ? 'OUT' : 'IN')}`;
        } else if (button.textContent.includes('Reativar')) {
          button.textContent = `Reativar ${(isActive ? 'OUT' : 'IN')}`;
        }
      });
    });
  }
}

//---------------------------------------------------------------------------------------------------

function setup() {
  // Emergency sequence for safety
  document.addEventListener('keydown', function (event) {
    const key = event.key;
    const isTyping = ['INPUT', 'TEXTAREA'].includes(event.target.tagName);
    if (isTyping) return;
    if (key === ' ') {
      emergencyStep++;
      if (emergencyStep === 2) {
        if (!hasRole()) return;
        broadcastDataBridge.sendToIndex("A"); broadcastDataBridge.sendToIndex("COFF");
        showBlockedCommandPopup('Sequência de emergência detetada! Robô desativado.', 9);
        emergencyStep = 0;
      }
      event.preventDefault();
      return;
    }
    emergencyStep = 0;
  });

  bc.onmessage = (event) => broadcastDataBridge.bcOnMessage(event);// Receve data from script.js

  IOsModule.addIOsButtonListeners();

  IOsModule.updateIOsText();

  // Listener for send the text inside input when enter is clicked
  broadcastDataBridge.input.addEventListener("keydown", (e) => {
    if (e.key === "Enter") {
      const command = broadcastDataBridge.input.value.trim();
      broadcastDataBridge.CommandFilter(command);
      broadcastDataBridge.input.value = "";
    }
  });

  // Button to hide/show the shortcutsModule
  shortcutsModule.toggleShortcuts.addEventListener("click", () => {
    shortcutsModule.visibleShortcuts = !shortcutsModule.visibleShortcuts;
    if (shortcutsModule.visibleShortcuts) {
      shortcutsModule.shortcuts.classList.remove("hidden");
      shortcutsModule.toggleShortcuts.innerHTML = "&#129094;";
    } else {
      shortcutsModule.shortcuts.classList.add("hidden");
      shortcutsModule.toggleShortcuts.innerHTML = "&#129092";
    }
  });

  // Adiciona um evento de clique aos botões com data-comando
  document.querySelectorAll("[data-comando]").forEach((button) => {
    button.addEventListener("click", () => {
      const comando = button.getAttribute("data-comando");
      if (!hasRole()) return;
      shortcutsModule.shortcutCommand(comando);
    });
  });

  // Mostra ou esconde os blocos de I/O ao clicar no botão de alternância de visibilidade
  IOsModule.toggleIOsBtn.addEventListener("click", () => {
    IOsModule.iosVisivel = !IOsModule.iosVisivel;
    if (IOsModule.iosVisivel) {
      containerIOs.classList.remove("hidden"); // Mostra o contentor
    } else {
      containerIOs.classList.add("hidden"); // Esconde o contentor
    }
  });

  // Alterna entre modo de entradas e saídas ao clicar no botão de troca
  IOsModule.switchIO.addEventListener('click', () => {
    IOsModule.switchIO.classList.toggle('active'); // Ativa/desativa o modo OUT
    IOsModule.updateIOsText(); // Atualiza todos os textos consoante o novo modo
  });
}
function loop() { }

function main() { setup(); loop(); }

window.addEventListener("DOMContentLoaded", main);