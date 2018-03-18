#include <Python.h>
#include <math.h>
#include <stdio.h>
#include <pthread.h>

extern void restar_y_al_cuadrado(int n, int cant_vecinos, float x[n][cant_vecinos],float x_a[n][cant_vecinos], float res[n][cant_vecinos]);
extern void restar_y_al_cuadrado_ymm(int n, int cant_vecinos, float x[n][cant_vecinos],float x_a[n][cant_vecinos], float res[n][cant_vecinos]);
extern void sumar(int n, int cant_vecinos, float r[n][cant_vecinos],float x[n][cant_vecinos], float y[n][cant_vecinos], float z[n][cant_vecinos]);
void* procesar_data(void * arg);
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

struct data{
	PyObject* list_coordenadas; 
	PyObject* list_radios;
	int n, ini, fin;
	float probe, **p_g_sph, res;

};

struct lista* obtener_vecinos(int probe, int pos, PyObject* listaAtomos,PyObject* listaRadios){
	//Voy a tener que contar la cantidad, ubicar la memoria y volverlos a encontrar
	//Voy a disminuir los pasos guardando los que encuentro en una lista
	int cant = 0;
	//vantidad total de atomos
	int n = PyList_Size(listaAtomos); 
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
	for (j = 0; j < n; j++)
	{
		for(i = 0; i<cant_vecinos; i++)
		{

			int pos = actual->elem;
			actual = actual->next;
			if (j==0)
			{
				//recorro 1 sola vez para setear los radios
				PyArg_Parse(PyList_GET_ITEM(listaRadios, pos),"f",&(r[i]));
			}

			PyArg_Parse(PyList_GET_ITEM(PyList_GET_ITEM(listaAtomos, pos), 0),"f",&(x[j][i]));
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

void comparar_mayor_igual(int n,int cant_vecinos ,unsigned short aux[n], float x_puntos_atomo[n][cant_vecinos],float probe,float r[cant_vecinos]){
	int i=0,j=0,ret=0;

	for (i = 0; i < n; i++)
	{
		ret = 1;

		for (j = 0; j < cant_vecinos && ret == 1; j++)
		{
			float d1 = pow(r[j] + probe,2);
			if (x_puntos_atomo[i][j]<d1)
			{
				ret = 0;
			}
		}
		aux[i] = ret;
	}	

}


int sumar_elementos(int n,int cant_vecinos ,unsigned short puntos[cant_vecinos]){
	int i=0, ret=0;
	for (i = 0; i < n; i++)
	{
		ret+=puntos[i];
	}

	return ret;
}

int asa_func_pipeline(int n, int cant_vecinos,  float probe, unsigned short aux[n], float x[n][cant_vecinos],float y[n][cant_vecinos],float z[n][cant_vecinos],float x_a[n][cant_vecinos],float y_a[n][cant_vecinos],float z_a[n][cant_vecinos], float r[cant_vecinos]){

	float m_aux[n][cant_vecinos];
	restar_y_al_cuadrado(n, cant_vecinos, x, x_a, m_aux);
	restar_y_al_cuadrado(n, cant_vecinos, y, y_a, x);
	restar_y_al_cuadrado(n, cant_vecinos, z, z_a, y);

	sumar(n, cant_vecinos, x_a, m_aux, x, y);

	comparar_mayor_igual(n, cant_vecinos ,aux , x_a, probe, r);
	return sumar_elementos(n, cant_vecinos, aux);
}



static PyObject *module_func(PyObject *self, PyObject *args) {
	PyObject *list_coordenadas;
	PyObject *list_radios;
	int n_threads;
	if (!PyArg_ParseTuple(args, "OOi", &list_coordenadas, &list_radios, &n_threads)) 
		return Py_BuildValue("s", "Error en los parametros de entrada");
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

	n_threads = (n_threads%2==0?n_threads:n_threads-1);
	int tot_atomos = PyList_Size(list_coordenadas);
	float constante = 4.0 * M_PI / 900;//Magic baby, yeha!
	float probe = 1.4; 
	float puntos_golden_sphere[900][3];
	float areaTotal = 0.0;
	generar_esfera(900, puntos_golden_sphere);

	//int n_threads = 6;
	pthread_t threads[n_threads];
	struct data allData[n_threads];
	int j=0;
	int paso = tot_atomos / n_threads;
	for (j=0;j<n_threads;j++){
	//------Datos de los threads--------
		
		allData[j].list_coordenadas = list_coordenadas;
		allData[j].list_radios = list_radios;
		allData[j].n = 900;
		allData[j].probe = probe;
		allData[j].p_g_sph = (float**)puntos_golden_sphere;
		allData[j].res = 0.0;
		allData[j].ini = j * paso;

		if(j==n_threads-1){
			allData[j].fin = tot_atomos;
		}else{
			allData[j].fin = allData[j].ini + paso;
		}

	//----------------------------------
		//printf("S - Creo thread con rango [%d,%d) \n",allData[j].ini, allData[j].fin);
		if (pthread_create(&threads[j], NULL,procesar_data,(void *)&allData[j])){
			//printf("S - Error en la creacion del thread %d \n",j);
			exit(1);
		}
		
	}

	while(n_threads>0){
		//printf("S - Esperando a thread %d \n",n_threads);
		pthread_join(threads[n_threads-1], NULL) ;
		areaTotal+=allData[n_threads-1].res;
		//printf("S - Fin espera a thread %d sumaParcial = %f\n",n_threads,areaTotal);

		n_threads--;
	}
	areaTotal*=constante;
	
	
	return Py_BuildValue("f", areaTotal);
}

void* procesar_data(void * arg){

	struct data* data = arg;
	unsigned short auxiliar[data->n];
	float areaTotal = 0.0;
	float area = 0.0;
	//printf("	T -- Inicio con rango (%d,%d) \n",data->ini, data->fin);

	int i = 0;
	for (i = data->ini; i < data->fin; i++)
	{		
		struct lista* lista_vecinos = obtener_vecinos(data->probe, i, data->list_coordenadas, data->list_radios);
		int cant_vecinos = lista_vecinos->n_elem;
		float radio ;
		PyArg_Parse(PyList_GET_ITEM(data->list_radios, i),"f",&radio);
		radio+=data->probe;
		int puntos = data->n;

		if (cant_vecinos>0)
		{
			float x[data->n][cant_vecinos];
			float y[data->n][cant_vecinos];
			float z[data->n][cant_vecinos];		
			float r[cant_vecinos];
			transformar_a_matriz_extendiendo_a_columnas(data->n, cant_vecinos, lista_vecinos, data->list_coordenadas, data->list_radios, x, y, z, r);
			float x_puntos_atomo[data->n][cant_vecinos];
			float y_puntos_atomo[data->n][cant_vecinos];
			float z_puntos_atomo[data->n][cant_vecinos];
			transformar_puntos_y_extender_a_fila(data->n, cant_vecinos, x_puntos_atomo, y_puntos_atomo, z_puntos_atomo,(float(*)[3])data->p_g_sph, PyList_GET_ITEM(data->list_coordenadas, i), PyList_GET_ITEM(data->list_radios, i), data->probe);	
			puntos = asa_func_pipeline(data->n ,cant_vecinos ,data->probe , auxiliar ,x ,y ,z ,x_puntos_atomo,y_puntos_atomo,z_puntos_atomo, r);
		}

		area =puntos * radio * radio;
		areaTotal += area;
	}
	data->res = areaTotal;
	//printf("	T -- Fin con rango (%d,%d) res = %f\n",data->ini, data->fin,areaTotal);
	pthread_exit(NULL);
	
}

    static PyMethodDef module_methods[] = {
    	{ "calcular", module_func, METH_VARARGS, NULL },
    	{ NULL, NULL, 0, NULL }
    };

    void initasaMIMD(void)
    {
    	Py_InitModule3("asaMIMD", module_methods, "Calculo de ASA");
    }
