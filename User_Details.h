#include<stdio.h>

//This structure is just a skeleton structure which will be used in other structures.
struct Account_Details{

  double Balance;
  int Account_Number;
  enum Status {INACTIVE_ACC=0,ACTIVE_ACC=1} Acc_status;    // This is used to reduce comparisions in case of Joint account comparisions when if one user is inactive and other is active instead
  //of doing 2 comparisions then we do only one comparision.
enum Account_Type {NORMAL=1,JOINT=2}  Acc_Type;
};


struct Customer_Details{
  char Username[1024];
  char Password[1024];
  struct Account_Details Account;

  enum User_Acc_Status {INACTIVE_PRIMARY=0,ACTIVE_PRIMARY=1} U_Status;    //primary user satus either active or inactive.

};



struct Joint_Account_Holder {
  char Secondary_Username[1024];
  char Secondary_Password[1024];
  enum Secondary_User_Acc_Status {INACTIVE_SECONDARY=0,ACTIVE_SECONDARY=1} Sec_Status;  //secondary user status either active or inactive.
  //Secondary user can be active in joint account even if primary user is inactive.

};


struct Joint_Account {

  struct Customer_Details Primary;
  struct Joint_Account_Holder Secondary;
  //Mark joint account, account_status inactive only if both users are inactive.
};



struct Employee_Details{
  char Employee_Username[1024];
  char Employee_Password[1024];
  //char Employee_Email[1024];
};



struct Transaction_Details {
int Account_Number;
double Amount;
// Add date too
double Balance_remaining;
};
