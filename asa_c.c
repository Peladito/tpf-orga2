#include <Python.h>
#include <math.h>
#include <stdio.h>

FILE *f;

void generar_esfera(int n, float puntos[n][3]) {

	float inc = M_PI * (3-sqrt(5));
	float offset2 = 2/(float)n;
	float offset = offset2/2;
	float y, r, phi;
	int k = 0;
	for(k = 0; k<n; k++){
		y = k * offset2 -1 + offset;
		r = sqrt(1-y*y);
		phi = k * inc;
		puntos[k][0] = cos(phi)*r;
		puntos[k][1] = y;
		puntos[k][2] = sin(phi)*r;
	}

}
struct nodo{
	struct nodo* next;
	int elem;
};
struct lista{
	struct nodo* cabeza;
	int n_elem;
};

struct lista* obtener_vecinos(int probe, int pos, PyObject* listaAtomos,PyObject* listaRadios){
	//Voy a tener que contar la cantidad, ubicar la memoria y volverlos a encontrar
	//Voy a disminuir los pasos guardando los que encuentro en una lista
	int cant = 0;
	int n = PyList_Size(listaAtomos); //vantidad total de atomos
	struct lista *l = malloc( sizeof(struct nodo) );
	struct nodo *cabeza = NULL;
	struct nodo *actual;
	actual = malloc( sizeof(struct nodo) );
	cabeza = actual;
	l->cabeza = cabeza;
	l->n_elem = 0;


	//Recupero las coordenadas y el radio de mi atomo
	float r_a;
	float x_a; 
	float y_a;
	float z_a; 
	PyArg_Parse(PyList_GET_ITEM(listaRadios, pos), "f", &r_a);
	r_a = r_a*2 + 2*probe;

	PyArg_Parse(PyList_GET_ITEM(PyList_GET_ITEM(listaAtomos, pos), 0), "f",&x_a );	
	PyArg_Parse(PyList_GET_ITEM(PyList_GET_ITEM(listaAtomos, pos), 1), "f",&y_a );
	PyArg_Parse(PyList_GET_ITEM(PyList_GET_ITEM(listaAtomos, pos), 2), "f",&z_a );
	int j = 0;

	for(j=0;j<n;j++)
	{
		
		//Recupero el radio y coordenadas de cada atomo a probar
		float r;
		PyArg_Parse(PyList_GET_ITEM(listaRadios, j), "f", &r);

		double x_j;
		double y_j;
		double z_j;
		PyArg_Parse(PyList_GET_ITEM(PyList_GET_ITEM(listaAtomos, j), 0), "d",&x_j );
		PyArg_Parse(PyList_GET_ITEM(PyList_GET_ITEM(listaAtomos, j), 1), "d",&y_j );
		PyArg_Parse(PyList_GET_ITEM(PyList_GET_ITEM(listaAtomos, j), 2), "d",&z_j );

		//Calculo la norma 2 al cuadrado
		x_j = pow(x_j - x_a,2);
		y_j = pow(y_j - y_a,2);
		z_j = pow(z_j - z_a,2);

		double dist_2 = x_j + y_j + z_j;
		//Y en el caso de estar cerca, agrego el indice por el cual lo encuentro en la lista
		//Y lo cuento como vecino
		if(dist_2 < pow(r_a + r,2) && dist_2>0.0001)
		{
			cant++;
			actual->elem = j;
			actual->next = malloc( sizeof(struct nodo) );
			actual = actual->next;

		}

	}
	l->n_elem = cant;

	return l;
}
//n son la cantidad de puntos en la golden sphere
void transformar_a_matriz_extendiendo_a_columnas(int n, int cant_vecinos, struct lista* list,PyObject* listaAtomos, PyObject* listaRadios, float x[n][cant_vecinos], float y[n][cant_vecinos], float z[n][cant_vecinos], float r[cant_vecinos]){

	struct nodo* cabeza = list->cabeza;
	struct nodo* actual = list->cabeza;
	int i = 0, j=0;
	for (j = 0; j < n; j++)//repito el valor para todos los puntos de la esfera
	{
		for(i = 0; i<cant_vecinos; i++)//recorro todos los vecinos
		{

			int pos = actual->elem;
			actual = actual->next;
			if (j==0)
			{
				//recorro 1 sola vez para setear los radios
				PyArg_Parse(PyList_GET_ITEM(listaRadios, pos),"f",&(r[i]));
			}
					
			PyArg_Parse(PyList_GET_ITEM(PyList_GET_ITEM(listaAtomos, pos), 0),"f",&(x[j][i]));//comentario viejo//*x se refiere al array (*x)[i] a la posicion del array &((*x)[i]) puntero a la posicion i
			PyArg_Parse(PyList_GET_ITEM(PyList_GET_ITEM(listaAtomos, pos), 1),"f",&(y[j][i]));
			PyArg_Parse(PyList_GET_ITEM(PyList_GET_ITEM(listaAtomos, pos), 2),"f",&(z[j][i]));
			
		}
		actual = list->cabeza;
	}
	for (i = 0; i <= list->n_elem; i++)//hay n+1 elementos
	{
		actual = cabeza->next;
		free(cabeza);
		cabeza = actual;
	}

	free(list);

}
//Ajustar los puntos de golden al centro y radio del atomo objetivo, se guarda todo en los 3 primeeros arreglos parametrizados
//extender los puntos a toda la fila: n corresponde al numero de puntos de la golden sphere y cant a la cantidad de vecinos del punto
void transformar_puntos_y_extender_a_fila(int n, int cant_vecinos, float xs[n][cant_vecinos], float ys[n][cant_vecinos], float zs[n][cant_vecinos], float p_g_sph[n][3], PyObject* coord_punto, PyObject* radio_punto, float probe){
	//inicializar radio
	float rad;
	PyArg_Parse(radio_punto, "f", &rad);
	float radio= rad + probe;
	//inicializar terna x,y,z del punto
	float x, y, z, x_i, y_i, z_i; 
	PyArg_Parse(PyList_GET_ITEM(coord_punto, 0), "f", &x);
	PyArg_Parse(PyList_GET_ITEM(coord_punto, 1), "f", &y);
	PyArg_Parse(PyList_GET_ITEM(coord_punto, 2), "f", &z);

	int i = 0,j=0;
	for (i = 0; i < n; i++)
	{
		//printf("checkpoint 1.1 paso:%d\n",i);
		x_i = p_g_sph[i][0] * radio + x;
		y_i = p_g_sph[i][1] * radio + y;
		z_i = p_g_sph[i][2] * radio + z;
		//fprintf(f, "[%f, %f, %f] %f\n",x_i, y_i, z_i, radio);
		for (j=0; j<cant_vecinos; j++){
			xs[i][j] = x_i;
			ys[i][j] = y_i;
			zs[i][j] = z_i;
		}
	}
}


void restar(int filas, int columnas, float x[filas][columnas], float x_1[filas][columnas]){
	int i=0, j=0;
	for (i = 0; i < filas; i++)
	{
		for (j = 0; j < columnas; j++)
		{
			x[i][j] -= x_1[i][j];
		}
	}
}

void al_cuadrado(int filas, int columnas, float x[filas][columnas]){
	int i=0, j=0;
	for (i = 0; i < filas; i++)
	{
		for (j = 0; j < columnas; j++)
		{
			x[i][j] = pow(x[i][j],2);
		}
	}
}

void sumar(int filas, int columnas,float res[filas][columnas],float x[filas][columnas],float y[filas][columnas],float z[filas][columnas]){
	int i=0, j=0;
	for (i = 0; i < filas; i++)
	{
		for (j = 0; j < columnas; j++)
		{
			res[i][j] = x[i][j] + y[i][j] + z[i][j];
		}
		
	}
}
void imprimir(int filas, int columnas, float x[filas][columnas],float y[filas][columnas],float z[filas][columnas]){
	int i=0, j=0;
	for (i = 0; i < filas; i++)
	{
		fprintf(f, "%d)", i);
		for (j = 0; j < columnas; j++)
		{
			fprintf(f, "[%f, %f, %f] ", x[i][j], y[i][j], z[i][j]);


		}
		fprintf(f, "\n");
	}
}

void comparar_mayor_igual(int n,int cant_vecinos ,float x_puntos_atomo[n][cant_vecinos],float probe,float r[cant_vecinos]){
	int i=0,j=0,ret=0;

	for (i = 0; i < n; i++)
	{
		ret = 1;
		
		for (j = 0; j < cant_vecinos; j++)
		{
			float d1 = pow(r[j] + probe,2);
			if (x_puntos_atomo[i][j]<d1)
			{
				//fprintf(f, "%d %f\n", 0, x_puntos_atomo[i][j]-d1);
				ret = 0;
			}else{
				//fprintf(f, "%d %f\n", 1, x_puntos_atomo[i][j]-d1);
			}
			//	break;//hay contacto, ya no lo considerto mas
		}
		x_puntos_atomo[i][0] = ret;
	}	

}


int sumar_elementos(int n,int cant_vecinos ,float x_puntos_atomo[n][cant_vecinos]){
	int i=0, ret=0;
	for (i = 0; i < n; i++)
	{
		ret+=x_puntos_atomo[i][0];
	}
	
	return ret;
}

static PyObject *module_func(PyObject *self, PyObject *args) {
	PyObject *list_coordenadas;//aca recibo la lista de atomos. Tiene que ser 4 valores por atomo, para indicar el radio de cada uno
	PyObject *list_radios;

	if (!PyArg_ParseTuple(args, "OO", &list_coordenadas, &list_radios)) {
		return Py_BuildValue("i", 2);
	}

	//printf("Inicio del modulo\n");
	if(PyList_Size(list_coordenadas) == 0 && PyList_Size(list_radios) == 0){
		PyErr_SetString(PyExc_ValueError, "Error: Listas vacias");
		return NULL;
	}


	if(PyList_Size(list_coordenadas) != PyList_Size(list_radios)){
		PyErr_SetString(PyExc_ValueError, "Error: Cantidad de elementos en las listas no coinciden");
		return NULL;
	}
	
	if(!PyArg_Parse(list_coordenadas, "O", &PyList_Type)){
		PyErr_SetString(PyExc_ValueError, "Error: EL primer parametro debe recibir [[f,f,f]]");
		return NULL;
	}
	//float f; //ojito ojota que con las coordenadas vamos a tener el problema de granularidad grande con perdida de presicion en los floats
	//PyArg_Parse(PyList_GET_ITEM(list, 0), "f", &f);
	//printf("%i - %f\n", PyList_Size(list), f);

	int tot_atomos = PyList_Size(list_coordenadas);
	int n = 900; //puntos de discretizacion golden sphere

	float probe = 1.4; //Ver dato real, u obtener como parametro
	float puntos_golden_sphere[n][3];
	generar_esfera(n, puntos_golden_sphere);

	float constante = 4.0 * M_PI / n;
	float areaTotal = 0.0;
	float area = 0.0;
	int i = 0;


    //f = fopen("puntos-sin-sumar.txt", "w");


	for (i = 0; i < tot_atomos; i++)//definir tot_atomos
	{		
		
		//printf("atomo nº %d de %d | area calculada: %f \r ",i, tot_atomos, areaTotal);
		// Obtengo los vecinos a mi atomo
		struct lista* lista_vecinos = obtener_vecinos(probe, i, list_coordenadas, list_radios);
		int cant_vecinos = lista_vecinos->n_elem;
		//Coordenadas x, y ,z de los distintos puntos repetidos por columnas
		float x[n][cant_vecinos];// 20*900
		float y[n][cant_vecinos];
		float z[n][cant_vecinos];
		float r[cant_vecinos];
		//Completo todos los datos en formato matriz repitiendo cada coordenada por columna (osea n repeticiones [n filas])
		transformar_a_matriz_extendiendo_a_columnas(n, cant_vecinos, lista_vecinos, list_coordenadas, list_radios, x, y, z, r);//aca se borra la lista
		
		//Desde aca se pasa a un pipeline en assembler------------------------------------
		//--------------------------------------------------------------------------------

		//Coordenadas de la golden_sphere separados por coordenadas y adecuados a la posicion y tamaño de mi atomo
		float x_puntos_atomo[n][cant_vecinos];//900*20
		float y_puntos_atomo[n][cant_vecinos];
		float z_puntos_atomo[n][cant_vecinos];
		//A partir de los puntos de golden_sphere, utilizo como plantilla y lo adecuo a mi atomo: cada dato lo repito a lo largo de cada fila
		transformar_puntos_y_extender_a_fila(n, cant_vecinos, x_puntos_atomo, y_puntos_atomo, z_puntos_atomo, puntos_golden_sphere, PyList_GET_ITEM(list_coordenadas, i), PyList_GET_ITEM(list_radios, i), probe);//el 3er parametro debe ser un puntero a float
		

		restar(n, cant_vecinos, x ,x_puntos_atomo);//queda guardado en el vector x
		restar(n, cant_vecinos, y, y_puntos_atomo);
		restar(n, cant_vecinos, z, z_puntos_atomo);


		al_cuadrado(n, cant_vecinos, x);//potencia 2 de cada elemento
		al_cuadrado(n, cant_vecinos, y);
		al_cuadrado(n, cant_vecinos, z);

		//imprimir(n, cant_vecinos, x, y, z);

		sumar(n, cant_vecinos, x_puntos_atomo, x, y, z);//suma entre todas las matrices, uso variable vieja para no relocar memoria
		comparar_mayor_igual(n, cant_vecinos ,x_puntos_atomo, probe, r);// Compara cada elemento con probe y pone un 1 si es mayor o igual

		float radio ;
		PyArg_Parse(PyList_GET_ITEM(list_radios, i),"f",&radio);
		radio+=probe;
		int puntos = sumar_elementos(n, cant_vecinos, x_puntos_atomo);
		area = puntos * radio * radio * constante;
		//printf("C: vecinos: %d puntos: %d area: %f radio: %f\n",cant_vecinos ,puntos, area, radio);
		areaTotal += area;// sumo el area libre del atomo al total del conjunto
	
	}


	//return Py_BuildValue("i", i); //funciona!!
	//fclose(f);
	return Py_BuildValue("f", areaTotal);//funciona!!
	
}
/*
static char helloworld_docs[] =
    "helloworld( ): Any message you want to put here!!\n";
*/
    static PyMethodDef module_methods[] = {
    	{ "calcular", module_func, METH_VARARGS, NULL },
    	{ NULL, NULL, 0, NULL }
    };

    void initasac(void)
    {
    	Py_InitModule3("asac", module_methods, "Calculo de ASA");
    }
