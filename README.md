# Bienvenido.  

--------------------------------------------------------------------------------------------------------

## 📦 Sistema de Gestión Comercial RAQFRAN

Sistema de gestión comercial desarrollado en lenguaje C, orientado a consola, que permite administrar inventario, realizar ventas, controlar caja y generar reportes básicos. El proyecto simula el funcionamiento de un punto de venta (POS) para pequeños comercios.

--------------------------------------------------------------------------------------------------------

## 🚀 Funcionalidades principales

## 🗂 Gestión de Inventario

Registro manual de nuevos productos.

Carga inicial de productos desde archivo .txt.

Consulta del catálogo completo.

Búsqueda de productos por nombre o palabra clave.

Modificación de:

Precio

IVA

Stock

Eliminación lógica de productos.

Alerta visual cuando el stock es bajo.

--------------------------------------------------------------------------------------------------------

## 🛒 Punto de Venta

Venta de productos con validación de stock.

Carrito de compras dinámico.

Ordenamiento del carrito por cantidad (algoritmo burbuja).

Cálculo automático de:

Subtotal

IVA

Total a pagar

Confirmación o cancelación de la venta.

Actualización automática del inventario.

--------------------------------------------------------------------------------------------------------

## 💰 Control de Caja

Apertura de caja.

Registro de ingresos por ventas.

Cierre de caja con total de la sesión.

--------------------------------------------------------------------------------------------------------

## 📊 Reportes

Estado de la caja (abierta/cerrada).

Total vendido en la sesión.

Cantidad total de productos en inventario.

Valor monetario del inventario.

--------------------------------------------------------------------------------------------------------

## 🧠 Estructuras y Algoritmos Usados

Estructuras (struct) para productos y carrito.

Arreglos estáticos para inventario y ventas.

Búsqueda secuencial por ID y por nombre.

Ordenamiento burbuja del carrito por cantidad.

Lectura y escritura desde archivo de texto.

Validación de entradas (enteros y decimales).

--------------------------------------------------------------------------------------------------------

## 📁 Archivo de Productos

El sistema carga productos desde un archivo de texto con el siguiente formato:

Codigo;Nombre;Descripcion;Precio;Stock

tomar en cuenta que en el codigo se debe cambiar la direccion del archivo de inventario txt
Ejemplo:
A001;Arroz;Arroz blanco 1kg;1.25;50
A002;Azucar;Azucar refinada;1.10;30

--------------------------------------------------------------------------------------------------------


## ⚠️ Si el archivo no existe, el sistema inicia sin productos.

🖥 Requisitos

Compilador GCC o compatible.

Sistema operativo:

Windows

Linux

macOS

Consola que soporte códigos ANSI (colores).

--------------------------------------------------------------------------------------------------------


## 📌 Consideraciones Importantes

El inventario máximo es de 100 productos.

El carrito permite hasta 50 ítems.

El IVA por defecto es 12% (configurable).

El stock mínimo para alerta es 5 unidades.

La eliminación de productos es lógica, no física.

--------------------------------------------------------------------------------------------------------

## 👨‍💻 Autor

Proyecto académico desarrollado como práctica de: PROGRAMACION I  

Por: EMILY ARMIJO Y FRANCISCO YEPEZ

Programación estructurada en C

Manejo de archivos

Algoritmos básicos

Diseño de menús en consola
