#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_PRODS 100
#define MAX_CART 50
#define NAME_LEN 50
#define DESC_LEN 120
#define STOCK_MINIMO 5

/* --- COLORES Y FORMATO PARA CONSOLA --- */
#define BLANCO "\x1b[0m"
#define ROJO     "\x1b[31m"
#define VERDE    "\x1b[32m"
#define AMARILLO "\x1b[33m"
#define AZUL     "\x1b[34m"
#define MAGENTA  "\x1b[35m"
#define CELESTE     "\x1b[36m"
#define BLANCO   "\x1b[37m"
#define NEGRITA  "\x1b[1m"

/* Estructuras */
typedef struct {
    int id;                 // ID interno numérico (1, 2, 3...)
    char codigo_txt[20];    // Para guardar el código A001, B001 del archivo
    char nombre[NAME_LEN];
    char descripcion[DESC_LEN]; 
    double precio;
    int stock;
    int iva;    
    int activo; // 1 = existe, 0 = eliminado
} Producto;

typedef struct {
    int prod_id;
    int cantidad;
    double precio_unitario;
    int iva;
} ItemCarrito;

/* Almacenamiento Global */
Producto productos[MAX_PRODS + 1]; // índice desde 1
int productos_count = 0;

/* Caja Global */
int caja_abierta = 0;
double caja_ingresos = 0.0;
int IVA_POR_DEFECTO = 12;

/* ---------- PROTOTIPOS ---------- */

void leer_linea(char *buf, int len);
int leer_entero(const char *msg);
double leer_double(const char *msg);
void esperar_enter();
void limpiar_pantalla();
Producto* buscar_producto_por_id(int id);
void listar_y_ajustar_productos();

/* ---------- FUNCIONES AUXILIARES ---------- */

void limpiar_pantalla() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void esperar_enter() {
    printf("\nPresiona ENTER para continuar...");
    char temp[10]; 
    leer_linea(temp, 10);
}

void leer_linea(char *buf, int len) {
    if (fgets(buf, len, stdin) != NULL) {
        size_t n = strlen(buf);
        if (n > 0 && buf[n-1] == '\n')
            buf[n-1] = '\0';
    } else {
        buf[0] = '\0';
    }
}

int leer_entero(const char *msg) {
    char buf[128];
    int x;
    while (1) {
        if (msg) printf("%s", msg);
        leer_linea(buf, sizeof(buf));
        if (sscanf(buf, "%d", &x) == 1) return x;
        printf("Entrada invalida. Ingresa un numero entero.\n");
    }
}

double leer_double(const char *msg) {
    char buf[128];
    double x;
    while (1) {
        if (msg) printf("%s", msg);
        leer_linea(buf, sizeof(buf));
        if (sscanf(buf, "%lf", &x) == 1) return x;
        printf("Entrada invalida. Ingresa un numero decimal.\n");
    }
}

/* ---------- GENERAR ID ---------- */
int generar_id() {
    static int next = 1;
    return next++;
}

/* ---------- CARGA DE ARCHIVOS ---------- */
void cargar_productos_desde_archivo(const char *nombre_archivo) {
    FILE *f = fopen(nombre_archivo, "r");
    if (!f) {
        printf(AMARILLO "ADVERTENCIA: No se pudo abrir el archivo '%s'. Iniciando vacio.\n" BLANCO, nombre_archivo);
        esperar_enter();
        return;
    }

    char linea[256];

    while (fgets(linea, sizeof(linea), f)) {
        if (productos_count + 1 > MAX_PRODS) {
            printf("Memoria llena, no se pueden cargar mas productos.\n");
            break;
        }

        if (strlen(linea) < 5) continue;

        Producto p;
        memset(&p, 0, sizeof(Producto));

        // Formato: Codigo;Nombre;Categoria;Precio;Stock
        int leidos = sscanf(
            linea,
            "%19[^;];%49[^;];%119[^;];%lf;%d",
            p.codigo_txt,    
            p.nombre,        
            p.descripcion,   
            &p.precio,       
            &p.stock         
        );

        if (leidos == 5) {
            p.id = generar_id(); 
            p.iva = IVA_POR_DEFECTO;
            p.activo = 1;
            productos[++productos_count] = p;
        }
    }

    fclose(f);
    printf(VERDE "--> Se cargaron %d productos desde el archivo.\n" BLANCO, productos_count);
    esperar_enter();
}

/* ---------- BUSCAR PRODUCTO ---------- */
Producto* buscar_producto_por_id(int id) {
    for (int i = 1; i <= productos_count; i++) {
        if (productos[i].id == id && productos[i].activo)
            return &productos[i];
    }
    return NULL;
}

/* ---------- GESTION PRODUCTOS ---------- */

void registrar_producto() {
    limpiar_pantalla();
    printf(CELESTE "========================================\n");
    printf("        REGISTRAR NUEVO PRODUCTO        \n");
    printf("========================================\n" BLANCO);

    if (productos_count + 1 > MAX_PRODS) {
        printf(ROJO "Error: Memoria llena (Max %d productos).\n" BLANCO, MAX_PRODS);
        esperar_enter();
        return;
    }

    Producto p;
    p.id = generar_id();
    strcpy(p.codigo_txt, "MANUAL"); 

    printf("ID Interno asignado: " NEGRITA "%d\n\n" BLANCO, p.id);

    printf(AMARILLO "Nombre del producto: " BLANCO);
    leer_linea(p.nombre, NAME_LEN);

    printf(AMARILLO "Categoria/Descripcion: " BLANCO);
    leer_linea(p.descripcion, DESC_LEN);

    do {
        printf(AMARILLO "Precio unitario: " BLANCO);
        p.precio = leer_double(NULL);
        if (p.precio <= 0) printf(ROJO "   [!] El precio debe ser positivo.\n" BLANCO);
    } while (p.precio <= 0);

    do {
        printf(AMARILLO "Stock inicial: " BLANCO);
        p.stock = leer_entero(NULL);
        if (p.stock < 0) printf(ROJO "   [!] El stock no puede ser negativo.\n" BLANCO);
    } while (p.stock < 0);

    p.iva = IVA_POR_DEFECTO;
    p.activo = 1;

    productos[++productos_count] = p;

    printf(VERDE "\n[OK] Producto registrado exitosamente.\n" BLANCO);
    esperar_enter();
}

void listar_y_ajustar_productos() {
    while(1) {
        limpiar_pantalla();
        if (productos_count == 0) {
            printf(ROJO "No hay productos registrados.\n" BLANCO);
            esperar_enter();
            return;
        }

        printf(CELESTE "===============================================================\n");
        printf("                  CATALOGO DE PRODUCTOS                        \n");
        printf("===============================================================\n" BLANCO);
        printf(NEGRITA "%-4s | %-6s | %-20s | %-10s | %-8s\n" BLANCO, 
               "ID", "CODIGO", "NOMBRE", "PRECIO", "STOCK");
        printf("---------------------------------------------------------------\n");

        for (int i = 1; i <= productos_count; i++) {
            if (!productos[i].activo) continue;

            if (productos[i].stock <= STOCK_MINIMO) {
                printf(ROJO "%-4d | %-6s | %-20s | $%-9.2f | %-6d (BAJO)\n" BLANCO,
                    productos[i].id, productos[i].codigo_txt, productos[i].nombre, productos[i].precio, productos[i].stock);
            } else {
                printf("%-4d | %-6s | %-20s | $%-9.2f | %-6d\n",
                    productos[i].id, productos[i].codigo_txt, productos[i].nombre, productos[i].precio, productos[i].stock);
            }
        }
        printf("---------------------------------------------------------------\n");

        printf(AMARILLO "\n[OPCIONES]\n" BLANCO);
        printf("1) Modificar un producto\n");
        printf("2) Volver al menu anterior\n");
        
        int op = leer_entero("\nSeleccione opcion: ");

        if (op == 2) return;
        if (op != 1) continue;

        /* MODIFICAR PRODUCTO */
        int id;
        printf(AMARILLO "\nIngrese ID del producto a modificar: " BLANCO);
        id = leer_entero(NULL);
        
        Producto *p = buscar_producto_por_id(id);
        if (!p) {
            printf(ROJO "[!] Producto no encontrado.\n" BLANCO);
            esperar_enter();
            continue;
        }

        int sub;
        do {
            limpiar_pantalla();
            printf(CELESTE "--- EDITANDO: %s (ID %d) ---\n" BLANCO, p->nombre, p->id);
            printf("Precio actual: $%.2f  |  Stock actual: %d\n", p->precio, p->stock);
            printf("--------------------------------\n");
            printf("1) Cambiar precio\n");
            printf("2) Cambiar IVA\n");
            printf("3) Ajustar stock (Inventario)\n");
            printf("4) " ROJO "Eliminar producto\n" BLANCO);
            printf("5) Volver al catalogo\n");

            printf(AMARILLO "\nSeleccione: " BLANCO);
            sub = leer_entero(NULL);

            if (sub == 1) {
                printf("Nuevo precio: ");
                p->precio = leer_double(NULL);
                printf(VERDE "[OK] Precio actualizado.\n" BLANCO);
                esperar_enter();
            }
            else if (sub == 2) {
                printf("Nuevo IVA (%%): ");
                p->iva = leer_entero(NULL);
            }
            else if (sub == 3) {
                printf("Nuevo stock total: ");
                p->stock = leer_entero(NULL);
                printf(VERDE "[OK] Stock actualizado.\n" BLANCO);
                esperar_enter();
            }
            else if (sub == 4) {
                char conf[10];
                printf(ROJO "¿Seguro que desea eliminarlo? (s/n): " BLANCO);
                leer_linea(conf, 10);
                if(tolower(conf[0]) == 's'){
                    p->activo = 0;
                    printf(ROJO "Producto eliminado.\n" BLANCO);
                    esperar_enter();
                    sub = 5; // Salir forzado
                }
            }
        } while (sub != 5);
    }
}

/* ---------- CAJA ---------- */

void abrir_caja() {
    limpiar_pantalla();
    printf(CELESTE "--- CONTROL DE CAJA ---\n\n" BLANCO);
    
    if (caja_abierta) {
        printf(AMARILLO "[!] La caja ya se encuentra abierta.\n" BLANCO);
        printf("Ingresos acumulados hasta el momento: $%.2f\n", caja_ingresos);
    } else {
        caja_abierta = 1;
        caja_ingresos = 0;
        printf(VERDE "[OK] CAJA APERTURADA CORRECTAMENTE.\n" BLANCO);
        printf("El sistema esta listo para realizar ventas.\n");
    }
    esperar_enter();
}

void cerrar_caja() {
    limpiar_pantalla();
    printf(CELESTE "--- CIERRE DE CAJA ---\n\n" BLANCO);

    if (!caja_abierta) {
        printf(ROJO "[!] La caja no esta abierta.\n" BLANCO);
    } else {
        printf("Cerrando sistema de cobro...\n");
        printf("--------------------------------\n");
        printf(VERDE NEGRITA "INGRESOS TOTALES DE LA SESION: $%.2f\n" BLANCO, caja_ingresos);
        printf("--------------------------------\n");
        
        caja_abierta = 0;
        caja_ingresos = 0;
        printf("La caja ha sido cerrada y los contadores reiniciados.\n");
    }
    esperar_enter();
}

/* ---------- VENTAS ---------- */

void venta() {
    limpiar_pantalla(); 

    /* 1. VALIDACIONES INICIALES */
    if (!caja_abierta) {
        printf(ROJO "\n[!] ERROR: La caja esta cerrada.\n" BLANCO);
        printf("Por favor, abre la caja desde el menu principal para comenzar a vender.\n");
        esperar_enter(); return;
    }

    if (productos_count == 0) {
        printf(ROJO "\n[!] ERROR: No hay productos cargados.\n" BLANCO);
        esperar_enter(); return;
    }

    ItemCarrito cart[MAX_CART + 1];
    int ccount = 0;

    /* 2. BUCLE DE VENTA */
    while (1) {
        limpiar_pantalla(); 
        
        // CABECERA
        printf(CELESTE "===============================================================\n");
        printf("                  PUNTO DE VENTA - NUEVA ORDEN                 \n");
        printf("===============================================================\n" BLANCO);

        // TABLA DE PRODUCTOS
        printf(NEGRITA "%-4s | %-25s | %-10s | %-8s\n" BLANCO, "ID", "PRODUCTO", "PRECIO", "STOCK");
        printf("---------------------------------------------------------------\n");
        
        for(int i = 1; i <= productos_count; i++) {
            if(productos[i].activo) {
                if(productos[i].stock <= STOCK_MINIMO) {
                    printf(ROJO "%-4d | %-25s | $%-9.2f | %-8d (BAJO)\n" BLANCO, 
                        productos[i].id, productos[i].nombre, productos[i].precio, productos[i].stock);
                } else {
                    printf("%-4d | %-25s | $%-9.2f | %-8d\n", 
                        productos[i].id, productos[i].nombre, productos[i].precio, productos[i].stock);
                }
            }
        }
        printf("---------------------------------------------------------------\n");

        // MOSTRAR CARRITO ACTUAL
        if (ccount > 0) {
            printf(VERDE "\n   --- CARRITO ACTUAL (%d items) ---\n" BLANCO, ccount);
            double sub_temp = 0;
            for(int k=1; k<=ccount; k++) {
                Producto *p_temp = buscar_producto_por_id(cart[k].prod_id);
                printf("   + %dx %s\n", cart[k].cantidad, p_temp->nombre);
                sub_temp += (cart[k].cantidad * cart[k].precio_unitario);
            }
            printf(VERDE "   Subtotal parcial: $%.2f\n" BLANCO, sub_temp);
            printf("---------------------------------------------------------------\n");
        }

        // INPUT
        int id;
        printf(AMARILLO "\n Ingresa el ID del producto (0 para finalizar/pagar): " BLANCO);
        id = leer_entero(NULL);

        if (id == 0) break; // Ir a pagar

        Producto *p = buscar_producto_por_id(id);
        if (!p) {
            printf(ROJO "\n[X] Producto con ID %d no encontrado.\n" BLANCO, id);
            esperar_enter();
            continue;
        }

        printf(CELESTE "\n   Seleccionado: %s\n" BLANCO, p->nombre);
        printf("   Precio: $%.2f | Stock: %d\n", p->precio, p->stock);

        int cant;
        printf(AMARILLO "    Cantidad a llevar (0 para cancelar): " BLANCO);
        cant = leer_entero(NULL);

        if (cant <= 0) continue;

        // VERIFICACIÓN DE STOCK
        int en_carrito = 0;
        for (int i=1; i<=ccount; i++) {
            if (cart[i].prod_id == id) en_carrito += cart[i].cantidad;
        }

        if ((cant + en_carrito) > p->stock) {
            printf(ROJO "\n[!] STOCK INSUFICIENTE\n" BLANCO);
            printf("    Stock real: %d | En carrito: %d | Solicitado: %d\n", p->stock, en_carrito, cant);
            esperar_enter();
            continue;
        }

        // AGREGAR AL CARRITO
        int found = 0;
        for (int i = 1; i <= ccount; i++) {
            if (cart[i].prod_id == id) {
                cart[i].cantidad += cant;
                found = 1;
                break;
            }
        }

        if (!found) {
            if (ccount + 1 > MAX_CART) {
                printf(ROJO "Carrito lleno.\n" BLANCO);
                break;
            }
            cart[++ccount].prod_id = id;
            cart[ccount].cantidad = cant;
            cart[ccount].precio_unitario = p->precio;
            cart[ccount].iva = p->iva;
        }
    }

    /* 3. FINALIZAR VENTA (TICKET) */
    if (ccount == 0) {
        printf(AMARILLO "\nOperacion cancelada. No se agregaron productos.\n" BLANCO);
        esperar_enter();
        return;
    }

    /* ==========================================================
       ALGORITMO DE ORDENAMIENTO (BURBUJA) - ASCENDENTE POR CANTIDAD
       ========================================================== */
    for (int i = 1; i < ccount; i++) {
        for (int j = 1; j <= ccount - i; j++) {
            // Si la cantidad actual es MAYOR que la siguiente, intercambiamos
            // para que los menores queden arriba (Ascendente).
            if (cart[j].cantidad > cart[j+1].cantidad) {
                ItemCarrito temp = cart[j];
                cart[j] = cart[j+1];
                cart[j+1] = temp;
            }
        }
    }
    /* ========================================================== */

    limpiar_pantalla();
    double subtotal = 0, total_iva = 0, total = 0;

    printf("\n\n");
    printf(BLANCO "************************************************\n" BLANCO);
    printf(CELESTE   "              TICKET DE VENTA                   \n" BLANCO);
    printf(BLANCO "************************************************\n" BLANCO);
    printf("%-20s %-5s %-10s %-10s\n", "PRODUCTO", "CANT", "P.UNIT", "TOTAL");
    printf("------------------------------------------------\n");
    
    // Imprimimos el carrito YA ORDENADO
    for (int i = 1; i <= ccount; i++) {
        Producto *p = buscar_producto_por_id(cart[i].prod_id);
        double imp = cart[i].cantidad * cart[i].precio_unitario;
        double iva_val = imp * ((double)cart[i].iva / 100.0);

        printf("%-20s %-5d $%-9.2f $%-9.2f\n",
               p->nombre, cart[i].cantidad, cart[i].precio_unitario, imp);

        subtotal += imp;
        total_iva += iva_val;
    }

    total = subtotal + total_iva;

    printf("------------------------------------------------\n");
    printf("Subtotal:      $%.2f\n", subtotal);
    printf("IVA:           $%.2f\n", total_iva);
    printf(VERDE NEGRITA "TOTAL A PAGAR: $%.2f\n" BLANCO, total);
    printf("************************************************\n");

    char buf[16];
    printf(AMARILLO "\nConfirmar venta (S = Si / N = No): " BLANCO);
    leer_linea(buf, sizeof(buf));

    if (tolower(buf[0]) == 's') {
        for (int i = 1; i <= ccount; i++) {
            Producto *p = buscar_producto_por_id(cart[i].prod_id);
            p->stock -= cart[i].cantidad;
        }
        caja_ingresos += total;
        printf(VERDE "\n[OK] Venta registrada correctamente.\n" BLANCO);
    } else {
        printf(ROJO "\n[X] Venta cancelada.\n" BLANCO);
    }
    esperar_enter();
}

/* ---------- REPORTES ---------- */

void reporte_resumen() {
    limpiar_pantalla();
    printf(CELESTE "========================================\n");
    printf("           REPORTE GERENCIAL            \n");
    printf("========================================\n" BLANCO);
    
    double valor_inventario = 0;
    int stock_total = 0;
    for(int i=1; i<=productos_count; i++) {
        if(productos[i].activo) {
            valor_inventario += (productos[i].precio * productos[i].stock);
            stock_total += productos[i].stock;
        }
    }

    printf("Estado de Caja:     ");
    if(caja_abierta) printf(VERDE "ABIERTA\n" BLANCO);
    else             printf(ROJO "CERRADA\n" BLANCO);

    printf("Ventas Sesion:      " VERDE "$%.2f\n" BLANCO, caja_ingresos);
    printf("----------------------------------------\n");
    printf("Productos unicos:   %d\n", productos_count);
    printf("Items en inventario: %d unidades\n", stock_total);
    printf("Valor del Inventario: " AMARILLO "$%.2f\n" BLANCO, valor_inventario);
    printf("========================================\n");
    
    esperar_enter();
}

/* ---------- ALGORITMO DE BUSQUEDA POR NOMBRE ---------- */
void buscar_producto_por_nombre() {
    limpiar_pantalla();
    printf(CELESTE "========================================\n");
    printf("        BUSQUEDA DE PRODUCTOS           \n");
    printf("========================================\n" BLANCO);

    if (productos_count == 0) {
        printf(ROJO "No hay productos para buscar.\n" BLANCO);
        esperar_enter();
        return;
    }

    char busqueda[50];
    printf(AMARILLO "Ingrese nombre o palabra clave: " BLANCO);
    leer_linea(busqueda, 50);

    // Convertir busqueda a minusculas para comparar
    char busqueda_lower[50];
    strcpy(busqueda_lower, busqueda);
    for(int i = 0; busqueda_lower[i]; i++){
        busqueda_lower[i] = tolower(busqueda_lower[i]);
    }

    printf("\n" NEGRITA "%-4s | %-25s | %-10s | %-8s\n" BLANCO, 
           "ID", "NOMBRE", "PRECIO", "STOCK");
    printf("----------------------------------------------------------\n");

    int encontrados = 0;

    for (int i = 1; i <= productos_count; i++) {
        if (!productos[i].activo) continue;

        // Crear copia temporal del nombre del producto en minusculas
        char nombre_temp[NAME_LEN];
        strcpy(nombre_temp, productos[i].nombre);
        for(int j = 0; nombre_temp[j]; j++){
            nombre_temp[j] = tolower(nombre_temp[j]);
        }

        // ALGORITMO: strstr busca si la 'busqueda' está dentro del 'nombre'
        if (strstr(nombre_temp, busqueda_lower) != NULL) {
            printf("%-4d | %-25s | $%-9.2f | %-8d\n", 
                productos[i].id, productos[i].nombre, productos[i].precio, productos[i].stock);
            encontrados++;
        }
    }

    printf("----------------------------------------------------------\n");
    
    if (encontrados == 0) {
        printf(ROJO "No se encontraron coincidencias con '%s'.\n" BLANCO, busqueda);
    } else {
        printf(VERDE "Se encontraron %d resultados.\n" BLANCO, encontrados);
    }

    esperar_enter();
}


/* ---------- MENÚS FORMALES ---------- */

void menu_productos() {
    int op;
    do {
        limpiar_pantalla();
        printf("\n");
        printf(CELESTE "   ___________________________________________________\n");
        printf("  |                                                   |\n");
        printf("  |            ADMINISTRACION DE INVENTARIO           |\n");
        printf("  |___________________________________________________|\n" BLANCO);
        printf("\n");
        printf("    1. Registrar Nuevo Articulo\n");
        printf("    2. Consultar Catalogo Completo\n");
        printf("    3. " AMARILLO "Buscar Producto por Nombre" BLANCO "\n"); // <--- NUEVA OPCION
        printf("    4. Regresar al Menu Principal\n");
        printf("\n");
        printf(CELESTE "   ___________________________________________________\n" BLANCO);
        
        printf(AMARILLO "\n  >> Seleccione una operacion: " BLANCO);
        op = leer_entero(NULL);

        if (op == 1) registrar_producto();
        else if (op == 2) listar_y_ajustar_productos();
        else if (op == 3) buscar_producto_por_nombre(); // <--- LLAMADA A LA FUNCION
        else if (op != 4) {
            printf(ROJO "\n  [!] Opcion no valida.\n" BLANCO);
            esperar_enter();
        }

    } while (op != 4);
}

void menu_principal() {
    int op;
    do {
        limpiar_pantalla();
        /* Encabezado estilo corporativo limpio */
        printf(AZUL "\n");
        printf("  ======================================================\n");
        printf("          SISTEMA DE GESTION COMERCIAL RAQFRAN          \n");
        printf("  ======================================================\n" BLANCO);
        printf("\n");
        
        printf("  " NEGRITA "MODULOS DEL SISTEMA:" BLANCO "\n\n");
        
        printf("    [1] Gestion de Inventario\n");
        printf("        " CELESTE "Catalogos y precios." BLANCO "\n\n");
        
        printf("    [2] Punto de Venta \n");
        printf("        " CELESTE "Facturacion y salida de mercaderia." BLANCO "\n\n");
        
        printf("    [3] Control de Caja\n");
        printf("        " CELESTE "Apertura, cierre." BLANCO "\n\n");
        
        printf("    [4] Reportes \n");
        printf("        " CELESTE "Resumen financiero y valoracion de stock." BLANCO "\n\n");
        
        printf("    [5] Finalizar Sesion\n");
        
        printf(AZUL "\n  ======================================================\n" BLANCO);
        printf(AMARILLO "  >> Ingrese el numero de modulo: " BLANCO);

        op = leer_entero(NULL);

        switch (op) {
            case 1: menu_productos(); break;
            case 2: venta(); break;
            case 3: {
                limpiar_pantalla();
                printf(AZUL "\n  === CONTROL DE CAJA ===\n\n" BLANCO);
                printf("    1. Apertura de Caja\n");
                printf("    2. Cierre de Caja (Arqueo)\n");
                printf("    3. Cancelar operacion\n");
                printf(AMARILLO "\n  >> Seleccione: " BLANCO);
                int s = leer_entero(NULL);
                if (s == 1) abrir_caja();
                else if (s == 2) cerrar_caja();
                break;
            }
            case 4: reporte_resumen(); break;
            case 5: 
                printf(VERDE "\n  Cerrando sistema de forma segura. Hasta luego.\n" BLANCO); 
                return;
            default: 
                printf(ROJO "\n  [!] Opcion no reconocida por el sistema.\n" BLANCO);
                esperar_enter();
        }

    } while (1);
}


int main() {
    printf("Iniciando STORE RAQFRAN...\n");
    /* IMPORTANTE: Asegúrate de que este archivo existe, 
       o el programa iniciará sin productos.
    */
    cargar_productos_desde_archivo("C:\\Users\\EliteBook\\Documents\\Primer semestre\\Programacion\\Proyecto\\Stock.txt");

    menu_principal();
    return 0;
}
