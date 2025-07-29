#include "assembler.h"

/*
 * פונקציה: encode_10bit_to_chars
 * תיאור: מקודדת את 10 הביטים הנמוכים של מספר שלם לשני תווים
 * הביטים העליונים (8) נשמרים ב-a, הביטים הנמוכים (2) נשמרים בביטים העליונים של b
 */
void encode_10bit_to_chars(int num, char *a, char *b)
{
	unsigned short value;
	/* שמירה רק של ال-10 ביטים הנמוכים */
	value = (unsigned short)(num & 0x3FF);
	/* שמירת 8 הביטים העליונים (ביטים 2 עד 9) ב-a */
	*a = (char)((value >> 2) & 0xFF);
	/* שמירת 2 הביטים הנמוכים בביטים 7-6 של b */
	*b = (char)((value & 0x3) << 6);
}

/*
 * פונקציה: base4_convert
 * תיאור: ממירה 8 ביטים מ-a ו-2 ביטים עליונים מ-b למחרוזת בסיס 4
 * 00 -> 'a', 01 -> 'b', 10 -> 'c', 11 -> 'd'
 */
char *base4_convert(char a, char b, char* result)
{
	int i;
	unsigned char two_bits;
	
	/* המרת 8 הביטים של a: עיבוד 4 קבוצות של 2 ביטים מגבוה לנמוך */
	for (i = 0; i < 4; i++)
	{
		two_bits = (a >> (6 - i * 2)) & 0x03;
		switch (two_bits)
		{
			case 0: result[i] = 'a'; break;
			case 1: result[i] = 'b'; break;
			case 2: result[i] = 'c'; break;
			case 3: result[i] = 'd'; break;
		}
	}
	/* חילוץ 2 הביטים העליונים של b (ביטים 7-6) */
	two_bits = (b >> 6) & 0x03;
	switch (two_bits)
	{
		case 0: result[4] = 'a'; break;
		case 1: result[4] = 'b'; break;
		case 2: result[4] = 'c'; break;
		case 3: result[4] = 'd'; break;
	}
	/* סיום המחרוזת */
	result[5] = '\0';
	return result;
}

/*
 * פונקציה: BinaryToBase4
 * תיאור: ממירה מערך בינארי לפורמט בסיס 4 וכותבת לקובץ
 * פרמטרים:
 *   array - מצביע למערך המבנים
 *   argc - מספר הארגומנטים
 *   argv - מערך הארגומנטים  
 *   i - אינדקס הקובץ הנוכחי
 *   file_type - סוג הקובץ
 *   struct_type - סוג המבנה (1=binary_code, 2=binary_directive)
 */
/*void BinaryToBase4(void** array, int argc, char *argv[], int i, FILE* f2, int struct_type,int* semel_count)
{
	 printf("BinaryToBase4 called: struct_type=%d, array=%p, f2=%p\n", struct_type, (void*)array, (void*)f2);
				
	char a, b;			
	char* result;
	int j;				
	int k;				
	SEMEL** semel;
	binary_directive** bd_array;	
	binary_code** bc_array;
	result = (char *)malloc(6 * sizeof(char));
	if (result == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return;
    }
	printf("BinaryToBase4 called: struct_type=%d, array=%p, f2=%p\n", 
           struct_type, (void*)array, (void*)f2);
    
    if (array == NULL) {
        printf("Error: array is NULL\n");
        return;
    }
    
    if (f2 == NULL) {
        printf("Error: file is NULL\n");
	free(result);
        return;
    }
	printf("File pointer is valid\n");
	if (result == NULL)
	{
		fprintf(stderr, "Error: Memory allocation failed\n");
		return;
	}
	
	
	
	printf("About to check IC + DC condition\n");
if (IC + DC <= 250) {
printf("IC + DC condition passed\n");
		
		if (error == 0) {
		printf("No errors, proceeding\n");
			
			
			
						
			IC = 100;	
			j = 0;			
			switch (struct_type) {
				case 1: {
					
					bc_array = (binary_code**)array;
					
					
					while (j < current_size_instaction_struct)
					{
						
						encode_10bit_to_chars(IC, &a, &b);
						 base4_convert(a, b, result);
						
						
						for (k = 0; result[k] != '\0'; k++) {
							fprintf(f2, "%c", result[k]);
						}
						fprintf(f2, " ");
						
						
					base4_convert((*bc_array)[j].first, (*bc_array)[j].second, result);
						for (k = 0; result[k] != '\0'; k++) {
							fprintf(f2, "%c", result[k]);
						}
						fprintf(f2, "\n");
						
						j++;
						IC++;
					}
					break;
				}
				
				case 2: {
					
					 bd_array = (binary_directive**)array;
					
					
					while (j < current_size_directive_struct)
					{
						
						encode_10bit_to_chars(IC, &a, &b);
						base4_convert(a, b, result);
						
						
						for (k = 0; result[k] != '\0'; k++) {
							fprintf(f2, "%c", result[k]);
						}
						fprintf(f2, " ");
						
						
						base4_convert((*bd_array)[j].first, (*bd_array)[j].second, result);
						for (k = 0; result[k] != '\0'; k++) {
							fprintf(f2, "%c", result[k]);
						}
						fprintf(f2, "\n");
						
						j++;
						IC++;
					}
					break;
				}
				case 3:
				
				 {
					j=0;					
					semel=(SEMEL**)array;
					while (j < *semel_count){
						if(semel[j]->ex_en==0){
					fprintf(f2,"%s ",semel[j]->name);
					a = (char)(semel[j]->addres);            
					b = (char)((semel[j]->ex_en) << 6);  
					 base4_convert(a,b, result);
						for (k = 0; result[k] != '\0'; k++) {
							fprintf(f2, "%c", result[k]);
						}
						fprintf(f2, "\n");
					}
					j++;
					}
						
					break;	
				}
			case 4:{
			{
				j=0;					
					semel=(SEMEL**)array;
					while (j < *semel_count){
						if(semel[j]->ex_en==1){
					fprintf(f2,"%s ",semel[j]->name);
					a = (char)(semel[j]->addres);            
					b = (char)((semel[j]->ex_en) << 6);  
					 base4_convert(a,b, result);
						for (k = 0; result[k] != '\0'; k++) {
							fprintf(f2, "%c", result[k]);
						}
						fprintf(f2, "\n");
					}
					j++;
					}
						
					break;	
			}	
				default:
					fprintf(stderr, "Error: Invalid struct_type\n");
					break;
			
			
			free(result);
		}
		else {
			
			fprintf(stderr, "ERROR: Cannot open ob file since you have errors in your code\n");
		}
	}
	else {
		
		fprintf(stderr, "Error: Too many rows of code\n");
	}
}



	
	
	
}*/



void BinaryToBase4(void** array, int argc, char *argv[], int i, FILE* f2, int struct_type, int* semel_count)
{
	char a, b;
	char* result;
	int j;
	int k;
	SEMEL** semel;
	binary_directive** bd_array;	
	binary_code** bc_array;
	
	if (array == NULL) {
		printf("Error: array is NULL\n");
		return;
	}
	
	if (f2 == NULL) {
		printf("Error: file is NULL\n");
		return;
	}
	
	result = (char *)malloc(6 * sizeof(char));
	if (result == NULL) {
		fprintf(stderr, "Error: Memory allocation failed\n");
		return;
	}
	
	if (IC + DC <= 250) {
		if (error == 0) {
			IC = 100;
			j = 0;
			
			switch (struct_type) {
				case 1: {
					bc_array = (binary_code**)array;
					
					while (j < current_size_instaction_struct)
					{
						a=(char)IC;
						b=0;
						base4_convert(a, b, result);
						
						for (k = 0; k<4; k++) {
							fprintf(f2, "%c", result[k]);
						}
						fprintf(f2, " ");
						
						base4_convert((*bc_array)[j].first, (*bc_array)[j].second, result);
						for (k = 0; result[k] != '\0'; k++) {
							fprintf(f2, "%c", result[k]);
						}
						fprintf(f2, "\n");
						
						j++;
						IC++;
					}
					break;
				}
				
				case 2: {
					bd_array = (binary_directive**)array;
					
					while (j < current_size_directive_struct)
					{
						a=(char)IC;
						b=0;
						base4_convert(a, b, result);
						
						for (k = 0; k<4;  k++) {
							fprintf(f2, "%c", result[k]);
						}
						fprintf(f2, " ");
						
						base4_convert((*bd_array)[j].first, (*bd_array)[j].second, result);
						for (k = 0; result[k] != '\0'; k++) {
							fprintf(f2, "%c", result[k]);
						}
						fprintf(f2, "\n");
						
						j++;
						IC++;
					}
					break;
				}
				
				case 3: {
					j = 0;					
					semel = (SEMEL**)array;
					while (j < *semel_count) {
						if(semel[j]->ex_en == 0) {
							fprintf(f2, "%s ", semel[j]->name);
							a = (char)(semel[j]->addres);            
							b = 0; 
							base4_convert(a, b, result);
							for (k = 0; k <4; k++) {
								fprintf(f2, "%c", result[k]);
							}
							fprintf(f2, "\n");
						}
						j++;
					}
					break;	
				}
				
				case 4: {
					j = 0;					
					semel = (SEMEL**)array;
					while (j < *semel_count) {
						if(semel[j]->ex_en == 1) {
							fprintf(f2, "%s ", semel[j]->name);
							a = (char)(semel[j]->addres);            
							b = 0; 
							base4_convert(a, b, result);
							for (k = 0; k <4  ; k++) {
								fprintf(f2, "%c", result[k]);
							}
							fprintf(f2, "\n");
						}
						j++;
					}
					break;	
				}
				
				default:
					fprintf(stderr, "Error: Invalid struct_type\n");
					break;
			}
		}
		else {
			fprintf(stderr, "ERROR: Cannot open ob file since you have errors in your code\n");
		}
	}
	else {
		fprintf(stderr, "Error: Too many rows of code\n");
	}
	
	free(result);
}
