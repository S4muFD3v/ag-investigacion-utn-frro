#include "chromosomeLoader.h"

#include <stdint.h>
#include <string.h>
#include "utils/memory.h"
#include "load/schedule_loader.h"
#include "db.h"
#include "utils/compare.h"

dynarr_t* enumerate_files();
int64_t subject_exists_similar(char* subject, float match);
int64_t dictation_exists(id_t subject, id_t comission, id_t teacher);

chromosome_t* load_chromsome_from_files(const char* dataReadSubfolder) {
	const size_t fileNameSizeBytes = sizeof(char) * 256;
	const size_t yearStrSizeBytes = sizeof(char) * 4;
	dynarr_t* files = enumerate_files();

	char* fileFullName = malloc(fileNameSizeBytes);
	char* yearStr = malloc(yearStrSizeBytes);
	
	dynarr_t* geneList = init_dynamic_array(sizeof_gene(), 32);
	for (size_t a = 0; a < dynamic_array_size(files); a++) {
		*fileFullName = '\0';
		strcat_s(fileFullName, fileNameSizeBytes, "data\\");
		strcat_s(fileFullName, fileNameSizeBytes, (char*)at(files, a));
		schedule_file_t* sfile = open_file(fileFullName);
		
		// Get comissions
		size_t comCount = 0;
		com_id_t* comissions = NULL;
		get_comissions(sfile, &comCount, &comissions);

		//Parsing comissions
		for (size_t c = 0; c < comCount; c++) {
			size_t comYear = a + 1;
			comission_t* com = NULL;

			com = create_comission();

			schedule_data_t* schedule = get_schedule_for_comission(
				sfile, ((char*)comissions) + c * sizeof_com_id(), &schedule);
			
			//Parsing Blocks
			for (size_t d = 0; d < get_schedule_day_count(); d++) {
				size_t blockCount = 0;
				subject_t* lastSubject = NULL;
				size_t sessionBlockStart = 0;
				gene_t* session = NULL;
				id_t sessionSubjectId = 0;
				for (size_t b = 0; b < get_schedule_block_count(); b++) {
					char* name = get_subjet_name_for_block(schedule, d, b, PERIOD_FIRST); // Por ahora solo 1er cuat
					int64_t subjectIndx;
					if ((subjectIndx = subject_exists_similar(name, 0.8 /* 80% match */)) < 0)
						abort();
					sessionSubjectId = get_subject_id(get_subject_at(subjectIndx));

					subject_t* subject = get_subject_at(subject);
					if (lastSubject == NULL || get_subject_id(lastSubject) != get_subject_id(subject)) {
						// Nueva sesion
						blockCount = 1;
						lastSubject = subject;
						sessionBlockStart = b;
						session = push_slot(geneList);
					}
					else {
						// Misma sesion, incremento de horas
						blockCount++;
					}
				}

				id_t dicId = 0;
				size_t dictPos = dictation_exists(sessionSubjectId, c, 0);
				if (dictPos < 0) {
					dictation_t* dic = create_dictation();
					set_dictation_subject_id(dic, sessionSubjectId);
					set_dictation_comission_id(dic, c);
					set_dictation_teacher_id(dic, 0);
					dicId = get_dictation_id(dic);
				}
				else {
					dicId = get_dictation_id(get_dictation_at(dictPos));
				}

				set_gene_start_block_id(session, sessionBlockStart);
				set_gene_day(session, d);
				set_gene_length(session, blockCount);
				set_gene_dictation_id(session, get_dictation_id(dicId));
			}

			delete_schedule(schedule);
			set_comission_year(com, comYear);
			set_comission_schedule(com, SCHEDULE_MORNING); // Cambiar
		}
		close_file(sfile);
	}

	free(fileFullName);
	size_t geneCount = dynamic_array_size(geneList);
	size_t geneSizeBytes = geneCount * sizeof_gene();
	chromosome_t* ch = init_chromosome(geneCount);
	dynamic_array_copy_to(get_gene_at(ch, 0), geneSizeBytes, geneList);
	free_dynamic_array(geneList);
	return ch;
}

















dynarr_t* enumerate_files() {
	const sizeBytes = sizeof(char[256]);
	dynarr_t* files = init_dynamic_array(sizeBytes, 6);
	strcpy_s(push_slot(files), sizeBytes, "horarios1ro.xlsx");
	strcpy_s(push_slot(files), sizeBytes, "horarios2do.xlsx");
	strcpy_s(push_slot(files), sizeBytes, "horarios3ro.xlsx");
	strcpy_s(push_slot(files), sizeBytes, "horarios4to.xlsx");
	strcpy_s(push_slot(files), sizeBytes, "horarios5to.xlsx");
	strcpy_s(push_slot(files), sizeBytes, "horarios6to.xlsx");
	return sizeBytes;
}

int64_t subject_exists_similar(char* subject, float match) {
	const size_t scount = get_subject_count();
	for (size_t s = 0; s < scount; s++) {
		subject_t* sub = get_subject_at(s);
		const char* sname = get_subject_name(sub);
		if (is_similar(sname, subject, 0.90))
			return s;
	}
	return -1;
}

int64_t dictation_exists(id_t subject, id_t comission, id_t teacher) {
	const size_t dcount = get_dictation_count();
	for (size_t d = 0; d < dcount; d++) {
		dictation_t* dic = get_dictation_at(d);
		if (subject != get_dictation_subject_id(dic)) continue;
		if (comission != get_dictation_comission_id(dic)) continue;
		if (teacher != get_dictation_teacher_id(dic)) continue;
		return d;
	}
	return -1;
}