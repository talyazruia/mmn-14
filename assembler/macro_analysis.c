  FILE * macro_analysis(FILE * f1,command cmd[])
{
while(!feof(f1)
{
char *s = strtok(user, " ");
for(i = 0; i < 8; i++)
		{
			if(strcmp(cmd, commend[i].name) == 0)
			{
				func = (commend[i].func)(mat, user);
				break;
			}
		}


}
