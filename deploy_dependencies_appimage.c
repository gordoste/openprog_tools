#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
	FILE *file_in;
	char *buffer_in;
	char tmp;
	unsigned int i, par, grater;
	char *command;
	int len_prefix;

	file_in = fopen("./dependencies.txt", "r");
	i = 0, par = 0, grater = 0;
	do {
		tmp = getc(file_in);
		if(i == 0)
			buffer_in = (char *)malloc(sizeof(char));
		else
			buffer_in = (char *)realloc(buffer_in, sizeof(char) * (i + 1));
		
		if(tmp == '>')
			grater = 1;
		else if (tmp == '(')
			par = 1;
		else if ((grater == 1) && (par == 0))
			buffer_in[i++] = tmp;
		else
			;

		if(tmp == '\n'){
			par = 0; 
			grater = 0;
			//buffer_in[i++] = ',';
		}
	} while(tmp != EOF);
	fclose(file_in);
	buffer_in[strlen(buffer_in) - 1] = '\0';
	system("mkdir -p ./opgui.AppDir/usr/{lib,lib64}");
	command = (char *)malloc(sizeof(char) * strlen(buffer_in) * 3);
	sprintf(command, "for i in %s\ndo\n\tcp $i ./opgui.AppDir/$i\ndone", buffer_in);
	system(command);
	free(buffer_in);
	free(command);
	return 0;
}
