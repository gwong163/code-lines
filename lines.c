#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <dirent.h>

#define USR_EXT_NUM 7
char *usr_exts[] = {
	"h",
	"c",
	"cpp",
	"java",
	"hpp",
	"cc",
	"s",
};
static unsigned long total_lines = 0;


unsigned long get_lines(char* text)
{
	char *p;
	char buf[65535];
	unsigned long lines = 0;
	memset(buf, 0, sizeof(buf));
	if(text) {
		char *file = text;
		/* printf("get file:%s\n", file); */
		FILE *fp = fopen(file, "rt");
		if (fp) {
			while(p = fgets(buf, 65535, fp)) {
				if(strlen(p) > 1)
					++ lines;
			}
			fclose(fp);
		} else {
			printf("open file error！\n");
		}

	} else {
		printf("please input the file name！\n");
	}

	return lines;
}



char *get_ext(char *name)
{
	char *p;
	int l;
	int len = strlen(name);
	if (len <= 1)
		return NULL;

	p = name + len - 1;

	while (*p != '.' && p - name > 0) {
		--p;
	}


	if (*p == '.' && p - name < len - 1)
		return p + 1;
	else
		return NULL;
}







void list_dir(char *d_name)
{
	DIR *dp;
	struct dirent *dirp;
	struct stat statbuf;
	char localDir[256];
	int len;
	char *p, *ext;
	int i, match = 0;

	memset(localDir, 0, sizeof(localDir));


	if((dp = opendir(d_name)) == NULL)
	{
		printf("can't open:%s\n", d_name);
		return;
	}

	while((dirp = readdir(dp)) != NULL)
	{
		if(strcmp(".", dirp->d_name) == 0 || 
			strcmp("..", dirp->d_name) == 0)
			continue;

		strcpy(localDir,d_name);
		strcat(localDir, "/");
		strcat(localDir, dirp->d_name);

		lstat(localDir, &statbuf);
		if(S_ISDIR(statbuf.st_mode)) {
			list_dir(localDir);
			continue;
		}

		if(S_ISREG(statbuf.st_mode)) {
			/*普通文件,权限没有判断*/

			p = get_ext(dirp->d_name);
			ext = p;
			if(p) {
				while(*p != '\0') {
					*p = tolower(*p);
					++p;
				}

				match = 0;
				for (i = 0; i < USR_EXT_NUM; ++i) {
					if(strcmp(ext, usr_exts[i]) == 0) {
						match = 1;
						break;
					}
				}
				if (match) {
					/* printf("%s\n", localDir); */
					total_lines += get_lines(localDir);
				}
			}
		}
	}
	closedir(dp);
	
}


int main(int argc, char *argv[])
{
	if(argc > 1) {
		struct stat statbuf;
		lstat(argv[1], &statbuf);
		if(!S_ISDIR(statbuf.st_mode)) {
			printf("it is not dir\n");
			return -1;
		}
		list_dir(argv[1]);

		printf("total_lines:%u\n", total_lines);
	} else
		printf("please input dir\n");

	return 0;
}
