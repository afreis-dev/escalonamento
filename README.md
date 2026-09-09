# Escalonamento de tarefas críticas de voo

Simulador de escalonamento preemptivo de tarefas periódicas com deadline, da disciplina de Infraestrutura de Software (Implementação 3). Compara os algoritmos rate-monotonic (`rate`) e earliest-deadline-first (`edf`).

## Compilar

Requer `gcc` e `make` (Linux, Unix, macOS ou WSL).

    make

Limpar:

    make clean

## Executar

    ./scheduler <rate|edf> <arquivo>

Exemplo:

    ./scheduler rate voo.txt
    ./scheduler edf voo.txt

O arquivo de entrada tem o tempo total de simulação na primeira linha e uma tarefa por linha nas seguintes, no formato `NOME PERIODO DEADLINE BURST`. O resultado é gravado em `rate_afsr.out` ou `edf_afsr.out`, conforme o algoritmo escolhido. Nada é impresso na saída padrão durante a execução normal; entradas inválidas geram mensagem em stderr e código de saída diferente de zero, sem criar arquivo de saída.

## Testar

Não há alvo `test` no Makefile. A validação foi feita rodando o exemplo do enunciado com os dois algoritmos e conferindo a saída contra o rastro de execução descrito no PDF, além dos casos de entrada inválida exigidos (argumentos incorretos, algoritmo inválido, arquivo inexistente, campo faltando, valor não numérico ou não positivo, violação de C ≤ D ≤ P). Os comandos reais e as saídas dessa verificação estão em `evidencias.log`.

## Arquivos

| Arquivo | Responsabilidade |
|---|---|
| `main.c` | Lê os argumentos, decide o algoritmo, chama a leitura do arquivo e a simulação |
| `io.c` | Leitura e validação do arquivo de tarefas |
| `sim.c` | Motor de simulação preemptivo (rate-monotonic e edf) e escrita do arquivo de saída |
| `scheduler.h` | Structs de tarefas e protótipos compartilhados |

## Ambiente

Desenvolvido e testado no WSL2 (Ubuntu), Windows 11.

## Autor

afsr@cesar.school
