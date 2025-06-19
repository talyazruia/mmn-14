
   FILE * macro_analysis(FILE * f1, command cmd[], command1 cmd1[]) {
    macro **macros = NULL;
    int macro_count = 0;
    int macro_capacity = 0;
    char *name;
    char *content;
    char row[MAX_LEN_OF_ROW + 2];
    char *token;
    char *token1;
    int i;
    FILE *f2; /* Output file pointer - ADDED */
    int changes_made = 0; /* Track if any macro replacements were made - ADDED */
    
    /* Open output file - ADDED */
    f2 = fopen("output.txt", "w");
    if (f2 == NULL) {
        fprintf(stderr, "Error: Cannot open output file\n");
        return NULL;
    }
    
    while (!feof(f1)) {
        /* Read a line from file */
        if (fgets(row, sizeof(row), f1) == NULL) {
            break; /* End of file or error - ADDED CHECK */
        }
        
        /* Check if line is too long */
        if (strchr(row, '\n') == NULL && !feof(f1)) { /* ADDED feof check */
            fprintf(stderr, "Error: line too long (more than %d characters)\n", MAX_LEN_OF_ROW);
            error = 1;
            /* clear_row_arry(); - COMMENTED OUT - function not defined */
           /* ADDED continue to skip this line */
        }
        
        token = strtok(row, "\n\t\r ");
        if (token != NULL && strcmp(token, "mcro") == 0) {
            token = strtok(NULL, "\n\t\r "); /* Get the next token after 'mcro' */
            
            /* Check if macro name exists - ADDED NULL CHECK */
            if (token == NULL) {
                fprintf(stderr, "Error: macro name missing\n");
                error = 1;
                continue;
            }
            
            /* Check if macro name is same as command name */
            for (i = 0; i < 21; i++) {
                if (strcmp(cmd[i].name, token) == 0 || strcmp(cmd1[i].name, token) == 0) {
                    error = 1;
                    fprintf(stderr, "Error: macro name not valid\n"); /* FIXED MESSAGE */
                    break; /* ADDED break to exit loop */
                }
            }
            
            /* Skip if error found */
            if (error == 1) {
                continue;
            }
            
            /* Allocate memory for macro name */
            name = (char *)malloc(strlen(token) + 1);
            if (name == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
			continue;
             /*   fclose(f2);  ADDED file cleanup */
               /* return NULL;  CHANGED from exit(1) */
            }
            strcpy(name, token);
            
            /* Allocate and initialize content buffer */
            content = (char *)calloc(1, sizeof(char) * MAX_CONTENT_SIZE);
            if (content == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                free(name); /* ADDED cleanup */
                fclose(f2); /* ADDED file cleanup */
                return NULL; /* CHANGED from exit(1) */
            }
            
            /* Collect macro content until 'endmcro' */
            while (fgets(row, sizeof(row), f1) != NULL) {
                if (strstr(row, "endmcro") != NULL) {
                    break;
                }
                /* Check if content buffer has enough space - ADDED */
                if (strlen(content) + strlen(row) >= MAX_CONTENT_SIZE - 1) {
                    fprintf(stderr, "Error: macro content too large\n");
                    error = 1;
                    break;
                }
                strcat(content, row);
            }
            
            /* Add macro to macro array manually - FIXED */
            /* Expand array if needed */
            if (macro_count >= macro_capacity) {
                macro_capacity = (macro_capacity == 0) ? 1 : macro_capacity * 2;
                macros = (macro **)realloc(macros, macro_capacity * sizeof(macro *));
                if (macros == NULL) {
                    fprintf(stderr, "Memory allocation failed\n");
                    free(name);
                    free(content);
                    fclose(f2);
                    return NULL;
                }
            }
            
            /* Allocate memory for new macro */
            macros[macro_count] = (macro *)malloc(sizeof(macro));
            if (macros[macro_count] == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                free(name);
                free(content);
                fclose(f2);
                return NULL;
            }
            
            /* Set macro name and content */
            macros[macro_count]->name = name;
            macros[macro_count]->content = content;
            macro_count++;
        }
        else if (token != NULL) {
            /* ADDED: Handle macro replacement */
            /* Extract single word from line */
            token1 = strtok(NULL, " \t\r\n");
            if (token1 != NULL) {
                error = 1; /* More than one word - mark error */
                fprintf(f2, "%s", row); /* Write original line - ADDED */
                continue;
            }
            
            /* Check if token matches any macro name */
            int found = 0;
            for (i = 0; i < macro_count; i++) {
                if (strcmp(token, macros[i]->name) == 0) {
                    fprintf(f2, "%s", macros[i]->content); /* REMOVED \n - content already has newlines */
                    found = 1;
                    changes_made = 1; /* Mark that changes were made - ADDED */
                    break;
                }
            }
            
            /* If no macro found, write original token */
            if (!found) {
                fprintf(f2, "%s\n", token);
            }
        }
        else {
            /* Empty line or only whitespace - ADDED */
            fprintf(f2, "%s", row);
        }
    }
    
    /* Cleanup and return - ADDED */
    for (i = 0; i < macro_count; i++) {
        free(macros[i]->name);
        free(macros[i]->content);
        free(macros[i]);
    }
    free(macros);
    
    /* Return appropriate file based on whether changes were made - MODIFIED */
    if (changes_made) {
        return f2; /* Return modified file */
    } else {
        fclose(f2); /* Close and don't return the output file */
        return f1;  /* Return original file */
    }
}
