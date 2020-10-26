#include <stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>

//==============================
//User header files
//==============================
#include  "User_Details.h"


struct Employee_Creation {

char name[1024];
char password[1024];
//char email[50];

};

int main()
{
  struct Employee_Creation admin,temp;
  printf("Enter Username of the Employee to be created: ");
  scanf(" %[^\n]",admin.name);
printf("Enter Passowrd of the Employee to be created: ");
  scanf(" %[^\n]",admin.password);

int fd =open("Emp_login_details.txt",O_CREAT|O_RDWR|O_EXCL,0666);
if(fd==-1)
{
//  perror("open");
  fd=open("Emp_login_details.txt",O_RDWR|O_APPEND,S_IRWXU|S_IRWXG);
}

while(read(fd,&temp,sizeof(temp))>0)
{
  if(strcmp(temp.name,admin.name)==0)
  {
    printf("Username already taken.Try again\n");
    exit(1);
  }
}

int siz=write(fd,&admin,sizeof(admin));   //O_APPEND sets the cursor in file to EOF just before write is being performed on file opened in APpend mode.

int d;
lseek(fd,0,SEEK_SET);

while(d=read(fd,&admin,sizeof(admin))!=0)
{
  printf("User name: %s\n",admin.name);
  printf("Password: %s\n",admin.password);

}


/*lseek(fd, 0, SEEK_SET);
read(fd,&admin,siz);
//admin.name="abcdefijk";
printf("renter username\n");
scanf("%s",admin.name);
lseek(fd, 0, SEEK_SET);

siz=write(fd,&admin,sizeof(admin));
lseek(fd, 0, SEEK_SET);
read(fd,&usr2,siz);
//write(1,&usr2,siz);
printf("%s\n",usr2.name);
printf("%s\n",usr2.password);*/


return 0;
}
