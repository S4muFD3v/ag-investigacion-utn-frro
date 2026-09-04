#include "r7_matrix.h"

#include <stdint.h>
#include <string.h>

#define SUBJECT_BIT(id) (UINT64_C(1) << ((id) - 1))
#define NO_PREREQUISITES UINT64_C(0)

#define SHARED_PREREQUISITE_BONUS_MAX 12
#define ONE_OPTIONAL_FACTOR_PERCENT 80
#define TWO_OPTIONAL_FACTOR_PERCENT 60
#define ONE_OPTIONAL_SAME_YEAR_BONUS 10
#define TWO_OPTIONAL_SAME_YEAR_BONUS 5

typedef struct {
    r7_subject_info_t info;
    uint64_t regularPrerequisites;
    uint64_t approvedPrerequisites;
} r7_subject_record_t;

#define SUBJECT(id, code, name, year, kind, regular, approved) \
    {{id, code, name, year, kind}, regular, approved}

static const r7_subject_record_t SUBJECTS[] = {
    SUBJECT(1, "1", "Analisis Matematico I", 1, R7_SUBJECT_REQUIRED,
            NO_PREREQUISITES, NO_PREREQUISITES),
    SUBJECT(2, "2", "Algebra y Geometria Analitica", 1, R7_SUBJECT_REQUIRED,
            NO_PREREQUISITES, NO_PREREQUISITES),
    SUBJECT(3, "3", "Fisica I", 1, R7_SUBJECT_REQUIRED,
            NO_PREREQUISITES, NO_PREREQUISITES),
    SUBJECT(4, "4", "Ingles I", 1, R7_SUBJECT_REQUIRED,
            NO_PREREQUISITES, NO_PREREQUISITES),
    SUBJECT(5, "5", "Logica y Estructuras Discretas", 1, R7_SUBJECT_REQUIRED,
            NO_PREREQUISITES, NO_PREREQUISITES),
    SUBJECT(6, "6", "Algoritmos y Estructuras de Datos", 1, R7_SUBJECT_REQUIRED,
            NO_PREREQUISITES, NO_PREREQUISITES),
    SUBJECT(7, "7", "Arquitectura de Computadoras", 1, R7_SUBJECT_REQUIRED,
            NO_PREREQUISITES, NO_PREREQUISITES),
    SUBJECT(8, "8", "Sistemas y Procesos de Negocio", 1, R7_SUBJECT_REQUIRED,
            NO_PREREQUISITES, NO_PREREQUISITES),

    SUBJECT(9, "9", "Analisis Matematico II", 2, R7_SUBJECT_REQUIRED,
            SUBJECT_BIT(1) | SUBJECT_BIT(2), NO_PREREQUISITES),
    SUBJECT(10, "10", "Fisica II", 2, R7_SUBJECT_REQUIRED,
            SUBJECT_BIT(1) | SUBJECT_BIT(3), NO_PREREQUISITES),
    SUBJECT(11, "11", "Ingenieria y Sociedad", 2, R7_SUBJECT_REQUIRED,
            NO_PREREQUISITES, NO_PREREQUISITES),
    SUBJECT(12, "12", "Ingles II", 2, R7_SUBJECT_REQUIRED,
            SUBJECT_BIT(4), NO_PREREQUISITES),
    SUBJECT(13, "13", "Sintaxis y Semantica de los Lenguajes", 2,
            R7_SUBJECT_REQUIRED, SUBJECT_BIT(5) | SUBJECT_BIT(6),
            NO_PREREQUISITES),
    SUBJECT(14, "14", "Paradigmas de Programacion", 2, R7_SUBJECT_REQUIRED,
            SUBJECT_BIT(5) | SUBJECT_BIT(6), NO_PREREQUISITES),
    SUBJECT(15, "15", "Sistemas Operativos", 2, R7_SUBJECT_REQUIRED,
            SUBJECT_BIT(7), NO_PREREQUISITES),
    SUBJECT(16, "16", "Analisis de Sistemas de Informacion", 2,
            R7_SUBJECT_REQUIRED, SUBJECT_BIT(6) | SUBJECT_BIT(8),
            NO_PREREQUISITES),

    SUBJECT(17, "17", "Probabilidad y Estadistica", 3, R7_SUBJECT_REQUIRED,
            SUBJECT_BIT(1) | SUBJECT_BIT(2), NO_PREREQUISITES),
    SUBJECT(18, "18", "Economia", 3, R7_SUBJECT_REQUIRED,
            NO_PREREQUISITES, SUBJECT_BIT(1) | SUBJECT_BIT(2)),
    SUBJECT(19, "19", "Base de Datos", 3, R7_SUBJECT_REQUIRED,
            SUBJECT_BIT(13) | SUBJECT_BIT(16),
            SUBJECT_BIT(5) | SUBJECT_BIT(6)),
    SUBJECT(20, "20", "Desarrollo de Software", 3, R7_SUBJECT_REQUIRED,
            SUBJECT_BIT(14) | SUBJECT_BIT(16),
            SUBJECT_BIT(5) | SUBJECT_BIT(6)),
    SUBJECT(21, "21", "Comunicacion de Datos", 3, R7_SUBJECT_REQUIRED,
            NO_PREREQUISITES, SUBJECT_BIT(3) | SUBJECT_BIT(7)),
    SUBJECT(22, "22", "Analisis Numerico", 3, R7_SUBJECT_REQUIRED,
            SUBJECT_BIT(9), SUBJECT_BIT(1) | SUBJECT_BIT(2)),
    SUBJECT(23, "23", "Diseno de Sistemas de Informacion", 3,
            R7_SUBJECT_REQUIRED, SUBJECT_BIT(14) | SUBJECT_BIT(16),
            SUBJECT_BIT(4) | SUBJECT_BIT(6) | SUBJECT_BIT(8)),

    SUBJECT(24, "24", "Legislacion", 4, R7_SUBJECT_REQUIRED,
            SUBJECT_BIT(11), NO_PREREQUISITES),
    SUBJECT(25, "25", "Ingenieria y Calidad de Software", 4,
            R7_SUBJECT_REQUIRED,
            SUBJECT_BIT(19) | SUBJECT_BIT(20) | SUBJECT_BIT(23),
            SUBJECT_BIT(13) | SUBJECT_BIT(14)),
    SUBJECT(26, "26", "Redes de Datos", 4, R7_SUBJECT_REQUIRED,
            SUBJECT_BIT(15) | SUBJECT_BIT(21), NO_PREREQUISITES),
    SUBJECT(27, "27", "Investigacion Operativa", 4, R7_SUBJECT_REQUIRED,
            SUBJECT_BIT(17) | SUBJECT_BIT(22), NO_PREREQUISITES),
    SUBJECT(28, "28", "Simulacion", 4, R7_SUBJECT_REQUIRED,
            SUBJECT_BIT(17), SUBJECT_BIT(9)),
    SUBJECT(29, "29", "Tecnologias para la Automatizacion", 4,
            R7_SUBJECT_REQUIRED, SUBJECT_BIT(10) | SUBJECT_BIT(22),
            SUBJECT_BIT(9)),
    SUBJECT(30, "30", "Administracion de Sistemas de Informacion", 4,
            R7_SUBJECT_REQUIRED, SUBJECT_BIT(18) | SUBJECT_BIT(23),
            SUBJECT_BIT(16)),

    SUBJECT(31, "31", "Inteligencia Artificial", 5, R7_SUBJECT_REQUIRED,
            SUBJECT_BIT(28), SUBJECT_BIT(17) | SUBJECT_BIT(22)),
    SUBJECT(32, "32", "Ciencia de Datos", 5, R7_SUBJECT_REQUIRED,
            SUBJECT_BIT(28), SUBJECT_BIT(17) | SUBJECT_BIT(19)),
    SUBJECT(33, "33", "Sistemas de Gestion", 5, R7_SUBJECT_REQUIRED,
            SUBJECT_BIT(18) | SUBJECT_BIT(27), SUBJECT_BIT(23)),
    SUBJECT(34, "34", "Gestion Gerencial", 5, R7_SUBJECT_REQUIRED,
            SUBJECT_BIT(24) | SUBJECT_BIT(30), SUBJECT_BIT(18)),
    SUBJECT(35, "35", "Seguridad en los Sistemas de Informacion", 5,
            R7_SUBJECT_REQUIRED, SUBJECT_BIT(26) | SUBJECT_BIT(30),
            SUBJECT_BIT(20) | SUBJECT_BIT(21)),
    SUBJECT(36, "36", "Proyecto Final", 5, R7_SUBJECT_REQUIRED,
            SUBJECT_BIT(25) | SUBJECT_BIT(26) | SUBJECT_BIT(30),
            SUBJECT_BIT(12) | SUBJECT_BIT(20) | SUBJECT_BIT(23)),

    SUBJECT(37, "ADUSI", "Seminario Integrador ADUSI", 3,
            R7_SUBJECT_SEMINAR, SUBJECT_BIT(16),
            SUBJECT_BIT(6) | SUBJECT_BIT(8) | SUBJECT_BIT(13) |
                SUBJECT_BIT(14)),

    SUBJECT(38, "E01", "Entornos Graficos", 2, R7_SUBJECT_ELECTIVE,
            SUBJECT_BIT(5), SUBJECT_BIT(6) | SUBJECT_BIT(8)),
    SUBJECT(39, "E02", "Analisis y Diseno de Datos e Informacion", 2,
            R7_SUBJECT_ELECTIVE, SUBJECT_BIT(8) | SUBJECT_BIT(13),
            SUBJECT_BIT(6)),
    SUBJECT(40, "E03", "Sistemas de Informacion Geografica", 2,
            R7_SUBJECT_ELECTIVE, SUBJECT_BIT(1) | SUBJECT_BIT(6),
            SUBJECT_BIT(2)),
    SUBJECT(41, "E04", "Formacion de Emprendedores", 2,
            R7_SUBJECT_ELECTIVE, NO_PREREQUISITES, SUBJECT_BIT(2)),
    SUBJECT(42, "E05", "Algoritmos Geneticos", 3, R7_SUBJECT_ELECTIVE,
            SUBJECT_BIT(13) | SUBJECT_BIT(14),
            SUBJECT_BIT(5) | SUBJECT_BIT(6) | SUBJECT_BIT(7) |
                SUBJECT_BIT(8)),
    SUBJECT(43, "E06", "Informatica Juridica", 3, R7_SUBJECT_ELECTIVE,
            SUBJECT_BIT(15),
            SUBJECT_BIT(6) | SUBJECT_BIT(7) | SUBJECT_BIT(8)),
    SUBJECT(44, "E07", "Lenguaje de Programacion JAVA", 3,
            R7_SUBJECT_ELECTIVE, NO_PREREQUISITES, SUBJECT_BIT(14)),
    SUBJECT(45, "E08", "Tecnologias de Desarrollo de Software IDE", 3,
            R7_SUBJECT_ELECTIVE, NO_PREREQUISITES,
            SUBJECT_BIT(5) | SUBJECT_BIT(13) | SUBJECT_BIT(14)),
    SUBJECT(46, "E09", "Gestion Ingenieril", 3, R7_SUBJECT_ELECTIVE,
            NO_PREREQUISITES, SUBJECT_BIT(8)),
    SUBJECT(47, "E10", "Introduccion a la Practica Profesional", 3,
            R7_SUBJECT_ELECTIVE, NO_PREREQUISITES, SUBJECT_BIT(16)),
    SUBJECT(48, "E11", "Quimica Aplicada a la Informatica", 3,
            R7_SUBJECT_ELECTIVE,
            SUBJECT_BIT(4) | SUBJECT_BIT(5) | SUBJECT_BIT(6) |
                SUBJECT_BIT(7) | SUBJECT_BIT(8),
            SUBJECT_BIT(1) | SUBJECT_BIT(2) | SUBJECT_BIT(3)),
    SUBJECT(49, "E12", "Infraestructura Tecnologica", 4,
            R7_SUBJECT_ELECTIVE, SUBJECT_BIT(16), SUBJECT_BIT(15)),
    SUBJECT(50, "E13", "Soporte a las Bases de Datos con Programacion Visual", 4,
            R7_SUBJECT_ELECTIVE, SUBJECT_BIT(19),
            SUBJECT_BIT(13) | SUBJECT_BIT(14)),
    SUBJECT(51, "E14", "Metodologia de la Investigacion", 4,
            R7_SUBJECT_ELECTIVE, SUBJECT_BIT(17), SUBJECT_BIT(17)),
    SUBJECT(52, "E15", "Metodologias Agiles en el Desarrollo de Software", 4,
            R7_SUBJECT_ELECTIVE, SUBJECT_BIT(25),
            SUBJECT_BIT(14) | SUBJECT_BIT(16)),
    SUBJECT(53, "E16", "Fabricacion Aditiva", 5, R7_SUBJECT_ELECTIVE,
            SUBJECT_BIT(28) | SUBJECT_BIT(29) | SUBJECT_BIT(30),
            SUBJECT_BIT(7) | SUBJECT_BIT(15) | SUBJECT_BIT(18)),
    SUBJECT(54, "E17", "Direccion de Recursos Humanos", 5,
            R7_SUBJECT_ELECTIVE, SUBJECT_BIT(30), NO_PREREQUISITES),
    SUBJECT(55, "E18", "Informatica en la Administracion Publica", 5,
            R7_SUBJECT_ELECTIVE, SUBJECT_BIT(16), NO_PREREQUISITES),
    SUBJECT(56, "E19", "Sistemas de Informacion Integrados para la Industria", 5,
            R7_SUBJECT_ELECTIVE,
            SUBJECT_BIT(25) | SUBJECT_BIT(27) | SUBJECT_BIT(30),
            NO_PREREQUISITES),
    SUBJECT(57, "E20", "Programacion Competitiva", 2,
            R7_SUBJECT_ELECTIVE, NO_PREREQUISITES, NO_PREREQUISITES)
};

_Static_assert(sizeof(SUBJECTS) / sizeof(SUBJECTS[0]) == R7_SUBJECT_COUNT,
               "R7_SUBJECT_COUNT must match the subject catalog");

static uint64_t prerequisiteClosure[R7_SUBJECT_COUNT];
static unsigned char conflictMatrix[R7_SUBJECT_COUNT][R7_SUBJECT_COUNT];
static int matrixInitialized;

static int subject_index(id_t subjectId) {
    if (subjectId < 1 || subjectId > R7_SUBJECT_COUNT) {
        return -1;
    }
    return (int)(subjectId - 1);
}

static size_t bit_count(uint64_t value) {
    size_t count = 0;
    while (value != 0) {
        value &= value - 1;
        count++;
    }
    return count;
}

static int base_year_score(size_t firstYearValue, size_t secondYearValue) {
    int firstYear = (int)firstYearValue;
    int secondYear = (int)secondYearValue;
    int distance = firstYear > secondYear
        ? firstYear - secondYear
        : secondYear - firstYear;

    if (distance == 0) {
        return 50 + 10 * firstYear;
    }

    int lowerYear = firstYear < secondYear ? firstYear : secondYear;
    int score = 65 + 5 * (lowerYear - 1) - 15 * (distance - 1);
    return score < 10 ? 10 : score;
}

static int is_optional(r7_subject_kind_t kind) {
    return kind != R7_SUBJECT_REQUIRED;
}

static int calculate_conflict_score(size_t firstIndex, size_t secondIndex) {
    if (firstIndex == secondIndex) {
        return 0;
    }

    uint64_t firstBit = UINT64_C(1) << firstIndex;
    uint64_t secondBit = UINT64_C(1) << secondIndex;

    if ((prerequisiteClosure[firstIndex] & secondBit) != 0 ||
        (prerequisiteClosure[secondIndex] & firstBit) != 0) {
        return 0;
    }

    const r7_subject_info_t* first = &SUBJECTS[firstIndex].info;
    const r7_subject_info_t* second = &SUBJECTS[secondIndex].info;
    int score = base_year_score(first->year, second->year);
    int optionalCount = is_optional(first->kind) + is_optional(second->kind);

    if (optionalCount == 1) {
        score = (score * ONE_OPTIONAL_FACTOR_PERCENT + 50) / 100;
        if (first->year == second->year) {
            score += ONE_OPTIONAL_SAME_YEAR_BONUS;
        }
    } else if (optionalCount == 2) {
        score = (score * TWO_OPTIONAL_FACTOR_PERCENT + 50) / 100;
        if (first->year == second->year) {
            score += TWO_OPTIONAL_SAME_YEAR_BONUS;
        }
    }

    uint64_t shared = prerequisiteClosure[firstIndex] &
                      prerequisiteClosure[secondIndex];
    uint64_t either = prerequisiteClosure[firstIndex] |
                      prerequisiteClosure[secondIndex];
    size_t eitherCount = bit_count(either);

    if (eitherCount > 0) {
        size_t sharedCount = bit_count(shared);
        score += (int)((SHARED_PREREQUISITE_BONUS_MAX * sharedCount +
                       eitherCount / 2) / eitherCount);
    }

    return score > R7_CONFLICT_MAX ? R7_CONFLICT_MAX : score;
}

static void initialize_matrix(void) {
    if (matrixInitialized) {
        return;
    }

    for (size_t i = 0; i < R7_SUBJECT_COUNT; i++) {
        prerequisiteClosure[i] = SUBJECTS[i].regularPrerequisites |
                                 SUBJECTS[i].approvedPrerequisites;
    }

    for (size_t prerequisite = 0;
         prerequisite < R7_SUBJECT_COUNT;
         prerequisite++) {
        uint64_t prerequisiteBit = UINT64_C(1) << prerequisite;
        for (size_t subject = 0; subject < R7_SUBJECT_COUNT; subject++) {
            if ((prerequisiteClosure[subject] & prerequisiteBit) != 0) {
                prerequisiteClosure[subject] |=
                    prerequisiteClosure[prerequisite];
            }
        }
    }

    for (size_t first = 0; first < R7_SUBJECT_COUNT; first++) {
        conflictMatrix[first][first] = 0;
        for (size_t second = first + 1;
             second < R7_SUBJECT_COUNT;
             second++) {
            int score = calculate_conflict_score(first, second);
            conflictMatrix[first][second] = (unsigned char)score;
            conflictMatrix[second][first] = (unsigned char)score;
        }
    }

    matrixInitialized = 1;
}

size_t r7_subject_count(void) {
    return R7_SUBJECT_COUNT;
}

const r7_subject_info_t* r7_subject_at(size_t index) {
    if (index >= R7_SUBJECT_COUNT) {
        return NULL;
    }
    return &SUBJECTS[index].info;
}

const r7_subject_info_t* r7_subject_by_id(id_t subjectId) {
    int index = subject_index(subjectId);
    return index < 0 ? NULL : &SUBJECTS[index].info;
}

const r7_subject_info_t* r7_subject_by_plan_code(const char* planCode) {
    if (planCode == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < R7_SUBJECT_COUNT; i++) {
        if (strcmp(SUBJECTS[i].info.planCode, planCode) == 0) {
            return &SUBJECTS[i].info;
        }
    }
    return NULL;
}

const char* r7_subject_kind_name(r7_subject_kind_t kind) {
    switch (kind) {
        case R7_SUBJECT_REQUIRED:
            return "required";
        case R7_SUBJECT_SEMINAR:
            return "seminar";
        case R7_SUBJECT_ELECTIVE:
            return "elective";
    }
    return "unknown";
}

int r7_is_direct_regular_prerequisite(id_t subjectId, id_t prerequisiteId) {
    int subjectIndex = subject_index(subjectId);
    int prerequisiteIndex = subject_index(prerequisiteId);
    if (subjectIndex < 0 || prerequisiteIndex < 0) {
        return 0;
    }

    uint64_t prerequisiteBit = UINT64_C(1) << prerequisiteIndex;
    return (SUBJECTS[subjectIndex].regularPrerequisites & prerequisiteBit) != 0;
}

int r7_is_direct_approved_prerequisite(id_t subjectId, id_t prerequisiteId) {
    int subjectIndex = subject_index(subjectId);
    int prerequisiteIndex = subject_index(prerequisiteId);
    if (subjectIndex < 0 || prerequisiteIndex < 0) {
        return 0;
    }

    uint64_t prerequisiteBit = UINT64_C(1) << prerequisiteIndex;
    return (SUBJECTS[subjectIndex].approvedPrerequisites & prerequisiteBit) != 0;
}

int r7_requires_subject(id_t subjectId, id_t prerequisiteId) {
    int subjectIndex = subject_index(subjectId);
    int prerequisiteIndex = subject_index(prerequisiteId);
    if (subjectIndex < 0 || prerequisiteIndex < 0) {
        return 0;
    }

    initialize_matrix();
    uint64_t prerequisiteBit = UINT64_C(1) << prerequisiteIndex;
    return (prerequisiteClosure[subjectIndex] & prerequisiteBit) != 0;
}

int r7_conflict_score_by_index(size_t firstIndex, size_t secondIndex) {
    if (firstIndex >= R7_SUBJECT_COUNT || secondIndex >= R7_SUBJECT_COUNT) {
        return R7_CONFLICT_UNKNOWN;
    }

    initialize_matrix();
    return conflictMatrix[firstIndex][secondIndex];
}

int r7_conflict_score(id_t firstSubjectId, id_t secondSubjectId) {
    int firstIndex = subject_index(firstSubjectId);
    int secondIndex = subject_index(secondSubjectId);
    if (firstIndex < 0 || secondIndex < 0) {
        return R7_CONFLICT_UNKNOWN;
    }

    return r7_conflict_score_by_index((size_t)firstIndex,
                                      (size_t)secondIndex);
}

double r7_conflict_weight(id_t firstSubjectId, id_t secondSubjectId) {
    int score = r7_conflict_score(firstSubjectId, secondSubjectId);
    return score < 0 ? -1.0 : (double)score / R7_CONFLICT_MAX;
}
