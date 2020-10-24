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
#include  "User_Details.h"


#define EMP_AUTH_SUCCESS 1
#define EMP_AUTH_FAILED 0
#define EMP_SUCCESS 1
#define EMP_FAILURE 0




int main(int argc,char* argv[])
{
int connfd_from_server=atoi(argv[1]);
//printf("connfd_from_server=%d\n",connfd_from_server);
char username_emp[1024];
char passwd_emp[1024];
int fd_lgn_emp;   //file descriptor for authenticating admin

char username_password_prompt[250]="\n==================Enter Username and Password.Press Enter to continue==================\n";
char username_prompt[100]="Username: ";
char password_prompt[100]="Password: ";
char response_ch;
//printf("\n==================Enter Username and Password==================\n");
/*printf("Username: ");
scanf(" %[^\n]",username_emp);
printf("Password: ");
scanf(" %[^\n]",passwd_emp);*/

write(connfd_from_server,username_password_prompt,sizeof(username_password_prompt));
read(connfd_from_server,&response_ch,1);

if(response_ch=='\n')
{
  write(connfd_from_server,username_prompt,sizeof(username_prompt));
  read(connfd_from_server,username_emp,sizeof(username_emp));

  write(connfd_from_server,password_prompt,sizeof(password_prompt));
  read(connfd_from_server,passwd_emp,sizeof(passwd_emp));


  struct flock lock_login_emp;

  struct Employee_Details credentials_emp;
  int n_emp;  //keep track of how many characters read has read from file

  lock_login_emp.l_type=F_RDLCK;    //Read lock
  lock_login_emp.l_whence=SEEK_SET;
  lock_login_emp.l_start=0;
  lock_login_emp.l_len=0; //entire file is locked from start
  lock_login_emp.l_pid=getpid();
  fd_lgn_emp=open("Emp_login_details.txt",O_RDONLY);
  perror("open");

  fcntl(fd_lgn_emp,F_SETLKW,&lock_login_emp);
  perror("fcntl");

  int flagp=0,flagu=0,rcrd_len=0,record_len=0;

  n_emp=read(fd_lgn_emp,&credentials_emp,sizeof(credentials_emp));
  rcrd_len=sizeof(credentials_emp);
  record_len=sizeof(credentials_emp);    //this is just for updating the record len in every intration linearly

  char Auth_success[100]="Authenticated Sucessfully\n";
  char Auth_fail[100]="Username OR Password didn't match. Please Try again :(\n";
  char close_msg[100]="===============Closed the connection to server.===============\n";

  while(n_emp!=0)
  {
    //  printf("Inside verification loop\n");
      if((strcmp(credentials_emp.Employee_Username,username_emp)==0)&& strcmp(credentials_emp.Employee_Password,passwd_emp)==0)
      {
                  printf("\nBoth username and Passwd matched\n");
                  flagu=1;
                  flagp=1;
                  break;
      }

      else
      {   //we will unlock the previous record for which the username comparision failed.

          lock_login_emp.l_type=F_RDLCK;    //Read lock
          lock_login_emp.l_whence=SEEK_SET;
          lock_login_emp.l_start=rcrd_len+1;
          lock_login_emp.l_len=0; //entire file is locked from start
          lock_login_emp.l_pid=getpid();
          rcrd_len=rcrd_len+record_len;
          fcntl(fd_lgn_emp,F_SETLKW,&lock_login_emp);
          printf("\nUsername or Password din't match\n");

      }

      n_emp=read(fd_lgn_emp,&credentials_emp,sizeof(credentials_emp));

  }

      flagu=htonl(flagu);
      flagp=htonl(flagp);
      write(connfd_from_server,&flagu,sizeof(flagu));
      write(connfd_from_server,&flagp,sizeof(flagp));

      flagu=ntohl(flagu);
      flagp=ntohl(flagp);


      if(flagu==1 && flagp==1)
      {
        write(connfd_from_server,Auth_success,sizeof(Auth_success));
        lock_login_emp.l_type=F_UNLCK;
        fcntl(fd_lgn_emp,F_SETLKW,&lock_login_emp);
        close(fd_lgn_emp);
        char welcome_user1[100]="\n==========Welcome User ";
        char welcome_user2[100]="==========\n";
        write(connfd_from_server,welcome_user1,sizeof(welcome_user1));
        write(connfd_from_server,welcome_user2,sizeof(welcome_user2));

      }
      else
      {
            write(connfd_from_server,Auth_fail,sizeof(Auth_fail));
            write(connfd_from_server,close_msg,sizeof(close_msg));
            close(connfd_from_server);
            exit(EXIT_FAILURE);
      }

      char operations_menu[350]="\nChoose what you want to do from below menu\n1. Add an account\n2. Delete an account\n3. Modify an account\n4. Search\n5. Press Anyother other number to exit\n";
      write(connfd_from_server,operations_menu,sizeof(operations_menu));
      int op_choice;
      read(connfd_from_server,&op_choice,sizeof(op_choice));
      op_choice=ntohl(op_choice);
      //printf("Employee selected %d operation\n",op_choice);

      switch(op_choice)
      {
        case 1:
              break;
        case 2:
              break;
        case 3:
              break;
        case 4:
              break;
        default:
                write(connfd_from_server,"Invalid option selected. Closed the connection\n",sizeof("Invalid option selected. Closed the connection\n"));
                close(connfd_from_server);
                exit(1);

      }


}

return 0;
}
