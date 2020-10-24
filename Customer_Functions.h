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
    while(read(fd,&dep,sizeof(dep))>0)
    {
      if(acc_no==dep.Account.Account_Number)
      {

      }
      else  //reccord didn't find. Remove this record from lock
      {

      }
    }


}

int depositJoint(int acc_no)
{

}
