//client for connection establish

#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<netinet/in.h>


//#include  "User_Details.h"
#include "Employee_functions.h"

#define SERV_PORT 5061

//common client for both 34a and 34b. So if we want to execute both 34a and 34b then we need to change the port number after executing one of these
int main()
{
        int sockfd;
        struct sockaddr_in servaddr;
        sockfd=socket(AF_INET,SOCK_STREAM,0);
        perror("socket");
        bzero(&servaddr,sizeof(servaddr));
        servaddr.sin_family=AF_INET;
        servaddr.sin_port=htons(SERV_PORT);
        servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
        connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
        perror("connect");


        char welcome_msg_client[150];
        read(sockfd,welcome_msg_client,sizeof(welcome_msg_client));
        printf("%s",welcome_msg_client);
        char ch='\n';
        getchar();
        write(sockfd,&ch,1);
        // Choosing roles available.
        char role_msg[100];
        read(sockfd,role_msg,sizeof(role_msg));
        int role_cli;
        printf("%s\n",role_msg);
        scanf("%d",&role_cli);
        role_cli=htonl(role_cli);
        write(sockfd,&role_cli,sizeof(role_cli));

        //===========================================================
            //Bank employee data
       //===========================================================
            char username_and_password_prompt[250];
            char username_prompt[100]="Username: ";
            char password_prompt[100]="Password: ";
            char username[1024];
            char password[1024];

            read(sockfd,username_and_password_prompt,sizeof(username_and_password_prompt));
            printf("%s",username_and_password_prompt);
            ch='\n';
            getchar();
            write(sockfd,&ch,1);

            read(sockfd,username_prompt,sizeof(username_prompt));
            printf("%s",username_prompt);
            scanf(" %[^\n]",username);
            write(sockfd,username,sizeof(username));


            read(sockfd,password_prompt,sizeof(password_prompt));
            printf("%s",password_prompt);
            scanf(" %[^\n]",password);
            write(sockfd,password,sizeof(password));

            //Authentication messages
           char authentication_msg[100];
           char auth_fail_msg[100];
           char connection_close_msg[150];
           char welcome_user1[100],welcome_user2[100];


           int flagu,flagp;
           read(sockfd,&flagu,sizeof(flagu));
           read(sockfd,&flagp,sizeof(flagp));
           flagu=ntohl(flagu);
           flagp=ntohl(flagp);

           if(flagu ==1 && flagp==1)
             {
                 read(sockfd,authentication_msg,sizeof(authentication_msg));
                 printf("%s",authentication_msg);
                 read(sockfd,welcome_user1,sizeof(welcome_user1));
                 printf("%s",welcome_user1);
                 printf("%s",username);
                 read(sockfd,welcome_user2,sizeof(welcome_user2));
                 printf("%s",welcome_user2);

             }
           else
             {
                  read(sockfd,auth_fail_msg,sizeof(auth_fail_msg));
                  printf("%s",auth_fail_msg);
                  read(sockfd,connection_close_msg,sizeof(connection_close_msg));
                  printf("%s",connection_close_msg);
                  close(sockfd);
                  exit(EXIT_FAILURE);
             }

        //Selection of operation to be performed.
          char operations_menu[350];
          read(sockfd,operations_menu,sizeof(operations_menu));
          printf("%s",operations_menu);
          int op_menu;
          scanf("%d",&op_menu);
          op_menu=htonl(op_menu);
          write(sockfd,&op_menu,sizeof(op_menu));

          op_menu=ntohl(op_menu);
          printf("OP_MENU=%d\n",op_menu);

          //Retrieving teh corresponding o/p for the above choosen operation.
          if(op_menu==1 || op_menu==2 || op_menu==3 || op_menu==4)
          {
            //print the corresponding msg
            if(op_menu==1)
            {
              //add_ret==-1  then username duplication
              //add_ret==-2 then account number duplication
              int add_ret=addAccount();
              if(add_ret==-1)
              {
                printf("Username already Taken\n");
              }
              else if(add_ret==-2)
              {
                printf("Account Number already Taken\n");
              }
              else{
                printf("Account Created Successfully\n");
              }

            }

            else if(op_menu==2)
            {

            }

          else if(op_menu==3)
            {

            }
            else if(op_menu==4)
            {
                printf("Enter the Type of Account to be searched for:\n1. NORMAL\n2. JOINT\n");
                int search_type;
                scanf("%d",&search_type);
                if(search_type==1)    //NORMAL account
                {
                    int normal_acc_no;
                    struct Customer_Details user_normal;
                    printf("Enter the Account number to be searched for\n");
                    scanf("%d",&normal_acc_no);

                    int ret_search=search_normal(normal_acc_no,&user_normal);

                    if(ret_search==1)
                    {
                        printf("Username: %s\n",user_normal.Username);
                        printf("User Status: %d\n",user_normal.U_Status);
                        printf("Account number: %d\n",user_normal.Account.Account_Number);
                        printf("Balance: %lf\n",user_normal.Account.Balance);
                        printf("Account Status: %d\n",user_normal.Account.Acc_status);
                        printf("Account Type: %d\n",user_normal.Account.Acc_Type);
                    }
                    else if(ret_search==-1)
                    {
                      printf("No Such Account exists\n");
                    }


                }
                else if(search_type==2) //joint account
                {
                        //search_joint();
                }
            }

          }
          else{
            char invalid_oper[100];
            read(sockfd,invalid_oper,sizeof(invalid_oper));
            printf("%s",invalid_oper);
            close(sockfd);
            exit(1);
          }



return 0;
}
