red=`tput setaf 1`
green=`tput setaf 2`
reset=`tput sgr0`
echo Chequeando dependencias:
python -c 'import numpy' 2>/dev/null && echo "-${green}Numpy detectado${reset}" || (echo "-${red}No se detecto Numpy${reset}" && sh install-numpy.sh)
python -c 'import Bio.PDB' 2>/dev/null && echo "-${green}Biopython detectado${reset}" || (echo "-${red}No se detecto Biopython${reset}" && sh install-biopython.sh)
echo Compilando assembler
nasm -felf64 -g -F dwarf asa.asm
touch asa-mimd.c 
echo Solicitando permiso para instalar bibliotecas...
sudo python mimd-setup.py install 
sudo python c-setup.py install
echo ==============[Ejecutando tp]==============
python tp.py
