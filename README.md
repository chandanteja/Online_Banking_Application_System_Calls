# Online_Banking_Application_System_Calls
This is an online banking application in C using system calls

Bank_server.c ---> Server file where it creates the connections and welcomes clients and depending the role choosen by client it will move the connection to particular child by creating a child.

Bank_employye_creation ---> is for creating bank_employee as employee/admin won't exist initially. This is analogous to creating a user when anyone newly joins a company.

Bank_employee.c ---> this is the file which will be inovkedd if a user chooses the role as Employee/Admin

Connection_stub_to_server.c   ----> this is basically like a client who tries to connect to server and depending and chooses a role. This same file will contain code for both employee and 
