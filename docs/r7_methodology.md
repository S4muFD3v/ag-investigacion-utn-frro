# Matriz de conflicto R7

Este modulo convierte el plan de correlatividades en una matriz simetrica
`K[i][j]` de 57 por 57. Un valor alto indica que conviene evitar con mayor
fuerza la superposicion de las materias. No es una probabilidad ni una
restriccion fuerte: es el peso de conflicto usado por R7.

## Catalogo canonico

- IDs `1..36`: materias troncales, conservando la numeracion del plan.
- ID `37`: Seminario Integrador ADUSI.
- IDs `38..57`: electivas `E01..E20`, en el orden del documento fuente.

El archivo `r7_subject_catalog.csv` deja visible este mapeo, el anio, el tipo y
las correlatividades directas requeridas para cursar y aprobar. Los nombres se
normalizaron a ASCII para mantener el criterio actual del proyecto.

Los IDs que cargue la base deben coincidir con este catalogo. Si el importador
del Excel usa otra numeracion, hay que traducir sus IDs a estos IDs canonicos o
ajustar el catalogo antes de calcular R7.

Los datos se transcribieron de las dos grillas del Plan 2023 entregadas junto
con la tarea. En la fuente, la electiva E14 (Metodologia de la Investigacion)
indica la materia 17 tanto como regular como aprobada; el catalogo conserva
literalmente ese dato aunque resulte redundante.

## Construccion de Kij

Primero se obtiene el cierre transitivo de todas las correlatividades, tanto de
regularizacion como de aprobacion. Para cada par de materias diferentes:

1. Si una materia es correlativa directa o indirecta de la otra, `Kij = 0`,
   porque el plan impide cursarlas simultaneamente.
2. Si son del mismo anio `y`, el valor base es `50 + 10*y`. Esto produce
   `60, 70, 80, 90, 100` y refleja la mayor mezcla entre comisiones esperada en
   los anios superiores.
3. Si son de anios distintos, el valor base es
   `max(10, 65 + 5*(min(y_i,y_j)-1) - 15*(|y_i-y_j|-1))`. La penalizacion baja
   al aumentar la distancia y conserva algo mas de peso entre anios superiores.
4. Una electiva o seminario reduce el valor al 80%; si ambos lo son, al 60%.
   En su anio natural se agregan 10 o 5 puntos respectivamente. Esta reduccion
   evita duplicar en R7 todo el efecto especifico que luego mide R8.
5. Se agregan hasta 12 puntos segun la proporcion de correlatividades
   transitivas compartidas: `12 * |P_i interseccion P_j| / |P_i union P_j|`.
   Es una aproximacion a la cercania entre trayectorias academicas.
6. El resultado se redondea y limita al intervalo `0..100`.

Estas reglas son una linea de base explicita y reproducible. No pretenden
reemplazar datos reales de inscripcion: cuando existan historiales anonimizados,
la matriz puede calibrarse con la frecuencia observada de cursado simultaneo sin
cambiar la interfaz usada por el algoritmo genetico.

## Uso desde R7

```c
#include "utils/r7_matrix.h"

int score = r7_conflict_score(firstSubjectId, secondSubjectId);
double weight = r7_conflict_weight(firstSubjectId, secondSubjectId);
```

`score` esta en `0..100`; `weight` contiene el mismo valor normalizado a
`0.0..1.0`. Ambos devuelven un valor negativo si alguno de los IDs no pertenece
al catalogo. La penalizacion de R7 puede acumular `weight * bloquesSolapados`
para cada par `i < j` perteneciente a comisiones diferentes.

La duracion anual o cuatrimestral no aparece todavia en `subject_t`. Por eso la
matriz supone que el evaluador compara solo dictados que realmente coexisten en
el mismo periodo academico. La ubicacion global de las electivas respecto de
varias comisiones sigue siendo responsabilidad de R8.

## Archivos generados

- `r7_subject_catalog.csv`: catalogo y correlatividades auditables.
- `r7_conflict_matrix.csv`: matriz numerica completa para analisis o planillas.
- `r7_conflict_heatmap.png`: visualizacion con la referencia de IDs al costado.

Para regenerarlos desde la raiz del proyecto:

```bash
cc -std=c17 -Isrc src/utils/r7_matrix.c tools/export_r7_matrix.c \
  -o /tmp/export_r7_matrix
/tmp/export_r7_matrix \
  docs/r7_subject_catalog.csv docs/r7_conflict_matrix.csv
python3 tools/render_r7_heatmap.py \
  docs/r7_subject_catalog.csv docs/r7_conflict_matrix.csv \
  docs/r7_conflict_heatmap.png
```

La imagen requiere Pillow. La matriz y el catalogo no tienen dependencias
adicionales: los exporta directamente el mismo modulo C que usa el algoritmo.
