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
#define ARRAY_SIZE 1024

//This will be used in connection_stub_to_server file to collect user details.

int addAccount()
{
  //This function returns -1 on username duplicate exists and -2 on account number duplication.
  //printf("came here\n");
  int acc_type;

  printf("Enter below details to create your account\n");
  printf("Choose the type of Account you want to create among the below options.\n1. NORMAL\n2. JOINT\n");
  scanf("%d",&acc_type);

  if(acc_type==1)
  {
      struct Customer_Details customer,temp;
      struct Joint_Account joint_temp;
    //  struct Account_Details acc_normal,acc_temp;
      int fd_Normal=open("customer.txt",O_CREAT|O_RDWR|O_APPEND,0666);
      int fd_joint=open("joint_accounts.txt",O_CREAT|O_RDWR|O_APPEND,0666);
      int flag=0;					// 1 if that user already exists


      printf("Enter Username(UNIQUE) of the customer: ");
      scanf(" %[^\n]",customer.Username);
      // code here to check for username exists or not.
    //  lseek(fd_Normal,0,SEEK_SET);

      while(read(fd_Normal, &temp, sizeof(temp))>0)
    	{

    		if( strcmp(temp.Username, customer.Username)==0 && temp.U_Status==ACTIVE_PRIMARY)
    		{
          //printf("Username is already taken\n");
    			flag=1;
          return -1;    //username duplication
    		}
    	}


      printf("Enter Password of Customer: ");
      scanf(" %[^\n]",customer.Password);

      printf("Enter Account number(UNIQUE): ");
      scanf("%d",&customer.Account.Account_Number);

    //  int fd_acc=open("Accounts.txt",O_CREAT|O_RDWR|O_APPEND,0666);
      lseek(fd_Normal,0,SEEK_SET);
      while(read(fd_Normal, &temp, sizeof(temp))>0)
    	{
    		if(temp.Account.Account_Number==customer.Account.Account_Number)
        //here I am considering that if an account number is used once then it can't be used again even if its deleted as it is needed for history of the account
    		{
        //  printf("Account number %d is already taken\n",temp.Account.Account_Number);
    			flag=1;
          return -2;    //Account number duplication
    		}

    	}

      while(read(fd_joint,&joint_temp,sizeof(joint_temp)))      //checking with joint accounts
      {
        if(customer.Account.Account_Number==joint_temp.Primary.Account.Account_Number)
        {
          flag=1;
          return -2;
        }
      }

      customer.Account.Balance = 0.0;
      customer.U_Status=ACTIVE_PRIMARY;
      customer.Account.Acc_Type=NORMAL;   //as user selected normal account.
      customer.Account.Acc_status=ACTIVE_ACC;

printf("User status= %d\n",customer.U_Status);
//This is for setting Account details

  //    acc_normal.Account_Number=customer.Account.Account_Number;
  //    acc_normal.Balance=0.0;
  //    acc_normal.Acc_Type=NORMAL;
  //    acc_normal.Acc_status=ACTIVE_ACC;

      write(fd_Normal, &customer, sizeof(customer));

  //    write(fd_acc,&acc_normal,sizeof(acc_normal));
      close(fd_Normal);
  //    close(fd_acc);

  }

  else if(acc_type==2)    //for joint account
  {
    struct Joint_Account joint_holders,temp;
    struct Customer_Details acc_joint,acc_joint_temp;
    printf("Enter Primary Username(UNIQUE) of the customer: ");
    scanf(" %[^\n]",joint_holders.Primary.Username);
    printf("Enter Primary User Password: ");
    scanf(" %[^\n]",joint_holders.Primary.Password);
    printf("Enter Secondary Username(UNIQUE) of the customer: ");
    scanf(" %[^\n]",joint_holders.Secondary.Secondary_Username);
    printf("Enter Secondary User Password: ");
    scanf(" %[^\n]",joint_holders.Secondary.Secondary_Password);


    if(strcmp(joint_holders.Primary.Username,joint_holders.Secondary.Secondary_Username)==0)
    {
          return -3;      //primary user and secondary user have same name
    }

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
      printf("searching joint accounts for name duplication\n");
    }



    printf("Enter Account number(UNIQUE): ");
    scanf("%d",&joint_holders.Primary.Account.Account_Number);

    int fd_acc=open("customer.txt",O_CREAT|O_RDWR|O_APPEND,0664);
    //int fd_joint_acc_no=open("joint_accounts.txt",O_CREAT|O_RDWR|O_APPEND,0664);

    lseek(fd_joint,0,SEEK_SET);
    while((read(fd_acc, &acc_joint_temp, sizeof(acc_joint_temp))>0))
    {
      if(acc_joint_temp.Account.Account_Number==joint_holders.Primary.Account.Account_Number)
      {
        flag=1;
        return -2;    //Account number duplication
      }

    }

    while((read(fd_joint, &temp, sizeof(temp))>0))
    {

        if(temp.Primary.Account.Account_Number==joint_holders.Primary.Account.Account_Number)
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

  //  acc_joint.Account.Account_Number=joint_holders.Primary.Account.Account_Number;
  //  acc_joint.Account.Balance=0.0;
  //  acc_joint.Account.Acc_Type=JOINT;
  //  acc_joint.Account.Acc_status=ACTIVE_ACC;
    //writing into joint account is still pending
    write(fd_joint,&joint_holders,sizeof(joint_holders));
    close(fd_joint);
    close(fd_acc);

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
          if(temp.Account.Account_Number==acc_no && temp.Account.Acc_status==ACTIVE_ACC)    //here checking account status is enough
          {
            flag=1;   //found
            *user=temp;
            return 1;   //success
          }
        }
        *user=temp;     // setting this structure so that we can check at caller side whether the account is deleted success fully or not
        return -1;    //Failure

    }

int search_joint(int joint_acc_no,struct Joint_Account *user)
    {
        int flag=0;
        struct Joint_Account temp;
        char username_search_jnt[ARRAY_SIZE];

        printf("Enter your username to access your account data: ");
        scanf(" %[^\n]",username_search_jnt);
        int fd_joint=open("joint_accounts.txt",O_CREAT|O_RDWR|O_APPEND,0666);

        while(read(fd_joint, &temp, sizeof(temp))>0)
        {
            if(strcmp(username_search_jnt,temp.Primary.Username)==0 && temp.Primary.U_Status==ACTIVE_PRIMARY)    //if he is primary user
            {
                if(temp.Primary.Account.Account_Number==joint_acc_no)
                {
                  flag=1;   //found
                  *user=temp;   //copying the details found.
                  return 1;   //success
                }
            }
           else if(strcmp(username_search_jnt,temp.Secondary.Secondary_Username)==0  && temp.Secondary.Sec_Status==ACTIVE_SECONDARY)
           {
               if(temp.Primary.Account.Account_Number==joint_acc_no)
               {
                  flag=1;   //found
                 *user=temp;   //copying the details found.
                  return 1;   //success
               }
           }

        }
        return -1;    //Failure


    }

int deleteAcc(int acc_no)
  {
    int acc_type;
    printf("Choose the type of Account you want to delete.\n1. NORMAL\n2. JOINT\n");
    scanf("%d",&acc_type);
      if(acc_type==1)   //normal acc
      {
            struct Customer_Details acc_del;
            int fd_normal=open("customer.txt",O_CREAT|O_RDWR,0666);
            while(read(fd_normal, &acc_del, sizeof(acc_del))>0)
            {
                if(acc_del.Account.Account_Number==acc_no)
                {
                  printf("Account number matched\n");
                  acc_del.Account.Acc_status=INACTIVE_ACC;
                  acc_del.U_Status=INACTIVE_PRIMARY;

                  if(acc_del.Account.Balance!=0.0)
                      acc_del.Account.Balance=0.0;
                  lseek(fd_normal,-sizeof(acc_del),SEEK_CUR);   // after reading entire structure into a record variable to move to start of the record, we do lseek

                  write(fd_normal,&acc_del,sizeof(acc_del));
                  return 1;   //successfully deleted
                }
            }
      }

      else if(acc_type==2)    //joint account deletion
      {
          struct Joint_Account jnt_acc_del;
          printf("Deleting account makes the account inaccessible for other user too.\n");
          printf("If you want the account to be deleted only for you and accessible for others, Press 1 else Press 2\n");
          int choice;
          scanf("%d",&choice);
          if(choice==1)
          {
              printf("Enter Username: ");
              char del_username[ARRAY_SIZE];
              scanf(" %[^\n]",del_username);
              int fd_jnt=open("joint_accounts.txt",O_CREAT|O_RDWR,0666);
              while(read(fd_jnt,&jnt_acc_del,sizeof(jnt_acc_del))>0)
              {
                    if(strcmp(del_username,jnt_acc_del.Primary.Username)==0)      //if the username matched with primary user
                    {
                        jnt_acc_del.Primary.U_Status=INACTIVE_PRIMARY;      //deactivating primary

                        if(jnt_acc_del.Secondary.Sec_Status==INACTIVE_SECONDARY)    //if secondary account is also inactive then we need to deactivate the account itself
                        {
                            jnt_acc_del.Primary.Account.Acc_status=INACTIVE_ACC;      //Making entire account inactive i.e delete
                            if(jnt_acc_del.Primary.Account.Balance!=0.0)
                                  jnt_acc_del.Primary.Account.Balance=0.0;


                        }

                        lseek(fd_jnt,-sizeof(jnt_acc_del),SEEK_CUR);   // after reading entire structure into a record variable to move to start of the record, we do lseek

                        write(fd_jnt,&jnt_acc_del,sizeof(jnt_acc_del));
                        return 2;

                    }
                    else if(strcmp(del_username,jnt_acc_del.Secondary.Secondary_Username)==0)
                    {
                        jnt_acc_del.Secondary.Sec_Status=INACTIVE_SECONDARY;

                        if(jnt_acc_del.Primary.U_Status==INACTIVE_PRIMARY)    //if secondary account is also inactive then we need to deactivate the account itself
                        {
                            jnt_acc_del.Primary.Account.Acc_status=INACTIVE_ACC;      //Making entire account inactive i.e delete

                            if(jnt_acc_del.Primary.Account.Balance!=0.0)
                                  jnt_acc_del.Primary.Account.Balance=0.0;

                        }

                        lseek(fd_jnt,-sizeof(jnt_acc_del),SEEK_CUR);   // after reading entire structure into a record variable to move to start of the record, we do lseek

                        write(fd_jnt,&jnt_acc_del,sizeof(jnt_acc_del));
                        
                        return 2;
                    }
              }


        }
        else if(choice==2)
        {   //delete for both
              int fd_jnt=open("joint_accounts.txt",O_CREAT|O_RDWR,0666);
              while(read(fd_jnt,&jnt_acc_del,sizeof(jnt_acc_del))>0)
              {
                  if(jnt_acc_del.Primary.Account.Account_Number==acc_no)    //if account exists
                  {
                      jnt_acc_del.Primary.U_Status=INACTIVE_PRIMARY;
                      jnt_acc_del.Secondary.Sec_Status=INACTIVE_SECONDARY;
                      jnt_acc_del.Primary.Account.Acc_status=INACTIVE_ACC;
                      if(jnt_acc_del.Primary.Account.Balance!=0.0)
                            jnt_acc_del.Primary.Account.Balance=0.0;
                      return 2;
                  }
              }

        }

      }
      return -1;  //failure
  }

int modifyAccount(int acc_no,char password[])
{
    printf("Enter the type of the account for which you want to modify the username\n1. NORMAL\n2. JOINT\n");
    int type;
    scanf("%d",&type);    //1--Normal, 2-- Joint

    if(type==1)
    {
        struct Customer_Details temp,modify_temp;
        int fd=open("customer.txt",O_CREAT|O_RDWR,0666);
        int modify_fd=open("customer.txt",O_CREAT|O_RDWR,0666);  //this for just for checking the new username already taken or not;

        char new_username[1024];
        while(read(fd,&temp,sizeof(temp))>0)
        {
            if(temp.Account.Account_Number== acc_no && (strcmp(temp.Password,password)==0) && temp.U_Status==ACTIVE_PRIMARY)
            {
                printf("Enter new username(UNIQUE): ");
                scanf(" %[^\n]",new_username);
                //this loop is for verification whether the new username entered already taken by some one or not
                while(read(modify_fd,&modify_temp,sizeof(modify_temp))>0)
                {
                    if(strcmp(modify_temp.Username,new_username)==0 && modify_temp.U_Status==ACTIVE_PRIMARY)
                    {
                      printf("The new username you are requesting already exists. Choose other name\n");
                      exit(1);
                    }
                }
                strcpy(temp.Username,new_username);
                lseek(fd,-sizeof(temp),SEEK_CUR);
                write(fd,&temp,sizeof(temp));
                return 1;   //successfully modified
            }

        }
        return -1;
    }

    else if(type==2)  //joint account
    {
        //  printf("Enter the new username you would like to modify(UNIQUE): ");
        struct Joint_Account modify_user,temp;
        int fd=open("joint_accounts.txt",O_CREAT|O_RDWR,0666);
        int modify_fd=open("joint_accounts.txt",O_CREAT|O_RDWR,0666);   //for modifying
        while(read(fd,&temp,sizeof(temp))>0)
        {

            if(temp.Primary.Account.Account_Number==acc_no && (strcmp(temp.Primary.Password,password)==0) && temp.Primary.U_Status==ACTIVE_PRIMARY)
            {
                  char new_username[1024];
                  printf("Enter new username(UNIQUE): ");
                  scanf(" %[^\n]",new_username);
                  while(read(modify_fd,&modify_user,sizeof(modify_user))>0)
                  {
                      if(strcmp(modify_user.Primary.Username,new_username)==0 && modify_user.Primary.U_Status==ACTIVE_PRIMARY)
                      {
                        printf("The new username you are requesting already exists. Choose other name\n");
                        exit(1);
                      }
                  }

                  strcpy(temp.Primary.Username,new_username);
                  lseek(fd,-sizeof(temp),SEEK_CUR);
                  write(fd,&temp,sizeof(temp));
                  return 2;   //success

            }

            else if(temp.Primary.Account.Account_Number==acc_no && (strcmp(temp.Secondary.Secondary_Password,password)==0) && temp.Secondary.Sec_Status==ACTIVE_SECONDARY)
            {
                    char new_username_sec[1024];
                    printf("Enter new username(UNIQUE): ");
                    scanf(" %[^\n]",new_username_sec);
                    while(read(modify_fd,&modify_user,sizeof(modify_user))>0)
                    {
                        if(strcmp(modify_user.Secondary.Secondary_Username,new_username_sec)==0 && modify_user.Secondary.Sec_Status==ACTIVE_SECONDARY)
                        {
                          printf("The new username you are requesting already exists. Choose other name\n");
                          exit(1);
                        }
                    }

                    strcpy(temp.Secondary.Secondary_Username,new_username_sec);
                    lseek(fd,-sizeof(temp),SEEK_CUR);
                    write(fd,&temp,sizeof(temp));
                    return 2;   //success

            }
        }
        return -1;


    }
}
