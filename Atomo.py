import numpy
class Atomo:
	id=""
	coordenadas=[0.0,0.0,0.0]
	tipo = ""
	radio = 0
	beta = 0
	cw_id = "" #identificador de padre
	resid = 0
	
	def __init__(self, biopython_atom=None):

		if biopython_atom!=None:
			self.id=biopython_atom.get_id()
			self.name = biopython_atom.get_name()
			residue = biopython_atom.get_parent()
			self.resid = residue.get_resname()
			#print(biopython_atom.get_name()+" "+str(biopython_atom.get_full_id()))
			self.beta = biopython_atom.get_bfactor()
			self.coordenadas = biopython_atom.get_coord()
			self.tipo=self.id[0]
			if self.tipo=="C":
				self.radio = 1.70
			elif self.tipo=="H":
				self.radio = 0.79
			elif self.tipo=="O":
				self.radio = 1.52
			elif self.tipo=="N" :
				self.radio = 1.55
			elif self.tipo=="S" :
				self.radio = 1.80
			elif self.tipo=="P" :
				self.radio = 1.23
			elif self.tipo=="FE" :
				self.radio = 1.72
			elif self.tipo=="CU" :
				self.radio = 1.57
			elif self.tipo=="MG" :
				self.radio = 1.72
			elif self.tipo=="I" :
				self.radio = 1.32
		else:
			self.id=""
			self.coordenadas=[0.0,0.0,0.0]
			self.tipo = ""
			self.radio = 0
			self.beta = 0

		self.cw_id=""

	def __sub__(self, other):
		return numpy.sqrt(numpy.sum([(x-y)**2 for x,y in zip(self.coordenadas, other.coordenadas)]))