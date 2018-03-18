from Bio.PDB import *
from Atomo import Atomo
import asac
import asaMIMD as asami
import timeit

 #Funciones interfaces para la biblioteca timeit
def calcular_asa_mimd(la, lr, th):
  return asami.calcular(la, lr, th)
def calcular_asa_c(la, lr):
  return asac.calcular(la, lr)

#Funcion interfaz entre metodo asa y objeto proteina de biopython
def getParametros(proteina):
 listaA = []
 lista_radios = []
 listaAtomo = []
 for model in proteina:
     for chain in model:
         for residue in chain:
           hetfield = residue.get_id()
           if hetfield[0]!="W":
             for atom in residue.get_unpacked_list():
               a = Atomo(atom)
               listaAtomo.append(a)
               listaA.append([float(a.coordenadas[0]),float(a.coordenadas[1]),float(a.coordenadas[2])])
               lista_radios.append(a.radio)
               #if len(listaA)>1:

 return listaA, lista_radios, listaAtomo
               

 

#Se obtiene la proteina de la carpeta datasource
parser = PDBParser()
structure = parser.get_structure("proteina","datasource/proteina.pdb")

#Se obtienen los parametros necesarios para la funcion
lista_atomos, lista_radios, listaAtomo = getParametros(structure)

th= 6
print("Midiendo tiempos del algoritmo SISD con 10 iteraciones")
print("Resultado SISD: "+str(timeit.timeit("tp.calcular_asa_c(tp.lista_atomos, tp.lista_radios)", setup="import tp", number=10))+" seg.")
print("Midiendo tiempos del algoritmo MIMD con 10 iteraciones")
print("Resultado MIMD: "+str(timeit.timeit("tp.calcular_asa_mimd(tp.lista_atomos, tp.lista_radios, tp.th)", setup="import tp", number=10))+ " seg.")



