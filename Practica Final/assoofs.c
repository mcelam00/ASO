#include <linux/module.h>       /* Needed by all modules */
#include <linux/kernel.h>       /* Needed for KERN_INFO  */
#include <linux/init.h>         /* Needed for the macros */
#include <linux/fs.h>           /* libfs stuff           */
#include <linux/buffer_head.h>  /* buffer_head           */
#include <linux/slab.h>         /* kmem_cache, kmalloc   */
#include "assoofs.h"

/*
 *  Operaciones sobre ficheros
 */
int assoofs_save_inode_info(struct super_block *sb, struct assoofs_inode_info *inode_info);
ssize_t assoofs_read(struct file * filp, char __user * buf, size_t len, loff_t * ppos);
ssize_t assoofs_write(struct file * filp, const char __user * buf, size_t len, loff_t * ppos);

const struct file_operations assoofs_file_operations = {  //leer y escribir
    .read = assoofs_read,
    .write = assoofs_write,
};

/*PERMITE LEER DE UN ARCHIVO (Descriptor de fichero, buffer donde mandaremos los datos que se quieren leer y len es el tamanio a leer. Ppos es el puntero por donde voy leyendo con respecto al inicio del fichero)*/

ssize_t assoofs_read(struct file * filp, char __user * buf, size_t len, loff_t * ppos) { //se va a ejecutar cuando leamos


	//*Del descriptor saco la info que me interesa
	struct inode *inode;
	struct super_block *sb;
	struct assoofs_inode_info *inode_info;
	struct buffer_head *bh;
	char * buffer;
	int nbytes; 

    printk(KERN_INFO "Read request\n");

	inode = filp->f_path.dentry->d_inode;
	inode_info = inode->i_private;
	sb=inode->i_sb;
	

	//1.Antes de leer nada voy a ver no sea que ya este en el final (ES COMO EL MARCAPAGINAS, SIMPLEMENTE NOS VALE PARA CUANDO LLEGUE AL FINAL NO LEER MAS)
	if(*ppos >= inode_info->file_size){ //si el puntero es mayor o igual que el tamanio del fichero pues no hay mas que enviar al usuario para que lea
	    	printk(KERN_ERR "End of File. No more to read\n");
		return 0;
	}

	//2.Necesitare traerme el bloque de contenido del fichero
	
	bh = sb_bread(sb, inode_info->data_block_number);
	buffer = (char *)bh->b_data; //el bloque de contenido de un fichero (que no habiamos visto hasta ahora es de tipo char * que es todo el contenido en string)

		//voy a componer lo que le tengo que mandar en buf a mi usuario
	
	nbytes = min((size_t)inode_info->file_size, len); //los bytes que voy a leer realmente de buffer y mandar en buf, seran los minimos disponibles, si lo que nos dicen leer es posible, sera el minimo, si por el contrario no es posible y queda menos fichero, lo que quede del fichero

	copy_to_user(buf, buffer, nbytes); //le envio al usuario los nbytes de buffer en buf
	
	//3. Actualizo el marcapaginas para la siguiente lectura

	*ppos = *ppos + nbytes;

    	printk(KERN_INFO "Read successfull\n");
	brelse(bh);
    return nbytes;
}

ssize_t assoofs_write(struct file * filp, const char __user * buf, size_t len, loff_t * ppos) {



	//1.Obtenemos la info del desciptor
	struct inode *inode;
	struct assoofs_inode_info *inode_info;
	struct super_block *sb;
	struct buffer_head *bh;
	char * buffer;

    printk(KERN_INFO "Write request\n");

	inode = filp->f_path.dentry->d_inode;
	inode_info = inode->i_private;
	sb=inode->i_sb;



	//2.Necesitare traerme el bloque de contenido donde estoy escribiendo y con el marcapaginas me situo en la parte del bloque a partir de la cual me quede escribiendo

	bh = sb_bread(sb, inode_info->data_block_number);
	buffer = (char *)bh->b_data;

	
	buffer = buffer + *ppos; //avanzo a donde me llegaba escribiendo (se que es dentro del archivo porque ya comprobé que no me saliera)

	
	copy_from_user(buffer, buf, len);//escribo en buffer lo que me da el ususario en buf (1 ARG ES DONDE VOY A ESCRIBIR Y EL SEGUNDO DE DONDE y despues cuanto)

	*ppos = *ppos + len; //avanzo el marcapaginas con la cantidad que he escrito

	
	//3.Una vez escrito, necesito actualizar el bloque de contenido en el disco AUMENTANDO SU TAMAÑO Y con la nueva informacion

	mark_buffer_dirty(bh);
	sync_dirty_buffer(bh);	


	inode_info->file_size = *ppos; 
	//llamo a la funcion que actualiza el inodo en el almacen	
	assoofs_save_inode_info(sb, inode_info);

    	printk(KERN_INFO "Write successful\n");
   	brelse(bh);
	return len; //retorno los bytes escritos
}








/*
 *  Operaciones sobre directorios
 */
static int assoofs_iterate(struct file *filp, struct dir_context *ctx);

const struct file_operations assoofs_dir_operations = {  //mostrar el contenido del directorio
    .owner = THIS_MODULE,
    .iterate = assoofs_iterate, 
};


/*FUNCION QUE NOS PERMITE VER EL CONTENIDO DE UN DIRECTORIO*/

static int assoofs_iterate(struct file *filp, struct dir_context *ctx) { //Descriptor del fichero y el contexto


	//1. Extraigo el inodo en memoria del descriptor del fichero

	struct inode *inode;
	struct assoofs_inode_info *inode_info;
	struct super_block *sb;
	struct buffer_head *bh;
	struct assoofs_dir_record_entry *record;
	int i;
    printk(KERN_INFO "Iterate request\n");

	inode = filp->f_path.dentry->d_inode;

	//necesitare el superbloque en memoria, la parte persistente del inodo 

	sb = inode->i_sb;
	
	inode_info = inode->i_private;

	//2.Comprobar si el contexto esta ya en la cache. Si ya esta (=1), return 0
	
	if(ctx->pos){
		return 0;
	}

	//3.Comprobar que el inodo que nos pasan no es de un archivo sino que de un directorio

	if((!S_ISDIR(inode_info->mode))){ //no es directorio

		return -1;

	}


	//4.Traer el bloque contenido del directorio datablocknumber y crear el contexto con el dir emit

	bh = sb_bread(sb, inode_info->data_block_number);
	record = (struct assoofs_dir_record_entry *)bh->b_data; //ya vengo apuntando a la primera dupla


	for(i = 0; i < inode_info->dir_children_count; i++){ //recorremos todas las duplas del bloque de contenido del directorio

		dir_emit(ctx, record->filename, ASSOOFS_FILENAME_MAXLEN, record->inode_no, DT_UNKNOWN); //aniadimos al contexto (como una lista) el nombre de cada dupla y el numero de inodo, los dos campos vaya
						//indico el tamanio max de nombre de archivo
		ctx->pos += sizeof(struct assoofs_dir_record_entry);//avanza el contexto 1, para que el siguiente se grabe en la siguiente como una lista y esa cantidad que avanza es lo que ocupa lo que acaba de guardar que no es mas que un dir record entry


 	    record++;
	}

	brelse(bh);
    return 0;
}









/*
 *  Operaciones sobre inodos
 */

static int assoofs_create(struct inode *dir, struct dentry *dentry, umode_t mode, bool excl); //el inodo padre, la entrada de directorio, la entrada del directorio padre para sacar el nombre y el tercero mode son los permisos como el mode de inode info
struct dentry *assoofs_lookup(struct inode *parent_inode, struct dentry *child_dentry, unsigned int flags);
static int assoofs_mkdir(struct inode *dir, struct dentry *dentry, umode_t mode);


static struct inode_operations assoofs_inode_ops = {
    .create = assoofs_create, //crear inodos nuevos (ficheros regulares)
    .lookup = assoofs_lookup, //dado el nombre de fichero obtener el identificador de inodo
    .mkdir = assoofs_mkdir, //crear inodos nuevos (directorios/carpetas)
};



/*
*FUNCION AUXILIAR QUE ME DA LA INFO PERSISTENTE DEL INODO EN EL ALMACEN DE INODOS (bloque 1)
*/

//Con esta funcion podemos obtener la informacion en disco (del almacen de inodos) para el inodo concreto que se le pasa como parametro.

struct assoofs_inode_info *assoofs_get_inode_info(struct super_block *sb, uint64_t inode_no){
    
   
    // 1. Leer de disco el almacen de inodos de manera similar a como leiamos el superbloque
     
   struct assoofs_super_block_info *afs_sb = sb->s_fs_info; //es una variable como en la que leimos el superbloque de disco, pero como ya lo tenemos en memoria ese contenido porque lo salvamos en ese cajon de sastre de la variable sb (sb->s_fs_info = assoofs_sb) nos ahorramos el leerlo de disco     

   struct assoofs_inode_info *inode_info = NULL; //aqui es donde metemos la info del almacen de inodos que traemos de disco. (la analogia con la assoofs_super_block_info)
   struct buffer_head *bh;
     
  
   struct assoofs_inode_info *buffer = NULL; //Aqui almacenamos lo que devolvemos
    
     int i;
    
     printk(KERN_INFO "Obtain persistent i-node info request (from disk inode store)\n");
       
     
     bh = sb_bread(sb, ASSOOFS_INODESTORE_BLOCK_NUMBER); //le paso el superbloque en memoria y el nÂº de bloque = 1
     inode_info = (struct assoofs_inode_info *)bh->b_data; //Lo trasplanto a mi variable
     
     
    //2. Como ya tenemos el almacen en memoria en inode_info, vamos recorriendolo hasta dar con el inodo que se nos pide proporcionar su info en disco (el que se pasa como parametro)
    

    
    //inodes_count es 2 ahora, porque solamente tenemos el inodo raiz y el del fichreo de bienvenida
    for (i = 0; i < afs_sb->inodes_count; i++) { //recorremos todos los inodos de mi sistema de ficheros, esa info se guarda en el superbloque, que hemos recuperado a esa variable desde el cajon de sastre
        
            //si coincide el inodo actual con el de parametro
        if (inode_info->inode_no == inode_no) {
            buffer = kmalloc(sizeof(struct assoofs_inode_info), GFP_KERNEL); //segundo arg no olvidarlo
            memcpy(buffer, inode_info, sizeof(*buffer)); //reservo espacio y guardo en buffer la info a retornar (la posicion donde estoy del almacen de inodos que lei de disco y estoy recorriendo para buscar el numero de inodo que me piden, en este caso el raíz)
            break;
        }
        inode_info++; //muevo 1 la variable almacen de inodos, que en binario sera el inodo siguiente en el almacen
                //++ = +1 (= 00000001 en binario) semantica de punteros, avanzo la direccion de memoria
    }

    printk(KERN_INFO "Obtain persistent i-node info (from disk inode store) completed!\n");
    
    //3. libero el furgon en el que me traigo el inodo de disco (la variable auxiliar) y retorno
    brelse(bh);
    return buffer;    
    
}



/*
*FUNCION AUXILIAR QUE CREA UN INODO EN MEMORIA
*/


static struct inode *assoofs_get_inode(struct super_block *sb, int ino){ //recibe el superbloque y el numero de inodo en el almacen de inodos
	
	//1.Creo la variable inodo en memoria
	struct inode *inode; //nuevo inodo en memoria
	//2. Me traigo del almacen de inodos en disco la informacion que tengo alli para este inodo
	
	struct assoofs_inode_info *infoPersistente;

	infoPersistente = assoofs_get_inode_info(sb, ino); //busca el inodo ino en el almacen de inodos (no tiene porque estar en orde, el inodo 4 igual es el estante 10)




	inode = new_inode(sb); //lo inicializo con su superbloque
        
       	printk(KERN_INFO "Creating new I-Node.");

	
	//3. Asigno valor a sus campos

	inode->i_ino = ino; //el numero de inodo que me llega y usé para buscarlo en el almancen de inodos
	inode->i_sb = sb; //superbloque que recibe como parametro
	inode->i_op = &assoofs_inode_ops; //operaciones sobre inodo
	
	//antes de las operaciones sobre fichero/dir necesito saber que es, poorque dentro de un diretorio puede haber otro y como se representan igual que archivos pues la dupla no decia nada
	//me lo dice la info del inodo que estaba en el almacen. COMO LA OPERACION ES GENERICA NO LO VOY A SABER COMO SI SABIA QUE EL DIR RAIZ ERA UN DIRECTORIO
	
		if(S_ISDIR(infoPersistente->mode)){ //si es directorio las operaciones del directorio
			inode->i_fop = &assoofs_dir_operations;
			
		}
		else if(S_ISREG(infoPersistente->mode)){ //si es regular las de archivo
			inode->i_fop = &assoofs_file_operations;			
			
		}
		else{
			printk(KERN_ERR "Unknown inode type. Neither a directory nor a file.");
		}
	
	
	inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);
	
        
        
        
        
        
	inode->i_private = infoPersistente; //salvo en su cajon de sastre la info de disco que he leido (por si por ejemplo fuese un directorio este y se lo pasaramos como padre al metodo del que venimos, el lookup)
	
	printk(KERN_INFO "I-node created sucesfully.");


	return inode;
	
}



//devuelve una entrada de directorio del nombre de fichero que le pasamos (es generico para cualquier nombre, es una operacion)

struct dentry *assoofs_lookup(struct inode *parent_inode, struct dentry *child_dentry, unsigned int flags) {
    int i;
    
    //Dado un inodo padre tenemos que buscar en el, la entrada de directorio correspondiente al nombre que se nos pasa en child_dentry, concretamente en child_dentry->d_name.name
    
    
    //1. Como tenemos que buscar esa entrada (nombre) en el directorio padre, tendremos que tener el CONTENIDO (no el bloque de informacion) del bloque en disco correspondiente a ese directorio para ver el archivo de dentro de ese directorio que es coincidente. Entonces traemos el bloque

    
    //cuidado porque me pasan el struct inode y yo quiero el bloque o la informacion que lei (assoo inode info) de todo lo de ese directorio en disco. Que a su vez fue lo que use para crear esa varible inodo
    //estara en el cajon de sastre del inodo
    
    struct assoofs_inode_info *parent_info = parent_inode->i_private;
    struct super_block *sb = parent_inode->i_sb; //obtengo su superbloque 
    struct buffer_head *bh;
    struct assoofs_dir_record_entry *record;
    struct inode *inode;


    printk(KERN_INFO "Lookup request\n");

    
    
    bh = sb_bread(sb, parent_info->data_block_number); //le tenemos que pasar el superbloque (que no tenemos, luego hay que sacarlo del inodo) y tambien hay que saber el numero de bloque que es el directorio en el que queremos mirar (tambien lo sacamos del inodo)
    
    //saco del furgon mi directorio de tipo dir_record_entry
    record = (struct assoofs_dir_record_entry *)bh->b_data; 
    

    
    printk(KERN_INFO "Parent dir content succesfully fetched\n");
    printk(KERN_INFO "Lookup in progress\n");
    
    
    //2. teniendo mi bloque de disco que es el directorio con sus archivos leido (como lista de archivos en el imaginemos, duplas) que sería el bloque indicado por data_block_number dentro de la info persistente del inodo padre que esta en el almacen de inodos vamos a recorrerlo en busca de la entrada que es igual al nombre que nos pasan dentro de child dentry y cuando encuentre esa entrada deberÃ© 
    
    for(i = 0; i < parent_info->dir_children_count; i++){ //recorro el bloque de contenido del directorio padre, igual que cuando recorriamos el almacen de inodos, solo que en lugar de buscar el num de inodo ahora buscamos el nombre concreto
    	
		if(!strcmp(record->filename, child_dentry->d_name.name)){ //si coinciden los nombres perfecto!
		
			//ya encontre la dupla o el dir_record_entry, con su nombre y su numero de inodo correspondiente
			//vamos a generar un nuevo inodo en memoria para él
			inode= assoofs_get_inode(sb, record->inode_no); //vamos a hacer lo mismo que cuando creamos en memoria el inodo raiz pero en una funcion auxiliar y el numero de inodo a buscar en el almacen y sacado de la dupla es ahora lo que antes era la contante 1 que indicaba a getinode info la posicion a buscar en el almacen de inodos
		
			//ya tengo el inodo y sus operaciones. Me falta, aniadirlo a la jerarquia de inodos y los permisos
		
			//Permisos
			inode_init_owner(inode, parent_inode, ((struct assoofs_inode_info *)inode->i_private)->mode); //el nuevo inodo en memoria, su dir padre (del que hemos sacado todo) por eso esta aqui, porque no lo tengo en get inode, si es archivo o dir que tenemos de nuevo que dilucidarlo con el campo mode. Hay que castearlo
		
			//Jerarquia de inodos en memoria. Al ser inodo normal y no raiz llamo a d_add()
		
			d_add(child_dentry, inode); //le digo el dentry que recibí con el nombre y el inodo que he creado en memoria
		
		    printk(KERN_INFO "Lookup succesfull.\n");

			return NULL;
			
			
		}    	
    	//si no coincide la dupla con el nombre pasado hay que avanzar a la siguiente dupla dentro de ese bloque de contenido de directorio, igual que cuando avanzabamos en el almacen de inodos con la aritmetica de punteros
 		record++;   //me muevo al siguiente dir record entry del bloque que leí (contenido del directorio)	
    	   	
	}
    
    
    
    //Si se recorre todas la duplas que trajimos en el bloque de contenido de directorio (data block) y no esta es que el nombre ese no aparece
    
    
    printk(KERN_ERR "Lookup failed: No inode (number) entry found for such name on the parent directory provided.");
    return NULL;

}









/*FUNCION AUXILIAR QUE BUSCA EL INODO EN EL ALMACEN PARA ACTUALIZARLO*/

struct assoofs_inode_info *assoofs_search_inode_info(struct super_block *sb, struct assoofs_inode_info *start, struct assoofs_inode_info *search){

uint64_t count = 0;

printk(KERN_INFO "Searching for the inode\n");

//buscar el inodo a actualizar



while (start->inode_no != search->inode_no && count < ((struct assoofs_super_block_info *)sb->s_fs_info)->inodes_count) {
	count++;
	start++; //avanzo el puntero del almacen
}


//devolver el puntero en posicion del inodo encontrado

	if (start->inode_no == search->inode_no){ //primera condicion del while al reves, los numeros coinciden
		printk(KERN_INFO "Inode succesfully found\n");
		return start;

	}else{ //fue porque terminamos los inodos y no se encontró
		printk(KERN_INFO "Inode search FAILED, no inode with that number in the store\n");
		 return NULL;

	}

}










/*FUNCION AUXILILAR QUE ACTUALIZA LA INFORMACION DE UN INODO EN EL ALMACEN (ultima del create)*/

int assoofs_save_inode_info(struct super_block *sb, struct assoofs_inode_info *inode_info){


//traer el almacen de inodos

	struct buffer_head *bh;
	struct assoofs_inode_info *posicion;
	
printk(KERN_INFO "Inode update in inode store request\n");

	bh = sb_bread(sb, ASSOOFS_INODESTORE_BLOCK_NUMBER);
	

	posicion = assoofs_search_inode_info(sb, (struct assoofs_inode_info *)bh->b_data, inode_info);

	//actualizarlo con memcpy
	memcpy(posicion, inode_info, sizeof(*posicion));


	mark_buffer_dirty(bh);
	sync_dirty_buffer(bh);

printk(KERN_INFO "Inode updated succesfully\n");

	return 0;

}











/*AUXILIAR: Sobreescribe el superbloque con los cambios del bitmap y count(count cambia en add inode info)*/


void assoofs_save_sb_info(struct super_block *vsb){

 //voy a leer el superbloque de disco y lo sobreescribo con el campo s_fs_info, info persistente actualizada
	struct buffer_head *bh;
	struct assoofs_super_block *sb = vsb->s_fs_info; // Informacion persistente del superbloque en memoria

printk(KERN_INFO "Superblock update request\n");

	bh = sb_bread(vsb, ASSOOFS_SUPERBLOCK_BLOCK_NUMBER); //le digo bloque 0

	//ahora en lugar de sacar b_data lo que hago es pisarlo con el de mi superbloque

	bh->b_data = (char *)sb;
	
	//Una vez sobreescrito, debo volverlo a disco	

	mark_buffer_dirty(bh);
	sync_dirty_buffer(bh);
	brelse(bh);
printk(KERN_INFO "Superblock updated succesfully on disk\n");
}












/*FUNCION AUXILIAR QUE GRABA LA NUEVA INFO PERSISTENTE EN EL ALMACEN DE INODOS*/
//En el almacen de inodos, CREA la info persistente correspondiente a nuestro nuevo inodo (EL SUPERBLOQUE EN MEMORIA QUE LE PASAMOS LO NECESITO PARA LEER) 

void assoofs_add_inode_info(struct super_block *sb, struct assoofs_inode_info *inode){




//leo el almacen de inodos y busco moviendo el puntero hasta la ultima entrada ocupada, la primera
//entrada vacia para escribir mi entrada en el bloque de almacen de inodos que me he traido y una vez modificado con la nueva entrada lo escribo de nuevo en disco como cuando añadí la dupla


	struct buffer_head *bh;
	struct assoofs_inode_info *inode_info;
	struct assoofs_super_block_info *assoofs_sb;
	//se cuantos inodos hay, me lo dice el superbloque avanzo el puntero en el almacen hasta el final
	assoofs_sb = sb->s_fs_info; 

printk(KERN_INFO "Create new inode entry into the inode store\n");

	bh = sb_bread(sb, ASSOOFS_INODESTORE_BLOCK_NUMBER);
	inode_info = (struct assoofs_inode_info *)bh->b_data;



	inode_info += assoofs_sb->inodes_count;

	//teniendo el puntero avanzado, simplemente añado el nuevo inodo

	memcpy(inode_info, inode, sizeof(struct assoofs_inode_info));

	//lo ensucio para guardarlo
	mark_buffer_dirty(bh);
	sync_dirty_buffer(bh);


	//He añadido al almacen un nuevo inodo, lo registo en el suprebloque

	assoofs_sb->inodes_count++; //al haberlo cambiado hay que actualizarlo en disco
	assoofs_save_sb_info(sb);
	


printk(KERN_INFO "New inode added succesfully to the store\n");


}











/*AUXILIAR: Recorre bitmap y asigna el nuevo bloque*/


int assoofs_sb_get_a_freeblock(struct super_block *sb, uint64_t *block){



//block es el data block number que quiero obtener y que me dice el for
int i;
struct assoofs_super_block_info *assoofs_sb = sb->s_fs_info;

printk(KERN_INFO "Data block number search request\n");


for (i = 2; i < ASSOOFS_MAX_FILESYSTEM_OBJECTS_SUPPORTED; i++)
	if (assoofs_sb->free_blocks & (1 << i))
		break; // cuando aparece el primer bit 1 en free_block dejamos de recorrer el mapa de bits, i tiene la posicion del primer bloque libre




if(i >= ASSOOFS_MAX_FILESYSTEM_OBJECTS_SUPPORTED){
	//esta lleno
	printk(KERN_ERR "Cannot fetch a new free block. Bitmap is full\n");
	return -1;
}

*block = i; // Escribimos el valor de i en la direccion de memoria indicada como segundo argumento en la funcion

printk(KERN_INFO "Data block number fetch to the new inode\n");

assoofs_sb->free_blocks &= ~(1 << i);
assoofs_save_sb_info(sb);

return 0;


}













static int assoofs_create(struct inode *dir, struct dentry *dentry, umode_t mode, bool excl) {


	//1. Crear el inodo en memoria, mismos pasos que los dos anteriores pero ahora no traemos la informacion persistente con get inod info sino que la creamos nosotros
	struct inode *inode;
	struct super_block *sbMem; 
	uint64_t noInodos;
	struct assoofs_inode_info *inodePersistente;


	struct buffer_head *bh;
	struct assoofs_dir_record_entry *duplas; 
	struct assoofs_inode_info *parentPersistente;

    	printk(KERN_INFO "New file request\n");
	
		//para inicializarlo hay que pasarle el superbloque y no lo tengo, pero, lo puedo sacar del inodo padre
	sbMem = dir->i_sb;
	noInodos = ((struct assoofs_super_block_info *)sbMem->s_fs_info)->inodes_count;
		//puedo inicializar el inodo
	inode = new_inode(sbMem);
	


		//asignar informacion
	//(el numero de inodo ahora es nuevo, con lo cual hay que ver los inodos que hay mirando el superbloque la info persistente y poner 1 mas)
	
	inode->i_ino = noInodos+1;

	if(noInodos > ASSOOFS_MAX_FILESYSTEM_OBJECTS_SUPPORTED){

	    printk(KERN_ERR "The hole file system is full, number of actual blocks is 64\n");
		return -1; //cancelariamos la creacion del nuevo archivo

	}
	
	
	inode->i_sb = sbMem; //puntero al superbloque
	inode->i_op = &assoofs_inode_ops;
 	inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);

	

	
		//reservo memoria para una variable de tipo inode info
	inodePersistente = kmalloc(sizeof(struct assoofs_inode_info), GFP_KERNEL); 

	inodePersistente->inode_no = inode->i_ino;//es el mismo que en get inode info nos pasaban para encontrarlo en el almacen de inodos, como su identificador en el almacen
		//en lugar de asignar el datablock number se lo paso como referencia a la funcion siguiente que ya lo hace

	
	inodePersistente->mode = mode;
	//asigno el tamaño de archivo que linka el inodo de disco
	inodePersistente->file_size = 0;

	inode->i_private = inodePersistente; //se lo meto al inodo en memoria en el cajon de sastre

	inode->i_fop = &assoofs_file_operations; //como el inodo corresponde a la creacion de un archivo, se asignan las operaciones sobre archivos

	assoofs_sb_get_a_freeblock(sbMem, &inodePersistente->data_block_number);
	
		//guardamos en disco la informacion persistente del nuevo inodo en el almacen de inodos
	assoofs_add_inode_info(sbMem, inodePersistente);

	
	//permisos, le asigno el padre (inodo,inodo padre y permisos que me viene como parametro
	//para las operaciones sobre archivo creo una funcion
	
	inode_init_owner(inode, dir, mode);

	//por ultimo lo aniadimos a la jerarquia de inodos (nombre, no que hay en memoria y que se inicio en fill supper cuando creamos el inodo raiz)
	d_add(dentry, inode);

	printk(KERN_INFO "inode successfully created\n");


	//2.Crear una nueva dupla en el bloque de contenido del directorio padre (un nuevo dir_record_entry)

		
		//traer el bloque de contenido de disco (superbloque, no bloque a leer sera el correspondiente al contenido de ese inodo)
	
	parentPersistente = dir->i_private; //saco la informacion persistente del inodo padre en memoria

	bh = sb_bread(sbMem, parentPersistente->data_block_number);
	duplas = (struct assoofs_dir_record_entry *)bh->b_data;
	

		//tengo el bloque de contenido entonces necesito situarme en el ultimo record entry para poner la nueva dupla en el siguiente
		//como se el numero de entradas del directorio simplemente se las sumo al puntero y como aparece en la 1 si tiene 20 = 20+1 la entrada 21, justo donde quiero poner mi nueva dupla


	duplas = duplas + parentPersistente->dir_children_count;

		//ahora actualizo con una nueva dupla (nombre y no inodo, el nombre no hay asignacion per se, debe ser mediante strcpy)
	duplas->inode_no = inodePersistente->inode_no; //el mismo que le identifica en el almacen de inodos y el mismo que tiene el inodo en memoria (inode->i_ino)
	strcpy(duplas->filename, dentry->d_name.name);

		//escribir en disco de nuevo este bloque que me he traido y le he puesto una dupla mas
	mark_buffer_dirty(bh); //marco el bloque como sucio
	sync_dirty_buffer(bh); //llamo a forzar el volcado de cambios
	brelse(bh);

	printk(KERN_INFO "Added new dir_record_entry to the parent dir content block\n");

	//3.Actualizar el contador de elementos en el directorio padre (dir_children_count) en su info persistente

		//aqui en memoria le indico que tiene un arch. mas
	parentPersistente->dir_children_count++;

		//debo aplicar este cambio sobre el almacen de inodos (ACTUALIZARLO alli buscandolo por block number y sustituyendo la entrada inode info por la de este)
	assoofs_save_inode_info(sbMem, parentPersistente);
	
	printk(KERN_INFO "AssooCreate Finished succesfully\n");


    return 0;
}


/*NOS PERMITE CREAR INODOS PARA DIRECTORIOS NUEVOS*/
//se nos pasa el directorio padre, la entry y el modo

static int assoofs_mkdir(struct inode *dir , struct dentry *dentry, umode_t mode) {
    
   
	struct inode *inode;
	struct assoofs_inode_info *inode_info;
	struct super_block *sb;
	struct assoofs_super_block_info *sbPersistente;
	struct assoofs_inode_info *inodePPadre;
	struct buffer_head *bh;
	struct assoofs_dir_record_entry *duplas;
	uint64_t count;

printk(KERN_INFO "New directory request\n");

	//1.Creamos el inodo en memoria
	
	sb = dir->i_sb;
	
	sbPersistente = sb->s_fs_info;

	count = sbPersistente->inodes_count; 

	inode = new_inode(sb);
	
	inode_init_owner(inode, dir, (S_IFDIR | mode)); //OJO CON EL MODO!
	
	//2. Asigno operaciones al inodo

	inode->i_ino = count + 1;
	
	if(count > ASSOOFS_MAX_FILESYSTEM_OBJECTS_SUPPORTED){

	    printk(KERN_ERR "The hole file system is full, number of actual blocks is 64\n");
		return -1; //cancelariamos la creacion del nuevo archivo

	}

	inode->i_sb = sb;
	inode->i_op = &assoofs_inode_ops;
	inode->i_fop = &assoofs_dir_operations;
	inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode); 

///////////////////////////////////////CREO LA INFO PERSISTENTE////////////////////////////////////////

	inode_info = kmalloc(sizeof(struct assoofs_inode_info), GFP_KERNEL);

	inode_info->mode = (S_IFDIR | mode); //El modo no es tal cual el que me pasan, sino que hay que hacerle un or
	inode_info->inode_no = inode->i_ino;
	assoofs_sb_get_a_freeblock(sb, &inode_info->data_block_number);
	inode_info->dir_children_count = 0; //inicialmente el directorio se crea vacío
	
		//grabamos en disco la info persistente (almacen de inodos COMO NUEVA ENTRADA)
	assoofs_add_inode_info(sb, inode_info);


////////////////////////////////////////////////////////////////////////////////////////////////////////	

	inode->i_private = inode_info;

	d_add(dentry, inode);


	//3.Aniadir la dupla correspondiente en el contenido del directorio padre
	
	inodePPadre = dir->i_private;
	
	bh = sb_bread(sb, inodePPadre->data_block_number);
	duplas = (struct assoofs_dir_record_entry *)bh->b_data;

		//hay tantas duplas como elementos en el directorio padre
	duplas = duplas + inodePPadre->dir_children_count;

	duplas->inode_no = inode_info->inode_no;
	strcpy(duplas->filename, dentry->d_name.name); //no hay asignacion estatica de strings

		//he modificado el bloque aniadindo la dupla (no es inodo ni superbloque no tengo funcion para actualizar), entonces lo que hago es actualizarlo en disco
	
	mark_buffer_dirty(bh); //marco el bloque como sucio
	sync_dirty_buffer(bh); //llamo a forzar el volcado de cambios
	brelse(bh);


	//4. Actualizar el inodo padre en disco con un children mas y como lo hemos modificado (el inodo y esta en memoria cambiado) actualizarlo en disco
	
	inodePPadre->dir_children_count++;
	
		//llamo a la funcion que me actualiza el inodo en el almacen de inodos
	assoofs_save_inode_info(sb, inodePPadre);

	
	printk(KERN_INFO "AssooMKdir Finished succesfully\n");

	 return 0;
}




















/*
 *  Operaciones sobre el superbloque (se asignan al sb, que es mi superbloque en memoria)
 */
static const struct super_operations assoofs_sops = {
    .drop_inode = generic_delete_inode, //funcion de libfs cuando se borre un inode
};










/*
 *  InicializaciÃ³n del superbloque
 */
int assoofs_fill_super(struct super_block *sb, void *data, int silent) {   //FUNCION QUE LE PASAMOS AL MOUNT PARA QUE NOS INICIALICE EL SUPERBLOQUE fill super (=llenar superbloque) CON LOS DATOS PARA ASSOOFS
   
    
    // 1.- Leer la informaciÃ³n persistente del superbloque del disco
      
        struct buffer_head *bh; //para manejar la lectura (es como auxiliar)
        struct assoofs_super_block_info *assoofs_sb; //sera nuestro bloque leido de disco
        struct inode *root_inode; //creo mi inodo en este caso raiz 

        bh = sb_bread(sb, ASSOOFS_SUPERBLOCK_BLOCK_NUMBER); //le pasare el puntero al superbloque en memoria (de mi sistema de ficheros)(me viene como parametro) y el numero de bloque que quiero leer de disco
        printk(KERN_INFO "assoofs_fill_super request\n");

        /*cogemos el contenido del bloque 0 (campo b_data del struct que maneja la lectura) en nuestro bloque leido de disco*/
        
        assoofs_sb = (struct assoofs_super_block_info *)bh->b_data; //guardo el contenido que tenia el superbloque en disco bdata es un cajon de satre
        

            
    printk(KERN_INFO "Correctly read information from disk\n");

    // 2.- Comprobar los parÃ¡metros del superbloque (que lo que he leido de disco, del bloque en disco, esta bien)
    
    
        if(assoofs_sb->magic == ASSOOFS_MAGIC && assoofs_sb->block_size == ASSOOFS_DEFAULT_BLOCK_SIZE)
        {
                printk(KERN_INFO "Testing of read info succesfully passed. Correct block size and Magicnumber\n");
            
        }
        else
        {
                printk(KERN_INFO "Testing of read info failed\n");
                return -1;

        }
    
        
    // 3.- Escribir la informaciÃ³n persistente leÃ­da del dispositivo de bloques en el superbloque sb, incluÃ­do el campo s_op con las operaciones que soporta. (cargamos el superbloque leido de disco en memoria)
    
        //Numero magico
            sb->s_magic = ASSOOFS_MAGIC;
        
        //Block size
            sb->s_maxbytes = ASSOOFS_DEFAULT_BLOCK_SIZE;
        
        //Operaciones
            sb->s_op = &assoofs_sops;
        
        //Guardo el contenido del bloque 0 de disco (superbloque) en memoria para no tener que estar leyendolo de disco cosntantemente si lo necesitara luego
            sb->s_fs_info = assoofs_sb;
         
            printk(KERN_INFO "Superblock correctly loaded into memory\n");
    
    
	
	
	
	
	// 4.- Crear el inodo raÃ­z y asignarle operaciones sobre inodos (i_op) y sobre directorios (i_fop)
    
        root_inode = new_inode(sb); //funcion que inicializa el inodo
    
        //asignacion de propietario y permisos
        inode_init_owner(root_inode, NULL, S_IFDIR); //le paso el inodo, el directorio padre del directorio (es el raiz y no tiene padre) y permisos (S_IFDIR porque es una carpeta; S_IFREG si es un fichero regular el inodo)
        
        root_inode->i_ino = ASSOOFS_ROOTDIR_INODE_NUMBER; //numero de inodo raiz
        root_inode->i_sb = sb; //puntero al superbloque en memoria
        root_inode->i_op = &assoofs_inode_ops; //operaciones sobre el inodo
        root_inode->i_fop = &assoofs_dir_operations; //operaciones sobre el archivo o directorio que representa dependiendo
        root_inode->i_atime = root_inode->i_mtime = root_inode->i_ctime = current_time(root_inode); //fecha ult. acceso, fecha modificacion y fecha de creacion
       
	    root_inode->i_private = assoofs_get_inode_info(sb, ASSOOFS_ROOTDIR_INODE_NUMBER); //informacion que queramos que persista en el i-nodo (al ser un struct inode admite campos genericos y este almacena la info que queramos que persista) la informacion persistente en disco del directorio raiz. ES UNA CONSTANTE QUE ES UN 1, ES DECIR, LA PRIMERA ENTRADA DEL ALAMACEN DE INODOS
        
        /*INTRODUCCION DE INODO EN LA JERARQUÃA DE INODOS en memoria*/
        
        sb->s_root = d_make_root(root_inode); //creamos la jerarquia de inodos con dmake root, le hacemos la raiz de todo el sistema de ficheros
        
       		/*if(!sb->s_root){
       			printk(KERN_INFO "Error when calling d_make_root\n");
			return -1;
       			
		}
	   */
	   
	   
	    printk(KERN_INFO "Root i-node correctly created\n");

      /*Como el struct que me ayuda en la lectura no lo uso mas, lo libero*/
    brelse(bh);
    return 0;
}

/*
 *  Montaje de dispositivos assoofs
 */
static struct dentry *assoofs_mount(struct file_system_type *fs_type, int flags, const char *dev_name, void *data) {  //FUNCION QUE SE LLAMA AL MONTAR ("mount -t assoofs")
    struct dentry *ret;    
printk(KERN_INFO "assoofs_mount request\n");

    ret = mount_bdev(fs_type, flags, dev_name, data, assoofs_fill_super); //funcion de libfs que nos permite montar cualquier dispositivo de bloques, los argumentos son los mismos que los de mount salvo el ultimo (llenar superbloque)
    // Control de errores a partir del valor de ret. En este caso se puede utilizar la macro IS_ERR: if (IS_ERR(ret)) ...
    if(IS_ERR(ret)){
	printk(KERN_ERR "MOUNT ERROR\n");
    }
printk(KERN_INFO "Mounted Correctly\n");
    return ret;

}

/*
 *  assoofs file system type (INFORMACION MINIMA QUE DEFINE NUESTRO SISTEMA DE FICHEROS)
 */
static struct file_system_type assoofs_type = {  //Declaramos nuestra propia variable de tipo struct file_system_type
    .owner   = THIS_MODULE, //modulo actual que es el que quiero cargar
    .name    = "assoofs", //nombre del sistema de ficheros (se lo tendremos que poner al montarlo "mount -t" asi que tenerlo presente)
    .mount   = assoofs_mount, //llamar a esta funcion cuando se monte el sistema de ficehros
    .kill_sb = kill_litter_super, //llamar a esta funcion cuando se desmonte el sistema de ficheros
};

static int __init assoofs_init(void) {   //funcion que se ejecuta cuando se carga el modulo del kernel
    int ret;
    printk(KERN_INFO "assoofs_init request\n");
    ret = register_filesystem(&assoofs_type);  //funciones para montar y desmontar el modulo, ya vienen el libfs con lo cual solo las usamos pero necesitamos pasar por referencia el struct
    

	// Control de errores a partir del valor de ret

	return ret;



}

static void __exit assoofs_exit(void) {   //funcion que se ejecuta cuando solicitamos desmontar el modulo del kernel
    int ret;
    printk(KERN_INFO "assoofs_exit request\n");
    ret = unregister_filesystem(&assoofs_type);
    // Control de errores a partir del valor de ret
}

module_init(assoofs_init);
module_exit(assoofs_exit);
