#include "assembly.h"

func* add_number(int num, func* array) 
{
	func* temp;
    /* הקצאה מחדש עם מקום לעוד מבנה אחד */
    	temp = (func*) realloc(array, (IC + 1) * sizeof(func));
    	if (temp == NULL) 
	{
  /* במקרה של כישלון הקצאה */
        free(array);
        return NULL;
    	}

    /* רק 8 הסיביות הראשונות - נשמרות ב-first */
    	temp[IC].first = (char)(num & 0xFF);

    /* שאר הסיביות (אם בכלל) נשמרות ב-second */
    	temp[IC].second = (char)((num >> 8) & 0xFF);

    /* עדכון IC */
    	IC++;

    	return temp;
}
func* add_two_numbers(int num1, int num2, func* array) 
{
    	func* temp;

    	temp = (func*) realloc(array, (IC + 1) * sizeof(func));
    	if (temp == NULL) 
	{
        	free(array);
        	return NULL;
    	}

    	temp[IC].first = (char)(((num1 & 0x0F) << 4) | (num2 & 0x0F));
    	temp[IC].second = 0;

    	IC++;

    	return temp;
}
