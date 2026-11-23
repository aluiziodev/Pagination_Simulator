# Plano de implementação do simulador de paginação

## Decisões aprovadas

- C++17, Makefile, biblioteca padrão e Linux.
- Motor compartilhado, uma `struct` por política de substituição e interface de terminal separada do motor.
- Visualização ativada por `--visual`, com comandos seguidos de Enter para avançar, voltar e executar até o fim.

A interface obrigatória do enunciado é `./pager --algo <ALGO> --frames <N> --trace <arquivo> [--verbose]`. Os algoritmos aceitos são `fifo`, `lru`, `opt`, `clock`, `nru`, `lfu` e `mfu`. A extensão `--visual` usa os mesmos parâmetros de entrada.

## Projeto aprovado

- **Frames.** Usar um vetor indexado pelo número do frame. Cada posição usa `std::optional` para distinguir uma vaga de uma página residente. A página residente guarda o ID, a ordem de carga, o último acesso, a frequência e os bits R/M.
- **Políticas.** Usar `std::variant` e `std::visit` para despachar as sete `structs`. O motor aplica cargas e substituições e atualiza as métricas. A política escolhe a vítima e atualiza o estado específico das suas regras. O Ótimo consulta acessos futuros, o Clock controla o ponteiro e os bits R, e o NRU zera R após cada quarta referência.
- **Comandos.** Usar `a` para avançar, `v` para voltar e `f` para executar até o fim, sempre seguidos de Enter. Exibir os comandos e limitar a navegação ao estado inicial e ao último passo.
- **Histórico.** Guardar uma cópia dos frames exibidos e o resultado de cada passo à medida que a simulação progride. Navegar para um passo já registrado altera a posição de leitura no histórico. O uso de memória é proporcional a referências × frames.
- **Modos de saída.** Tratar `--visual` e `--verbose` como exclusivos. Se ambos forem informados, rejeitar a execução com uma mensagem de uso.
- **Validação do traço.** Aceitar IDs inteiros não negativos e tipos `R` ou `W`, com leitura como padrão. Aceitar traços sem referências com métricas zeradas e taxa de 0%. Rejeitar arquivos ilegíveis e referências malformadas, como IDs inválidos, tipos desconhecidos ou campos extras. Retornar código de saída diferente de zero e indicar a linha quando o erro estiver em uma referência.

## Tasks e dependências

Uma dependência indica o que precisa estar disponível para concluir e verificar a task. Os três desenvolvedores podem preparar suas partes em paralelo enquanto os contratos são definidos.

| ID | Responsável | Task e critério de aceite | Depende de |
|---|---|---|---|
| **I1** | Dev 1, com Dev 2 e Dev 3 | Registrar os contratos de referência, frame, política e resultado de passo. O motor consegue entregar à saída a posição, a referência, os frames e os dados do acesso ou da evicção. | — |
| **D1.1** | Dev 1 | Criar o Makefile. `make` gera `pager` em Linux usando C++17. | — |
| **D1.2** | Dev 1 | Implementar argumentos e leitura de `.trace`. Aceitar a entrada obrigatória, validar `N ≥ 1`, assumir leitura quando o tipo for omitido e ignorar linhas vazias e comentários. Aceitar traços vazios, rejeitar referências malformadas e a combinação de flags, e informar a linha para erros em referências. | I1 |
| **D1.3** | Dev 1 | Implementar o motor. Usar o menor frame livre, reutilizar o frame da vítima e despachar a política com `std::visit` e disponibilizar os frames e o resultado de cada passo. | I1 |
| **D1.4** | Dev 1 | Calcular referências, faltas, taxa, evicções, escritas em disco e páginas finais por frame. Contar escrita somente na evicção de página modificada e usar taxa de 0% quando o traço estiver vazio. | D1.3 |
| **D2.1** | Dev 2 | Implementar FIFO, LRU e Ótimo, com uma `struct` por política. FIFO usa ordem de carga, LRU usa último acesso e Ótimo usa próximo acesso, priorizando páginas sem acesso futuro. | I1 |
| **D2.2** | Dev 2 | Implementar Clock e NRU. Clock inicia o ponteiro no frame 0 e o move somente na evicção. NRU usa a classe `2·R + M` e zera R depois de cada quarta referência. | I1 |
| **D2.3** | Dev 2 | Implementar LFU e MFU. A frequência conta os acessos desde a carga e reinicia quando a página é carregada novamente. | I1 |
| **D2.4** | Dev 2 | Integrar as sete políticas e verificar a carga mais antiga nos empates entre vítimas elegíveis, além do reinício do estado entre execuções. | D1.3, D2.1, D2.2, D2.3 |
| **D3.1** | Dev 3 | Implementar resumo textual e `--verbose`. Exibir uma linha por referência, usar `.` para vagas e indicar a página removida e se houve escrita em disco. | D1.3, D1.4 |
| **D3.2** | Dev 3 | Implementar `--visual`. Exibir todos os frames, resultado do acesso e páginas que entram e saem. Guardar os frames exibidos e o resultado de cada passo. Usar `a`, `v` e `f` com Enter. Voltar e avançar novamente reproduz o mesmo estado, inclusive após executar até o fim. | D1.2, D1.3, D1.4 |
| **D3.3** | Dev 3 | Preparar `silberschatz2001.trace`, `belady.trace` e `escritas.trace` conforme o enunciado. | — |
| **I2** | Dev 1, Dev 2 e Dev 3 | Integrar primeiro com LRU e 3 frames no traço oficial. Conferir entrada, métricas, saída verbose e navegação visual. | D1.1, D1.2, D2.1, D3.1, D3.2, D3.3 |
| **D3.4** | Dev 3, com Dev 1 e Dev 2 | Rodar os 35 cenários de referência, comparar com as tabelas do enunciado e conferir a interface com um traço adicional. Cada responsável corrige divergências da sua área. | I2, D2.4 |
| **D3.5** | Dev 3, com Dev 1 e Dev 2 | Fechar README e documento de decisões. Explicar compilação, uso, comandos, estruturas, resultados, anomalia de Belady e localidade de referência. Cada desenvolvedor revisa os comentários do código que entregou. | D3.4 |

## Verificação da entrega

- Executar os sete algoritmos com 3 e 4 frames para cada um dos dois traços de leitura, mais os sete algoritmos com 3 frames em `escritas.trace`. São 35 execuções.
- No traço oficial com LRU e 3 frames, conferir 20 referências, 12 faltas, taxa de 60%, 9 evicções, nenhuma escrita e frames finais `[1, 0, 7]`.
- Em `belady.trace`, verificar o aumento de 9 para 10 faltas com FIFO, Clock e MFU ao passar de 3 para 4 frames, conforme a checagem de referência feita durante o planejamento.
- Usar um traço adicional com leituras e escritas para conferir a entrada variável e a navegação. Comparar o estado do mesmo passo em `--verbose` e `--visual`, inclusive depois de voltar e avançar. Ao executar até o fim, conferir as mesmas métricas do modo padrão.
- Conferir que traços vazios produzem métricas zeradas e que referências inválidas indicam a linha do erro. Verificar também a mensagem de uso ao combinar `--visual` com `--verbose`.
