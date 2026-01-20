#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_PRODS 100
#define MAX_CART 50
#define NAME_LEN 50
#define DESC_LEN 120
#define STOCK_MINIMO 5


typedef struct {
    int id;
    char nombre[NAME_LEN];
    char descripcion[DESC_LEN];
    double precio;
    int stock;
    int iva;    // porcentaje entero 0, 12, 15, etc.
    int activo; // 1 = existe, 0 = eliminado
} Producto;

typedef struct {
    int prod_id;
    int cantidad;
    double precio_unitario;
    int iva;
} ItemCarrito;

/* almacenamiento */
Producto productos[MAX_PRODS + 1]; // índice desde 1
int productos_count = 0;

/* caja */
int caja_abierta = 0;
double caja_ingresos = 0.0;

/* IVA por defecto fijo */
int IVA_POR_DEFECTO = 12;

/* ---------- FUNCIONES DE LECTURA SEGURA ---------- */

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
        printf("Entrada inválida. Ingresa un número entero.\n");
    }
}

double leer_double(const char *msg) {
    char buf[128];
    double x;
    while (1) {
        if (msg) printf("%s", msg);
        leer_linea(buf, sizeof(buf));
        if (sscanf(buf, "%lf", &x) == 1) return x;
        printf("Entrada inválida. Ingresa un número decimal.\n");
    }
}

void leer_cadena(const char *msg, char *out, int len) {
    if (msg) printf("%s", msg);
    leer_linea(out, len);
}

/* ---------- GENERAR ID ---------- */

int generar_id() {
    static int next = 1;
    return next++;
}

/* ---------- PRODUCTOS ---------- */

void registrar_producto() {
    if (productos_count + 1 > MAX_PRODS) {
        printf("No se pueden agregar más productos.\n");
        return;
    }

    Producto p;
    p.id = generar_id();

    leer_cadena("Nombre: ", p.nombre, NAME_LEN);
    leer_cadena("Descripción: ", p.descripcion, DESC_LEN);

    do {
        p.precio = leer_double("Precio: ");
        if (p.precio <= 0) printf("El precio debe ser positivo.\n");
    } while (p.precio <= 0);

    do {
        p.stock = leer_entero("Stock inicial: ");
        if (p.stock < 0) printf("El stock no puede ser negativo.\n");
    } while (p.stock < 0);

    p.iva = IVA_POR_DEFECTO;
    p.activo = 1;

    productos[++productos_count] = p;

    printf("Producto agregado con ID %d\n", p.id);
}

Producto* buscar_producto_por_id(int id) {
    for (int i = 1; i <= productos_count; i++) {
        if (productos[i].id == id && productos[i].activo)
            return &productos[i];
    }
    return NULL;
}

void listar_y_ajustar_productos() {
    if (productos_count == 0) {
        printf("No hay productos registrados.\n");
        return;
    }

    printf("\n--- LISTA DE PRODUCTOS ---\n");
    printf("%-4s %-20s %-8s %-6s %-4s %s\n",
           "ID", "NOMBRE", "PRECIO", "STOCK", "IVA", "DESCRIPCION");

for (int i = 1; i <= productos_count; i++) {
    if (!productos[i].activo) continue;
    printf("%-4d %-20s $%6.2f %-6d %-4d %s",
           productos[i].id, productos[i].nombre, productos[i].precio,
           productos[i].stock, productos[i].iva, productos[i].descripcion);

    if (productos[i].stock <= STOCK_MINIMO) {
        printf("  <-- ALERTA: Stock bajo\n");
    } else {
        printf("\n");
    }
}


    printf("\n¿Deseas ajustar un producto? \n1=Sí  \n2=No\n");
int op;
do {
    op = leer_entero("Opción: ");
    if (op != 1 && op != 2) {
        printf("Opción inválida. Ingresa 1 o 2.\n");
    }
} while (op != 1 && op != 2);

if (op != 1) return;



    int id = leer_entero("ID del producto: ");
    Producto *p = buscar_producto_por_id(id);
    if (!p) {
        printf("Producto no encontrado.\n");
        return;
    }

    int sub;
    do {
        printf("\n--- Ajustes para %s (ID %d) ---\n", p->nombre, p->id);
        printf("1) Cambiar precio\n");
        printf("2) Cambiar IVA\n");
        printf("3) Cambiar stock\n");
        printf("4) Eliminar producto\n");
        printf("5) Volver\n");
        sub = leer_entero("Seleccione: ");

        if (sub == 1) {
            double nuevo;
            do {
                nuevo = leer_double("Nuevo precio: ");
                if (nuevo <= 0) printf("El precio debe ser positivo.\n");
            } while (nuevo <= 0);
            p->precio = nuevo;
        }
        else if (sub == 2) {
            int nuevo;
            do {
                nuevo = leer_entero("Nuevo IVA (%): ");
                if (nuevo < 0) printf("El IVA no puede ser negativo.\n");
            } while (nuevo < 0);
            p->iva = nuevo;
        }
        else if (sub == 3) {
            int nuevo;
            do {
                nuevo = leer_entero("Nuevo stock: ");
                if (nuevo < 0) printf("El stock no puede ser negativo.\n");
            } while (nuevo < 0);
            p->stock = nuevo;
        }
        else if (sub == 4) { p->activo = 0; printf("Producto eliminado.\n"); }
        else if (sub != 5) printf("Opción inválida.\n");

    } while (sub != 5);
}

/* ---------- CAJA ---------- */

void abrir_caja() {
    if (caja_abierta) {
        printf("La caja ya está abierta.\n");
        return;
    }
    caja_abierta = 1;
    caja_ingresos = 0;
    printf("Caja abierta.\n");
}

void cerrar_caja() {
    if (!caja_abierta) {
        printf("La caja no está abierta.\n");
        return;
    }
    printf("Caja cerrada. Ingresos: $%.2f\n", caja_ingresos);
    caja_abierta = 0;
    caja_ingresos = 0;
}

/* ---------- VENTAS ---------- */

void venta() {
    if (!caja_abierta) {
        printf("La caja está cerrada. Ábrela para vender.\n");
        return;
    }

    ItemCarrito cart[MAX_CART + 1];
    int ccount = 0;

    printf("\n--- VENTAS ---\n");

    while (1) {
        int id;
        do {
            id = leer_entero("ID del producto (0 para terminar): ");
            if (id < 0) printf("El ID no puede ser negativo.\n");
        } while (id < 0);

        if (id == 0) break;

        Producto *p = buscar_producto_por_id(id);
        if (!p) {
            printf("Producto no encontrado.\n");
            continue;
        }

        printf("Producto: %s | Precio: %.2f | Stock: %d | IVA: %d%%\n",
               p->nombre, p->precio, p->stock, p->iva);

        int cant;
        do {
            cant = leer_entero("Cantidad (0 cancela): ");
            if (cant < 0) printf("La cantidad no puede ser negativa.\n");
        } while (cant < 0);

        if (cant == 0) continue;

        while (cant > p->stock) {
            printf("Stock insuficiente. Disponible: %d\n", p->stock);
            do {
                cant = leer_entero("Nueva cantidad (0 cancela): ");
                if (cant < 0) printf("La cantidad no puede ser negativa.\n");
            } while (cant < 0);

            if (cant == 0) break;
        }
        if (cant == 0) continue;

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
                printf("Carrito lleno.\n");
                break;
            }
            cart[++ccount].prod_id = id;
            cart[ccount].cantidad = cant;
            cart[ccount].precio_unitario = p->precio;
            cart[ccount].iva = p->iva;
        }

        printf("Agregado.\n");
    }

    if (ccount == 0) {
        printf("Venta cancelada.\n");
        return;
    }

    double subtotal = 0, total_iva = 0, total = 0;

    printf("\n--- RESUMEN ---\n");
    for (int i = 1; i <= ccount; i++) {
        Producto *p = buscar_producto_por_id(cart[i].prod_id);
        double imp = cart[i].cantidad * cart[i].precio_unitario;
        double iva_val = imp * ((double)cart[i].iva / 100);

        printf("ID %d | %s | Cant %d | Subtotal %.2f | IVA %.2f | Total %.2f\n",
               p->id, p->nombre, cart[i].cantidad, imp, iva_val, imp + iva_val);

        subtotal += imp;
        total_iva += iva_val;
    }

    total = subtotal + total_iva;

    printf("Subtotal: %.2f\n", subtotal);
    printf("IVA: %.2f\n", total_iva);
    printf("TOTAL: %.2f\n", total);

    char buf[16];
    printf("Confirmar venta (s/n): ");
    leer_linea(buf, sizeof(buf));

if (tolower(buf[0]) == 's') {
    for (int i = 1; i <= ccount; i++) {
        Producto *p = buscar_producto_por_id(cart[i].prod_id);
        p->stock -= cart[i].cantidad;

        if (p->stock <= STOCK_MINIMO) {
            printf("ALERTA: El producto '%s' (ID %d) tiene stock bajo (%d unidades).\n",
                   p->nombre, p->id, p->stock);
        }
    }
    caja_ingresos += total;
    printf("Venta realizada.\n");
} else {
    printf("Venta cancelada.\n");
}

}

/* ---------- REPORTES ---------- */

void reporte_resumen() {
    printf("\n--- REPORTE ---\n");
    printf("Ingresos actuales: %.2f\n", caja_ingresos);
    printf("Productos registrados: %d\n", productos_count);
}

/* ---------- MENÚS ---------- */

void menu_productos() {
    int op;
    do {
        printf("\n--- MENU PRODUCTOS ---\n");
        printf("1) Registrar producto\n");
        printf("2) Listar y ajustar\n");
        printf("3) Volver\n");
        op = leer_entero("Opción: ");

        if (op == 1) registrar_producto();
        else if (op == 2) listar_y_ajustar_productos();
        else if (op != 3) printf("Opción inválida.\n");

    } while (op != 3);
}

void menu_principal() {
    int op;
    do {
        printf("\n=== STORE RAQFRAN - MENU PRINCIPAL ===\n");
        printf("1) Productos (catalogo)\n");
        printf("2) Ventas\n");
        printf("3) Caja (abrir/cerrar)\n");
        printf("4) Reportes\n");
        printf("5) Salir\n");

        op = leer_entero("Seleccione: ");

        switch (op) {
            case 1: menu_productos(); break;
            case 2: venta(); break;
            case 3: {
                printf("1) Abrir caja\n2) Cerrar caja\n");
                int s = leer_entero("Opción: ");
                if (s == 1) abrir_caja();
                else if (s == 2) cerrar_caja();
                else printf("Opción inválida.\n");
                break;
            }
            case 4: reporte_resumen(); break;
            case 5: printf("Saliendo...\n"); return;
            default: printf("Opción inválida.\n");
        }

    } while (1);
}

int main() {
    printf("STORE RAQFRAN - Sistema de Punto de Venta\n");
    menu_principal();
    return 0;
}