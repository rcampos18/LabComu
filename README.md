# LabComu
MSM platform
Requerimientos 
>>

La programación del sistema (celular, Raspberry, servidores y terminales) deberá ser implementado por los estudiantes utilizando el lenguaje de programación C únicamente.

El sistema operativo de los servidores en CentOS de 64 bits.

El estudiante puede utilizar como máximo una Raspberry o sistema similar.

El estudiante no puede utilizar Arduinos o hardware compatible con Arduino.

La base de datos es MariaDB.

El celular cumple los detalles expuestos en la descripción general.

La interfaz del celular con el transmisor inalámbrico cumple los detalles expuestos en la descripción general.

El servidor de señalización cumple los detalles expuestos en la descripción general.

El servidor de Base de Datos. cumple los detalles expuestos en la descripción
general.

Las terminales cumplen los detalles expuestos en la descripción general.

Si el usuario se queda sin saldo, debe de notificársele y la comunicación debe de cesar hasta que el usuario realice una recarga.

Si el usuario envía un mensaje y este no se logra entregar con éxito a la terminal, el sistema debe de ser capaz de identificar esta situación y debe de comunicarle al usuario que su mensaje no fue entregado.

El sistema de pruebas que está implementado en el celular lo que hace es medir la cantidad de paquetes que se pierden en la transmisión (basándose en un histórico), y muestra en forma gráfica una barra que representa la cantidad relativa de paquetes que se pierden en la transmisión. Para esto debe de realizar un sistema que transmita información constantemente y determine un porcentaje historial de la información que no se ha entregado exitosamente.

El sistema no puede tener bugs, glitches o cualquier otra evidencia de un mal
funcionamiento.

El sistema cobra saldo por byte transmitido (1 byte vale 1 crédito).

Si el usuario envía una imagen mayor a la cantidad de saldo que tiene disponible, el mensaje se entregará exitosamente, sin embargo, el saldo del usuario quedará negativo en correspondencia a la cantidad en la que se excedió en el saldo.

El flujo de comunicación debe de ser como en un sistema de mensajería multimedia normal, en el cual cualquiera de las dos partes puede enviar un mensaje en cualquier momento (comunicación asíncrona). No se permite el esquema de que el envío de mensajes debe de llevar un orden.

El servidor de señalización es el encargado de orquestar toda la comunicación recibiendo y enviando mensajes asíncronamente a través del transmisor inalámbrico, y a su vez direccionando los mensajes a través de internet hacia la Terminal A y la Terminal B.

Cuando el usuario ejecute una recarga, el sistema de la base de datos y la el servidor de señalización le indicarán si la operación se pudo realizar con éxito.

OPCIONAL: El sistema tiene interfaz gráfica.

El sistema debe tener un mecanismo para detectar que un paquete de la imagen se corrompió y retrasmitir el mismo. La imagen ensamblada debe apreciarse correctamente en la terminal receptora.
