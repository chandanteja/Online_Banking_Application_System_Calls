#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>


#include  "User_Details.h"
#define ARRAY_SIZE 1024


int depositNormal(int acc_no,double amnt)
{
    printf("inside deposit fn\n");
    int fd=open("customer.txt",O_CREAT|O_RDWR,0666);
    struct flock lock;
    struct Customer_Details dep;

    lock.l_type=F_RDLCK;    //Read lock
    lock.l_whence=SEEK_SET;
    lock.l_start=0;
    lock.l_len=0; //entire file is locked from start
    lock.l_pid=getpid();

    fcntl(fd,F_SETLKW,&lock);   //locked

    int record_len=sizeof(dep);
    printf("Inside dep fn, acc_no: %d\n",acc_no);
    while(read(fd,&dep,sizeof(dep))>0)
    {
      if(acc_no==dep.Account.Account_Number)
      {
          //converting readlock on whole file to write lock on that particular record
          lseek(fd,-sizeof(dep),SEEK_CUR);
          lock.l_type=F_WRLCK;    //Read lock
          lock.l_whence=SEEK_CUR;
          lock.l_start= 0;
          lock.l_len= sizeof(dep); //entire file is locked from start
          lock.l_pid=getpid();
          fcntl(fd,F_SETLKW,&lock);

          struct Transaction_Details trans;
          trans.Account_Number=dep.Account.Account_Number;
          trans.Prev_Balance=dep.Account.Balance;
          dep.Account.Balance=dep.Account.Balance+amnt;

          write(fd,&dep,sizeof(dep));
          printf("Updated the balance in account\n");
          lock.l_type=F_UNLCK  ;
          fcntl(fd,F_SETLKW,&lock);   //unlocked the file after writing to it

          int fd_trans=open("transactions.txt",O_CREAT|O_RDWR|O_APPEND,0666);

          trans.Curr_Balance=dep.Account.Balance;
          write(fd_trans,&trans,sizeof(trans));
          return 1;   //deposit success


      }
      else  //reccord didn't find. Remove this record from lock
      {

        lock.l_whence=SEEK_SET;
        lock.l_start=record_len+1;
        lock.l_len=0; //entire file is locked from start
        record_len=record_len+sizeof(dep);
        fcntl(fd,F_SETLKW,&lock);     //removed the record which failed comparision from lock.

      }
    }
    return -1; //failure in depositing


}

int depositJoint(int acc_no,double amnt)
{
  printf("inside joint deposit fn\n");
  int fd=open("joint_accounts.txt",O_CREAT|O_RDWR,0666);
  struct flock lock;
  struct Joint_Account dep;

  lock.l_type=F_RDLCK;    //Read lock
  lock.l_whence=SEEK_SET;
  lock.l_start=0;
  lock.l_len=0; //entire file is locked from start
  lock.l_pid=getpid();

  fcntl(fd,F_SETLKW,&lock);   //locked

  int record_len=sizeof(dep);
  printf("Inside joint dep fn, acc_no: %d\n",acc_no);
  while(read(fd,&dep,sizeof(dep))>0)
  {
    if(acc_no==dep.Primary.Account.Account_Number)
    {
        //converting readlock on whole file to write lock on that particular record
        lseek(fd,-sizeof(dep),SEEK_CUR);
        lock.l_type=F_WRLCK;    //Read lock
        lock.l_whence=SEEK_CUR;
        lock.l_start= 0;
        lock.l_len= sizeof(dep); //entire file is locked from start
        lock.l_pid=getpid();
        fcntl(fd,F_SETLKW,&lock);

        struct Transaction_Details trans;
        trans.Account_Number=dep.Primary.Account.Account_Number;
        trans.Prev_Balance=dep.Primary.Account.Balance;
        dep.Primary.Account.Balance=dep.Primary.Account.Balance+amnt;

        write(fd,&dep,sizeof(dep));
        printf("Updated the balance in account\n");
        lock.l_type=F_UNLCK  ;
        fcntl(fd,F_SETLKW,&lock);   //unlocked the file after writing to it

        int fd_trans=open("transactions.txt",O_CREAT|O_RDWR|O_APPEND,0666);

        trans.Curr_Balance=dep.Primary.Account.Balance;
        write(fd_trans,&trans,sizeof(trans));
        return 1;   //deposit success


    }
    else  //reccord didn't find. Remove this record from lock
    {

      lock.l_whence=SEEK_SET;
      lock.l_start=record_len+1;
      lock.l_len=0; //entire file is locked from start
      record_len=record_len+sizeof(dep);
      fcntl(fd,F_SETLKW,&lock);     //removed the record which failed comparision from lock.

    }
  }
  return -1; //failure in depositing

}

int withdrawNormal(int acc_no,double amnt)
{
  printf("inside withdraw function fn\n");
  int fd=open("customer.txt",O_CREAT|O_RDWR,0666);
  struct flock lock;
  struct Customer_Details with;

  lock.l_type=F_RDLCK;    //Read lock
  lock.l_whence=SEEK_SET;
  lock.l_start=0;
  lock.l_len=0; //entire file is locked from start
  lock.l_pid=getpid();

  fcntl(fd,F_SETLKW,&lock);   //locked

  int record_len=sizeof(with);
  printf("Inside withdraw fn, acc_no: %d\n",acc_no);
  while(read(fd,&with,sizeof(with))>0)
  {
    if(acc_no==with.Account.Account_Number)
    {
        //converting readlock on whole file to write lock on that particular record
        lseek(fd,-sizeof(with),SEEK_CUR);
        lock.l_type=F_WRLCK;    //Read lock
        lock.l_whence=SEEK_CUR;
        lock.l_start= 0;
        lock.l_len= sizeof(with); //entire file is locked from start
        lock.l_pid=getpid();
        fcntl(fd,F_SETLKW,&lock);

        struct Transaction_Details trans;
        trans.Account_Number=with.Account.Account_Number;
        trans.Prev_Balance=with.Account.Balance;

        if(with.Account.Balance >= amnt)   //amount trying to withdraw is < available balance
            with.Account.Balance=with.Account.Balance-amnt;
        else
            return -2;    //insufficient balance
        write(fd,&with,sizeof(with));
        printf("Updated the balance in account\n");
        lock.l_type=F_UNLCK  ;
        fcntl(fd,F_SETLKW,&lock);   //unlocked the file after writing to it

        int fd_trans=open("transactions.txt",O_CREAT|O_RDWR|O_APPEND,0666);

        trans.Curr_Balance=with.Account.Balance;
        write(fd_trans,&trans,sizeof(trans));
        return 1;   //withdraw success


    }
    else  //reccord didn't find. Remove this record from lock
    {

      lock.l_whence=SEEK_SET;
      lock.l_start=record_len+1;
      lock.l_len=0; //entire file is locked from start
      record_len=record_len+sizeof(with);
      fcntl(fd,F_SETLKW,&lock);     //removed the record which failed comparision from lock.

    }
  }
  return -1; //failed to withdraw

}

int withdrawJoint(int acc_no,double amnt)
{
  printf("inside joint withdraw fn\n");
  int fd=open("joint_accounts.txt",O_CREAT|O_RDWR,0666);
  struct flock lock;
  struct Joint_Account with;

  lock.l_type=F_RDLCK;    //Read lock
  lock.l_whence=SEEK_SET;
  lock.l_start=0;
  lock.l_len=0; //entire file is locked from start
  lock.l_pid=getpid();

  fcntl(fd,F_SETLKW,&lock);   //locked

  int record_len=sizeof(with);
  printf("Inside joint withdraw fn, acc_no: %d\n",acc_no);
  while(read(fd,&with,sizeof(with))>0)
  {
    if(acc_no==with.Primary.Account.Account_Number)
    {
        //converting readlock on whole file to write lock on that particular record
        lseek(fd,-sizeof(with),SEEK_CUR);
        lock.l_type=F_WRLCK;    //Read lock
        lock.l_whence=SEEK_CUR;
        lock.l_start= 0;
        lock.l_len= sizeof(with); //entire file is locked from start
        lock.l_pid=getpid();
        fcntl(fd,F_SETLKW,&lock);

        struct Transaction_Details trans;
        trans.Account_Number=with.Primary.Account.Account_Number;
        trans.Prev_Balance=with.Primary.Account.Balance;
        if(with.Primary.Account.Balance >= amnt)   //amount trying to withdraw is < available balance
              with.Primary.Account.Balance=with.Primary.Account.Balance-amnt;
        else
             return -2;    //insufficient balance

        write(fd,&with,sizeof(with));
        printf("Updated the balance in account\n");
        lock.l_type=F_UNLCK  ;
        fcntl(fd,F_SETLKW,&lock);   //unlocked the file after writing to it

        int fd_trans=open("transactions.txt",O_CREAT|O_RDWR|O_APPEND,0666);

        trans.Curr_Balance=with.Primary.Account.Balance;
        write(fd_trans,&trans,sizeof(trans));
        return 1;   //withdrawl success


    }
    else  //reccord didn't find. Remove this record from lock
    {

      lock.l_whence=SEEK_SET;
      lock.l_start=record_len+1;
      lock.l_len=0; //entire file is locked from start
      record_len=record_len+sizeof(with);
      fcntl(fd,F_SETLKW,&lock);     //removed the record which failed comparision from lock.

    }
  }
  return -1; //failure in withdrawl

}

int checkBalanceNormal(int acc_no)
{
    printf("inside balance check normal fn\n");
    int fd=open("customer.txt",O_CREAT|O_RDWR,0666);
    struct flock lock;
    struct Customer_Details enq;

    lock.l_type=F_RDLCK;    //Read lock
    lock.l_whence=SEEK_SET;
    lock.l_start=0;
    lock.l_len=0; //entire file is locked from start
    lock.l_pid=getpid();

    fcntl(fd,F_SETLKW,&lock);   //locked

    int record_len=sizeof(enq);
    printf("Inside enquiry normal fn, acc_no: %d\n",acc_no);
    while(read(fd,&enq,sizeof(enq))>0)
    {
      if(acc_no==enq.Account.Account_Number)
      {
          //converting readlock on whole file to write lock on that particular record
          lseek(fd,-sizeof(enq),SEEK_CUR);
          lock.l_type=F_RDLCK;    //Read lock
          lock.l_whence=SEEK_CUR;
          lock.l_start= 0;
          lock.l_len= sizeof(enq); //entire file is locked from start
          lock.l_pid=getpid();
          fcntl(fd,F_SETLKW,&lock);

          struct Transaction_Details trans;
          trans.Account_Number=enq.Account.Account_Number;
          trans.Prev_Balance=enq.Account.Balance;

          write(fd,&enq,sizeof(enq));
          lock.l_type=F_UNLCK  ;
          fcntl(fd,F_SETLKW,&lock);   //unlocked the file after writing to it

          int fd_trans=open("transactions.txt",O_CREAT|O_RDWR|O_APPEND,0666);

          trans.Curr_Balance=enq.Account.Balance;
          write(fd_trans,&trans,sizeof(trans));
          return 1;   //balance enquiry success


      }
      else  //reccord didn't find. Remove this record from lock
      {

        lock.l_whence=SEEK_SET;
        lock.l_start=record_len+1;
        lock.l_len=0; //entire file is locked from start
        record_len=record_len+sizeof(enq);
        fcntl(fd,F_SETLKW,&lock);     //removed the record which failed comparision from lock.

      }
    }
    return -1; //failure in enquiry

}

int checkBalanceJoint(int acc_no)
{
  printf("inside balance check joint fn\n");
  int fd=open("joint_accounts.txt",O_CREAT|O_RDWR,0666);
  struct flock lock;
  struct Joint_Account enq;

  lock.l_type=F_RDLCK;    //Read lock
  lock.l_whence=SEEK_SET;
  lock.l_start=0;
  lock.l_len=0; //entire file is locked from start
  lock.l_pid=getpid();

  fcntl(fd,F_SETLKW,&lock);   //locked

  int record_len=sizeof(enq);
  printf("Inside enquiry joint fn, acc_no: %d\n",acc_no);
  while(read(fd,&enq,sizeof(enq))>0)
  {
    if(acc_no==enq.Primary.Account.Account_Number)
    {
        //converting readlock on whole file to write lock on that particular record
        lseek(fd,-sizeof(enq),SEEK_CUR);
        lock.l_type=F_RDLCK;    //Read lock
        lock.l_whence=SEEK_CUR;
        lock.l_start= 0;
        lock.l_len= sizeof(enq); //entire file is locked from start
        lock.l_pid=getpid();
        fcntl(fd,F_SETLKW,&lock);

        struct Transaction_Details trans;
        trans.Account_Number=enq.Primary.Account.Account_Number;
        trans.Prev_Balance=enq.Primary.Account.Balance;

        write(fd,&enq,sizeof(enq));
        lock.l_type=F_UNLCK  ;
        fcntl(fd,F_SETLKW,&lock);   //unlocked the file after writing to it

        int fd_trans=open("transactions.txt",O_CREAT|O_RDWR|O_APPEND,0666);

        trans.Curr_Balance=enq.Primary.Account.Balance;
        write(fd_trans,&trans,sizeof(trans));
        return 1;   //balance enquiry success


    }
    else  //reccord didn't find. Remove this record from lock
    {

      lock.l_whence=SEEK_SET;
      lock.l_start=record_len+1;
      lock.l_len=0; //entire file is locked from start
      record_len=record_len+sizeof(enq);
      fcntl(fd,F_SETLKW,&lock);     //removed the record which failed comparision from lock.

    }
  }
  return -1; //failure in enquiry

}


int viewDetailsNormal(int acc_no)
{
    //need to design in a way different from above other functionalities as we need to print all transaction details
}

int viewDetailsJoint(int acc_no)
{

}

int passwordChangeNormal(int acc_no, char new_password[])
{

  int fd=open("customer.txt",O_CREAT|O_RDWR,0666);
  struct flock lock;
  struct Customer_Details pass;

  lock.l_type=F_RDLCK;    //Read lock
  lock.l_whence=SEEK_SET;
  lock.l_start=0;
  lock.l_len=0; //entire file is locked from start
  lock.l_pid=getpid();

  fcntl(fd,F_SETLKW,&lock);   //locked

  int record_len=sizeof(pass);
  printf("Inside password change normal fn, acc_no: %d\n",acc_no);
  while(read(fd,&pass,sizeof(pass))>0)
  {
    if(acc_no==pass.Account.Account_Number)
    {
        //converting readlock on whole file to write lock on that particular record
        lseek(fd,-sizeof(pass),SEEK_CUR);
        lock.l_type=F_RDLCK;    //Read lock
        lock.l_whence=SEEK_CUR;
        lock.l_start= 0;
        lock.l_len= sizeof(pass); //entire file is locked from start
        lock.l_pid=getpid();
        fcntl(fd,F_SETLKW,&lock);

        strcpy(pass.Password,new_password);
        printf("Updated the password successfully\n");
        write(fd,&pass,sizeof(pass));
        lock.l_type=F_UNLCK  ;
        fcntl(fd,F_SETLKW,&lock);   //unlocked the file after writing to it

        return 1;   //balance enquiry success


    }
    else  //reccord didn't find. Remove this record from lock
    {

      lock.l_whence=SEEK_SET;
      lock.l_start=record_len+1;
      lock.l_len=0; //entire file is locked from start
      record_len=record_len+sizeof(pass);
      fcntl(fd,F_SETLKW,&lock);     //removed the record which failed comparision from lock.

    }
  }
  return -1; //failure in enquiry

}

int passwordChangeJoint(int acc_no,char username[],char new_password[]) //need username of which user to change
{
  int fd=open("joint_accounts.txt",O_CREAT|O_RDWR,0666);
  struct flock lock;
  struct Joint_Account pass;

  lock.l_type=F_RDLCK;    //Read lock
  lock.l_whence=SEEK_SET;
  lock.l_start=0;
  lock.l_len=0; //entire file is locked from start
  lock.l_pid=getpid();

  fcntl(fd,F_SETLKW,&lock);   //locked

  int record_len=sizeof(pass);
  printf("Inside password change joint fn, acc_no: %d\n",acc_no);
  while(read(fd,&pass,sizeof(pass))>0)
  {
    if(acc_no==pass.Primary.Account.Account_Number && strcmp(username,pass.Primary.Username)==0)
    {
        //converting readlock on whole file to write lock on that particular record
        lseek(fd,-sizeof(pass),SEEK_CUR);
        lock.l_type=F_RDLCK;    //Read lock
        lock.l_whence=SEEK_CUR;
        lock.l_start= 0;
        lock.l_len= sizeof(pass); //entire file is locked from start
        lock.l_pid=getpid();
        fcntl(fd,F_SETLKW,&lock);

        strcpy(pass.Primary.Password,new_password);
        printf("Updated the password successfully\n");
        write(fd,&pass,sizeof(pass));
        lock.l_type=F_UNLCK  ;
        fcntl(fd,F_SETLKW,&lock);   //unlocked the file after writing to it

        return 1;   //balance enquiry success


    }
    else if(acc_no==pass.Primary.Account.Account_Number && strcmp(username,pass.Secondary.Secondary_Username)==0)
    {
      lseek(fd,-sizeof(pass),SEEK_CUR);
      lock.l_type=F_RDLCK;    //Read lock
      lock.l_whence=SEEK_CUR;
      lock.l_start= 0;
      lock.l_len= sizeof(pass); //entire file is locked from start
      lock.l_pid=getpid();
      fcntl(fd,F_SETLKW,&lock);

      strcpy(pass.Secondary.Secondary_Password,new_password);
      printf("Updated the password successfully\n");
      write(fd,&pass,sizeof(pass));
      lock.l_type=F_UNLCK  ;
      fcntl(fd,F_SETLKW,&lock);   //unlocked the file after writing to it

      return 1;  //password change successful

    }
    else  //reccord didn't find. Remove this record from lock
    {

      lock.l_whence=SEEK_SET;
      lock.l_start=record_len+1;
      lock.l_len=0; //entire file is locked from start
      record_len=record_len+sizeof(pass);
      fcntl(fd,F_SETLKW,&lock);     //removed the record which failed comparision from lock.

    }
  }
  return -1; //failure in enquiry

}
