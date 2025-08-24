const functions = {
  setUser(clienteIndex) { },
  atualizarTabela() { },
  processarClientes(message) { },
  limparLinhaDaTabela(index) { },
  downloadComandos() { },
}
/*-------------------------------
Body
-------------------------------*/
// Animação do Título Robótico
document.addEventListener("DOMContentLoaded", () => {
  const texto = "⚙️Controlo do Robô Scorbot-ER VII⚙️ ";
  const titulo = document.querySelector("#titulo");

  let index = 0;
  let apagando = false;

  function escreverTexto() {
    if (!apagando) {
      if (index <= texto.length) {
        titulo.textContent = texto.substring(0, index);
        index++;
        setTimeout(escreverTexto, 150);
      } else {
        setTimeout(() => {
          apagando = true;
          escreverTexto();
        }, 10000);
      }
    } else {
      if (index >= 0) {
        titulo.textContent = texto.substring(0, index);
        index--;
        setTimeout(escreverTexto, 100);
      } else {
        apagando = false;
        setTimeout(escreverTexto, 3000);
      }
    }
  }
  escreverTexto();
});

// efeito dos leds que fazem de hr (index.html)
function randomBlink(led) {
  setInterval(() => {
    const isOn = led.classList.contains("on");
    if (Math.random() > 0.5) {
      // 50% de chance de mudar estado
      led.classList.toggle("on", !isOn);
    }
  }, 1000); // tempo fixo de 2 segundos
} document.addEventListener("DOMContentLoaded", () => {
  const leds = document.querySelectorAll(".ledT");
  leds.forEach((led) => {
    randomBlink(led);
  });
});
/**
 * Define o estado de um LED na mainframe.
 * Ativa ou desativa o LED correspondente com base no nome do IO.
 *
 * @param {string} ioName - O nome do IO (ex: "in1", "out2").
 * @param {boolean} isActive - true para ativar o LED, false para desativar.
 * @example
 * setLedState('in1', true); // Ativa o LED correspondente ao IO "in1"
 * setLedState('out2', false); // Desativa o LED correspondente ao IO "out2"
 * @note Esta função pode ser chamada por um comando enviado no terminal.
 * @note Esta função vai ser util para atualizar o estado dos LEDs na interface quando, em um programa, alguma saida ou entrada for ativada.
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
/**
 * Exibe uma mensagem de popup no canto superior direito da tela.
 * Esta função cria um elemento visual temporário para exibir mensagens de feedback ao usuário,como erros, informações ou notificações. Os popups desaparecem automaticamente após 10 segundos ou podem ser removidos manualmente pelo código.
 * @param {string} message - A mensagem a ser exibida no popup.
 * @example
 * popup("Conexão estabelecida com sucesso.");
 * popup(`Erro no WebSocket: ${error}`);
 * @note
 * - No máximo, 3 popups podem ser exibidos simultaneamente. Se um novo popup for criado
 *   quando já houver 3, o mais antigo será removido.
 * - A posição dos popups é recalculada automaticamente para evitar sobreposição.
 * - A função substitui o uso de `alert()` para evitar interrupções na experiência do usuário.
 * @see
 * - A classe CSS `.popup`.
 * - A classe CSS `.hide`.
 */
function popup(message) {
  // Verifica se já existem 3 popups
  const existingPopups = document.querySelectorAll(".popup");
  if (existingPopups.length >= 3) {
    // Remove o popup mais antigo (primeiro da lista)
    existingPopups[0].remove();
  }

  // Cria o elemento do popup
  const popupElement = document.createElement("div");
  popupElement.className = "popup";
  popupElement.textContent = message;

  // Adiciona o popup ao corpo do documento
  document.body.appendChild(popupElement);

  // Recalcula as posições de todos os popups
  const updatedPopups = document.querySelectorAll(".popup");
  updatedPopups.forEach((popup, index) => {
    popup.style.top = `${20 + index * 60}px`; // 20px de margem inicial + 60px por popup
  });

  // Remove o popup após 10 segundos
  setTimeout(() => {
    popupElement.classList.add("hide"); // Adiciona a classe para animação de saída
    setTimeout(() => {
      popupElement.remove();
      // Recalcula as posições novamente após a remoção
      const remainingPopups = document.querySelectorAll(".popup");
      remainingPopups.forEach((popup, index) => {
        popup.style.top = `${20 + index * 60}px`;
      });
    }, 500); // Tempo para a animação de saída
  }, 10000);
}
/*-------------------------------
footer
-------------------------------*/
// mostra a hora no footer
setInterval(() => {
  const now = new Date();
  const timeStr = now.toLocaleTimeString("pt-PT");
  document.getElementById("footer-clock").textContent = timeStr;
}, 1000);

/*-------------------------------
contentor de funções
-------------------------------*/
//Função para mostrar o contentor WebSocket.Exibe o painel de conexão WebSocket na interface.
function mostrarPainelConect() {
  document.getElementById("websocket-conteiner").classList.remove("hidden");
}
//Função para esconder o contentor WebSocket. Oculta o painel de conexão WebSocket na interface.
function fecharPainelConec() {
  document.getElementById("websocket-conteiner").classList.add("hidden");
}
// função para descarregar o ficheiro dos comandos
function downloadComandos() {
  if (!socket || socket.readyState !== WebSocket.OPEN) {
    popup("Não há um WebSocket ativo. Verifique o estado da ligação");
    return;
  }

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
        a.download = "comandos.txt";
        a.click();
        URL.revokeObjectURL(url);
      })
      .catch((error) => {
        console.error("Erro ao obter comandos.txt:", error);
        popup("Erro ao descarregar o ficheiro.");
      });
  }
}
//para mostrar e esconder a secção do D-PAD ao clicar no botão
document.getElementById("toggle-move-arows").addEventListener("click", () => {
  const controlsSection = document.getElementById("controls-section");
  const buttonArows = document.getElementById("toggle-move-arows");

  controlsSection.classList.toggle("hidden");

  if (controlsSection.classList.contains("hidden")) {
    buttonArows.innerText = "🎮 SHOW";
  } else {
    buttonArows.innerText = "🎮 HIDE";

    // Faz scroll suave até à secção visível
    controlsSection.scrollIntoView({ behavior: "smooth", block: "start" });
  }
});

/*-------------------------------
Estado websocket
-------------------------------*/
let socket = null;
let isADM = false;
let isUSER = false;
const bc = new BroadcastChannel("terminal_channel"); // Canal de comunicação entre abas
let terminalWindow = null;

/**
 * Função para configurar a conexão WebSocket.
 * Estabelece uma nova conexão WebSocket com o servidor no endereço 'ws://<hostname>:81'.
 * Garante que uma nova ligação só é criada se não houver uma ligação ativa.
 *
 * Eventos tratados:
 * - socket.onopen: executado quando a conexão é estabelecida com sucesso.
 * - socket.onclose: executado quando a conexão é encerrada (pelo servidor ou cliente).
 * - socket.onerror: executado quando ocorre um erro na conexão.
 * - socket.onmessage: executado quando o cliente recebe uma mensagem do servidor.
 */
function conectarWebSocket() {
  const btn = document.getElementById("buttonConectWebsocket");

  // Evita múltiplos cliques rápidos
  btn.disabled = true; // Desativa imediatamente
  setTimeout(() => {
    btn.disabled = false; // Reativa após 3 segundos
  }, 2500);

  if (socket && socket.readyState === WebSocket.OPEN) {
    popup("Já conectado ao WebSocket.");
    return;
  }

  socket = new WebSocket("ws://" + window.location.hostname + ":81");

  socket.onopen = () => {
    popup("Handshake com o Servidor iniciado.");
  };

  socket.onclose = () => {
    popup("Conexão WebSocket fechada.");
    socket = null;
    setRole(null);
    document.getElementById("admin-environment").style.display = "none";
    for (let i = 0; i < 8; i++) {
      limparLinhaDaTabela(i);
    }
    updateConnectionState();
    if (terminalWindow && !terminalWindow.closed) {
      terminalWindow.close();
    }
  };

  socket.onerror = (error) => {
    popup(`Erro no WebSocket: ${error}`);
  };

  socket.onmessage = handleMessage;
}

/**
 * Função para desconectar o cliente.
 * Fecha a conexão WebSocket com o servidor.
 */
function desconectar() {
  if (socket) {
    popup("Desconectando...");
    socket.close();
    updateConnectionState();
  } else {
    popup("Nenhuma conexão ativa");
    updateConnectionState();
  }
}
// se o botão de terminal estiver presente, adiciona o evento de clique que abre o terminal.
document.addEventListener("DOMContentLoaded", () => {
  const toggleButton = document.getElementById("toggleTerminal");
  if (toggleButton) {
    toggleButton.addEventListener("click", () => {
      if (!socket || socket.readyState !== WebSocket.OPEN) {
        popup("Não há um Websocket ativo. Verifique o estado da ligação");
        return;
      }
      if (hasRole()) {
        // Define os papéis antes de abrir a nova janela
        localStorage.setItem("isADM", isADM);
        localStorage.setItem("isUSER", isUSER);
        //abre a janela
        terminalWindow = window.open("/html/terminal.html", "_blank");
      }
    });
  }
});

// função que recebe mensagens do terminal
bc.onmessage = (event) => {
  switch (event.data.type) {
    case "input-command": {
      const command = event.data.command; // Extrai o comando textual enviado (ex: "PING", "GET INFO", etc.)
      // Verifica se o WebSocket existe e se está aberto (pronto a comunicar)
      if (socket && socket.readyState === WebSocket.OPEN) {
        socket.send(command); // Envia o comando como está para o ESP via WebSocket
      }
      break;
    }
  }
};
/**
 * Lida com mensagens recebidas do ESP através do WebSocket.
 * Executa ações conforme o conteúdo da mensagem.
 *
 * @param {MessageEvent} event - Evento WebSocket contendo a mensagem recebida.
 */
function handleMessage(event) {
  const message = event.data;
  const input = document.getElementById("senhaAdmin");
  let i = ""; // Variável temporária de stack
  //console.log("[DEBUG]Mensagem recebida:", message); // Log para depuração

  if (message.startsWith("POPUP:")) {
    popup(message.substring(6));
  } else if (message.startsWith("ID:")) {
    processarClientes(message);
  } else if (message.startsWith("WARNING:")) {
    console.log(message);
    popup("Chegou um WARNING.(F12)->Consola");
  } else if (message === "DISCONNECTEDBYTIMEOUT") {
    popup("A sua conexão foi fechada devido ao timeout.");
  } else if (message === "DISCONNECTEDBYKICKED") {
    popup("Você foi desconectado pelo administrador.");
  } else if (message === "DISCONNECTEDBYNO_PONG") {
    popup("A tua ligação Wbsocket tem um problema de handshake.");
  } else if (message.startsWith("ADM")) {
    i = message.substring(3);
    if (i === "OK") {
      setRole("ADM");
      popup("Acesso de administrador concedido.");
      document.getElementById("admin-environment").style.display = "block";
      if (input) input.value = "";
    } else if (i === "FAIL") {
      popup("Palavra-passe de administrador incorreta.");
      if (input) input.value = "";
    } else if (i === "FULL") {
      popup("Já existe 1 administrador.");
      if (input) input.value = "";
    }
  } else if (message.startsWith("USER")) {
    i = message.substring(4);
    if (i === "OK") {
      popup("Acesso de utilizador concedido.");
      setRole("USER");
    } else if (i === "FULL") {
      popup("Já existe 1 utilizador.");
    } else if (i === "REMOVED") {
      popup("Foi-te removido o cargo de USER.");
      setRole("R_USER");
      if (terminalWindow && !terminalWindow.closed && !isADM) {
        terminalWindow.close();
      }
    }
    // Reencaminha para o terminal via BroadcastChannel
  } else if (message.startsWith("SYS@")) {
    bc.postMessage({
      type: "system-message",
      data: message,
    });
  } else {
    // Reencaminha para o terminal via BroadcastChannel
    bc.postMessage({
      type: "ESP-message",
      data: message,
    });
  }
}

/*-------------------------------
cargos
-------------------------------*/
/**
 * Define o papel atual do utilizador.
 * Garante que apenas um cargo (ADM ou USER) está ativo de cada vez.
 *
 * @param {'ADM'|'USER'|'R_USER'|null} role - O cargo a aplicar. Use null para limpar.
 * @example
 * setRole('ADM'); // Torna-se administrador
 * setRole(null);  // Remove os cargos
 */
function setRole(role) {
  switch (role) {
    case "ADM":
      isADM = true;
      break;
    case "USER":
      isUSER = true;
      break;
    case "R_USER":
      isUSER = false;
      break;
    case null:
      isADM = false;
      isUSER = false;
      break;
  }
}
/**
 * Verifica se o utilizador está autenticado (ADMIN ou USER).
 * Mostra um alerta se não estiver autorizado.
 * @returns {boolean}
 */
function hasRole() { if (isADM || isUSER) { return true; } else { popup("Não tens permissão para aceder a essa função."); return; } }
//funções de input Cargos
/**Função para enviar a identificação.
 * @note Envia `NOME:${nome}` para o ESP, onde `${nome}` é o valor do campo de input.
 */
function sendName() {
  // 1. Obter o elemento de input
  const nomeInput = document.getElementById("nome");
  if (!nomeInput) {
    popup("Erro: não foi possível encontrar o campo de nome.");
    return;
  }
  // 3. Verificar estado do WebSocket
  if (!socket || socket.readyState !== WebSocket.OPEN) {
    popup("Não há um Websocket ativo. Verifique o estado da ligação");
    return;
  }
  // 2. Validar valor
  const nome = nomeInput.value.trim();
  if (nome.length === 0) {
    popup("Por favor, insira o seu nome.");
    nomeInput.focus();
    return;
  }
  // 4. Enviar dados para o servidor
  socket.send(`NOME:${nome}`);
  popup("Nome enviado.");

  //5. (Opcional) Limpar o campo após envio
  nomeInput.value = "";
}
//Envia a senha de administrador para o servidor.
function GETADM() {
  const input = document.getElementById("senhaAdmin");
  if (!input) {
    popup("Erro: Campo de password não encontrado.");
    return;
  }
  if (!socket || socket.readyState !== WebSocket.OPEN) {
    popup("Não há um Websocket ativo. Verifique o estado da ligação");
    return;
  }
  const adminPassword = input.value.trim();
  if (adminPassword.length === 0) {
    popup("Password inválida. Por favor, introduza a sua password de administrador.");
    return;
  }
  socket.send("ADMPASS:" + adminPassword);
}
// Atualiza o estado da conexão no elemento HTML.Não interage com o ESP.
function updateConnectionState() {
  const connectionStateElement = document.getElementById("connection-state");
  if (!socket || socket.readyState === WebSocket.CLOSED) {
    connectionStateElement.textContent = "Sem ligação.";
    return;
  }
  switch (socket.readyState) {
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
// call updateConnectionState() at regular intervals or when needed
setInterval(updateConnectionState, 500); // Check every 0,5 seconds

/*-------------------------------
tabela dos clientes para ADM
-------------------------------*/
// Pede ao ESP para atualizar a tabela de clientes.
function atualizarTabela() {
  const comando = "CLIENTINFO";
  if (!socket || socket.readyState !== WebSocket.OPEN) {
    popup("Não há um Websocket ativo. Verifique o estado da ligação");
    return;
  }
  socket.send(comando);
  //console.log("Comando CLIENTINFO enviado para o ESP.");
}
// mess with the table
/**
 * Processa a lista de clientes recebida do servidor.
 * @param {string} message - A mensagem contendo os dados dos clientes.
 */
function processarClientes(message) {
  const linhas = message.trim().split("\n");

  for (let i = 0; i < 8; i++) {
    limparLinhaDaTabela(i);
  }

  linhas.forEach((linha) => {
    // Separa as partes da linha para tratamento (ID, IP, etc)
    const partes = linha.split("|");

    const cliente = {
      id: parseInt(partes[0].split(":")[1]), // Extrai o ID após "ID:"
      ip: partes[1], // IP
      admin: partes[2], // Admin (Sim/Não)
      user: partes[3], // User (Sim/Não)
      nome: partes[4], // Nome
      tempoConexao: partes[5], // Tempo no formato hh:mm:ss
    };

    // Adicionar cliente à tabela
    adicionarClienteNaTabela(cliente, cliente.id); // Usa o ID como índice
  });
}
/**
 * Limpa os dados de uma linha específica da tabela.
 * @param {number} index - O índice da linha a ser limpa (0 a 7).
 * @example
 * limparLinhaDaTabela(3); // Limpa a linha correspondente ao cliente com índice 3.
 */
function limparLinhaDaTabela(index) {
  document.getElementById(`cliente-${index}-id`).textContent = "";
  document.getElementById(`cliente-${index}-ip`).textContent = "";
  document.getElementById(`cliente-${index}-admin`).textContent = "";
  document.getElementById(`cliente-${index}-user`).textContent = "";
  document.getElementById(`cliente-${index}-nome`).textContent = "";
  document.getElementById(`cliente-${index}-tempo`).textContent = "";
}
/**
 * Atualiza os dados de um cliente na tabela.
 * @param {Object} cliente - Objeto contendo os dados do cliente.
 * @param {number} index - O índice da linha na tabela.
 */
function adicionarClienteNaTabela(cliente, index) {
  if (index < 0 || index >= 8) {
    popup(`Índice inválido: ${index}. Deve estar entre 0 e 7.`);
    return;
  }

  document.getElementById(`cliente-${index}-id`).textContent = cliente.id;
  document.getElementById(`cliente-${index}-ip`).textContent = cliente.ip;
  document.getElementById(`cliente-${index}-admin`).textContent = cliente.admin;
  document.getElementById(`cliente-${index}-user`).textContent = cliente.user;
  document.getElementById(`cliente-${index}-nome`).textContent = cliente.nome;
  document.getElementById(`cliente-${index}-tempo`).textContent =
    cliente.tempoConexao;
}
// Botões da tabela de clientes
/**
 * Pede ao ESP para define um cliente como usuário.
 *
 * @param {number} clienteIndex - O índice do cliente.
 *
 * @note A função envia `SETUSER:${clienteIndex}` para o ESP.
 */
function setUser(clienteIndex) {
  console.log(`Botão clicado: clienteIndex = ${clienteIndex}`);

  if (!socket || socket.readyState !== WebSocket.OPEN) { popup("Não há um Websocket ativo. Verifique o estado da ligação"); return; }

  if (isADM) {
    const comando = `SETUSER:${clienteIndex}`;
    socket.send(comando);
    console.log(`Comando SETUSER enviado para o cliente ${clienteIndex}`);
  } else {
    popup("Apenas o ADM pode definir utilizadores.");
  }
}
/**
 * Pede ao ESP para remover o status de usuário de um cliente.
 * @param {number} clienteIndex - O índice do cliente.
 * @note A função envia `REMUSER:${clienteIndex}` para o ESP. E desativa o botão REMOVE USER por 2 segundos.
 */
function removeUser(clienteIndex) {
  const comando = `REMOVEUSER:${clienteIndex}`;
  if (!socket || socket.readyState !== WebSocket.OPEN) {
    popup("Não há um Websocket ativo. Verifique o estado da ligação");
    return;
  }
  // Desativa o botão para evitar cliques repetidos
  const botao = document.getElementById(`btn-remove-user-${clienteIndex}`);
  if (botao) {
    botao.disabled = true;
  }
  socket.send(comando);
  // Reativa o botão após um tempo (opcional, dependendo da lógica do ESP)
  setTimeout(() => {
    if (botao) {
      botao.disabled = false;
    }
  }, 2000); // Reativa após 2 segundos
}
/**
 * Pede ao ESP para Desconecta um cliente.
 * @param {number} clienteIndex - O índice do cliente.
 * @note A função envia `KICKUSER:${clienteIndex}` para o ESP.
 */
function disconnectClient(clienteIndex) {
  if (!socket || socket.readyState !== WebSocket.OPEN) {
    popup("Não há um Websocket ativo. Verifique o estado da ligação");
    return;
  }
  const comando = `KICKUSER:${clienteIndex}`;
  socket.send(comando);
}
// SSID and wifi info
document.addEventListener("DOMContentLoaded", () => {// envia o SSID e a password para o ESP
  const btnSendSSID = document.getElementById("sendTheNewSSID");
  const modal = document.getElementById("ssid-confirm-modal");
  const btnContinue = document.getElementById("ssid-confirm-continue");
  const btnCancel = document.getElementById("ssid-confirm-cancel");

  let ssidInput, passwordInput;

  btnSendSSID.addEventListener("click", (e) => {
    e.preventDefault();
    ssidInput = document.getElementById("ssid-name");
    passwordInput = document.getElementById("ssid-password");
    modal.classList.remove("hidden");
  });

  btnContinue.addEventListener("click", () => {
    modal.classList.add("hidden");
    // Repete a lógica de envio do SSID
    if (!socket || socket.readyState !== WebSocket.OPEN) { popup("Não há um WebSocket ativo. Verifique o estado da ligação"); return; }

    const ssid = ssidInput.value.trim();
    const password = passwordInput.value.trim();

    if (hasRole()) {
      if (!ssid || !password) { popup("Não pode enviar com um campo vazio."); return; }
      if (ssid.length < 3 || password.length < 6) { popup("SSID deve ter pelo menos 3 caracteres e a password pelo menos 6."); return; }
      const mensagem = `NEWSTA:${ssid}||${password}`;
      socket.send(mensagem);
      ssidInput.value = "";
      passwordInput.value = "";
    }
  });

  btnCancel.addEventListener("click", () => {
    modal.classList.add("hidden");
  });
});
document.addEventListener("DOMContentLoaded", () => {// envia o comando GETREDEINFO para obter informações de rede
  const btnGetIP = document.getElementById("getNetInfo");

  btnGetIP.addEventListener("click", () => {
    if (!socket || socket.readyState !== WebSocket.OPEN) {
      popup("Não há um Websocket ativo. Verifique o estado da ligação");
      return;
    }
    if (hasRole()) {
      const comando = "GETREDEINFO";
      socket.send(comando);
      console.log("Comando enviado:", comando);
    }
  });
});

// ---------------------------------
// botão das tabelas de coor e enco
// ---------------------------------
// Aguarda o carregamento do DOM para garantir que o botão existe antes de adicionar o evento
document.addEventListener("DOMContentLoaded", () => {
  const button = document.getElementById("toggleAutoUpdate"); // Botão para ativar/desativar atualização automática
  if (button) {
    // Adiciona o evento de clique ao botão
    button.addEventListener("click", toggleAutoUpdate);
  }
});

let autoUpdateInterval = null;// Variável para armazenar o ID do intervalo de atualização automática
const updateInterval = 5000;// Define o intervalo de atualização em milissegundos (5 segundos)

/**
 * Função para ativar ou desativar a atualização automática das tabelas.
 * Altera o texto do botão conforme o estado.
 */
function toggleAutoUpdate() {
  const button = document.getElementById("toggleAutoUpdate");
  //if (socket && socket.readyState === WebSocket.OPEN) {    popup(      'Para atualizar os valores o estado da ligação deve ser "Sem ligação"'    );    return;  }

  // Agora permite atualização via HTTP mesmo com WebSocket ativo
  if (autoUpdateInterval !== null) {
    clearInterval(autoUpdateInterval);
    autoUpdateInterval = null;
    button.textContent = "Tabelas & I/O 🔄️";
    popup("Atualização automática desativada.");
  } else {
    autoUpdateInterval = setInterval(() => {
      downloadValuesFromHTTP();
    }, updateInterval);
    button.textContent = "Tabelas & I/O 🛑";
    popup("Atualização automática ativada.");
  }
}

/**
 * Função para desativar a atualização automática e redefinir o botão.
 * Usada, por exemplo, quando uma conexão WebSocket é estabelecida.

function disableAutoUpdateViewer() {
  const button = document.getElementById("toggleAutoUpdate");
  if (button) {
    // Redefine o texto do botão para o estado inicial
    button.textContent = "Tabelas & I/O 🔄️";

    // Cancela o intervalo de atualização automática, se estiver ativo
    if (autoUpdateInterval !== null) {
      clearInterval(autoUpdateInterval);
      autoUpdateInterval = null;
      popup("Atualização automática dos valores desativada para viewer.");
    }
  }
}
 */

// ---------------------------------
// atualização dos valores das tabelas
// ---------------------------------
async function downloadValuesFromHTTP() {
  //if (socket && socket.readyState === WebSocket.OPEN) {    popup(      "Atualização via HTTP desativada porque há uma conexão WebSocket ativa."    );    return;  }
  //console.log("Iniciando downloadValuesFromHTTP...");

  try {
    const resp = await fetch("/estado");
    if (!resp.ok) throw new Error("Erro ao obter dados");

    const rawText = await resp.text();

    // Separar coordenadas, encoders e base64
    const partes = rawText.split(";");
    if (partes.length < 3) throw new Error("Resposta inválida do servidor");

    const coordenadas = partes[0].split(",").map(Number);
    const encoders = partes[1].split(",").map(Number);
    const base64IO = partes[2];

    //console.log("Coordenadas:", coordenadas);
    //console.log("Encoders:", encoders);

    updateCoordinateTable(coordenadas);
    updateEncoderTable(encoders);

    // Decodificar os 4 bytes dos IOs
    const ioData = atob(base64IO);
    if (ioData.length !== 4) throw new Error("Dados binários inválidos");

    const inState = (ioData.charCodeAt(0) << 8) | ioData.charCodeAt(1);
    const outState = (ioData.charCodeAt(2) << 8) | ioData.charCodeAt(3);

    const inStateArray = Array.from({ length: 16 }, (_, i) => (inState >> (15 - i)) & 1);
    const outStateArray = Array.from({ length: 16 }, (_, i) => (outState >> (15 - i)) & 1);

    //console.log("InState:", inStateArray);
    //console.log("OutState:", outStateArray);

    updateInputState(inStateArray);
    updateOutputState(outStateArray);
  } catch (err) {
    popup(`Erro: ${err.message}`);
  }
}
// Atualiza os LEDs de input com base nos bits recebidos
function updateInputState(inStateArray) {
  for (let i = 0; i < inStateArray.length; i++) {
    const ioName = `in${i + 1}`; // "in1", "in2", ..., "in16"
    const isActive = inStateArray[i] === 1;
    setLedState(ioName, isActive);
  }
}
// Atualiza os LEDs de output com base nos bits recebidos
function updateOutputState(outStateArray) {
  for (let i = 0; i < outStateArray.length; i++) {
    const ioName = `out${i + 1}`; // "out1", "out2", ..., "out16"
    const isActive = outStateArray[i] === 1;
    setLedState(ioName, isActive);
  }
}
// Função para aplicar animação de flash se valor mudar
function updateCellIfChanged(cell, newValue) {
  if (cell.textContent !== String(newValue)) {
    cell.textContent = newValue;
    cell.classList.add("updated");
    setTimeout(() => cell.classList.remove("updated"), 500);
  }
}
// Função para atualizar a tabela de coordenadas
function updateCoordinateTable(valores) {
  const tabela = document.getElementById("coordinateValues");
  if (!tabela) {
    console.error("Tabela de coordenadas não encontrada no DOM.");
    return;
  }
  const row = tabela.querySelector("tbody tr");
  if (row) {
    valores.forEach((valor, index) => {
      updateCellIfChanged(row.cells[index], valor);
    });
  }
}
// Função para atualizar a tabela de encoders
function updateEncoderTable(valores) {
  const tabela = document.getElementById("encoderValues");
  if (!tabela) {
    console.error("Tabela de encoders não encontrada no DOM.");
    return;
  }
  const row = tabela.querySelector("tbody tr");
  if (row) {
    valores.forEach((valor, index) => {
      updateCellIfChanged(row.cells[index], valor);
    });
  }
}

/*
classList é uma propriedade dos elementos HTML em JavaScript que representa todas as classes CSS associadas a esse elemento.
Ela fornece vários métodos úteis, como:

.add('classe') – adiciona uma classe;

.remove('classe') – remove uma classe;

.contains('classe') – verifica se a classe está presente;

.toggle('classe') – ativa ou desativa uma classe.
*/
