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
#define ARRAY_SIZE 1024

#define SERV_PORT 5062

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
        role_cli=ntohl(role_cli);
        printf("completed ths\n");

        //===========================================================
            //Bank employee data
       //===========================================================
            char username_and_password_prompt[250];
            char username_prompt[100]="Username: ";
            char password_prompt[100]="Password: ";
            char username[ARRAY_SIZE];
            char password[ARRAY_SIZE];

if(role_cli==1)   //employee
{

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

//Retrieving the corresponding o/p for the above choosen operation.
if(op_menu==1 || op_menu==2 || op_menu==3 || op_menu==4)
{
  //print the corresponding msg
  if(op_menu==1)      //add account based on type of account either normal or joint
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

  else if(op_menu==2)     //Delete Account
  {
    printf("Enter the account number to be deleted\n");
    int acc_no;
    scanf("%d",&acc_no);
    int del_ret=deleteAcc(acc_no);
    if(del_ret==1)
    {
      struct Customer_Details cust;
      int fd=open("customer.txt",O_CREAT|O_RDWR,0666);
      while(read(fd, &cust, sizeof(cust))>0)
      {
          if(cust.Account.Account_Number==acc_no)
          {
            printf("============ Account Details ============\n");
            printf("Username: %s\n",cust.Username);
            printf("Account Number: %d\n",cust.Account.Account_Number);
            printf("Balance: %lf\n",cust.Account.Balance);
            if(cust.Account.Acc_status==INACTIVE_ACC && cust.U_Status==INACTIVE_PRIMARY)
            {
              printf("Acount is deleted successfully\n");
            }


          }
      }
      //printf("Your Normal Account deleted successfully\n");
    }
    else if(del_ret==2)
    {
      struct Joint_Account jnt_user;
      int fd=open("joint_customer.txt",O_CREAT|O_RDWR,0666);
      while(read(fd, &jnt_user, sizeof(jnt_user))>0)
      {
          if(jnt_user.Primary.Account.Account_Number==acc_no)
          {
            printf("============ Account Details ============\n");
            printf("Primary Username: %s\n",jnt_user.Primary.Username);
            printf("Secondary username: %s\n",jnt_user.Secondary.Secondary_Username);
            printf("Account Number: %d\n",jnt_user.Primary.Account.Account_Number);
            printf("Balance: %lf\n",jnt_user.Primary.Account.Balance);
            printf("Primary User Account status: %d\n",jnt_user.Primary.U_Status);
            printf("Secondary User Account Status: %d\n",jnt_user.Secondary.Sec_Status);
            printf("Account status: %d\n",jnt_user.Primary.Account.Acc_status);
          }
      }
       printf("Your Joint Account deleted successfully\n");
    }
    else if(del_ret==-1)
      printf("Your Entered non-existent account number\n");

  }

else if(op_menu==3)   //modify
  {
      printf("As of now only Modification of username is allowed\n");
      printf("Enter the account number for which u would like to modify username: ");
      int acc_no;
      scanf("%d",&acc_no);
      printf("Enter your password for authentication: ");
      char password[ARRAY_SIZE];
      scanf(" %[^\n]",password);
      int modify_ret=modifyAccount(acc_no,password);


      if(modify_ret==1)
      {
          printf("=====Username is updated successfully. Below are the details=====\n");
          int fd=open("customer.txt",O_CREAT|O_RDWR,0666);
          struct Customer_Details modify_user;
          while(read(fd,&modify_user,sizeof(modify_user))>0)
          {
            if(modify_user.Account.Account_Number==acc_no)
            {

                printf("New Username: %s\n",modify_user.Username);
                printf("Account number: %d\n",modify_user.Account.Account_Number);
                printf("Account status: %d\n",modify_user.Account.Acc_status);
                printf("User status: %d\n",modify_user.U_Status);

            }
          }
      }
      else if(modify_ret==2)    //joint account modification
      {
            printf("=====Username is updated successfully. Below are the details=====\n");
            int fd=open("joint_customer.txt",O_CREAT|O_RDWR,0666);
            struct Joint_Account user;
            while(read(fd,&user,sizeof(user))>0)
            {
              if(user.Primary.Account.Account_Number==acc_no)
              {
                  printf("Username Primary: %s\n",user.Primary.Username);
                  printf("Username Secondary: %s\n",user.Secondary.Secondary_Username);
                  printf("Account number: %d\n",user.Primary.Account.Account_Number);
                  printf("Account status: %d\n",user.Primary.Account.Acc_status);
                  printf("User status primary: %d\n",user.Primary.U_Status);
                  printf("User status secondary: %d\n",user.Secondary.Sec_Status);
              }
            }
      }
      else if(modify_ret==-1)
      {
        printf("Failed to modify. The account number you entered doesn't exist\n");

      }


  }
  else if(op_menu==4)     //searching for account based on the type of account
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

          if(ret_search==1)   //success in search
          {
              printf("Username: %s\n",user_normal.Username);
              printf("User Status: %d\n",user_normal.U_Status);
              printf("Account number: %d\n",user_normal.Account.Account_Number);
              printf("Balance: %lf\n",user_normal.Account.Balance);
              printf("Account Status: %d\n",user_normal.Account.Acc_status);
              printf("Account Type: %d\n",user_normal.Account.Acc_Type);
          }
          else if(ret_search==-1)   //search failed
          {
            //printf("Account number: %d\n",user_normal.Account.Account_Number);
            //printf("User Status: %d\n",user_normal.U_Status);
            //printf("Account Status: %d\n",user_normal.Account.Acc_status);


            printf("No Such Account exists\n");

          }


      }

      else if(search_type==2) //joint account
      {

                int joint_acc_no;
                struct Joint_Account user_joint;
                printf("Enter the Joint account number to be searched for\n");
                scanf("%d",&joint_acc_no);
                int joint_search_ret=search_joint(joint_acc_no,&user_joint);

                if(joint_search_ret==1) //success in search
                {
                        //print all the details of joint account.
                }

                else if(joint_search_ret==-1)   //search failed
                {
                  printf("No Such Account exists\n");
                }

      }

      else{
        printf("Sorry! We don't have this type of account\n");
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
}
else if(role_cli==2)
{
  printf("This is customer role\n");
  char acc_login_type[150];
  read(sockfd,acc_login_type,sizeof(acc_login_type));
  printf("%s",acc_login_type);
  int acc_login_choice;
  scanf("%d",&acc_login_choice);
  acc_login_choice=htonl(acc_login_choice);
  write(sockfd,&acc_login_choice,sizeof(acc_login_choice));
  acc_login_choice=ntohl(acc_login_choice);
  printf("selected choice: %d\n",acc_login_choice);

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

    char options_menu[400];
    read(sockfd,options_menu,sizeof(options_menu));
    printf("%s",options_menu);
    int op_menu;
    scanf("%d",&op_menu);
    op_menu=htonl(op_menu);
    write(sockfd,&op_menu,sizeof(op_menu));

    op_menu=ntohl(op_menu);
    printf("OP_MENU=%d\n",op_menu);

//ask account number for every operation by client and send it to server, then server will find the account type and invoke paricular function invloke a particular function and then reply to client.



}

else
{
    printf("We don't have this role. Select only from the mentioned roles.\n");
}



return 0;
}
