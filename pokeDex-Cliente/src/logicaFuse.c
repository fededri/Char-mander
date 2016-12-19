#include "headers/logicaFuse.h"
#include <protocolos/p_pokeDex.h>
#include <unistd.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif
#include <stdbool.h>
#include <dirent.h>


int archivo_borrado = 1;
/*
 * @DESC
 * Esta función va a ser llamada cuando a la biblioteca de FUSE le llege un pedido
 * para obtener la metadata de un archivo/directorio. Esto puede ser tamaño, tipo,
 * permisos, dueño, etc ...
 *
 * @PARAMETROS
 * 		path - El path es relativo al punto de montaje y es la forma mediante la cual debemos
 * 		       encontrar el archivo o directorio que nos solicitan
 * 		stbuf - Esta esta estructura es la que debemos completar
 *
 * 	@RETURN
 * 		O archivo/directorio fue encontrado. -ENOENT archivo/directorio no encontrado
 */
int osada_getattr(const char *path, struct stat *stbuf) {
 	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));
	bool existe = getAttr(path,stbuf);

	if(!existe) res = -ENOENT;

	return res;
}


/*
 * @DESC
 *  Esta función va a ser llamada cuando a la biblioteca de FUSE le llege un pedido
 * para obtener la lista de archivos o directorios que se encuentra dentro de un directorio
 *
 * @PARAMETROS
 * 		path - El path es relativo al punto de montaje y es la forma mediante la cual debemos
 * 		       encontrar el archivo o directorio que nos solicitan
 * 		buf - Este es un buffer donde se colocaran los nombres de los archivos y directorios
 * 		      que esten dentro del directorio indicado por el path
 * 		filler - Este es un puntero a una función, la cual sabe como guardar una cadena dentro
 * 		         del campo buf
 *
 * 	@RETURN
 * 		O directorio fue encontrado. -ENOENT directorio no encontrado
 */
 int osada_getdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	(void) offset;
	(void) fi;

	int res = 0; //TODO hay que manejar errrores, revisar que mando siempre que existe archivo

	getDir(path,buf,filler);



	return res;
}

/*
 * @DESC
 *  Esta función va a ser llamada cuando a la biblioteca de FUSE le llege un pedido
 * para tratar de abrir un archivo
 *
 * @PARAMETROS
 * 		path - El path es relativo al punto de montaje y es la forma mediante la cual debemos
 * 		       encontrar el archivo o directorio que nos solicitan
 * 		fi - es una estructura que contiene la metadata del archivo indicado en el path
 *
 * 	@RETURN
 * 		O archivo fue encontrado. -EACCES archivo no es accesible
 */
int osada_open(const char *path, struct fuse_file_info *fi) {

	bool existe = abrirArchivo(path,fi);


	if(existe) return 0;
	else return -EACCES;
}


int osada_truncate(const char * path, off_t offset){
	int res = truncarArchivo(path, offset);
	if (res==NO_HAY_ESPACIO){
		return -ENOMEM;
	}else if(res==ARCHIVO_INEXISTENTE){
		return -ENOENT;
	}else
		return 0;
}


int osada_write(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){

	int res=escribirArchivo(size,offset,path,buf);
	if (res==NO_HAY_ESPACIO)
		return -ENOMEM;
	else if(res==ARCHIVO_INEXISTENTE)
		return -ENOENT;
	else
		return res;
}


/*
 * @DESC
 *  Esta función va a ser llamada cuando a la biblioteca de FUSE le llege un pedido
 * para obtener el contenido de un archivo
 *
 * @PARAMETROS
 * 		path - El path es relativo al punto de montaje y es la forma mediante la cual debemos
 * 		       encontrar el archivo o directorio que nos solicitan
 * 		buf - Este es el buffer donde se va a guardar el contenido solicitado
 * 		size - Nos indica cuanto tenemos que leer
 * 		offset - A partir de que posicion del archivo tenemos que leer
 *
 * 	@RETURN
 * 		Si se usa el parametro direct_io los valores de retorno son 0 si  elarchivo fue encontrado
 * 		o -ENOENT si ocurrio un error. Si el parametro direct_io no esta presente se retorna
 * 		la cantidad de bytes leidos o -ENOENT si ocurrio un error. ( Este comportamiento es igual
 * 		para la funcion write )
 */
int osada_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {

	leerArchivo(size,offset,path,buf);
	return size;

}


//borra el archivo
int osada_unlink(const char * path){

	int res=borrarArchivo(path);
	if (res==DIRECTORIO_NO_VACIO)
		return -ENOTEMPTY;
	else if (res==ARCHIVO_INEXISTENTE)
		return -ENOENT;
	else
		return 0;

}


int osada_crearDirectorio(const char* path, mode_t mode){

	if(obtenerTamanioDeNombreDeArchivoDePath(path)>17){
		return -ENAMETOOLONG;
	}else {
		int res=crearDirectorio(path);
		if (res==_PATH_INVALIDO)
			return -ENOENT;
		else if(res==NO_HAY_DESCRIPTORES)
			return -EDQUOT;
		else
			return 0;
	}

}


int osada_renombrarArchivo(const char* from, const char* to){

	if(obtenerTamanioDeNombreDeArchivoDePath(to)>17){
		return -ENAMETOOLONG;
	}else {
		int res= renombrarArchivo(from,to);
		if (res==ARCHIVO_INEXISTENTE)
			return -ENOENT;
		else
			return 0;
	}

}


int osada_borrarDirectorio(const char* path){

	int res= borrarArchivo(path);
	if (res==ARCHIVO_INEXISTENTE)
		return -ENOENT;
	else if(res==DIRECTORIO_NO_VACIO)
		return -ENOTEMPTY;
	else
		return 0;
}

int osada_crearArchivo(const char * path, mode_t mode, struct fuse_file_info * fi){

	if(obtenerTamanioDeNombreDeArchivoDePath(path)>17){
		return -ENAMETOOLONG;
	}else {
		int res= crearArchivo(path);
		if (res==NO_HAY_DESCRIPTORES)
			return -EDQUOT;
		else if(res==_PATH_INVALIDO)
			return -ENOENT;
		else
			return 0;
	}
}


int osada_flush(const char* path){

	return 0;
}


int osada_utimens(const char* path, const struct timespec ts[2]){
	return cliente_utimens(path, ts);
}
//recibe el path completo de un archivo y devuelve la cantidad
//de caracteres que tiene el nombre del archivo
int obtenerTamanioDeNombreDeArchivoDePath(unsigned char* path){

	if (strcmp(path, "/")==0)
		return path;

	unsigned char** partes = string_split(path, "/");

	int indicePartes = 0;

	while (partes[indicePartes] != NULL) {
		indicePartes++;
	}
	int size =  strlen(partes[indicePartes-1]);
	free(partes);
	return size;
}

int osada_access(const char* path, int mask){

 	struct stat* stbuf = malloc(sizeof(struct stat));
	memset(stbuf, 0, sizeof(struct stat));
	bool existe = getAttr(path,stbuf);

	if(existe)
		return 0;
	else
		return -ENOENT;
}
int osada_chmod(const char* path, mode_t mode){

 	struct stat* stbuf = malloc(sizeof(struct stat));
	memset(stbuf, 0, sizeof(struct stat));
	bool existe = getAttr(path,stbuf);

	if(existe)
		return 0;
	else
		return -ENOENT;
}


int osada_chown(const char * path, uid_t userId, gid_t groupId){
	struct stat* stbuf = malloc(sizeof(struct stat));
		memset(stbuf, 0, sizeof(struct stat));
		bool existe = getAttr(path,stbuf);

		if(existe)
			return 0;
		else
			return -ENOENT;
}



