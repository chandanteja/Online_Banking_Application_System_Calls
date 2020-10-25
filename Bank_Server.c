#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<netinet/in.h>
#include<stdlib.h>

//==============================
//User header files
//================================
#include  "User_Details.h"

//=========================
//Define Section
//=========================

#define SERV_PORT 5061


int main()
{
int listenfd,connfd;
socklen_t clilen;

struct sockaddr_in cliaddr,servaddr;
        listenfd=socket(AF_INET,SOCK_STREAM,0);
        bzero(&servaddr,sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr=INADDR_ANY;
        servaddr.sin_port=htons(SERV_PORT);
        bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
        perror("bind");
        listen(listenfd,5);
        perror("listen");

        while(1)
        {
                        clilen=sizeof(cliaddr);
                        connfd=accept(listenfd,(struct sockaddr*)&cliaddr,&clilen);   //accepts the connection
                        //create a child and defer the work to child
                        perror("accept:");
                        char welcome_msg[150]="==========Welcome to Vijay Mallya Bank.Press Enter to Continue===========\n";
                        char role_msg[100]="==========How would you like to Login?==========\n1. Bank Employee\n2. Customer\n";
                        char ch;
                        int role;
                        write(connfd,welcome_msg,sizeof(welcome_msg));
                        read(connfd,&ch,1);

                        if(ch=='\n')
                          {
                            write(connfd,role_msg,sizeof(role_msg));
                            read(connfd,&role,sizeof(role));
                            role=ntohl(role);

                            printf("Role Selected=%d\n",role);
                          }
                          pid_t pid=fork();

                          if(pid==0)
                          {  //child handles the connecction
                            close(listenfd);
                            char connfd_str[20];
                            //printf("role=%d\n",role);
                            //return 1;
                            switch(role)
                            {
                              case 1:   printf("\nInvoked Employee stub\n");
                                        //printf("connfd=%d\n",connfd);
                                        snprintf(connfd_str,sizeof(connfd_str),"%d",connfd);
                                        //while calling execlp, convert the connfd to string and pass it to child and then again convert back to int in child.
                                        execlp("./Bank_Employee","Bank_Employee",connfd_str,NULL);
                                        //printf("After execlp\n");
                                        break;
                              case 2:   printf("\nInvoked Customer Stub\n");
                                        snprintf(connfd_str,sizeof(connfd_str),"%d",connfd);
                                        execlp("./Bank_Customer","Bank_Customer",connfd_str,NULL);

                                        break;
                              default:
                                        printf("Invalid role selected by user\n");
                                        exit(1);

                            }
                          }
                          else if(pid>0)
                          {
                            close(connfd);    //parent closes the connection which child is handling and goes to listen mode
                          }
                          else{
                            perror("Fork");
                            exit(1);
                          }



                  }
                  return 0;
                }
