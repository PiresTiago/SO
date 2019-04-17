Estrutura das directorias existentes no arquivo:
.
├── CircuitRouter-ParSolver
│   ├── CircuitRouter-ParSolver.c
│   ├── Makefile
│   ├── coordinate.c
│   ├── coordinate.h
│   ├── grid.c
│   ├── grid.h
│   ├── inputs
│   │   ├── generate.py
│   │   ├── random-x128-y128-z3-n128.txt
│   │   ├── random-x128-y128-z3-n64.txt
│   │   ├── random-x128-y128-z5-n128.txt
│   │   ├── random-x256-y256-z3-n256.txt
│   │   ├── random-x256-y256-z5-n256.txt
│   │   ├── random-x32-y32-z3-n64.txt
│   │   ├── random-x32-y32-z3-n96.txt
│   │   ├── random-x48-y48-z3-n48.txt
│   │   ├── random-x48-y48-z3-n64.txt
│   │   ├── random-x512-y512-z7-n512.txt
│   │   ├── random-x64-y64-z3-n48.txt
│   │   └── random-x64-y64-z3-n64.txt
│   ├── maze.c
│   ├── maze.h
│   ├── router.c
│   ├── router.h
│   ├── sync.c
│   └── sync.h
├── CircuitRouter-SeqSolver
│   ├── CircuitRouter-SeqSolver.c
│   ├── Makefile
│   ├── coordinate.c
│   ├── coordinate.h
│   ├── grid.c
│   ├── grid.h
│   ├── inputs
│   │   ├── generate.py
│   │   ├── random-x128-y128-z3-n128.txt
│   │   ├── random-x128-y128-z3-n64.txt
│   │   ├── random-x128-y128-z5-n128.txt
│   │   ├── random-x256-y256-z3-n256.txt
│   │   ├── random-x256-y256-z5-n256.txt
│   │   ├── random-x32-y32-z3-n64.txt
│   │   ├── random-x32-y32-z3-n96.txt
│   │   ├── random-x48-y48-z3-n48.txt
│   │   ├── random-x48-y48-z3-n64.txt
│   │   ├── random-x512-y512-z7-n512.txt
│   │   ├── random-x64-y64-z3-n48.txt
│   │   └── random-x64-y64-z3-n64.txt
│   ├── maze.c
│   ├── maze.h
│   ├── router.c
│   └── router.h
├── CircuitRouter-SimpleShell
│   ├── CircuitRouter-SimpleShell.c
│   └── Makefile
├── Makefile
├── README.txt
├── doTest.sh
├── lib
│   ├── commandlinereader.c
│   ├── commandlinereader.h
│   ├── list.c
│   ├── list.h
│   ├── pair.c
│   ├── pair.h
│   ├── queue.c
│   ├── queue.h
│   ├── timer.h
│   ├── types.h
│   ├── utility.h
│   ├── vector.c
│   └── vector.h
└── results
    ├── random-x128-y128-z3-n128.txt.speedups.csv
    ├── random-x128-y128-z3-n64.txt.speedups.csv
    ├── random-x128-y128-z5-n128.txt.speedups.csv
    ├── random-x256-y256-z3-n256.txt.speedups.csv
    ├── random-x256-y256-z5-n256.txt.speedups.csv
    ├── random-x32-y32-z3-n64.txt.speedups.csv
    ├── random-x32-y32-z3-n96.txt.speedups.csv
    ├── random-x48-y48-z3-n48.txt.speedups.csv
    ├── random-x48-y48-z3-n64.txt.speedups.csv
    ├── random-x64-y64-z3-n48.txt.speedups.csv
    └── random-x64-y64-z3-n64.txt.speedups.csv

7 directories, 78 files


COMPILAR PROJETO:
	Abrir terminal no diretório deste ficheiro e executar o seguinte comando: make

EXECUTAR PROJETO:
	Abrir terminal no diretório deste ficheiro e executar os seguintes comandos:
	Para executar utilizado "doTest.sh"
		 ./doTest.sh <numero max de tarefas> <ficheiro de input>
	Para executar manualmente
		1: cd CircuitRouter-ParSolver/
		2: ./CircuitRouter-ParSolver -t <numero de tarefas> <ficheiro de input>
		

SISTEMA OPERATIVO:	
	Ubuntu 18.04 (4.15.0-38-generic #41-Ubuntu SMP Wed Oct 10 10:59:38 UTC 2018 x86_64 x86_64 x86_64 GNU/Linux)


PROCESSADOR:	
	Intel(R) Core(TM) i5-7300HQ CPU @ 2.50GHz
	Cores: 4


NOTAS SOBRE O PROJETO:
	Função pathLOCK - Bloqueia os mutexes referentes aos pontos do caminho calculado, após o bloqueio do mutex de um ponto verifica-se se o ponto na grelha está vazio, se não estiver é enviada uma flag para o router_solve para que os mutexes até então bloqueados sejam desbloqueados antes de efetuar uma nova tentativa para calcular um caminho.
	Caso um mutex de um ponto desse caminho já esteja bloqueado, a função executa um backoff com um nanosleep de 100 a 1000 nanosegundos verificando de seguida se pode bloquear o mutex do ponto,se após 4 tentativas não for possível efetuar o lock do mutex, é enviada uma flag para o router_solve para que os mutexes até então bloqueados sejam desbloqueados antes de efetuar uma nova tentativa para calcular um caminho.

	Função pathUNLOCK - Desbloqueia os mutexes referentes aos pontos do caminho recebido.

	Função mutexGridAux - Devolve o offset para o array de mutexes, este offset corresponde a um ponto da grelha.
