La descripcion del proyecto se halla en la carpeta informes

Para ejecutar el proyecto, solo basta con ejecutar el archivo installNrun.sh, el script instalara las dependencias necesarias: De no funcionar se deben seguir los siguientes puntos:

-Descargar e instalar la biblioteca de python Biopython desde http://biopython.org/wiki/Download
-Compilar asa.asm con la siguiente instrucción: nasm -felf64 -g -F dwarf asa.asm
-Instalar proyecto con la instrucción:sudo python mimd-setup.py install

Para poder correr la captura de tiempos del algoritmo SISD versus MIMD, en donde se encuentra un ejemplo de aplicación, se debe hacer lo siguiente:

-Instalar el plugin correspondiente al código de asa calculado con SISD: sudo python c-setup.py install
-Asegurar la presencia del archivo <CARPETA TP>/datasource/proteina.pdb
-Ejecutar el tp: python tp.py


