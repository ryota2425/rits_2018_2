#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <unistd.h>
#include<sys/wait.h>
#include <signal.h>

#include <unistd.h>
#include <sys/stat.h>
#define _GNU_SOURCE      // sighandler_tを使用する為に定義
pid_t pids[100];
int pidIn = 0;
void Fg(int);
void Job();
void abrt_handler(int sig);
void abrt_handler2(int sig);
int pid = 0;

int main(void) {
	setpgid(0, 0);
	int roop = 1;

	if (signal(SIGINT, abrt_handler) == SIG_ERR) {
		exit(1);
	}
	if (signal(SIGCHLD, abrt_handler2) == SIG_ERR) {
		exit(1);
	}

	char mainconduct[100], *cp, *arg2[100], *envPath, *args3[100];

	//int maxChar=20;
	//  char *str[maxChar] ;
	int var1;
	for (var1 = 0; var1 < 100; var1++) {
		pids[var1] = 0;
		args3[var1] = "\0";

	}

	while (roop) {
		int back = 0;
		//命令の前にpid状況表示

		int varstatus;

		for (var1 = 0; pids[var1] != 0; var1++) {

			pid_t p = waitpid(pids[var1], &varstatus, WNOHANG);

			if (p > 0) {
				// 子プロセスが正常終了の場合
				//int exit_code = WEXITSTATUS(varstatus); //子プロセスの終了コード
				printf("[%d]子プロセス終了コードは%d\n", var1, WEXITSTATUS(varstatus));
			} else {
				//printf("[%d]子プロセス異常終了しています%x\n",var1, exit_code);
			}

		}
		signal(SIGABRT, SIG_IGN);
		puts("命令を入力してください");

		//sighandler_t sig = 0;

		// Ctrl + C を無視
		//sig =
		signal(SIGABRT, SIG_IGN);

		fgets(mainconduct, 30, stdin);

		if (SIG_ERR == signal(SIGABRT, SIG_IGN)) {
			fprintf(stderr, "error.\n");
			return EXIT_FAILURE;
		}

		mainconduct[strlen(mainconduct) - 1] = '\0';
		if (mainconduct[strlen(mainconduct) - 1] == '&') {
			mainconduct[strlen(mainconduct) - 1] = '\0';

			puts("backgroundで実行します");
			back = 1;
		}
		const char *delim = " ";
		int arg1;

		cp = mainconduct;
		for (arg1 = 0; arg1 < 100; arg1++) {
			if ((arg2[arg1] = strtok(cp, delim)) == NULL)
				break;
			cp = NULL;
		}

		//環境変数取得プロセス
envPath="";
		envPath = getenv("PATH");		//環境変数を取得
		/*	if(envPath!=NULL){
		 printf("%s",envPath);
		 }*/

		const char *delim2 = ":";

		//char *FullPathMainconduct[255];
cp = envPath;
		int var3 = 0;

		for (var3 = 0; var3 < 100; var3++) {
			if ((args3[var3] = strtok(cp, delim2)) == NULL){
				break;
		}
			printf("args3[%d] =%s\n",var3,args3[var3]);
			cp = NULL;
		}




		if (strcmp("exit", mainconduct) == 0
				|| strcmp("quiet", mainconduct) == 0) {

			roop = 0;

		} else if (strcmp("jobs", mainconduct) == 0) {
			Job();

		} else if (strcmp("fg", mainconduct) == 0) {
			char text[20];
			strcpy(text, arg2[1]);
			int i = atoi(text);
			Fg(i);
			continue;

		} else {
			pid = fork();
			if (pid < 0) {
				puts("子プロセス生成に失敗しました");
				exit(-1);
			} else if (pid == 0) {
				// 子プロセス


				char In0[] = "/";
					char link[256];
					for(var3=0;var3<20;var3++){
						printf("%d\n",var3);
						sprintf(link,"%s%s%s\n",args3[var3],In0,mainconduct);
						printf("%s\n",link);
						link[strlen(link)-1]='\0';
						execv(link, arg2);
					}



				printf("子プロセス終了\n");

				return 1; //子プロセスの終了はWEXITSTATUS()
			}

			// 親プロセス開始

			int status;
			if (back == 1) {

				pids[pidIn] = pid;
				pidIn++;

				continue;

			}
			pid_t r = waitpid(pid, &status, 0); //子プロセスのプロセスIDを指定して、終了を待つ
			if (r < 0) {
				puts("子プロセスが生成に失敗したので親も終了します");
				exit(-1);
			}

			//判定
			if (WIFEXITED(status)) {
				// 子プロセスが正常終了の場合
				int exit_code = WEXITSTATUS(status); //子プロセスの終了コード
				printf("子プロセス終了コードは%d\n", exit_code);
			} else {
				printf("子プロセス異常終了しています%x\n", status);
			}

		}

	}

	//puts("whileend");

	puts("プログラム終了");
	return EXIT_SUCCESS;
}

void Job() {
	int var2;
	int varstatus2;
	for (var2 = 0; pids[var2] != 0; var2++) {

		pid_t p = waitpid(pids[var2], &varstatus2, WNOHANG);

		if (p == 0) {
			// 子プロセスが正常終了の場合
			//int exit_code = WEXITSTATUS(varstatus); //子プロセスの終了コード
			printf("[%d]子プロセス実行中,pid=%d\n", var2, pids[var2]);
		}
	}
}

void Fg(int number) {
	int status;
	pid_t r = waitpid(pids[number], &status, 0);
	if (r < 0) {
		puts("子プロセスが生成に失敗したので親も終了します");
		exit(-1);
	}

	//判定
	if (WIFEXITED(status)) {
		// 子プロセスが正常終了の場合
		int exit_code = WEXITSTATUS(status); //子プロセスの終了コード
		printf("子プロセス終了コードは%d\n", exit_code);
	} else {
		printf("子プロセス異常終了しています%x\n", status);
	}

}
void abrt_handler(int sig) {
	puts("^Cが入力されました");
	if (pid != 0) {
		kill(pid, SIGINT);
	}
}
void abrt_handler2(int sig) {
	puts("子終了シグナル受信");
	int var1;

	int varstatus = 0;
	for (var1 = 0; pids[var1] != 0; var1++) {

		pid_t p = waitpid(pids[var1], &varstatus, WNOHANG);

		if (p > 0) {
			// 子プロセスが正常終了の場合
			//int exit_code = WEXITSTATUS(varstatus); //子プロセスの終了コード
			printf("[%d]子プロセス終了コードは%d\n", var1, WEXITSTATUS(varstatus));
		}
	}

}