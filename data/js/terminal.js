// --------------------
// terminal.js (para terminal.html)
// --------------------
const bc = new BroadcastChannel("terminal_channel"); // Canal de comunicação entre abas ou scripts
//const isADM = JSON.parse(localStorage.getItem("isADM") || "false");
const isUSER = JSON.parse(localStorage.getItem("isUSER") || "false");
const containerTerminal = document.getElementById("containerTerminal");
var isADM = true
// Aguarda que todo o conteúdo HTML da página esteja completamente carregado
window.addEventListener("DOMContentLoaded", () => {
  const input = document.getElementById("commandInput"); // Campo de input onde o utilizador escreve comandos
  const output = document.getElementById("output"); // Área de output onde são mostradas respostas e comandos

  // Ao carregar na tecla Enter dentro do campo de input
  input.addEventListener("keydown", (e) => {
    if (e.key === "Enter") {// aqui é onde o comando que estava na caixa de input vai ser processado 
      const command = input.value.trim(); // Remove espaços antes/depois do comando
      CommandFilter(command);
      input.value = "";  // Limpa o campo de input e faz scroll automático para mostrar nova linha
      //const ledMatch = command.toLowerCase().match(/^mainled\s+(in|out)\s+(\d{1,2}(?:,\d{1,2})*)\s+(on|off)$/);// Verifica se o comando corresponde ao formato esperado para controlo de LEDs
    }
  });

  // Recebe dados do script.js
  bc.onmessage = (event) => {
    const { type, data } = event.data; // Desestrutura o tipo e dados da mensagem recebida
    // Trata mensagens recebidas de WebSocket (por exemplo, do ESP32)
    if (type === "ESP-message") {
      // Garante que o conteúdo está em string (segurança adicional)
      const mensagem = typeof data === "string" ? data.trim() : JSON.stringify(data);
      output.innerText += `»» ${mensagem}\n`; // Adiciona a mensagem ao output com prefixo especial
      output.scrollTop = output.scrollHeight; // Scroll automático para mostrar o fim da mensagem
    }

    else if (type === "system-message") {
      let mensagem =
        typeof data === "string" ? data.trim() : JSON.stringify(data);

      // Remove prefixo "SYS@" se existir
      if (mensagem.startsWith("SYS@")) {
        mensagem = mensagem.slice(4); // Remove os primeiros 4 caracteres
      }

      output.innerText += `SYS_> ${mensagem}\n`;
      output.scrollTop = output.scrollHeight;
    }
    // Mensagens de tipo desconhecido (útil para debugging)
    else {
      output.innerText += `SYS_> [Mensagem desconhecida: ${type}]\n`;
      output.scrollTop = output.scrollHeight;
    }
  };

  //inicia os botões I/O ao carregar a pagina.
  updateTexts();
  addButtonListeners();
});

// bool function to check if the user has a role
function hasRole() {
  if (isADM || isUSER) {
    //console.log("[DEBUG] Utilizador tem cargo:", isADM ? "ADM" : "USER");
    return true; // Utilizador autorizado
  } else {
    //console.warn("[DEBUG] Utilizador sem cargo - bloqueado");
    showBlockedCommandPopup("🚫 Precisas de um cargo para enviar comandos!", 4);
    return false; // Bloqueado
  }
}

// --------------------
// HTML messing
// --------------------
const toggleBtn = document.getElementById("toggleAtalhos");
/**
 * @description Função para mostrar um popup em caso de ou um comando bloqueado ou uma sequência de emergência.
 * @param {string} texto - Texto para mostrar no popup.
 * @param {number} time - Define quanto tempo o popup vai ser mostrado,em segundos.
 */
function showBlockedCommandPopup(texto, time) {
  const popup = document.getElementById("blockedCommandPopup");
  const content = popup.querySelector(".popup-content");
  const overlay = document.getElementById("popupOverlay");
  const timeInMilisenconds = time * 1000;

  content.textContent = texto;

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
// esconder/mostrar atalhos
toggleBtn.addEventListener("click", () => {
  atalhosVisivel = !atalhosVisivel;
  if (atalhosVisivel) {
    atalhos.classList.remove("hidden");
    toggleBtn.textContent = "➤";
  } else {
    atalhos.classList.add("hidden");
    toggleBtn.textContent = "◀";
  }
});

// --------------------
// secção IOs
// --------------------
const switchIO = document.getElementById('switchIO');
const ioBlocks = document.querySelectorAll('.io-block');
const toggleIOsBtn = document.getElementById("toggleIOsBtn");
let iosVisivel = false;
// Esta função adiciona listeners aos botões dentro dos blocos de I/O.
function addButtonListeners() {
  ioBlocks.forEach(ioBlock => {
    const numIO = ioBlock.getAttribute('numIO'); // Número do I/O (identificador do bloco)
    const buttons = ioBlock.querySelectorAll('button'); // Seleciona todos os botões do bloco

    buttons.forEach(button => {
      button.addEventListener('click', () => {
        const isOut = switchIO.classList.contains('active'); // Verifica se estamos no modo OUT
        const tipo = isOut ? 'OUT' : 'IN'; // Define o tipo consoante o modo atual
        const text = button.textContent; // Obtém o texto do botão clicado
        if (!hasRole()) return; // Verifica se o utilizador tem um cargo válido antes de adicionar os listeners
        // Executa comandos com base no texto do botão
        if (text.includes('Ligar')) {
          sendToIndex(`disable ${tipo.toLowerCase()} ${numIO}`);
          sendToIndex(`force ${tipo.toLowerCase()} ${numIO} 1`);
        } else if (text.includes('Desligar')) {
          sendToIndex(`disable ${tipo.toLowerCase()} ${numIO}`);
          sendToIndex(`force ${tipo.toLowerCase()} ${numIO} 0`);
        } else if (text.includes('Reativar')) {
          sendToIndex(`enable ${tipo.toLowerCase()} ${numIO}`);// Reativa o I/O (retoma controlo à mainframe)
        }
      });
    });
  });
}
// Atualiza os textos dos rótulos e botões consoante o modo atual (IN ou OUT)
function updateTexts() {
  const isActive = switchIO.classList.contains('active'); // Verifica o modo atual
  const label = switchIO.querySelector('.labelIO'); // Rótulo principal do botão de modo
  label.textContent = isActive ? 'Saídas' : 'Entradas'; // Atualiza o texto do rótulo principal

  // Atualiza cada bloco de I/O
  ioBlocks.forEach(ioBlock => {
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
// Alterna entre modo de entradas e saídas ao clicar no botão de troca
switchIO.addEventListener('click', () => {
  switchIO.classList.toggle('active'); // Ativa/desativa o modo OUT
  updateTexts(); // Atualiza todos os textos consoante o novo modo
});
// Mostra ou esconde os blocos de I/O ao clicar no botão de alternância de visibilidade
toggleIOsBtn.addEventListener("click", () => {
  iosVisivel = !iosVisivel;
  if (iosVisivel) {
    containerIOs.classList.remove("hidden"); // Mostra o contentor
  } else {
    containerIOs.classList.add("hidden"); // Esconde o contentor
  }
});

// --------------------
// ATALHOS
// --------------------
let atalhosVisivel = true;
let showingCoodenates = false;// se for true o comando LISTPV POSITION vai ser enviado repetidamente
let intervaloEnvio = null;
var LISTPV_POSITION_delay = 2500;
let emergencyStep = 0; // Contador de passos da sequência de emergência
const prefixosTerminal = `\n- Comando enviado:            «« comando\n- Mensagem recebida (WS):     »» resposta\n- Mensagem de sistema:        SYS_> descrição\n`;

//emergency sequence for safety
document.addEventListener('keydown', function (event) {
  const key = event.key;
  const isTyping = ['INPUT', 'TEXTAREA'].includes(event.target.tagName);
  if (isTyping) return;
  if (key === ' ') {
    emergencyStep++;
    if (emergencyStep === 2) {
      if (!hasRole()) return;
      sendToIndex("A"); sendToIndex("COFF");
      showBlockedCommandPopup('Sequência de emergência detetada! Robô desativado.', 9);
      emergencyStep = 0;
    }
    event.preventDefault();
    return;
  }
  emergencyStep = 0;
});
// Adiciona um evento de clique aos botões com data-comando
document.querySelectorAll("[data-comando]").forEach((button) => {
  button.addEventListener("click", () => {
    const comando = button.getAttribute("data-comando");
    if (!hasRole()) return;
    shortcutCommand(comando);
  });
});

/**
 * @param {string} comando - O comando a ser processado.
 * @description Esta função usa um switch para enviar os comandos do atalho. Cada case representa um comando específico.
 * Caso o comando não seja reconhecido, uma mensagem de erro é exibida no console.
 */
function shortcutCommand(comando) {
  const botaoToggleCoordenadas = document.getElementById("toggle-coordenadas");

  // Função que usa switch para lidar com os comandos
  switch (comando) {
    case "HELP": sendToIndex(comando); showInOutput(prefixosTerminal); break;
    case "A": sendToIndex(comando); break;
    case "CTRL+C": sendToIndex("\x03"); showInOutput("CTRL+C enviado."); break;
    case "CON": sendToIndex(comando); break;
    case "COFF": sendToIndex(comando); break;
    case "LISTPV POSITION":
      if (!showingCoodenates) {
        // Inicia envio a cada segundo
        intervaloEnvio = setInterval(() => {
          if (!hasRole()) { return; }
          sendToIndex("LISTPV POSITION");
        }, LISTPV_POSITION_delay);
        var delay = LISTPV_POSITION_delay / 1000; // Converte para segundos
        var text = `O comando LISTPV POSITION vai ser enviado a cada ${delay} segundo${delay !== 1 ? "s" : ""}.`;//coloca um s se for plural
        showInOutput(text);

        // Atualiza o botão
        if (botaoToggleCoordenadas) {
          botaoToggleCoordenadas.textContent = "🔴 Parar coordenadas";
          botaoToggleCoordenadas.setAttribute("data-comando", "STOP LISTPV");
          botaoToggleCoordenadas.setAttribute("title", "Parar envio de coordenadas");
          showingCoodenates = true;
        }
      }
      break;
    case "STOP LISTPV":
      if (showingCoodenates) {
        clearInterval(intervaloEnvio);
        intervaloEnvio = null;
        showInOutput("O comando LISTPV POSITION parou de ser enviado.");
        // Atualiza o botão
        if (botaoToggleCoordenadas) {
          botaoToggleCoordenadas.textContent = "🟢 Mostrar coordenadas";
          botaoToggleCoordenadas.setAttribute("data-comando", "LISTPV POSITION");
          botaoToggleCoordenadas.setAttribute("title", "Envia LISTPV POSITION para a minframe repetidamente");
          showingCoodenates = false;
        }
      }
      break;
    case "ECHO": sendToIndex(comando); break;
    case "NOECHO": sendToIndex(comando); break;
    case "clearOutputHTML": output.innerText = ""; break;
    default: showInOutput(`Comando desconhecido: ${comando}`); break;
  }

  function showInOutput(text) {
    output.innerText += `SYS_> ${text}\n`;
    output.scrollTop = output.scrollHeight;
  }
}
/**
 * @param {string} command - vai passar por um filtro para ter a certeza que o comando é valido para o cargo.
 * @note Esta função só envia o comando do utilizador para a mainframe caso o comando não esteja na lista negra.
 * @note Filtra o cargo USER E NÃO O ADM, além de não permitir visualizadores ou conectados de enviar comandos.
 */
function CommandFilter(command) {
  //console.log("[DEBUG] CommandFilter chamado com comando:", command);
  if (!hasRole()) { return; }// Verifica se o utilizador tem algum cargo válido (USER ou ADM)
  const blacklist = new Set([
    "clr",         // CLR [n] / CLR *
    "toff",        // TOFF [n]
    "ton",         // TON [n]
    "~",           // ~ ou Ctrl+M
    "hhome",       // HHOME ou HHOME [n]
    "config",      // CONFIG ?
    "test"         // TEST
  ]);//console.log("[DEBUG] blacklist:", Array.from(blacklist));


  if (isUSER && !isADM) {
    const cmdLower = command.trim().toLowerCase();//console.log("[DEBUG] Comando limpo e em lowercase:", cmdLower);

    for (const item of blacklist) {
      if (cmdLower.startsWith(item)) {
        showBlockedCommandPopup("🚫 Não tem permissão para enviar esse comando!", 3);
        return; // Não envia
      }
    }
  }
  sendToIndex(command);
}

/**
 * @description Envia o comando para o a página principal através do BroadcastChannel.
 * @param {string} command - O comando a ser enviado.
 * @note Mostra no output o que foi enviado.
 */
function sendToIndex(command) {
  if (!hasRole()) { return; }
  message = {
    type: "input-command",
    command: command + "\r",
  };
  bc.postMessage(message);
  output.innerText += `«« ${command}\n`;
  output.scrollTop = output.scrollHeight;
}
