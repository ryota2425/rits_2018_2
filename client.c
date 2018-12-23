#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define PORT 12345    // 使用するポート番号



int main(int argc,char *argv[]){
	/* 変数宣言 */
	int sockfd;
	char username[16], message[157], orig[141];
	struct sockaddr_in serv_addr;
	fd_set fds, init;



	char ip[20] = "127.0.0.1";


	/* ソケット作成 */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	/* アドレス作成 */
	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ip);
	serv_addr.sin_port = htons(PORT);

	/* コネクション要求 */
	connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));

	puts("こんにちは\x1b[30m");



	printf("名前を入力>>");
				fgets(username, 15, stdin);

				username[strlen(username) - 1] = '\0';
	sprintf(message, "name%s", username);


	write(sockfd, message, 141);

	/* fd_set初期化 */
	FD_ZERO(&init);
	FD_SET(sockfd, &init);
	FD_SET(0, &init);

	while(1){
		/* select設定 */
		memcpy(&fds, &init, sizeof(fd_set));    // select対象のグループ初期化
		select(sockfd+1, &fds, NULL, NULL, NULL);    // selectで通信の監視対象を設定

		/* 読み込み */
		if(FD_ISSET(sockfd, &fds)){    // 新規の通信
	read(sockfd, message, 157);
			printf("%s\n", message);
			/* サーバダウン受信時 */
			if(strcmp(message, "サーバがダウン") == 0){
				break;
			}
			/* ユーザ上限時 */
			if(strcmp(message, "ユーザ数が上限です。") == 0){
				break;
			}
		}

		/* 書き込み */
		if(FD_ISSET(0, &fds)){    // キーボードからの標準入力




while(1){
	int check =0;
	printf(">>");
			fgets(orig, 15, stdin);
			//禁止文字が含まれていないかどうかのチェック

		 int a_cnt;


			orig[strlen(orig) - 1] = '\0';

			 for ( a_cnt = 0; orig[a_cnt] != '\0'; ++a_cnt ) {

			 if(orig[a_cnt] == '%'||orig[a_cnt]=='/'){
			puts("禁止文字が含まれています");

				check = 1;
				break;
					    }

				  }


if(check == 0){
	break;
}else{
	   memset(&orig, '\n', sizeof(orig));
	continue;
}



}


			write(sockfd, orig, 141);
			if(strcmp(orig, "exit") == 0){
				break;
			}
			/* メッセージの初期化 */
			strcpy(orig, "");
		}


	}

	/* クローズ */
close(sockfd);
	return 0;
}

