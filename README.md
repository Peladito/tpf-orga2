#Paralelizacion de calculo SASA

##adicional: benchmarking comparativo de estrategias SISD, SIMD, MISD , MIMD y CUDA

###Descripcion

El proyecto busca explorar una implementacion paralela del calculo superficial de las moleculas (SASA) de forma paralela, para poder disminuir el tiempo de su computo. Ademas, se realizo un pequeño benchmark entre distintas estrategias de computo.
Para mas informacion, los informes sse pueden encontrar en la carpeta informes :P

###Ejecucion

Para ejecutar el proyecto, solo basta con ejecutar el archivo installNrun.sh, el script instalara las dependencias necesarias: De no funcionar se deben seguir los siguientes puntos:

1. Descargar e instalar la biblioteca de python [Biopython](http://biopython.org/wiki/Download). 
1. Compilar asa.asm con la siguiente instrucción: nasm -felf64 -g -F dwarf asa.asm
1. Instalar proyecto con la instrucción:sudo python mimd-setup.py install

Para poder correr la captura de tiempos del algoritmo SISD versus MIMD, en donde se encuentra un ejemplo de aplicación, se debe hacer lo siguiente:

1. Instalar el plugin correspondiente al código de asa calculado con SISD: sudo python c-setup.py install
1. Asegurar la presencia del archivo <CARPETA TP>/datasource/proteina.pdb
1. Ejecutar el tp: python tp.py


