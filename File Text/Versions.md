# FILE: Versions.txt

## Versionamento Semântico (Embedded Systems / Firmware)

Formato:

```
MAJOR.MINOR.PATCH
```

---

## Significado

### MAJOR (X.0.0)

Mudanças incompatíveis:

* Alterações na arquitetura do firmware
* Mudança de protocolo de comunicação
* Reescrita de módulos críticos

### MINOR (0.X.0)

Novas funcionalidades compatíveis:

* Novos módulos
* Expansão de APIs
* Suporte a novos periféricos

### PATCH (0.0.X)

Correções e melhorias:

* Bug fixes
* Ajustes de performance
* Correções de estabilidade

---

## Exemplos

```
0.1.0 -> Estrutura inicial do firmware
0.2.0 -> Comunicação (HTTP/WebSocket/UART)
0.2.1 -> Fix de estabilidade
1.0.0 -> Versão estável
```

---

## Regras

* MAJOR: mudanças estruturais
* MINOR: novas funcionalidades
* PATCH: correções pequenas
