# encoding: utf-8

__doc__ = """ 
Calculate the accessible-surface area of atoms.
Uses the simple Shrake-Rupley algorithm, that generates a
relatively uniform density of dots over every atoms and
eliminates those within the sphere of another atom. The remaining
dots is used to calculate the area.
Reference: A. Shrake & J. A. Rupley. "Environment and Exposure to
Solvent of Protein Atoms. Lysozyme and Insulin." J Mol Biol. 79
(1973) 351- 371. """

import math
import numpy

f = None

def generate_sphere_points(n):
  """
  Returns list of coordinates on a sphere using the Golden-
  Section Spiral algorithm.
  """
  points = []
  inc = math.pi * (3 - math.sqrt(5))
  offset = 2 / float(n)
  for k in range(int(n)):
    y = k * offset - 1 + (offset / 2)
    r = math.sqrt(1 - y*y)
    phi = k * inc
    points.append([math.cos(phi)*r, y, math.sin(phi)*r])
  return points


def find_neighbor_indices(atoms, probe, k):
  """
  Returns list of indices of atoms within probe distance to atom k. 
  """
  neighbor_indices = []
  atom_k = atoms[k]
  radius = atom_k.radio + probe + probe
  indices = range(k)
  indices.extend(range(k+1, len(atoms)))
  for i in indices:
   atom_i = atoms[i]
   dist = atom_k - atom_i
   if dist < radius + atom_i.radio:
     neighbor_indices.append(i)
  return neighbor_indices


def calculate_asa(atoms, probe, n_sphere_point=960):
 """
 Returns the accessible-surface areas of the atoms, by rolling a
 ball with probe radius over the atoms with their radius
 defined.
 """
 #global f
 #f = open("pytable.txt", 'w')

 sphere_points = generate_sphere_points(n_sphere_point)
 #i=0
 #for p in sphere_points:
 # f.write(str(i)+")")
 # imprimir(p)
 # f.write("\n")


 const = 4.0 * math.pi / len(sphere_points)
 areas = []
 for i, atom_i in enumerate(atoms):

   neighbor_indices = find_neighbor_indices(atoms, probe, i)
   n_neighbor = len(neighbor_indices)
   j_closest_neighbor = 0
   radius = probe + atom_i.radio
   
   n_accessible_point = 0
   pos = 0
   for point in sphere_points:
     #f.write(str(pos)+")")
     pos=pos+1
     is_accessible = True
#el siguiente ciclo lo unico que hace es centrar los puntos en el atomo objetivo y desplazarlos al radio de prueba
     test_point = [y+z for y, z in zip([x*radius for x in point], atom_i.coordenadas)]
 #Este codigo agarra el indice mas cercano de la iteracion anterior y lo pone como primero
     cycled_indices = range(j_closest_neighbor, n_neighbor)
     cycled_indices.extend(range(j_closest_neighbor))
     
##La iteracion se minimiza al chequear el atomo mas cercano de la anterior iteracion
 #Si hay duda.. evaluar el pedazo que utiliza range para veer lo que hace
 #Por ahi se pueda aplicar la misma idea con el find_neighbouring_indices
     for j in cycled_indices:
       atom_j = atoms[neighbor_indices[j]]
       r = atom_j.radio + probe
       #imprimir(atom_j.coordenadas, test_point)
       diff = distancia(atom_j.coordenadas, test_point)

       if diff < r*r:
         j_closest_neighbor = j
         is_accessible = False

         #break

    # f.write("\n")
     if is_accessible:
       n_accessible_point += 1
   
   area = const*n_accessible_point*radius*radius 
   #print(atoms[i].coordenadas)
   #print(" P: vecinos: "+ str(len(cycled_indices)) +" npgs: "+str(len(sphere_points))+" puntos: "+str(n_accessible_point)+" area: " + str(area)+ " radio: "+str(radius))
   areas.append(area)

 return numpy.sum(areas)  
def distancia(lista1, lista2):
  return numpy.sum([(x-y)**2 for x,y in zip(lista1, lista2)])

def imprimir(lista1, lista2):
  global f
  #f.write("["+str(lista2[0])+", "+str(lista2[1])+", "+str(lista2[2])+"] "+str(fl))
  #f.write("["+str(lista1[0])+", "+str(lista1[1])+", "+str(lista1[2])+"]")
  #f.write("["+str((lista1[0]-lista2[0]))+", "+str((lista1[1]-lista2[1]))+", "+str((lista1[2]-lista2[2]))+"]")
  f.write("["+str((lista1[0]-lista2[0])**2)+", "+str((lista1[1]-lista2[1])**2)+", "+str((lista1[2]-lista2[2])**2)+"]")

def imprimir_lista(l):
  i=0
  for p in l:
    f.write(str(i)+")")
    imprimir(p)
    f.write("\n")

def getAtomos(proteina):
 listaA = []
 for model in proteina:
     for chain in model:
         for residue in chain:
           hetfield = residue.get_id()
           if hetfield[0]!="W":
             for atom in residue.get_unpacked_list():
               a = Atomo(atom)
               listaA.append(a)

 
 return listaA
