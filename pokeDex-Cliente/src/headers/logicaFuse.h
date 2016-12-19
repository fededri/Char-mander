/*
 * logicaFuse.h
 *
 *  Created on: 8/9/2016
 *      Author: utnso
 */

#ifndef HEADERS_LOGICAFUSE_H_
#define HEADERS_LOGICAFUSE_H_

#include <stddef.h>
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse/fuse_opt.h>
#include "pokeClienteConections.h"

int osada_getattr(const char *path, struct stat *stbuf);
int osada_getdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
int osada_open(const char *path, struct fuse_file_info *fi);
int osada_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int osada_write(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int osada_truncate(const char * path, off_t offset);
int osada_unlink(const char * path);
int osada_crearDirectorio(const char* path, mode_t mode);
int osada_borrarArchivo(const char* path);
int osada_renombrarArchivo(const char* from, const char* to);
int osada_borrarDirectorio(const char* path);
int osada_crearArchivo(const char *, mode_t mode, struct fuse_file_info *);
int osada_flush(const char* path);
int osada_utimens(const char* path, const struct timespec ts[2]);
int osada_access(const char* path, int mask);
int osada_chmod(const char* path, mode_t mode);
int osada_chown(const char *, uid_t, gid_t);

/** keys for FUSE_OPT_ options */
enum {
	KEY_VERSION,
	KEY_HELP,
};


#define DEFAULT_FILE_CONTENT "Hello World!\n"


#define DEFAULT_FILE_NAME "hello"

/*
 * Este es el path relativo al punto de montaje, archivo dentro del FS
 */
#define DEFAULT_FILE_PATH "/" DEFAULT_FILE_NAME



/*
 * Esta es una estructura auxiliar utilizada para almacenar parametros
 * que nosotros le pasemos por linea de comando a la funcion principal
 * de FUSE
 */
struct t_runtime_options {
	char* welcome_msg;
	char* IP;
	char* PUERTO;
} runtime_options;

/*
 * Esta Macro sirve para definir nuestros propios parametros que queremos que
 * FUSE interprete. Esta va a ser utilizada mas abajo para completar el campos
 * welcome_msg de la variable runtime_options
 */
#define CUSTOM_FUSE_OPT_KEY(t, p, v) { t, offsetof(struct t_runtime_options, p), v }




#endif /* HEADERS_LOGICAFUSE_H_ */
