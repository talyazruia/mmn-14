#include "encoding.h"


char *base4_convert(char a, char b, char* result)
{
	int i;                      /* Loop counter for processing bit pairs */
	unsigned char two_bits;     /* Temporary storage for extracted 2-bit values */
	
	/* Process the 8 bits of 'a': handle 4 groups of 2 bits from high to low */
	for (i = 0; i < 4; i++)
	{
		/* Extract 2 bits at a time, starting from most significant bits */
		/* (6 - i * 2) calculates shift amount: 6,4,2,0 for positions 7-6,5-4,3-2,1-0 */
		two_bits = (a >> (6 - i * 2)) & 0x03;  /* 0x03 = 00000011 binary, masks lower 2 bits */
		
		/* Convert bit patterns to base4 characters a,b,c,d */
		switch (two_bits)
		{
			case 0: result[i] = 'a'; break;  /* 00 binary -> 'a' */
			case 1: result[i] = 'b'; break;  /* 01 binary -> 'b' */
			case 2: result[i] = 'c'; break;  /* 10 binary -> 'c' */
			case 3: result[i] = 'd'; break;  /* 11 binary -> 'd' */
		}
	}
	/* Extract the upper 2 bits of 'b' (bits 7-6) for the 5th base4 character */
	two_bits = (b >> 6) & 0x03;  /* Shift right by 6 to get bits 7-6, then mask */
	
	/* Convert these bits to base4 character */
	switch (two_bits)
	{
		case 0: result[4] = 'a'; break;  /* 00 binary -> 'a' */
		case 1: result[4] = 'b'; break;  /* 01 binary -> 'b' */
		case 2: result[4] = 'c'; break;  /* 10 binary -> 'c' */
		case 3: result[4] = 'd'; break;  /* 11 binary -> 'd' */
	}
	/* Null-terminate the string to make it a valid C string */
	result[5] = '\0';
	return result;  /* Return pointer to the converted string */
}


void BinaryToBase4(void** array, int argc, char *argv[], int i, FILE* f2, int struct_type, int* semel_count,int ic)
{
	char a, b;                           /* Two bytes to be converted to base4 */
	char* result;                        /* Buffer to store base4 converted string */
	int j=0;                            /* Loop counter for iterating through arrays */
	int k;                              /* Inner loop counter for character output */
	SEMEL** semel;                      /* Pointer to array of symbol structures */
	binary_directive** bd_array;        /* Pointer to array of binary directive structures */
	binary_code** bc_array;             /* Pointer to array of binary instruction structures */
	extern_label ** extern_labels1;     /* Pointer to array of external label structures */
	
	/* Validate input parameters to prevent null pointer errors */
	if (array == NULL) 
	{
		fprintf(stderr,"error: array is NULL\n");
		return;
	}
	
	if (f2 == NULL) 
	{
		fprintf(stderr,"error: file is NULL\n");
		return;
	}
	
	/* Allocate memory for the base4 result string (5 chars + null terminator) */
	result = (char *)malloc(6 * sizeof(char));
	if (result == NULL) 
	{
		fprintf(stderr, "error: Memory allocation failed\n");
		return;
	}
	
	
			
			/* Handle different data structure types based on struct_type parameter */
			switch (struct_type) 
			{

				case 1: 
				{
					/* Process binary instruction code array */
					IC=100;  /* Initialize instruction counter to starting address 100 */
					bc_array = (binary_code**)array;  /* Cast void pointer to binary_code array */
					j=0;     /* Initialize array index counter */
					
					/* Iterate through all instruction structures in the array */
					while (j < current_size_instaction_struct)
					{
						/* Convert current instruction address to base4 format */
						a=(char)IC;  /* Current instruction counter value as first byte */
						b=0;         /* Second byte is zero for address */
						base4_convert(a, b, result);  /* Convert to base4 string */
						
						/* Write the first 4 characters of converted address to file */
						for (k = 0; k<4; k++) 
							fprintf(f2, "%c", result[k]);
						fprintf(f2, " ");  /* Add space separator between address and data */
						
						/* Convert instruction data bytes to base4 and write to file */
						base4_convert((*bc_array)[j].first, (*bc_array)[j].second, result);
						for (k = 0; result[k] != '\0'; k++)   /* Write until null terminator */
							fprintf(f2, "%c", result[k]);
						fprintf(f2, "\n");  /* Add newline after each instruction */
						
						j++;   /* Move to next instruction in array */
						IC++;  /* Increment instruction counter for next address */
					}
					break;
				}
				case 2: 
				{
					/* Process binary directive array (data directives like .data, .string) */
					bd_array = (binary_directive**)array;  /* Cast void pointer to binary_directive array */
					j=0;     /* Initialize array index counter */
					
					/* Iterate through all directive structures in the array */
					while (j < current_size_directive_struct)
					{
						/* Convert current data address to base4 format */
						a=(char)IC;  /* Current instruction counter value as first byte */
						b=0;         /* Second byte is zero for address */
						base4_convert(a, b, result);  /* Convert to base4 string */
						
						/* Write the first 4 characters of converted address to file */
						for (k = 0; k<4;  k++) 
							fprintf(f2, "%c", result[k]);
						fprintf(f2, " ");  /* Add space separator between address and data */
						
						/* Convert directive data bytes to base4 and write to file */
						base4_convert((*bd_array)[j].first, (*bd_array)[j].second, result);
						for (k = 0; result[k] != '\0'; k++)   /* Write until null terminator */
							fprintf(f2, "%c", result[k]);
						fprintf(f2, "\n");  /* Add newline after each directive */
						
						j++;   /* Move to next directive in array */
						IC++;  /* Increment instruction counter for next address */
					}
					break;
				}
				case 3: 
				{
					/* Process external labels array - labels defined in other files */
					j = 0;     /* Initialize array index counter */
					extern_labels1 = (extern_label**)array;  /* Cast void pointer to extern_label array */
					
					/* Iterate through all external labels */
					while (j < *semel_count)  /* semel_count contains number of external labels */
					{
						
							/* Write label name followed by its address in base4 */
							fprintf(f2, "%s ", (*extern_labels1)[j].name);  /* Label name from structure */
							a = (char)(*extern_labels1)[j].addres;  /* Address field from structure */
							b = 0;  /* Second byte is zero for address conversion */
							base4_convert(a, b, result);  /* Convert address to base4 */
							
							/* Write first 4 characters of converted address */
							for (k = 0; k <4; k++) 
								fprintf(f2, "%c", result[k]);
							fprintf(f2, "\n");  /* Add newline after each external label */
						
						j++;  /* Move to next external label in array */
					}
					break;	
				}
				case 4: 
				{
					/* Process symbol (SEMEL) array - internal labels and symbols */
					j = 0;     /* Initialize array index counter */
					semel = (SEMEL**)array;  /* Cast void pointer to SEMEL array */
					
					/* Iterate through all symbols in the symbol table */
					while (j < *semel_count) /* semel_count contains number of symbols */
					{ 
						/* Only process symbols that are not external/entry type */
						if(semel[j]->ex_en == 0) /* ex_en flag: 0=internal, 1=external/entry */
						{  
							/* Write symbol name and its address in base4 */
							fprintf(f2, "%s ", semel[j]->name);        /* Symbol name from structure */
							a = (char)(semel[j]->addres);              /* Address field from structure */
							b = 0;  /* Second byte is zero for address conversion */
							base4_convert(a, b, result);  /* Convert address to base4 */
							
							/* Write first 4 characters of converted address */
							for (k = 0; k <4  ; k++) 
								fprintf(f2, "%c", result[k]);
							fprintf(f2, "\n");  /* Add newline after each internal symbol */
						}
						j++;  /* Move to next symbol in array */
					}
					break;	
				}
				case 5:
				{
					/* Write instruction counter and data counter values in base4 format */
					/* This case outputs the final IC and DC values to the file header */
					
					/* Convert and write instruction counter (ic parameter) */
					a=(char)ic;  /* ic parameter contains final instruction counter value */
					b=0;         /* Second byte is zero for counter conversion */
				
						base4_convert(a, b, result);  /* Convert IC to base4 */
						
						/* Write all 4 characters of converted IC */
						for (k = 0; k<4; k++)
							fprintf(f2, "%c", result[k]);
						fprintf(f2, " ");  /* Add space separator between IC and DC */
						
					/* Convert and write data counter */
					a=(char)DC;  /* DC is global data counter variable */
					b=0;         /* Second byte is zero for counter conversion */
				
						base4_convert(a, b, result);  /* Convert DC to base4 */
						
						/* Write all 4 characters of converted DC */
						for (k = 0; k<4; k++) 
							fprintf(f2, "%c", result[k]);
						fprintf(f2, " ");  /* Add space after DC value */
					break;
				}
				default:
				{
					/* Handle invalid struct_type values */
					fprintf(stderr, "error: Invalid struct_type\n");
					break;
				}
			}
	
	/* Clean up allocated memory to prevent memory leaks */
	free(result);
}

void add_number(int num, void** array, array_type type, int ARE) 
{
	void* temp;              /* Temporary pointer for reallocation */
    	int* current_size;       /* Pointer to current array size counter */
    	size_t item_size;        /* Size of individual array element */
    
    	/* Determine array type and set corresponding parameters */
    	switch (type) 
	{
        	case TYPE_INSTRUCTION:
            		/* For instruction arrays */
            		current_size = &current_size_instaction_struct;  /* Global instruction array size */
            		item_size = sizeof(binary_code);                 /* Size of instruction structure */
            		break;
        	case TYPE_DIRECTIVE:
            		/* For directive/data arrays */
            		current_size = &current_size_directive_struct;   /* Global directive array size */
            		item_size = sizeof(binary_directive);            /* Size of directive structure */
            		break;
        	default:
            		return;  /* Invalid type, exit function */
    	}
    
    	/* Allocate or reallocate memory for the array */
    	if (*array == NULL)
        	/* First allocation - array doesn't exist yet */
        	temp = malloc(item_size);
   	else 
        	/* Expand existing array by one element */
        	temp = realloc(*array, (*current_size + 1) * item_size);
    
    	/* Check if memory allocation succeeded */
    	if (temp == NULL) 
	{
        	/* Allocation failed - clean up and exit */
        	free(*array);
        	*array = NULL;
        	return;
    	}
    
    	*array = temp;  /* Update array pointer to new memory location */
    
    	/* Add the number to the appropriate array structure */
    	if (type == TYPE_INSTRUCTION) 
	{
        	/* Add to instruction array */
        	binary_code* bc_array = (binary_code*)(*array);      /* Cast to instruction array type */
        	bc_array[*current_size].first = (char)num;           /* Store number in first byte */
        	bc_array[*current_size].second = (char)(ARE << 6);   /* Store ARE flag in upper 2 bits of second byte */
		IC++;  /* Increment global instruction counter */
    	} 
	else 
	{
        	/* Add to directive array - numbers can be larger, so split across two bytes */
        	binary_directive* bd_array = (binary_directive*)(*array);  /* Cast to directive array type */
        	bd_array[*current_size].first  = (char)(num >> 2);         /* Store upper bits in first byte */
        	bd_array[*current_size].second = (char)((num & 0x3) << 6); /* Store lower 2 bits in upper part of second byte */
    	}
    
    	/* Increment the size counter for the array */
    	(*current_size)++;
}

void add_two_numbers(int num1, int num2, binary_code** array) 
{
	binary_code* temp;  /* Temporary pointer for reallocation */
    
    	/* Validate input parameter */
    	if (array == NULL) 
        	return;  /* Invalid pointer, exit function */
    
    	/* Expand the array to accommodate new entry */
    	temp = (binary_code*) realloc(*array, (current_size_instaction_struct+1) * sizeof(binary_code));
    
    	/* Check if reallocation succeeded */
    	if (temp == NULL) 
	{
        	error=1;  /* Set global error flag to indicate memory failure */
        	return;
    	}
    
    	*array = temp;  /* Update array pointer to new memory location */
    
    	/* Pack two 4-bit numbers into one byte */
    	/* num1 (masked to 4 bits) goes in upper 4 bits, num2 (masked to 4 bits) in lower 4 bits */
    	(*array)[current_size_instaction_struct].first = (char)(((num1 & 0x0F) << 4) | (num2 & 0x0F));
    	(*array)[current_size_instaction_struct].second = 0;  /* Clear second byte - no data stored there */
    
    	/* Update global counters */
    	current_size_instaction_struct++;  /* Increment instruction array size */
	IC++;  /* Increment global instruction counter */
}

void to_binary(int opcode, int op1, int op2, binary_code** array) 
{
    	binary_code* temp;  /* Temporary pointer for memory allocation */
    	
	/* Check if array needs initial allocation */
	if (*array == NULL) 
	{
        	/* First time allocation - array doesn't exist yet */
        	*array = (binary_code*) malloc(sizeof(binary_code));
        	if (*array == NULL) 
		{
            		fprintf(stderr, "Error: malloc failed.\n");
			error=1;  /* Set global error flag */
            		return;
        	}
    	} 
	else 
	{
        	/* Expand existing array by one element */
        	temp = (binary_code*) realloc(*array, (current_size_instaction_struct + 1) * sizeof(binary_code));
        	if (temp == NULL) 
		{
            		fprintf(stderr, "Error: realloc failed.\n");
            		free(*array);  /* Clean up existing memory on failure */
            		*array = NULL;
			error=1;  /* Set global error flag */
            		return;
        	}
        	*array = temp;  /* Update array pointer to new memory location */
    	}
    	
  	/* Pack instruction components into first byte of the new array element:
	   - opcode (4 bits, masked with 0x0F) goes in bits 7-4 (shifted left by 4)
	   - op1 (2 bits, masked with 0x03) goes in bits 3-2 (shifted left by 2)  
	   - op2 (2 bits, masked with 0x03) goes in bits 1-0 (no shift) */
	(*array)[current_size_instaction_struct].first = ((opcode & 0x0F) << 4) | ((op1 & 0x03) << 2) | (op2 & 0x03);
	(*array)[current_size_instaction_struct].second = 0;  /* Clear second byte - instruction data fits in first byte */
	
	/* Update global counters */
	current_size_instaction_struct++;  /* Increment instruction array size */
	IC++;  /* Increment global instruction counter */
}
