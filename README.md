# Semáforo com FreeRTOS

## Descrição
Este projeto tem como objetivo trabalhar os conceitos de execução de tarefas em paralelo com o freeRTOS no raspberry pi pico w acopladpo na BitDogLab.

## Como Executar
1. Clone o repositório:
   ```bash
   git clone https://github.com/Daniel-Alencar/BDL-freertos-semaforo
   ```
2. Navegue até o diretório do projeto:
   ```bash
   cd BDL-freertos-semaforo
   ```
3. Abra o projeto no VS Code.
  
4. Compile e execute o projeto usando a placa BitDogLab.

5. Perceba o funcionamento do semáforo com o uso de multitarefas no freeRTOS. As indicações do semáforo são realizadas por meio do led RGB, da matriz de leds, do display oled e do buzzer (para trazer acessibilidade). O buzzer realiza bits de diferentes durações para cada cor do semáforo e cada modo do semáforo (modo normal e modo noturno).

## Vídeo de Demonstração
```bash
https://youtu.be/oqJsWmTG4Qc
```

Além disso, este é o [relatório do projeto](https://docs.google.com/document/d/1SpsMGkHY2brc4XM7Fd_koWpEmXC7DARU/edit?usp=sharing&ouid=104431951264694328022&rtpof=true&sd=true) com mais detalhes da implementação.

OBS.: O projeto foi rodado no linux. O freeRTOS é configurado no CMakeLists.txt apontando para o path do freeRTOS instalado. Pode ser que seja necessário mudar essa configuração a depender de onde o freeRTOS é instalado.


Abaixo temos a configuração mencionada:
```
set(FREERTOS_KERNEL_PATH "/home/engenheiro/FreeRTOS-Kernel")
```

## Licença
Este projeto é licenciado sob a licença MIT – consulte o arquivo LICENSE para mais detalhes.
