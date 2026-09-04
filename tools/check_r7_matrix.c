#include "utils/r7_matrix.h"

#include <stdio.h>
#include <string.h>

static int report_failure(const char* message, id_t firstId, id_t secondId) {
    fprintf(stderr, "R7 check failed: %s (%lld, %lld)\n",
            message, (long long)firstId, (long long)secondId);
    return 0;
}

int main(void) {
    int valid = 1;
    size_t zeroPairs = 0;
    size_t weightedPairs = 0;
    long long scoreSum = 0;
    int minimumWeightedScore = R7_CONFLICT_MAX;
    int maximumScore = 0;

    if (r7_subject_count() != R7_SUBJECT_COUNT) {
        fprintf(stderr, "R7 check failed: expected %d subjects, found %zu\n",
                R7_SUBJECT_COUNT, r7_subject_count());
        return 1;
    }

    for (size_t first = 0; first < r7_subject_count(); first++) {
        const r7_subject_info_t* firstSubject = r7_subject_at(first);
        id_t expectedId = (id_t)(first + 1);

        if (firstSubject == NULL || firstSubject->id != expectedId) {
            valid &= report_failure("catalog ID is not consecutive",
                                    expectedId, 0);
            continue;
        }
        if (firstSubject->year < 1 || firstSubject->year > 5) {
            valid &= report_failure("year is outside 1..5",
                                    firstSubject->id, 0);
        }
        if (r7_subject_by_id(firstSubject->id) != firstSubject ||
            r7_subject_by_plan_code(firstSubject->planCode) != firstSubject) {
            valid &= report_failure("catalog lookup is inconsistent",
                                    firstSubject->id, 0);
        }
        if (r7_requires_subject(firstSubject->id, firstSubject->id)) {
            valid &= report_failure("prerequisite graph contains a cycle",
                                    firstSubject->id, firstSubject->id);
        }

        for (size_t duplicate = first + 1;
             duplicate < r7_subject_count();
             duplicate++) {
            const r7_subject_info_t* other = r7_subject_at(duplicate);
            if (strcmp(firstSubject->planCode, other->planCode) == 0) {
                valid &= report_failure("duplicated plan code",
                                        firstSubject->id, other->id);
            }
        }

        for (size_t second = 0; second < r7_subject_count(); second++) {
            const r7_subject_info_t* secondSubject = r7_subject_at(second);
            int score = r7_conflict_score_by_index(first, second);
            int symmetricScore = r7_conflict_score_by_index(second, first);

            if (score < 0 || score > R7_CONFLICT_MAX) {
                valid &= report_failure("score is outside 0..100",
                                        firstSubject->id, secondSubject->id);
            }
            if (score != symmetricScore) {
                valid &= report_failure("matrix is not symmetric",
                                        firstSubject->id, secondSubject->id);
            }
            if (first == second && score != 0) {
                valid &= report_failure("diagonal is not zero",
                                        firstSubject->id, secondSubject->id);
            }

            int direct = r7_is_direct_regular_prerequisite(
                             firstSubject->id, secondSubject->id) ||
                         r7_is_direct_approved_prerequisite(
                             firstSubject->id, secondSubject->id);
            if (direct && !r7_requires_subject(firstSubject->id,
                                               secondSubject->id)) {
                valid &= report_failure("direct prerequisite missing from closure",
                                        firstSubject->id, secondSubject->id);
            }
            if ((r7_requires_subject(firstSubject->id, secondSubject->id) ||
                 r7_requires_subject(secondSubject->id, firstSubject->id)) &&
                score != 0) {
                valid &= report_failure("prerequisite pair has a non-zero score",
                                        firstSubject->id, secondSubject->id);
            }

            if (second <= first) {
                continue;
            }
            if (score == 0) {
                zeroPairs++;
            } else {
                weightedPairs++;
                scoreSum += score;
                if (score < minimumWeightedScore) {
                    minimumWeightedScore = score;
                }
                if (score > maximumScore) {
                    maximumScore = score;
                }
            }
        }
    }

    if (r7_conflict_score(0, 1) != R7_CONFLICT_UNKNOWN ||
        r7_conflict_score(1, R7_SUBJECT_COUNT + 1) != R7_CONFLICT_UNKNOWN) {
        valid &= report_failure("unknown IDs are not rejected", 0,
                                R7_SUBJECT_COUNT + 1);
    }

    if (!valid) {
        return 1;
    }

    double average = weightedPairs == 0
        ? 0.0
        : (double)scoreSum / (double)weightedPairs;
    printf("R7 matrix OK: %d subjects, %zu zero pairs, %zu weighted pairs, "
           "weighted range %d..%d, mean %.2f\n",
           R7_SUBJECT_COUNT, zeroPairs, weightedPairs,
           minimumWeightedScore, maximumScore, average);
    return 0;
}
