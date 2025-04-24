# Proyecto Chat Cliente-Servidor en C (Sockets TCP con Pthreads)

Este proyecto es una implementación de un sistema de chat en C utilizando sockets TCP y hilos (`pthread`). Permite que múltiples clientes se conecten a un servidor y compartan mensajes en tiempo real.

---

## Estructura del Proyecto

```
.
├── chat_server.c        # Código del servidor
├── chat_client.c        # Código del cliente
├── Makefile             # Script para compilar
├── chat.log             # Archivo de registro del servidor (se crea automáticamente)
└── README.md            # Este archivo
```

---

## Cómo Compilar y Ejecutar

### Requisitos

- Sistema Linux (o WSL en Windows)
- `gcc` (compilador de C)
- `make`

### Compilación

```bash
make
```

Esto compilará `chat_server` y `chat_client`.

### Ejecutar Servidor

```bash
./chat_server
```

El servidor escuchará en el puerto 12345 y registrará mensajes y conexiones en `chat.log`.

### Ejecutar Cliente

```bash
./chat_client
```

El cliente solicitará el nombre de usuario y permitirá enviar y recibir mensajes en tiempo real.

---

## Funcionalidades

### Cliente

- Se conecta con un nombre de usuario.
- Puede enviar y recibir mensajes en tiempo real.
- Comando especial `exit` o `quit` para salir del chat.

### Servidor

- Gestiona múltiples clientes con hilos (`pthread`).
- Reenvía mensajes de un cliente a todos los demás.
- Guarda registros de actividad en `chat.log`.
- Maneja señales (Ctrl+C) para cerrar el servidor limpiamente.

---

## Makefile

Compila automáticamente los binarios:

```Makefile
all:
	gcc -o chat_server chat_server.c -lpthread
	gcc -o chat_client chat_client.c

clean:
	rm -f chat_server chat_client chat.log
```

Usos:

```bash
make        # Compila ambos programas
make clean  # Limpia los binarios y logs
```

---

## Cómo ejecutar el proyecto en WSL con VS Code

Si estás usando Windows y deseas correr este proyecto con Linux usando WSL, existen dos maneras de hacerlo:

### Primera forma:

**Paso 1: Instalar WSL**

Abre PowerShell como administrador y ejecuta:

```powershell
wsl --install
```

> Asegúrate de reiniciar tu PC después de la instalación. Por defecto, se instalará Ubuntu.

**Paso 2: Instalar las extensiones necesarias en VS Code**

Abre VS Code y asegúrate de tener instaladas estas extensiones:

- **Remote - WSL** (de Microsoft)
- **C/C++** (de Microsoft)
- **Code Runner** (opcional, para ejecutar más fácil)

**Paso 3: Abrir el proyecto en WSL**

1. Abre VS Code.
2. Pulsa `Ctrl + Shift + P` y busca `WSL: Open Folder in WSL`.
3. Selecciona la carpeta donde está tu proyecto (`chat_server.c`, `chat_client.c`, etc).
4. VS Code se abrirá dentro del entorno Linux.

**Paso 4: Compilar y ejecutar**

En la terminal de VS Code (ya dentro de WSL):

```bash
make        # Para compilar
./chat_server   # Ejecutar servidor en una terminal
./chat_client   # Ejecutar cliente en otra terminal
```

### Segunda forma:

1. **Instalar WSL (si no está instalado):**

```powershell
wsl --install
```

2. **Instalar una distribución de Linux:** (por ejemplo Ubuntu desde Microsoft Store)

3. **Abrir VS Code en modo WSL:**

   - Abre la terminal de Windows.
   - Ejecuta: `wsl`
   - En Linux, navega a tu carpeta del proyecto y ejecuta:

```bash
code .
```

> Esto abrirá Visual Studio Code conectado directamente a tu entorno WSL.

## Librerías necesarias

Para compilar y ejecutar el proyecto en Ubuntu/WSL, asegúrate de tener instaladas las siguientes herramientas/librerías:

```bash
sudo apt update
sudo apt install build-essential
```

Esto instalará:

- `gcc` y `g++`: compiladores de C y C++
- `make`: para utilizar el Makefile
- `libc6-dev`: librerías estándar del sistema (incluye `libstdc++`)

**Nota:**  
Si estás usando una distribución **Alpine Linux**, por ejemplo en un contenedor Docker, debes instalar `libstdc++` explícitamente con:

```bash
apk add libstdc++
```

## Créditos

Desarrollado para el curso de Telemática, como parte del proyecto de chat en C con sockets TCP.
