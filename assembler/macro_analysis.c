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
macro** macros=NULL;
int *macro_count=0;
int *macro_capacity=0;
char *name;
char *content;
while(!feof(f1)
{
char row[MAX_LEN_OF_ROW+2];
char * token;
char * token1;
int i=0;
fgets(row, sizeof(row),f1);
	if (strchr(row, '\n') == NULL) 
	{
            	fprintf(stderr,"Error: line too long (more than %d characters)\n", MAX_LEN_OF_ROW);
		error=1;
		clear_row_arry();
	}

token=strtok(row,"\n\t\r ");
if(strcmp("mcro",token)
{
token=strtok(row,"\n\t\r ");
for(; i<16;i++)
{
if(strcmp(cmd[i].name,token)==0||strcmp(cmd1[i].name,token)==0)
{
	error=1;
	fprintf(stderr,"macro name not is valid");
}
}
if(error==0))
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
// קריאת שורה אחת מהקובץ
fgets(row, sizeof(row), f1);

// חילוץ המילה היחידה מהשורה (שוברת לפי רווחים, טאבים, סוף שורה)
token = strtok(row, " \t\r\n");

// בדיקה אם קיימת עוד מילה אחרי — אם כן, זו שגיאה (לפי מה שכתבת)
token1 = strtok(NULL, " \t\r\n");
if (token1 != NULL) {
    error = 1; // יש יותר ממילה אחת – סימון שגיאה
    continue;  // דלג לשורה הבאה
}

// משתנה עזר: האם נמצאה התאמה למאקרו
int found = 0;

// מעבר על כל המאקרואים
for ( i = 0; i < macro_count; i++) {
    // אם המילה שקלטתי תואמת ל-name של מאקרו כלשהו
    if (strcmp(token, macros[i]->name) == 0) {
        // כותב לפלט את התוכן של המאקרו במקום השם
        fprintf(f2, "%s\n", macros[i]->content);
        found = 1; // מצאנו והחלפנו – מסמן שהתבצעה החלפה
        break;     // לא צריך לבדוק עוד
    }
}

// אם לא מצאנו מאקרו תואם – כותבים את המילה המקורית כפי שהיא
if (!found) {
    fprintf(f2, "%s\n", token);
}

}
}
