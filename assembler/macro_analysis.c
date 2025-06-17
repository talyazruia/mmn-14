void add_macro(macro **macros, int *macro_count, int *macro_capacity, char *name, char *content) {
    // אם המערך מלא, הגדל אותו
    if (macro_count >= macro_capacity) {
        macro_capacity = (macro_capacity == 0) ? 1 : macro_capacity * 2;
        macros = (macro *)realloc(macros, macro_capacity * sizeof(macro));
        if (macros == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
    }
    
    // הקצה מקום לשם המאקרו
    macros[macro_count].name = (char *)malloc(strlen(name) + 1);
    if (macros[macro_count].name == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    strcpy(macros[macro_count].name, name);
    
    // הקצה מקום לתוכן המאקרו
    macros[macro_count].content = (char *)malloc(strlen(content) + 1);
    if (macros[macro_count].content == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    strcpy(macros[macro_count].content, content);
    
    macro_count++;
}
FILE * macro_analysis(FILE * f1,command cmd[],command1 cmd1[])
{
macro **macros=NULL;
int *macro_count=0;
int *macro_capacity=0;
char *name;
char *content;
while(!feof(f1)
{
char row[MAX_LEN_OF_ROW+2];
char * token;
int i=0;
fgets(row, sizeof(row),f1);
	if (strchr(row, '\n') == NULL) 
	{
            	fprintf(stderr,"Error: line too long (more than %d characters)\n", MAX_LEN_OF_ROW);
		error=1;
		clear_row_arry();
	}

token=strtok(row,"\n\t\r ");
if(token=="mcro")
{
token=strtok(row,"\n\t\r ");
for(; i<16;i++)
if(strcmp(cmd[i].name,token)==0||strcmp(cmd1[i].name,token)==0)
{
	error=1;
	fprintf(stderr,"macro name not is valid");
}
else
{
ctrcpy(name,token);
while (fgets(row, sizeof(raw), file1) != NULL) {
        // בדוק אם השורה מכילה "endmacro"
        if (strstr(raw, "endmacro") != NULL) {
            break; // צא מהלולאה
	}
        
        // הוסף את השורה לתוכן
        strcat(content, line);
	}
}
add_macro(macros, macro_count, macro_capacity, name, content);
}

}
