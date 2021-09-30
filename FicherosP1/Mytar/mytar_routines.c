#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

extern char *use;

/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int copynFile(FILE *origin, FILE *destination, int nBytes)
{
	// Complete the function
	if (origin == NULL)
	{
		perror("Apertura de origin en copynFile fallida\n");
		return -1;
	}

	if (destination == NULL)
	{
		perror("Apertura de destination en copynFile fallida\n");
		return -1;
	}

	char *buffer = malloc(sizeof(char) * nBytes);
	size_t resultado;
	int numbytes = 0;
	while (!feof(origin) && numbytes < nBytes)
	{
		resultado = fread(&buffer, sizeof(buffer), 1, origin);
		if (resultado != 1)
		{
			return -1;
		}
		numbytes++;
	}

	if (fwrite(&buffer, sizeof(buffer), 1, destination) != 1)
	{
		//error
		perror("Lectura del numero de fichero en readHeader fallida\n");
		return -1;
	}

	return numbytes;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char *
loadstr(FILE *file)
{
	// Complete the function
	if (file == NULL)
		return NULL;

	//Averiguamos el tamaño del string
	size_t tam = 0;
	char caracter;

	while ((caracter = getc(file)) != '\0')
	{
		if (caracter == EOF)
			return NULL;
		tam++;
	}

	char *str = malloc(tam + 1);

	//Retrocedemos en la lectura del archivo el numero de elementos que hemos
	//conseguido leer por el tamaño de esos elementos en bytes
	int result = fseek(file, -((tam + 1) * sizeof(char)), SEEK_CUR);
	if (result == -1)
		return NULL;

	if (fread(str, sizeof(char), tam + 1, file) < tam)
		return NULL;

	return str;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry *
readHeader(FILE *tarFile, int *nFiles)
{
	stHeaderEntry *array = NULL;

	//apertura de tarFile
	if (tarFile == NULL)
	{
		//error
		perror("Apertura en readHeader fallida\n");
	}

	// Leemos el numero de ficheros(N) del tarFile y lo volcamos en nr_files...
	if (fread(&nFiles, sizeof(int), 1, tarFile) != 1)
	{
		//error
		perror("Lectura del numero de fichero en readHeader fallida\n");
	}

	/* Reservamos memoria para el array */
	array = malloc(sizeof(stHeaderEntry) * (*nFiles));
	//Leemos la metainformacion del tarFile y la volcamos en el array...

	for (int i = 0; i < (*nFiles); ++i)
	{
		//lectura del nombre
		char *name = loadstr(tarFile);

		if (name == NULL)
			return NULL;

		array[i].name = malloc(strlen(name));

		if (array[i].name == NULL)
			return NULL;

		//copia en array name lo almacenado en buffer
		strcpy(array[i].name, name);

		//lectura del tamaño
		int s = 0;
		if (fread(&s, sizeof(int), 1, tarFile) != 1)
		{
			//error
			perror("Lectura del size en readHeader fallida\n");
			return NULL;
		}
		array[i].size = s;
	}

	/* Devolvemos los valores leidos a la funcion invocadora */
	return array;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int createTar(int nFiles, char *fileNames[], char tarName[])
{
	// Complete the function
	FILE *tar = fopen(tarName, "w");

	if (tar == NULL)
	{
		return EXIT_FAILURE;
	}

	stHeaderEntry *header = malloc(sizeof(stHeaderEntry) * nFiles);
	if(header == NULL)
		return EXIT_FAILURE;

	//Calculamos cuanto espacio vamos a necesitar para el header de nuestro tarball
	size_t headerSize = 0;
	headerSize += sizeof(int); //entero con el numero de ficheros

	for(int i = 0; (i< nFiles); i++){
		headerSize += sizeof(unsigned int ); //entero con el numero de bytes de cada fichero
		headerSize += strlen(fileNames[i]) + 1; //longitud del nombre del fichero +1 por \0
	}

	int result = fseek(tar, headerSize, SEEK_SET);
	if (result == -1)
		return EXIT_FAILURE;

	for (int i = 0; (i < nFiles); i++)
	{
		FILE *arc = fopen(fileNames[i], "r");
		int arcBytes = copynFile(arc, tar, sizeof(arc));
		if(arcBytes < 0)
			return EXIT_FAILURE;

		header[i].size = arcBytes;
		header[i].name = malloc(strlen(fileNames[i]) + 1);

		if(header[i].name == NULL)
			return EXIT_FAILURE;

		strcpy(header[i].name, fileNames[i]);

		fclose(arc);
	}

	//Nos movemos de nuevo al principio del fichero
	result = fseek(tar, 0L, SEEK_SET);
	if(result == -1)
		return EXIT_FAILURE;

	//Escribimos el numero de ficheros con los que cuenta nuestro tar
	if(fwrite(&nFiles, sizeof(int), 1, tar) != 1)return EXIT_FAILURE;

	//Escribimos las cabeceras para cada archivo
	for(int  i = 0; (i < nFiles); i++){
		if(fwrite(header[i].name, strlen(fileNames[i])+1, 1,tar) != 1)return EXIT_FAILURE; //Nombre
		if(fwrite(&header[i].size, sizeof(unsigned int), 1, tar)!=1)return EXIT_FAILURE;//tamano bytes
	}

	//Liberar memoria reservada, primero de los strings de los nombres de los
	//ficheros y despues del puntero a los distintos headers
	for(int i = 0; i< nFiles; i++){
		free(header[i].name);
	}

	free(header);

	//Fichero rellenado, lo cerramos
	fclose(tar);

	return EXIT_SUCCESS;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int extractTar(char tarName[])
{
	// Complete the function
	//Abrimos el archivo del tar
	FILE* tarFile= fopen(tarName,"r");
	//Comprobamos que no ha habido error
	if(tarFile == NULL) return EXIT_FAILURE;

	int nFiles = 0;
	//Creamos el header (devuelve por parametro el tamaño de nFiles)
	stHeaderEntry* header = readHeader(tarFile, &nFiles);
	//Comprobamos que no ha habido error
	if(header == NULL)return EXIT_FAILURE;


	//Creamos los nFiles que marca el Header
	for(int i = 0; (i< nFiles); i++){

		FILE* archivo = fopen(header[i].name, "w");
		if(archivo == NULL) return EXIT_FAILURE;

		//copiamos byte a byte en el archivo el contenido del tar usando el metodo copynFile
		int bytesCopiados = copynFile(tarFile,archivo, header[i].size);
		if(bytesCopiados != header[i].size)return EXIT_FAILURE;
		
		//cerramos archivo
		fclose(archivo);
	}

	//cerramos tar
	fclose(tarFile);

	//Si hemos llegado hasta aquí, extraido un tar con exito
	return EXIT_SUCCESS;
}
