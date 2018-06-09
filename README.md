# arduino-esp8266-restsrv-ledresource

V:1.0 | 08/jun/2018

Este programa permite controlar as portas digitais do dispositivo NODEMCU 1.0(Esp-12 MODULE) - esp8266 WIFII
------------------------------------------------------------------------------------------------------------

Como usar:

No Postman, informar os seguintes parametros:

=>HEADER
Content-Type:application/json

=>BODY
{"id":?A, 
 "gpio":?B,
 "action":?C,
 "status":?D
 }
 
 Sendo:
 ?A = BYTE - 1 a 255. Identificador do Recurso
 ?B = BYTE - 1 a 8 : Identifica uma porta digital do dispositivo
 ?C = BYTE - 1 ou 2: 1, Ação de inicializar o recurso; 2, Ação de ligar/desligar o recurso
 ?D = BYTE - 0 ou 1: 0, Desliga; 1, Liga
 
São disponibilizados 2 métodos, o GET que permite exibir os recursos inicializados e o método POST/PUT para executar o recurso

O método POST/PUT deve ser executado em duas etapas, ou requisição, sendo a primeira para inicializar um recurso:
ex>
{"id":1, 
 "gpio":5,
 "action":1,
 "status":0
 }

e em seguida uma nova requisição para ligar/desligar o led:
Liga>
{"id":1, 
 "gpio":5,
 "action":2,
 "status":1
 }
 
 Desliga>
 {"id":1, 
 "gpio":5,
 "action":2,
 "status":0
 }
 
 Podem ser inicializados e controlados (neste programa) até 8 saidas.
 
 ----------------------------------------------------------------------------------
 http://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html
 
 



 
