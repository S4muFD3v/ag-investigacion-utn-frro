#include "chromosomeLoader.h"

#include <stdint.h>
#include <string.h>
#include "utils/memory.h"
#include "load/schedule_loader.h"
#include "db.h"
#include "utils/compare.h"
#include <stdlib.h>
#include <stdio.h>

dynarr_t *enumerate_files();
int64_t subject_exists_similar(const char *subject, float match);
int64_t dictation_exists(id_t subject, id_t comission, id_t teacher);

schedule_t get_comission_schedule_time(const schedule_data_t* schedule);
static void load_period(schedule_data_t *schedule, period_t period,
	comission_t *comission, dynarr_t *geneList);
static com_subjects_t *register_subject_period(comission_t *comission,
	id_t subjectId, period_t period);
static int same_period_schedule(const schedule_data_t *schedule, id_t subjectId);

chromosome_t *load_chromsome_from_files(const char *dataReadSubfolder)
{
	const size_t fileNameSizeBytes = 256;
	dynarr_t *files = enumerate_files();

	char *fileFullName = calloc(fileNameSizeBytes, sizeof(char));

	dynarr_t *geneList = init_dynamic_array(sizeof_gene(), 32);
	for (size_t a = 0; a < dynamic_array_size(files); a++)
	{
		snprintf(fileFullName, fileNameSizeBytes, "%s%s",
			dataReadSubfolder, (char *)at(files, a));
		schedule_file_t sfile = open_file(fileFullName);
		if (sfile == NULL)
		{
			printf("No se pudo abrir el archivo '%s'\n", fileFullName);
			continue;
		}

		// Get comissions
		size_t comCount = 0;
		com_id_t *comissions = NULL;
		get_comissions(sfile, &comCount, &comissions);

		// Parsing comissions
		for (size_t c = 0; c < comCount; c++)
		{
			size_t comYear = a + 1;
			schedule_data_t *schedule = get_schedule_for_comission(
				sfile, (com_id_t *)(((char *)comissions) + c * sizeof_com_id()));
			if (schedule == NULL) {
				continue;
			}

			comission_t *com = create_comission();
			set_comission_id(com, (id_t)(get_comission_count() - 1));
			set_comission_year(com, comYear);
			set_comission_schedule(com, get_comission_schedule_time(schedule));

			// Parsing Blocks
			load_period(schedule, PERIOD_FIRST, com, geneList);
			load_period(schedule, PERIOD_SECOND, com, geneList);

			for (size_t s = 0; s < get_comission_length(com); s++) {
				com_subjects_t *comSubject = get_comission_subject_at(com, s);
				id_t subjectId = get_com_subject_id(comSubject);
				if (get_com_subject_q(comSubject) == FMP_BOTH &&
					!same_period_schedule(schedule, subjectId)) {
					fprintf(stderr, "Aviso: '%s' de la comision %lld tiene horarios "
						"distintos entre cuatrimestres. Se usa el primero como "
						"base del horario comun (FMP_BOTH).\n",
						get_subject_name(query_subject(subjectId)), get_comission_id(com));
				}
			}

			delete_schedule(schedule);
		}
		free(comissions);
		close_file(sfile);
	}

	free(fileFullName);
	free_dynamic_array(files);
	size_t geneCount = dynamic_array_size(geneList);
	if (geneCount == 0) {
		free_dynamic_array(geneList);
		return NULL;
	}
	size_t geneSizeBytes = geneCount * sizeof_gene();
	chromosome_t *ch = init_chromosome(geneCount);
	dynamic_array_copy_to(get_gene_at(ch, 0), geneSizeBytes, geneList);
	free_dynamic_array(geneList);
	return ch;
}


static void load_period(schedule_data_t *schedule, period_t period,
	comission_t *comission, dynarr_t *geneList)
{
	for (size_t d = 0; d < get_schedule_day_count(); d++)
	{
		size_t blockCount = 0;
		subject_t *lastSubject = NULL;
		size_t sessionBlockStart = 0;
		gene_t *session = NULL;
		id_t sessionSubjectId = 0;
		id_t dicId = -1;
		for (size_t b = 0; b < get_schedule_block_count(); b++)
		{
			const char *name = get_subjet_name_for_block(schedule, d, b, period);
			int64_t subjectIndx;
			if (strlen(name) == 0) {
				lastSubject = NULL;
				continue;
			}
			if ((subjectIndx = subject_exists_similar(name, 0.90)) < 0)
			{
				printf("No existe la materia '%s'\n", name);
				abort();
			}
			sessionSubjectId = get_subject_id(get_subject_at(subjectIndx));

			subject_t *subject = get_subject_at(subjectIndx);
			com_subjects_t *comSubject = register_subject_period(
				comission, sessionSubjectId, period);
			// Un dictado FMP_BOTH tiene un unico horario semanal en este modelo.
			if (period == PERIOD_SECOND && get_com_subject_q(comSubject) == FMP_BOTH) {
				lastSubject = NULL;
				continue;
			}
			if (lastSubject == NULL || get_subject_id(lastSubject) != get_subject_id(subject))
			{
				// Nueva sesion
				blockCount = 1;
				lastSubject = subject;
				sessionBlockStart = b;
				session = push_slot(geneList);

				id_t comissionId = get_comission_id(comission);
				int64_t dictPos = dictation_exists(sessionSubjectId, comissionId, 0);
				if (dictPos < 0)
				{
					dictation_t *dic = create_dictation();
					set_dictation_id(dic, (id_t)(get_dictation_count() - 1));
					set_dictation_subject_id(dic, sessionSubjectId);
					set_dictation_comission_id(dic, comissionId);
					set_dictation_teacher_id(dic, 0);
					dicId = get_dictation_id(dic);
				}
				else
				{
					dicId = get_dictation_id(get_dictation_at(dictPos));
				}

				set_gene_start_block_id(session, sessionBlockStart);
				set_gene_day(session, d);
				set_gene_length(session, blockCount);
				set_gene_dictation_id(session, dicId);
			}
			else
			{
				// Misma sesion, incremento de horas
				blockCount++;
				set_gene_length(session, blockCount);
			}
		}
	}
}


static com_subjects_t *register_subject_period(comission_t *comission,
	id_t subjectId, period_t period) {
	fmp_t fmp = period == PERIOD_FIRST ? FMP_FIRST : FMP_SECOND;
	size_t count = get_comission_length(comission);
	for (size_t i = 0; i < count; i++) {
		com_subjects_t *comSubject = get_comission_subject_at(comission, i);
		if (get_com_subject_id(comSubject) == subjectId) {
			if (get_com_subject_q(comSubject) != fmp) {
				set_com_subject_q(comSubject, FMP_BOTH);
			}
			return comSubject;
		}
	}

	com_subjects_t *subjects = realloc(get_comission_subjects(comission),
		(count + 1) * sizeof_com_subject());
	if (subjects == NULL) {
		fprintf(stderr, "No se pudo reservar memoria para las materias de la comision.\n");
		abort();
	}
	set_comission_subjects(comission, subjects);
	set_comission_length(comission, count + 1);
	com_subjects_t *comSubject = get_comission_subject_at(comission, count);
	set_com_subject_id(comSubject, subjectId);
	set_com_subject_q(comSubject, fmp);
	return comSubject;
}

static int same_period_schedule(const schedule_data_t *schedule, id_t subjectId) {
	for (size_t day = 0; day < get_schedule_day_count(); day++) {
		for (size_t block = 0; block < get_schedule_block_count(); block++) {
			int occupied[2] = {0};
			for (size_t period = 0; period < 2; period++) {
				const char *name = get_subjet_name_for_block(schedule, day, block,
					(period_t)period);
				if (name[0] != '\0') {
					int64_t position = subject_exists_similar(name, 0.90);
					occupied[period] = position >= 0 &&
						get_subject_id(get_subject_at((size_t)position)) == subjectId;
				}
			}
			if (occupied[0] != occupied[1]) {
				return 0;
			}
		}
	}
	return 1;
}

schedule_t get_comission_schedule_time(const schedule_data_t* schedule) {
	switch(get_schedule_time(schedule)) {
		case 0: return SCHEDULE_MORNING;
		case 1: return SCHEDULE_AFTERNOON;
		case 2: return SCHEDULE_EVENING;
		default: break;
	}
	return SCHEDULE_MAX_ENUM;
}


dynarr_t *enumerate_files()
{
	const size_t sizeBytes = sizeof(char[256]);
	dynarr_t *files = init_dynamic_array(sizeBytes, 6);
	snprintf(push_slot(files), sizeBytes, "%s", "horarios1ro.xlsx");
	snprintf(push_slot(files), sizeBytes, "%s", "horarios2do.xlsx");
	snprintf(push_slot(files), sizeBytes, "%s", "horarios3ro.xlsx");
	snprintf(push_slot(files), sizeBytes, "%s", "horarios4to.xlsx");
	snprintf(push_slot(files), sizeBytes, "%s", "horarios5to.xlsx");
	snprintf(push_slot(files), sizeBytes, "%s", "horarios6to.xlsx");
	return files;
}

int64_t subject_exists_similar(const char *subject, float match)
{
	const size_t scount = get_subject_count();
	for (size_t s = 0; s < scount; s++) {
		if (strcmp(get_subject_name(get_subject_at(s)), subject) == 0) {
			return (int64_t)s;
		}
	}
	for (size_t s = 0; s < scount; s++)
	{
		subject_t *sub = get_subject_at(s);
		const char *sname = get_subject_name(sub);
		if (is_similar(sname, subject, match))
			return s;
	}
	return -1;
}

int64_t dictation_exists(id_t subject, id_t comission, id_t teacher)
{
	const size_t dcount = get_dictation_count();
	for (size_t d = 0; d < dcount; d++)
	{
		dictation_t *dic = get_dictation_at(d);
		if (subject != get_dictation_subject_id(dic))
			continue;
		if (comission != get_dictation_comission_id(dic))
			continue;
		if (teacher != get_dictation_teacher_id(dic))
			continue;
		return d;
	}
	return -1;
}
