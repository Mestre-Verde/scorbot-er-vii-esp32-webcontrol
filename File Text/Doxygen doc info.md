# FILE: Doxygen doc info.txt

## Convenção de documentação (Doxygen)

Este ficheiro descreve as tags usadas para documentação de código em sistemas embebidos e software de baixo nível.

---

## Tabela de tags Doxygen (completa e avançada)

### 1. Informativas básicas

| Tag          | Descrição                             |
| ------------ | ------------------------------------- |
| `@brief`     | Resumo curto da função/módulo         |
| `@details`   | Explicação detalhada do comportamento |
| `@file`      | Descrição do ficheiro                 |
| `@author`    | Autor do código                       |
| `@version`   | Versão do ficheiro ou módulo          |
| `@date`      | Data de criação/modificação           |
| `@copyright` | Informação de direitos de autor       |

---

### 2. Funções e parâmetros

| Tag              | Descrição                     |
| ---------------- | ----------------------------- |
| `@param`         | Parâmetro de função           |
| `@param[in]`     | Parâmetro de entrada          |
| `@param[out]`    | Parâmetro de saída            |
| `@param[in,out]` | Parâmetro de entrada e saída  |
| `@return`        | Valor de retorno              |
| `@retval`        | Valores possíveis de retorno  |
| `@fn`            | Descrição explícita de função |

---

### 3. Estrutura e organização de código

| Tag          | Descrição               |
| ------------ | ----------------------- |
| `@class`     | Descrição de classe     |
| `@struct`    | Descrição de estrutura  |
| `@union`     | Descrição de union      |
| `@enum`      | Descrição de enumeração |
| `@namespace` | Descrição de namespace  |
| `@typedef`   | Definição de tipo       |
| `@var`       | Descrição de variável   |

---

### 4. Organização modular (muito importante em firmware)

| Tag           | Descrição                            |
| ------------- | ------------------------------------ |
| `@defgroup`   | Define um grupo de módulos           |
| `@ingroup`    | Adiciona elemento a um grupo         |
| `@addtogroup` | Adiciona elementos a grupo existente |
| `@weakgroup`  | Grupo opcional/fraco                 |

---

### 5. Controlo de qualidade e debugging

| Tag           | Descrição                       |
| ------------- | ------------------------------- |
| `@todo`       | Tarefas por implementar         |
| `@bug`        | Problemas conhecidos            |
| `@warning`    | Avisos críticos                 |
| `@attention`  | Atenção especial do programador |
| `@deprecated` | Função obsoleta                 |
| `@test`       | Informação de testes            |

---

### 6. Pré e pós condições (design robusto)

| Tag          | Descrição                   |
| ------------ | --------------------------- |
| `@pre`       | Condições antes da execução |
| `@post`      | Estado após execução        |
| `@invariant` | Condição sempre verdadeira  |

---

### 7. Exceções e controlo de fluxo

| Tag                  | Descrição                       |
| -------------------- | ------------------------------- |
| `@throw` / `@throws` | Exceções lançadas               |
| `@retval`            | Estados de retorno alternativos |

---

### 8. Exemplos e documentação prática

| Tag        | Descrição                 |
| ---------- | ------------------------- |
| `@example` | Exemplo de uso            |
| `@code`    | Início de bloco de código |
| `@endcode` | Fim de bloco de código    |
| `@par`     | Parágrafo personalizado   |

---

### 9. Referências cruzadas

| Tag       | Descrição                    |
| --------- | ---------------------------- |
| `@see`    | Referência a outras funções  |
| `@ref`    | Referência interna           |
| `@link`   | Link direto para símbolo     |
| `@anchor` | Ponto de referência no texto |

---

## Exemplo avançado realista (firmware ESP32)

```c
/**
 * @file wifi_manager.c
 * @brief Gestão de WiFi no sistema embebido
 * @details Responsável por conexão, reconexão e monitorização do estado WiFi.
 *
 * @defgroup network Módulo de rede
 * @ingroup system
 *
 * @param[in] ssid Nome da rede WiFi
 * @param[in] password Palavra-passe da rede
 * @param[out] status Estado da ligação
 * @return true se ligação estabelecida com sucesso
 * @retval true Ligação OK
 * @retval false Falha na ligação
 *
 * @pre Sistema de rede inicializado
 * @post WiFi ativo ou em estado de falha controlada
 * @warning Pode bloquear reconexão se watchdog não estiver ativo
 * @note Reconexão automática ocorre a cada 5 segundos
 * @todo Adicionar suporte a WPA3 enterprise
 * @see http_server.c
 */
bool wifi_init(const char* ssid, const char* password);
```
