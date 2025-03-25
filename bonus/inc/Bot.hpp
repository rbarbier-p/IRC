#ifndef BOT_HPP
# define BOT_HPP

# include <iostream>

class Bot
{
	//mensaje para dei del futuro :
	//
	//el servidor hace un socket luego un bind a la ip que queremos 

	//mensaje para dei del futuro :
	//
	//el servidor hace un socket luego un bind a la ip que queremos 
	//luego se queda escuchando desde el puerto habilitado
	//si nosotros queremos conectarnos al server debemos crear un socket
	//y luego utilizar la funcion connect
	//pasarle nuestro socket la ip que conectamos y el size de la struct 
	//y ya en principio estaria hecha la conexion.//luego se queda escuchando desde el puerto habilitado
	//si nosotros queremos conectarnos al server debemos crear un socket
	//y luego utilizar la funcion connect
	//pasarle nuestro socket la ip que conectamos y el size de la struct 
	//y ya en principio estaria hecha la conexion.
	private :
		//necesita una estructura sockaddr_in necesito setear esa estructura 
		//conectarme al puerto y a la ip que me pasen por parametro.
		//necesito un socket
		//sin_family -> familia de direcciones se usa AF_INET para ipv4
		//sin_port -> puerto
		//sin_addr -> ipv4 
		//sin_zero[8] -> no se usa debe ser 0
	public :
		Bot(void);
		~Bot(void);
		void	run(void);
};

#endif /*BOT_HPP*/
