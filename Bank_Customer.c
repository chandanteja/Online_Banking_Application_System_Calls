#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>


//==============================
//User header files
//================================
//#include  "User_Details.h"
#include "Customer_Functions.h"
#define ARRAY_SIZE 1024



int main(int argc,char* argv[])
{
  printf("Came to customer file\n");
      int connfd_customer=atoi(argv[1]);
      char acc_login_type[150]="How do you like to login\n1. NORMAL\n2. JOINT\n";
      write(connfd_customer,acc_login_type,sizeof(acc_login_type));
      int acc_login_choice;
      read(connfd_customer,&acc_login_choice,sizeof(acc_login_choice));
      acc_login_choice=ntohl(acc_login_choice);
      printf("User selected choice: %d\n",acc_login_choice);

      char username_cust[1024];
      char passwd_cust[1024];
      int fd_lgn_cust;   //file descriptor for authenticating admin

      char username_password_prompt[250]="\n==================Enter Username and Password.Press Enter to continue==================\n";
      char username_prompt[100]="Username: ";
      char password_prompt[100]="Password: ";
      char response_ch;
      write(connfd_customer,username_password_prompt,sizeof(username_password_prompt));
      read(connfd_customer,&response_ch,1);

      if(response_ch=='\n')
      {
        write(connfd_customer,username_prompt,sizeof(username_prompt));
        read(connfd_customer,username_cust,sizeof(username_cust));

        write(connfd_customer,password_prompt,sizeof(password_prompt));
        read(connfd_customer,passwd_cust,sizeof(passwd_cust));


        struct flock lock_login_cust;

        struct Customer_Details credentials_cust;
        int n_cust;  //keep track of how many characters read has read from file

        lock_login_cust.l_type=F_RDLCK;    //Read lock
        lock_login_cust.l_whence=SEEK_SET;
        lock_login_cust.l_start=0;
        lock_login_cust.l_len=0; //entire file is locked from start
        lock_login_cust.l_pid=getpid();
        fd_lgn_cust=open("customer.txt",O_RDONLY);
        perror("open");

        fcntl(fd_lgn_cust,F_SETLKW,&lock_login_cust);     // when 2 users access this functionality at same time as one will be locked, the other can't even see,
        //the below prompts too and welcome messages too.
        perror("fcntl");

        int flagp=0,flagu=0,rcrd_len=0,record_len=0;

        n_cust=read(fd_lgn_cust,&credentials_cust,sizeof(credentials_cust));
        rcrd_len=sizeof(credentials_cust);
        record_len=sizeof(credentials_cust);    //this is just for updating the record len in every intration linearly

        char Auth_success[100]="Authenticated Sucessfully\n";
        char Auth_fail[100]="Username OR Password didn't match. Please Try again :(\n";
        char close_msg[100]="===============Closed the connection to server.===============\n";

        while(n_cust!=0)
        {
          //  printf("Inside verification loop\n");
            if((strcmp(credentials_cust.Username,username_cust)==0)&& strcmp(credentials_cust.Password,passwd_cust)==0)
            {
                        printf("\nBoth username and Passwd matched\n");
                        flagu=1;
                        flagp=1;
                        break;
            }

            else
            {   //we will unlock the previous record for which the username comparision failed.

                lock_login_cust.l_type=F_RDLCK;    //Read lock
                lock_login_cust.l_whence=SEEK_SET;
                lock_login_cust.l_start=rcrd_len+1;
                lock_login_cust.l_len=0; //entire file is locked from start
                lock_login_cust.l_pid=getpid();
                rcrd_len=rcrd_len+record_len;
                fcntl(fd_lgn_cust,F_SETLKW,&lock_login_cust);
                printf("\nUsername or Password din't match\n");

            }

            n_cust=read(fd_lgn_cust,&credentials_cust,sizeof(credentials_cust));

        }

            flagu=htonl(flagu);
            flagp=htonl(flagp);
            write(connfd_customer,&flagu,sizeof(flagu));
            write(connfd_customer,&flagp,sizeof(flagp));

            flagu=ntohl(flagu);
            flagp=ntohl(flagp);


            if(flagu==1 && flagp==1)
            {
              write(connfd_customer,Auth_success,sizeof(Auth_success));
              lock_login_cust.l_type=F_UNLCK;
              fcntl(fd_lgn_cust,F_SETLKW,&lock_login_cust);
              close(fd_lgn_cust);
              char welcome_user1[100]="\n==========Welcome User ";
              char welcome_user2[100]="==========\n";
              write(connfd_customer,welcome_user1,sizeof(welcome_user1));
              write(connfd_customer,welcome_user2,sizeof(welcome_user2));

            }
            else
            {
                  write(connfd_customer,Auth_fail,sizeof(Auth_fail));
                  write(connfd_customer,close_msg,sizeof(close_msg));
                  close(connfd_customer);
                  exit(EXIT_FAILURE);
            }

            char options_menu[400]="\nChoose from the below availabe options.\n1. Deposit\n2. Withdraw\n3. Check Balance\n4. View Details\n5. Password Change\n6. Press Anyother other number to exit\n";

            write(connfd_customer,options_menu,sizeof(options_menu));
            int op_choice;
            read(connfd_customer,&op_choice,sizeof(op_choice));
            op_choice=ntohl(op_choice);
            printf("customer choice: %d\n",op_choice);

            switch(op_choice)
            {
              case 1:   //Deposit
                        if(acc_login_choice==1)   //NORMAL
                        {
                          printf("Collecting data for deposit\n");
                          int acc_no;
                          double amnt;
                          read(connfd_customer,&acc_no,sizeof(acc_no));
                          acc_no=ntohl(acc_no);
                          printf("acc_no: %d\n",acc_no);
                          read(connfd_customer,&amnt,sizeof(amnt));
                          printf("amount= %lf\n",amnt);
                          int dep_ret=depositNormal(acc_no,amnt);
                          if(dep_ret==1)
                          {
                              char success[150]="Amount deposited successfully\n";
                              struct Customer_Details dep;
                              int fd=open("customer.txt",O_CREAT|O_RDWR,0666);
                              struct flock lock;

                              lock.l_type=F_RDLCK;    //Read lock
                              lock.l_whence=SEEK_SET;
                              lock.l_start=0;
                              lock.l_len=0; //entire file is locked from start
                              lock.l_pid=getpid();

                              fcntl(fd,F_SETLKW,&lock);

                              while(read(fd,&dep,sizeof(dep))>0)
                              {
                                if(acc_no==dep.Account.Account_Number)
                                {
                                  write(connfd_customer,success,sizeof(success));
                                  write(connfd_customer,&dep,sizeof(dep));

                                }
                              }
                          }
                          else if(dep_ret==-1)
                          {
                                char failure[150]="Failed to deposit amount. Check the account number you entered\n";
                                write(connfd_customer,failure,sizeof(failure));
                          }
                      }
                      else if(acc_login_choice==2)  //joint acc
                      {

                      }

                    break;
              case 2:     //withdraw
                          if(acc_login_choice==1)   //NORMAL
                          {
                                printf("Collecting data for withdrawing\n");
                                int acc_no;
                                double amnt;
                                read(connfd_customer,&acc_no,sizeof(acc_no));
                                acc_no=ntohl(acc_no);
                                printf("acc_no: %d\n",acc_no);
                                read(connfd_customer,&amnt,sizeof(amnt));
                                printf("amount= %lf\n",amnt);
                                int with_ret=withdrawNormal(acc_no,amnt);
                                if(with_ret==1)
                                {
                                    char success[150]="Amount debited successfully\n";
                                    struct Customer_Details with;
                                    int fd=open("customer.txt",O_CREAT|O_RDWR,0666);
                                    struct flock lock;

                                    lock.l_type=F_RDLCK;    //Read lock
                                    lock.l_whence=SEEK_SET;
                                    lock.l_start=0;
                                    lock.l_len=0; //entire file is locked from start
                                    lock.l_pid=getpid();

                                    fcntl(fd,F_SETLKW,&lock);

                                    while(read(fd,&with,sizeof(with))>0)
                                    {
                                      if(acc_no==with.Account.Account_Number)
                                      {
                                        write(connfd_customer,success,sizeof(success));
                                        write(connfd_customer,&with,sizeof(with));

                                      }
                                    }
                                }
                                else if(with_ret==-1)
                                {
                                      char failure[150]="Failed to Withdraw amount. Check the account number you entered\n";
                                      write(connfd_customer,failure,sizeof(failure));
                                }
                                else if(with_ret==-2)
                                {
                                  char failure[150]="Insufficient Balance.\n";
                                  write(connfd_customer,failure,sizeof(failure));
                                }
                          }
                          else if(acc_login_choice==2)
                          {

                          }
                    break;
              case 3:   //check balance
                        if(acc_login_choice==1)   //NORMAL
                        {
                              printf("Collecting data for Balance enquiry\n");
                              int acc_no;
                            //  double amnt;
                              read(connfd_customer,&acc_no,sizeof(acc_no));
                              acc_no=ntohl(acc_no);
                              printf("acc_no: %d\n",acc_no);
                              //read(connfd_customer,&amnt,sizeof(amnt));
                              //printf("amount= %lf\n",amnt);
                              int enq_ret=checkBalanceNormal(acc_no);
                              if(enq_ret==1)    //successfully goot balance details
                              {
                                  char success[150]="Balance Retrieved Successfully\n";
                                  struct Customer_Details enq;
                                  int fd=open("customer.txt",O_CREAT|O_RDWR,0666);
                                  struct flock lock;

                                  lock.l_type=F_RDLCK;    //Read lock
                                  lock.l_whence=SEEK_SET;
                                  lock.l_start=0;
                                  lock.l_len=0; //entire file is locked from start
                                  lock.l_pid=getpid();

                                  fcntl(fd,F_SETLKW,&lock);

                                  while(read(fd,&enq,sizeof(enq))>0)
                                  {
                                        if(acc_no==enq.Account.Account_Number)
                                        {
                                          write(connfd_customer,success,sizeof(success));
                                          write(connfd_customer,&enq,sizeof(enq));

                                        }
                                  }
                              }
                              else if(enq_ret==-1)
                              {
                                    char failure[150]="Failed to Retrieve Balance. Check the account number you entered\n";
                                    write(connfd_customer,failure,sizeof(failure));
                              }
                        }
                        else if(acc_login_choice==2)  //Joint
                        {

                        }

                    break;
              case 4:     //View Details i.eprint all the users transactions
                    break;
              case 5: //password change
                    if(acc_login_choice==1)   //NORMAL
                    {
                          printf("password change functionality invoked\n");
                          int acc_no;
                          char new_password[ARRAY_SIZE];
                          read(connfd_customer,&acc_no,sizeof(acc_no));
                          read(connfd_customer,new_password,sizeof(new_password));
                          acc_no=ntohl(acc_no);
                          printf("acc_no: %d\n",acc_no);

                          int pass_ret=passwordChangeNormal(acc_no,new_password);
                          if(pass_ret==1)
                          {
                              char success[150]="Password changed successfully\n";
                              write(connfd_customer,success,sizeof(success));

                          }
                          else if(pass_ret==-1)
                          {
                                char failure[150]="Failed to change password. Check the account number you entered\n";
                                write(connfd_customer,failure,sizeof(failure));
                          }
                    }
                    else if(acc_login_choice==2)
                    {

                    }

                    break;
              default:
                      write(connfd_customer,"Invalid option selected. Closed the connection\n",sizeof("Invalid option selected. Closed the connection\n"));
                      close(connfd_customer);
                      exit(1);

            }


      }
}
