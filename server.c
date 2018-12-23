#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* 定数 */
#define PORT 12345    // 使用するポート番号

/* プロトタイプ宣言 */


/* 構造体 */
struct client{    // クライアント情報
	int sockfd;    // ソケット
	int state;    // 使用状況 1のとき使ってる
	char username[16];    // ユーザ名
};


int main(int argc, char* argv[]){

	int i, j, sockfd, announce, maxfd, closed;
	int active = 0;    // アクティブユーザ数
	int num_of_user = 0;    // 全ユーザ数
	char message[157], orig[141], username[16];
	struct sockaddr_in serv_addr;
	struct client user[4];    // 接続クライアント
	fd_set fds, init;

puts("サーバからのメッセージを入力できます");



	/* 構造体初期化 */
	for(i = 0; i < 4; i++){
		user[i].state = 0;
	}

	/* ソケット作成 */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);


	/* アドレス作成 */
	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);



	int a=1;
		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &a, sizeof(int));

	bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));

	/* コネクション要求待ち */
	listen(sockfd, 8);

	/* fd_set初期化 */
	FD_ZERO(&init);
	FD_SET(sockfd, &init);
	FD_SET(0, &init);
	maxfd = sockfd;

	while(1){
		/* select設定 */
		memcpy(&fds, &init, sizeof(fd_set));    // select対象のグループ初期化
		select(maxfd+1, &fds, NULL, NULL, NULL);    // selectで通信の監視対象を設定

		/* 新規ユーザ登録 */
		if(FD_ISSET(sockfd, &fds)){    // 新規の通信
			/* ユーザ上限 */
			/*
			ユーザ数が上限に達している場合その旨を送信
			*/
			if(4 <= num_of_user){
				announce = accept(sockfd, NULL, NULL);

				read(announce, username, 16);
				sprintf(message, "ユーザ数が上限です。");
				write(announce, message, 157);
			}
			/* 通常追加 */
			else{
				/* select対象に */
				user[num_of_user].sockfd = accept(sockfd, NULL, NULL);

				FD_SET(user[num_of_user].sockfd, &init);
				if(maxfd < user[num_of_user].sockfd){
					maxfd = user[num_of_user].sockfd;
				}
				/* ユーザ数情報更新 */
				num_of_user++;
			}
		}

		/* 既存ユーザからの受信 */
		for(i = 0; i < num_of_user; i++){
			if(FD_ISSET(user[i].sockfd, &fds)){    // ユーザからの通信
				read(user[i].sockfd, orig, 141);
				/* 新規登録メッセージ */
				/*
				ユーザを追加し，ユーザ追加情報を全クライアントに送信
				*/
				if(strncmp(orig, "name", 4) == 0){
					active++;
					user[i].state = 1;
					strncpy(user[i].username, orig+4, 15);
					sprintf(message, "%sが参加。利用者は%d人です。", user[i].username, active);
					for(j = 0; j < num_of_user; j++){
						if(user[j].state){
							write(user[j].sockfd, message, 157);
						}
					}
					continue;
				}
				/* 退出 */
				/*
				ユーザを未使用にし，退出情報を全クライアントに送信
				*/
				if(strcmp(orig, "exit") == 0){
					for(j = 0; j < num_of_user; j++){
						if(user[j].state){
							sprintf(message, "%sが退室しました。", user[i].username);
					write(user[j].sockfd, message, 157);
						}
					}
					FD_CLR(user[i].sockfd, &init);    // select監視グループから消去
					user[i].state = 0;
					active--;
					continue;
				}
				if(strcmp(orig, "あほ") == 0){//ここである特定の文字を検知しユーザを適切に処理
									for(j = 0; j < num_of_user; j++){
										if(user[j].state){

										sprintf(message, "\x1b[31m%sはbanされました\x1b[30m", user[i].username);
									write(user[j].sockfd, message, 157);
										}
									}
									FD_CLR(user[i].sockfd, &init);    // select監視グループから消去
									user[i].state = 0;
									active--;
									continue;
								}
				/* 空メッセージ処理 */
				if(strcmp(orig, "") == 0){
					continue;
				}
				/* 通常 */
				sprintf(message, "%-15s:%s", user[i].username, orig);
				for(i = 0; i < num_of_user; i++){
					if(user[i].state){
						write(user[i].sockfd, message, 157), "WRITE";
					}
				}
			}
		}

		/* サーバコマンド */
		if(FD_ISSET(0, &fds)){    // キーボードからの標準入力
			printf("入力>>");
							fgets(orig, 15, stdin);

							orig[strlen(orig) - 1] = '\0';
			/* サーバダウン */
			/*
			サーバ終了のメッセージを全クライアントに送信する
			*/
			if(strcmp(orig, "exit") == 0){
				for(i = 0; i < num_of_user; i++){
					if(user[i].state){
						write(user[i].sockfd, "サーバがダウン", 157);
					}
				}
				break;
			}
			/* サーバメッセージ */
			/*
			全クライアントにサーバからのメッセージを送信する
			*/
			sprintf(message, "server:%s", orig);
			for(i = 0; i <= num_of_user; i++){
				if(user[i].state){
				write(user[i].sockfd, message, 157);
				}
			}
		}
	}

	/* クライアント終了確認 */
	for(i = 0; i < num_of_user; i++){
		if(user[i].state){
			while(1){
				closed = read(user[i].sockfd, orig, 141);

				if(closed == 0){
					break;
				}
			}
		}
	}

	/* サーバサイド終了 */
	for(i = 0; i < num_of_user; i++){
		if(user[i].state){
	close(user[i].sockfd);
		}
	}
	close(sockfd);
	return 0;
}
