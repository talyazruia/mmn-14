#include "assembler.h"
#include "validation.h"
#include "chang_file_name.h"
#include "row_analysis.h"
#include "second_row_analysis.h"
#include "encoding.h"

/*
 * Global assembler state.
 *
 * IC  - Instruction Counter; initial logical address for code words (starts at 100).
 * DC  - Data Counter; counts words emitted by data directives.
 * error - Global error flag; set to non-zero when a recoverable error is detected.
 * current_size_instaction_struct - Number of instruction words currently stored.
 * current_size_directive_struct  - Number of data words currently stored.
 * sum_of_row - Logical line number for diagnostics across passes and files.
 */
int IC = start_IC;
int DC = 0;
int error = 0;
int current_size_instaction_struct = 0;
int current_size_directive_struct = 0;
int sum_of_row = 0;
int count_of_entry_labels = 0;
	
void print_binary_code_array(binary_code* array, int size);

/*
 * main - Assembler driver.
 *
 * Performs (per input file):
 *  1) Macro expansion and first pass (symbol table build, IC/DC sizing).
 *  2) Address fix-ups for data symbols.
 *  3) Second pass (instruction/data emission + extern usage collection).
 *  4) Output generation (.ob, .ext, .ent) in base-4 encoding if no errors occurred.
 *
 * Returns:
 *  0 on success for all processed files; 1 on early argument error.
 *
 * Notes:
 *  - All dynamic structures are freed between files and again at program end.
 *  - When any error is detected (error != 0), output files are skipped for that source.
 */
int main(int argc, char *argv[])
{
	/* Instruction set table: name, required operands, opcode */
	command cmd[] = {
		{"mov", 2, 0},
		{"cmp", 2, 1},
		{"add", 2, 2},
		{"sub", 2, 3},
		{"not", 1, 6},
		{"clr", 1, 5},
		{"lea", 2, 4},
		{"inc", 1, 7},
		{"dec", 1, 8},
		{"jmp", 1, 9},
		{"bne", 1, 10},
		{"red", 1, 12},
		{"prn", 1, 13},
		{"jsr", 1, 11},
		{"rts", 0, 14},
		{"stop", 0, 15}
	};

	/* Directive table for the first pass */
	command1 cmd1[] = {
		{".data", 1},
		{".string", 1},
		{".mat", 1},
		{".entry", 1},
		{".extern", 1}
	};

	FILE *f1;					/* Source file (.as) after name normalization */
	FILE *f2_ob;					/* Object/output file (.ob) */
	FILE *f3_ex;					/* Externals output (.ext) */
	FILE *f4_en;					/* Entries output (.ent) */
	FILE *f_used;					/* Stream used for macro-expanded source (.am or original) */
	macro **macros;					/* Macro table (second-level pointer) */
	int macro_count;				/* Number of collected macros */
	SEMEL **SEMELS = NULL;				/* Symbol table (array of pointers to SEMEL) */
	int semel_count = 0;				/* Number of symbols in table */
	binary_code *array = NULL;			/* Instruction words buffer (2*char per word) */
	binary_directive *struct_DC = NULL;		/* Data words buffer (2*char per word) */
	extern_label *extern_labels = NULL;		/* Collected extern usages (name + address) */
	int count_of_extern_labels = 0;			/* Count of extern usage records */
	int i = 1;					/* argv index for input files */
	int j = 0;					/* iteration counter for cleanups */
	int ic;					/* Snapshot of final IC after first pass for .ob header */

	/* Require at least one input filename */
	if (argc == 1)
	{
		fprintf(stderr, "error, ther no input files.\n");
		return 0;
	}
	/* Process each input file independently */
	for (; i < argc; i++)
	{
		/* Reset global state for this file */
		error = 0;
		IC = start_IC;
		DC = 0;
		error = 0;
		current_size_instaction_struct = 0;
		current_size_directive_struct = 0;
		sum_of_row = 0;
		count_of_entry_labels = 0;
		/* Release any previous extern symbol table before starting a new file */
		if (extern_labels != NULL) 
		{
        		for (j = 0; j < count_of_extern_labels; j++) 
            			free(extern_labels[j].name);
        		free(extern_labels);
        		extern_labels = NULL;
    		}
    		count_of_extern_labels = 0;
		/* Release any previous symbol table before starting a new file */
		if (SEMELS != NULL)
		{
			for (j = 0; j < semel_count; j++)
			{
				if (SEMELS[j])
				{
					free(SEMELS[j]->name);
					free(SEMELS[j]);
				}
			}
			free(SEMELS);
			SEMELS = NULL;
			semel_count = 0;
		}
		/* Release previous instruction buffer if any */
		if (array != NULL)
		{
			free(array);
			array = NULL;
		}
		/* Release previous data buffer if any */
		if (struct_DC != NULL)
		{
			free(struct_DC);
			struct_DC = NULL;
		}
		/* Resolve and open source file (.as) -> f1 */
		f1 = end_file_name_as(argc, argv, i , macro_count, &macros, SEMELS, &semel_count, &array, &struct_DC, &extern_labels, &count_of_extern_labels);
		if (f1 != NULL)
		{
			/* Expand macros into a working stream (possibly .am) */
			macros = NULL;
			macro_count = 0;
			f_used = macro_analysis(f1, cmd, argc, argv, i, &macros, &macro_count, SEMELS, &semel_count, &array, &struct_DC, &extern_labels, &count_of_extern_labels);
			if (f_used != NULL)
			{
				/* -------- First pass -------- */
				sum_of_row = 0;
				row_analysis(f_used, macro_count, macros, cmd, cmd1, &SEMELS, &semel_count, &array, &struct_DC, &extern_labels ,&count_of_extern_labels);
				/* Fix data symbol addresses by adding final IC */
				update_data_symbol_addresses(SEMELS, &semel_count);
				/* Prepare for second pass */
				rewind(f_used);
				ic = IC;		/* Save final IC from pass 1 for .ob  */
				IC = start_IC;		/* Reset IC to base for emission */
				sum_of_row = 0;
				/* -------- Second pass -------- */
				second_row_analysis(f_used, cmd, &SEMELS, &semel_count, &array, &struct_DC, &extern_labels, &count_of_extern_labels,macro_count, macros);
				/* Generate output files only if:
				*  - No errors were reported (error == 0)
				*  - Total words (IC from pass1 + DC) fit the allowed maximum
				*/
				if (error == 0 && ic + DC <= MAX_NUMBES_OF_ROW_IN_INPUT)
				{
					/* -------- .ob file -------- */
					f2_ob = end_file_name(argc, argv, i, OB, macro_count, &macros, SEMELS, &semel_count, &array, &struct_DC, &extern_labels, &count_of_extern_labels);
					if (f2_ob == NULL)
					{
						error = 1;
						return 0;
					}
					/* Header: IC/DC (base-4 as required by BinaryToBase4 with ENCODING_IC_DC) */
					BinaryToBase4(argc, argv, i, f2_ob, ENCODING_IC_DC, &semel_count, ic - FIRST_ADDRES, macro_count, &macros, SEMELS, &array, &struct_DC, &extern_labels, &count_of_extern_labels);
					fprintf(f2_ob, "\n");
					/*instruction words */
					BinaryToBase4(argc, argv, i, f2_ob, ENCODING_IC_COMMAND, &semel_count, 0, macro_count, &macros, SEMELS, &array, &struct_DC, &extern_labels, &count_of_extern_labels);
					/* directive words */
					BinaryToBase4(argc, argv, i, f2_ob, ENCODING_DC_COMMAND, &semel_count, 0, macro_count, &macros, SEMELS, &array, &struct_DC, &extern_labels, &count_of_extern_labels);
					fclose(f2_ob);
					/* -------- .ext file -------- */
					if(count_of_extern_labels>0)
					{
						f3_ex = end_file_name(argc, argv, i, EXT, macro_count, &macros, SEMELS, &semel_count, &array, &struct_DC, &extern_labels, &count_of_extern_labels);
						if (f3_ex == NULL)
						{
							fprintf(stderr, "error Could not open %s.ext file\n", argv[i]);
							error = 1;
							return 0;
						}
						BinaryToBase4(argc, argv, i, f3_ex, ENCODING_EXTERN_FILE, &semel_count, 0, macro_count, &macros, SEMELS, &array, &struct_DC, &extern_labels, &count_of_extern_labels);
						fclose(f3_ex);
					}
					/* -------- .ent file -------- */
					if(count_of_entry_labels>0)
					{
						f4_en = end_file_name(argc, argv, i, ENT, macro_count, &macros, SEMELS, &semel_count, &array, &struct_DC, &extern_labels, &count_of_extern_labels);
						if (f4_en == NULL)
						{
							fprintf(stderr, "error Could not open %s.ent file\n", argv[i]);
							error = 1;
							return 0;
						}
						BinaryToBase4(argc, argv, i, f4_en, ENCODING_ENTERY_FILE, &semel_count, 0, macro_count, &macros, SEMELS, &array, &struct_DC, &extern_labels, &count_of_extern_labels);
						fclose(f4_en);
					}
					/* Close the working source stream */
					fclose(f_used);
				}
				else if(error!=0)
					/* Skip outputs when errors were detected or size exceeded */
					fprintf(stderr, "errors found - skipping file generation for %s\n", argv[i]);
				else if(ic + DC < MAX_NUMBES_OF_ROW_IN_INPUT)
					fprintf(stderr, "more then %d rows - skipping file generation for %s\n",MAX_NUMBES_OF_ROW_IN_INPUT-start_IC, argv[i]);
				/* Note:
				 * The following block, if enabled, would remove the temporary .am file
				 * upon errors. It is intentionally commented to preserve intermediates.
				 */
			}
			else 
				/* Macro analysis/open failed; continue with next file */
				continue;
		}
		else
		{
			/* Could not open the requested source */
			fprintf(stderr, "error Could not open source file for %s\n", argv[i]);
			error=1;
		}
	fprintf(stdout,"\n");
	}
	/* -------- Final cleanup for last processed file -------- */
	/* Free symbol table */
	if (SEMELS != NULL)
	{
		for (j = 0; j < semel_count; j++)
		{
			if (SEMELS[j])
			{
				free(SEMELS[j]->name);
				free(SEMELS[j]);
			}
		}
		free(SEMELS);
	}
	/* Free extern labels */
	if (extern_labels != NULL)
	{
		for (j = 0; j < count_of_extern_labels; j++)
			free(extern_labels[j].name);
		free(extern_labels);
	}
	/* Free instruction buffer */
	if (array != NULL)
		free(array);
	/* Free data buffer */
	if (struct_DC != NULL)
		free(struct_DC);
	return 0;
}


