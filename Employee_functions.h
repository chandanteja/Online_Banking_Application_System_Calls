#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>

//===============================
//User header files
//================================
#include  "User_Details.h"

//This will be used in connection_stub_to_server file to collect user details.

int addAccount()
{
  //This function returns -1 on username duplicate exists and -2 on account number duplication.
  printf("came here\n");
  int acc_type;

  printf("Enter below details to create your account\n");
  printf("Choose the type of Account you want to create among the below options.\n1. NORMAL\n2. JOINT\n");
  scanf("%d",&acc_type);

  if(acc_type==1)
  {
      struct Customer_Details customer,temp;
      struct Account_Details acc_normal,acc_temp;
      int fd_Normal=open("customer.txt",O_CREAT|O_RDWR|O_APPEND,0666);

      int flag=0;					// 1 if that user already exists


      printf("Enter Username(UNIQUE) of the customer: ");
      scanf(" %[^\n]",customer.Username);
      // code here to check for username exists or not.
      while(read(fd_Normal, &temp, sizeof(temp))>0)
    	{
    		if( strcmp(temp.Username, customer.Username)==0)
    		{
    			flag=1;
          return -1;    //username duplication
    		}
    	}


      printf("Enter Password of Customer: ");
      scanf(" %[^\n]",customer.Password);
      customer.Account.Acc_Type=NORMAL;   //as user selected normal account.
      customer.Account.Acc_status=ACTIVE_ACC;
      printf("Enter Account number(UNIQUE): ");
      scanf("%d",&customer.Account.Account_Number);

      int fd_acc=open("Accounts.txt",O_CREAT|O_RDWR|O_APPEND,0666);

      while(read(fd_acc, &acc_temp, sizeof(acc_temp))>0)
    	{
    		if(acc_temp.Account_Number==customer.Account.Account_Number)
    		{
    			flag=1;
          return -2;    //Account number duplication
    		}
    	}


      customer.Account.Balance = 0.0;
      customer.U_Status=ACTIVE_PRIMARY;

//This is for setting Account details

      acc_normal.Account_Number=customer.Account.Account_Number;
      acc_normal.Balance=0.0;
      acc_normal.Acc_Type=NORMAL;
      acc_normal.Acc_status=ACTIVE_ACC;

      write(fd_Normal, &customer, sizeof(customer));

      write(fd_acc,&acc_normal,sizeof(acc_normal));
      close(fd_Normal);
      close(fd_acc);

  }
  else if(acc_type==2)
  {
    struct Joint_Account joint_holders,temp;
    struct Account_Details acc_joint,acc_joint_temp;
    printf("Enter Primary Username(UNIQUE) of the customer: ");
    scanf(" %[^\n]",joint_holders.Primary.Username);
    printf("Enter Primary User Password: ");
    scanf(" %[^\n]",joint_holders.Primary.Password);
    printf("Enter Secondary Username(UNIQUE) of the customer: ");
    scanf(" %[^\n]",joint_holders.Secondary.Secondary_Username);
    printf("Enter Secondary User Password: ");
    scanf(" %[^\n]",joint_holders.Secondary.Secondary_Password);

    // code here to check for both usernames exists or not
    int fd_joint=open("joint_accounts.txt",O_CREAT|O_RDWR|O_APPEND,0666);
    int flag=0;
    while(read(fd_joint, &temp, sizeof(temp))>0)
    {
      if( strcmp(temp.Primary.Username, joint_holders.Primary.Username)==0  || strcmp(temp.Primary.Username, joint_holders.Secondary.Secondary_Username)==0)
      {
        flag=1;
        return -1;    //username duplication
      }

    else if( strcmp(temp.Secondary.Secondary_Username, joint_holders.Primary.Username)==0  || strcmp(temp.Primary.Username, joint_holders.Secondary.Secondary_Username)==0)
      {
        flag=1;
        return -1;    //username duplication
      }
    }



    printf("Enter Account number(UNIQUE): ");
    scanf("%d",&joint_holders.Primary.Account.Account_Number);

    int fd_acc=open("Accounts.txt",O_CREAT|O_RDWR|O_APPEND,0664);

    while(read(fd_acc, &acc_joint_temp, sizeof(acc_joint_temp))>0)
    {
      if(acc_joint_temp.Account_Number==joint_holders.Primary.Account.Account_Number)
      {
        flag=1;
        return -2;    //Account number duplication
      }
    }


    joint_holders.Primary.Account.Acc_Type=JOINT;   //as user selected normal account.

    joint_holders.Primary.Account.Acc_status=ACTIVE_ACC;
    joint_holders.Primary.U_Status=ACTIVE_PRIMARY;
    joint_holders.Secondary.Sec_Status=ACTIVE_SECONDARY;

    joint_holders.Primary.Account.Balance = 0.00;

    acc_joint.Account_Number=joint_holders.Primary.Account.Account_Number;
    acc_joint.Balance=0.0;
    acc_joint.Acc_Type=JOINT;
    acc_joint.Acc_status=ACTIVE_ACC;
    //writing into joint account is still pending

  }
  else
  {
      printf("Sorry! We don't have this Account.Retry from first\n");
      exit(1);
  }

return 0;
}


  int search_normal(int acc_no,struct Customer_Details *user)
    {
        int flag=0;
        struct Customer_Details temp;
        int fd_Normal=open("customer.txt",O_CREAT|O_RDWR|O_APPEND,0666);

        while(read(fd_Normal, &temp, sizeof(temp))>0)
        {
          if(temp.Account.Account_Number==acc_no)
          {
            flag=1;   //found
            /*user->Username=temp.Username;
            user->Password=temp.Password;
            user->U_Status=temp.U_Status;
            user->(Account.Account_Number)=temp.Accout.Account_Number;
            user->(Account.Balance)=temp.Account.Balance;
            user->(Account.Acc_Type)=temp.Account.Acc_Type;*/
            *user=temp;
            return 1;   //success
          }
        }
        return -1;    //Failure

    }
