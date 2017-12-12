/*
 * main.cpp
 *
 *  Created on: 4 Dec 2017
 *      Author: bhzhang
 */
#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <systemc.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "time.h"
#include "CoSimManager.h"
SC_MODULE(server){
	sc_in_clk clk;
	sc_event tt;
	double current_time;
	SC_CTOR(server){
		current_time = 0;
		SC_THREAD(hello_world);
		SC_THREAD(hello_fzi);
		//SC_THREAD(hello_continental);
		SC_THREAD(activate_server);
			sensitive << tt;
	}
	void hello_world(){
		sc_time t_Wait(10, SC_MS);
		while(1){
			std::cout << "hello, world!" << std::endl;
			//trigger_e.notify();
			//std::cout << "event is triggered" << std::endl;

			wait(t_Wait);
			//tt.notify();
			//next_trigger();
		}
	}

	void hello_continental(){
		sc_time t_Wait(10, SC_MS);
		int cycle_num = 0;
		while(1){
			cycle_num++;
			printf("this is the %d external cycle\n", cycle_num);
			while(1){
				printf("this is in internal cycle\n");
				wait();
			}
		}
	}

	void hello_fzi(){
		sc_time t_Wait(20, SC_MS);
		while(1){
			std::cout << "hello, FZI!" << std::endl;
			//std::cout << "event is triggered" << std::endl;
			wait(t_Wait);
			//tt.notify();
		}
	}


	void activate_server(){

		int listenfd,connfd;
		struct sockaddr_in sockaddr;
		bool request = false;

		int n;
		sc_time wait_time(40, SC_MS);
		memset(&sockaddr,0,sizeof(sockaddr));

		sockaddr.sin_family = AF_INET;
		sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		sockaddr.sin_port = htons(10005);

		listenfd = socket(AF_INET,SOCK_STREAM,0);

		bind(listenfd,(struct sockaddr *) &sockaddr,sizeof(sockaddr));

		listen(listenfd,1024);

		printf("Please wait for the client information\n");

		while(1) {
			if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1) {
				printf("accpet socket error: %s errno :%d\n", strerror(errno), errno);
				continue;
			}

			while (1)  {
				n = recv(connfd, &request, sizeof(request), 0);
				if(n > 0){
					request = true;
					printf("recv msg from client\n");
					if(request){
						current_time = sc_time_stamp().to_seconds();
						float currentTime = float(current_time);
						printf("current Time is %f\n", current_time);

						if ((send(connfd, &currentTime, sizeof(currentTime), 0)) <= 0) {
							printf("Failure in Sending Mess\n");
							break;
						}
						printf("Time has been sent to Client\n");
						request = false;
						break;
					}
				}
				else if (n == -1) {
					perror("recv\n");
					//next_trigger(trigger_e);
					break;
				}
				else if (n == 0) {
					printf("No data\n");
					break;
				}
			}
			sc_time nextSimTime(SC_ZERO_TIME);
			this->simcontext()->next_time(nextSimTime);
			std::cout << "next time stamp is " << float(nextSimTime.to_seconds()) << std::endl;
			wait(nextSimTime - sc_time_stamp());
		}
		close(listenfd);
	}
};



int sc_main(int argc, char* argv[]){
	//sc_clock activation("activation", sc_time(1, SC_SEC), 0.5);
	//server my_server("my_server");
	//my_server.clk(activation);
	//sc_time t_PERIOD(100, SC_MS);

	CoSimManager manager("my_manager");
	sc_start();
	return 0;
}
