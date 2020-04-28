#include<stdio_ext.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h> 
#include"conio.h"

typedef struct train_detail
	{
//Train number  Train name   From   Destination    Price   Time   Number of seats left <---Format for train_details file
	char name[20], origin[20], destination[20];
	int price, seats, number;
	float time; 
	}train;
	
typedef struct date
	{
	int d, m, y;	
	}date;
	
typedef struct user_details
	{
//User id  Name  train name  train number  ticket number   date of travel  ticket status  charge <---Format for user_details file  
	char name[20], train[50], id[20];  
	int  status, ticket_number, seats, train_number; 
	float charge, time;  
	date *departure;
 	}user;
 
typedef struct pay
	{
	int pin,temp;
	long int mob;
	}pay;

typedef struct Account 
	{
	char username[20];
	char name[20];
	char pass[9];
	}Account;

/*
						Seats: Number of seats paid for.

							id: user id

						    Ticket number: Taken from the ticket_count file

						     User Status: 
						   -1 -> cancelled
				       +ve integer -> number of seats in waiting
						   0 -> confirmed
*/

int read_date(date *); //Reading date entered by the user
void display_date(date *);//To display date at certain places
int date_check(date *, date *); //Checks whether the user can book a ticket on the entered date or not
int compare(date *, date *); /*Used in the check date function to prevent the user from booking a ticket on a date previous to that of the current date*/
int train_seats_change(user*, int, user[], int); /*Function to alter the train details text file (needs to be called each time a user reserves or cancels a ticket)*/
int cancel(int, int); //Returns 0 if successful or -1 if not
int cancel_or_check(int (*fptr)(int, int)); //A callback which can be used either to check status or for cancellation
int user_compare(user, user);//Used to check equality of two user structures 
int check_status(); //For user to check ticket status
void delay(int);


void reservation(); //For user to book a ticket
void start_menu();
int viewdetails(int *, date *); //To display available trains on a particular date entered by the user
int waiting_list_adder(user); //To add tickets into waiting list
void waiting_list_writer(); //To rewrite the waiting list (when some seats are removed from it)
void ticket_counter(); //To keep count of the ticket number
 

#define SIGN_UP_SUCCESSFUL 1
#define SIGN_UP_UNSUCCESSFUL 2
#define LOGIN_SUCCESSFUL 1
#define LOGIN_FAILURE 2


int Signup ();
int Login (char username[],char name[], char pass[]);


int payment(user);
int reade(FILE *, pay*, float);
void ticket(pay ,user);
void refund(user);

//Some global variables required
char usrname[20]; //The user's username
char name[20]; //The user's name
user passdetails; //The current user's structure

int main()
	{
	char pasword[9], ch;
	int i, res;
	
	printf("\n\t\t\t                =======================  LOGIN FORM  =======================\n  ");
    	printf("\n\t\t\t                                 ENTER USERNAME: ");
	scanf("%s", usrname);
	
	__fpurge(stdin);
	printf("\n\t\t\t                                 ENTER NAME: ");
	scanf("%s", name);

	__fpurge(stdin);
	printf("\n\t\t\t                                 ENTER PASSWORD: ");
	
	for(i=0;i<8;i++)
		{
		ch = getch();
		pasword[i] = ch;
		ch = '*' ;
		printf("%c",ch);
		}
	pasword[8] = '\0';
	if(Login(usrname,name,pasword) == 1)
		{
		printf("\n\t\t\t\t              ================LOGIN SUCCESSFUL================");
		strcpy(passdetails.name, name); 
		strcpy(passdetails.id, usrname);
		delay(1250);
		start_menu();
		}
	return 0;
	}	
	

  
//***************************************************** Helper functions **************************************************************** 
//To simulate loading times
void delay(int number_of_seconds) 
	{ 
	int milli_seconds = 1000 * number_of_seconds; // Converting time into milli_seconds 
	clock_t start_time = clock(); // Storing start time 
	while (clock() < start_time + milli_seconds); // Looping till required time is not achieved  
	} 


//To read date entered by the user
int read_date(date *d)
	{
	date *current_date = (date*) malloc(sizeof(date));
	(*current_date).d = 10;
	(*current_date).m = 4;
	(*current_date).y = 2020;
	__fpurge(stdin); scanf("%d/%d/%d", &d->d, &d->m, &d->y);  //User has to enter date he wants to travel in dd/mm/yyyy format
	return date_check(d, current_date); //Returns an integer that can be used as a key to go to a certain block of code in the client file
	}
//To display date at certain places	
void display_date(date *d)
	{
	printf("%02d/%02d/%4d", d->d, d->m, d->y);
	}

//Checks whether the user can book a ticket on the entered date or not
int date_check(date *d, date *current)  
//The integer returned can be used to goto a certain block in the client code, like here to retype the date
	{ 
	int D, M, Y;
	D = d->d;
	M = d->m;
	Y = d->y;
	
	date *today = (date*)malloc(sizeof(date)); //Need a way to get time from the computer
	today->d = 9;
	today->m = 4;
	today->y = 2020;
		
	if ((M == 1 || M == 3 || M == 5 || M == 7 || M == 8 || M == 10 || M == 12) && D>31) 
		{printf("\n\t\tInvalid date\n"); free(current); return 0;}
	else if((M == 4 || M == 6 || M == 9 || M == 11) && D>30) {printf("\n\t\tInvalid date\n"); free(current); return 0;}
	else if (D<0 || M<0 || M > 12 || Y<0) {printf("\n\t\tInvalid date\n"); free(current); return 0;}
	else if (((Y%4==0)&&(Y%100!=0)) || (Y%400==0)) //Leap year
		{
		if(D>29 && M==2) {printf("\n\t\tInvalid date\n"); free(current); return 0;}
		else return 1;
		} 
	else if (M==2 && D>28) {printf("\n\t\tInvalid date\n"); free(current); return 0;}
	else if (compare(d,today) < 0) {printf("\n\t\tCannot book ticket for the entered date\n"); free(current); return 0;}
	else return 1; 
	}

//Used in the check date function to prevent the user from booking a ticket on a date previous to that of the current date	
int compare(date *d1, date *d2)
	{
	int D1, D2, M1, M2, Y1, Y2;
	D1 = d1->d;
	M1 = d1->m;
	Y1 = d1->y;
	D2 = d2->d;
	M2 = d2->m;
	Y2 = d2->y;
	if ((Y1>Y2) || ((Y1==Y2)&&(M1>M2)) || ((Y1==Y2)&&(M1==M2)&&(D1>D2))) return -1;
	else if (D1==D2 && M1==M2 && Y1==Y2) return 0; 
 	else return 1;
	}

int user_compare(user user1, user user2) //To check if two user structures are of the same user
	{
	if((user1.ticket_number == user2.ticket_number)&&(user1.train_number == user2.train_number)) return 1;
	else return 0;
	}

//To get the ticket number
void ticket_counter()
{
	FILE* fp;
	int count;
	fp = fopen("ticket_count.txt","r");
	fscanf(fp, "%d", &count);
	fclose(fp);
	count++;
	fp = fopen("ticket_count.txt","w");
	fprintf(fp, "%d\n", count);
	fclose(fp);
}

int cancel_or_check(int (*fptr)(int, int)) //A callback which can be used either to check status or for cancellation
	{
	int train_num, ticket_num;
	printf("\n\t\t\tEnter the train number: ");
	__fpurge(stdin); scanf("%d", &train_num);
		
	printf("\t\t\tEnter the ticket number: ");
	__fpurge(stdin); scanf("%d", &ticket_num);
	
	return fptr(train_num, ticket_num);
	}
//****************************************************************************************************************************************


//****************************************************** Main functions ******************************************************************

int Signup() 
	{
	printf("\e[1;1H\e[2J");
	FILE *sc;
	int i;
	char pasword[9], ch;
	struct Account acc;
	printf("\n\t\t\t                =======================  SIGNUP FORM  =======================\n");
	
	printf("\n\t\t\t                                 ENTER USERNAME: ");
	scanf("%s", usrname);
	
	__fpurge(stdin);
	printf("\n\t\t\t                                 ENTER NAME: ");
	scanf("%s", name);

	__fpurge(stdin);
	printf("\n\t\t\t               ENTER PASSWORD (8 characters long): ");
	
	for(i=0;i<19;i++)
		{
		ch = getch();
		if(ch == 0x0A) {pasword[i] == '\0'; break;}
		pasword[i] = ch;
		ch = '*' ;
		printf("%c",ch);
		}
	pasword[8] = '\0';
		
	strcpy(acc.username, usrname);
	strcpy(acc.name, name);
	strcpy(acc.pass, pasword);
	sc=fopen("accounts","ab");
	if(sc==NULL)
		{
		printf("\n\t\tError: Cannot open accounts database\n");
		return(SIGN_UP_UNSUCCESSFUL);
		}
	fwrite(&acc,sizeof(struct Account),1,sc);
	fclose(sc);
	return(SIGN_UP_SUCCESSFUL);
	}
    
int Login (char username[],char name[], char pass[])
	{          
	Account acc;
	int signup_res;
	int passwordmatch = 0, usernamematch = 0;
	char choice;

	FILE *sc;
	sc = fopen("accounts","rb");

	    
	while(!feof(sc))   //Traversing till end of file
	//Comparing the user entered username with all the usernames present as structures
		{
		fread(&acc,sizeof(struct Account),1,sc); //reading structures to the binary file
		//printf("\n %s\t,%s\t,%s\t",acc.username,acc.name,acc.pass); //For testing

		if((strcmp(username,acc.username)==0)&&(strcmp(name,acc.name)==0)) usernamematch = 1;

		if((strcmp(username,acc.username)==0)&&(strcmp(name,acc.name)==0)&&(strcmp(pass,acc.pass)==0)) passwordmatch =1;
		//printf("%d",passwordmatch); //For testing
		}
	fclose(sc);
	
	if(usernamematch == 0)
		{
		char ch;
		printf("\n\t\t\t\t\tLooks like you don't have an account yet\n");
		fclose(sc);
		
		printf("\n\t\t\t\t\tWould you like to sign-up?\n");
		printf("\n\t\t\t\t\tPress 'Enter' key to sign-up or any other key to exit the program: ");
		__fpurge(stdin); ch =  getchar();
		if (ch == 0x0A)
		{
		signup_res = Signup();
		if(signup_res == 1)
		{
			printf("\n\t\t\t\t              ================SIGNUP SUCCESSFUL================");
			delay(1250);
			passwordmatch = 1;
		}
		else
		{
			printf("\n\t\t\t\t\t\t\tUnsuccessful sign up\n");
		}
		
		}
		else 
			{
			printf("\e[1;1H\e[2J");
			printf("\n\t\t\t\t\t\t\tHave a great day!!\n");
			delay(500);
			exit(0);
			}
		}
	if(passwordmatch) return(LOGIN_SUCCESSFUL);
	   
	else 
		{
		printf("\n\n\t\t\t\t\t\t\tName or Pass incorrect\n");
		delay(800);
		return(LOGIN_FAILURE);
		}
	}

//Main menu of the program
void start_menu()
{
	strcpy(passdetails.name,name);
	strcpy(passdetails.id,usrname);
	int ret;//To catch return values of all the functions 
	int menu_choice;
	int train_num,ticket_num;
	while(1){
		printf("\e[1;1H\e[2J"); //To clear screen in gcc
		printf(" \t\t=================================================\n");
		printf(" \t\t|                                               |\n");
		printf(" \t\t|       --------------------------------        |\n");
		printf(" \t\t|       TRAIN TICKET RERSERVATION SYSTEM        |\n");
		printf(" \t\t|       --------------------------------        |\n");
		printf(" \t\t|                --Main Menu--                  |\n");
		printf(" \t\t|            1 -> Reserve A Ticket              |\n");
		printf(" \t\t|            2 -> Cancel Reservation            |\n");
		printf(" \t\t|            3 -> Check ticket status           |\n");
		printf(" \t\t|            4 -> Exit                          |\n");
		printf(" \t\t|                                               |\n");
		printf(" \t\t                Your choice: ");
		
		__fpurge(stdin); scanf("%d", &menu_choice);
		switch (menu_choice)
		{
		case 1:
			{
			reservation();	
			printf("\n\n\t\t\t\t\t\t\t\tRerouting to main menu....\n");
				delay(1250);
			break;
			}
		case 2:
			{
			int (*canc)(int, int) = &cancel;
			ret = cancel_or_check(canc);
			if(ret) 
				{
				char key_stroke;
				printf("\n\n\t\tPress any key to go back to main menu. ");
				__fpurge(stdin);
				key_stroke = getchar();
				}
			else	
				{
				printf("\n\n\t\t\t\t\t\t\t\tRerouting to main menu....\n");
				delay(1250);
				}
			break;
			}
		
		case 3: 
			{
			printf("\n");
			ret = check_status();
			if(ret) 
				{
				char key_stroke;
				printf("\n\n\t\tPress any key to go back to main menu. ");
				__fpurge(stdin);
				key_stroke = getchar();
				}
			else	
				{
				printf("\n\n\t\t\t\t\t\t\t\tRerouting to main menu....\n");
				delay(1250);
				}
			break;
			}
			
		case 4: 
			{
			printf("\n\t\t\t\tHave a great day!!\n");
			exit(0);
			}
		
		default:
			{
			printf("\n\t\t                  Invalid choice\n");
			delay(1250);  
			break;
			}
		}
		}
}

//To alter the train_details file
int train_seats_change(user *current_user, int seat_change, user list[], int number_of_elements_in_list) 
//Seat change is +ve for cancellation and -ve for reservation and is added to the seats available in the train
 	{
 	#define payment_error -2
 	#define no_booking 0
 	#define waiting_list_error -1
 	
	FILE *original = fopen("train_details.txt", "r");
	if (original == NULL)
		{
		printf("\n\t\t\t\tNO RECORD OF TRAINS ADDED.\n");
		fclose(original);
		return waiting_list_error; //Showing failure to check database
		}
	FILE *new = fopen("train_details_temp.txt", "w");
		
	int count; //Number of data entries is written on top of each text file
	fscanf(original, "%d", &count);
	fprintf(new, "%d\n", count);
	
	date *d1 = (date*)malloc(sizeof(date));
	fscanf(original, "%d/%d/%d\n", &d1->d, &d1->m, &d1->y);
		
	while((compare(d1,(*current_user).departure)) != 0) //Meaning dates aren't same
		{
		fprintf(new, "%d/%d/%d\n", d1->d, d1->m, d1->y);
		
		train *data = (train*)malloc(sizeof(train) * count);
		
		for (int i = 0; i<count; i++)
			{		
			//Reading data from the text file into the structure named train
			fscanf(original, "%d", &data[i].number);
			fscanf(original, "%50s %50s %10s", data[i].name, data[i].origin, data[i].destination);
			fscanf(original, "%d", &data[i].price);
			fscanf(original, "%f", &data[i].time);
			fscanf(original, "%d\n", &data[i].seats);
			
			fprintf(new, "%d %s %s %s %d %.2f %d\n", data[i].number, data[i].name, data[i].origin,
			 data[i].destination, data[i].price, data[i].time, data[i].seats);
			}
		free(data);
		
		fscanf(original, "%d", &count); //Reading next count
		fprintf(new, "%d\n", count);
		
		fscanf(original, "%d/%d/%d\n", &d1->d, &d1->m, &d1->y); //Reading next date
		}
	//Once the dates match loop is exited with final count
	fprintf(new, "%d/%d/%d\n", d1->d, d1->m, d1->y);
	
	train *data = (train*)malloc(sizeof(train) * count);
	int pos;
	for (int i = 0; i<count; i++)
		{		
		//Reading data from the text file into the structure named train
		fscanf(original, "%d", &data[i].number);
		fscanf(original, "%50s %50s %10s", data[i].name, data[i].origin, data[i].destination);
		fscanf(original, "%d", &data[i].price);
		fscanf(original, "%f", &data[i].time);
		fscanf(original, "%d\n", &data[i].seats);
		
		if ((data[i].number == (*current_user).train_number))  
			{
			pos = i; //The position of the user's train
			
			if(seat_change<0) //Only for booking a ticket
			//-ve => booking seat
			//+ve => cancelling
				{
				//Filling the user details
				strcpy((*current_user).train, data[i].name);
				(*current_user).time = data[i].time;
				 
				int tempo = (data[i].seats + seat_change); //To figure out the status of the train seats after booking
				
				if(tempo<0) //Meaning some seats will go to waiting list after this
					{
					#define dif(x,y) (x-y)>0?(x-y):-(x-y) //Gives absolute difference between two numbers
					int t = data[i].seats<0?-1:1; 
					//If initial seats available is +ve, 1 else -1	
					int wait;			
					s: switch(t)
						{
						case 1: //Some seats in waiting
							{
							wait = dif(data[i].seats, -seat_change);
							printf("\n\t\t%d seats will be in waiting list is that okay?\n",
								wait);							
							break;
							}
						
						case -1: //All seats in waiting
							{
							printf("\n\t\tAll seats will be in waiting list is that okay?\n");
							wait = (*current_user).seats;
							break;
							}
						default:
							{
							printf("\n\t\t\t\tSomething went wrong.\n");
							break;
							}
						}
						
					char check;
					printf("\n\n\t\tPress 'y' to confirm and 'n' to go back: ");
					__fpurge(stdin);  check = getchar();
				
					if(check == 'n') return no_booking;//This is to say that the the user doesnt want to book ticket;
						
					else if(check == 'y') 
						{
						(*current_user).status = wait;
						if(waiting_list_adder(*current_user)) //To add to waiting list
							{
							printf("\n\t\tBlocked seats, now redirecting to payment gateway...\n");
							(*current_user).charge = -data[i].price * seat_change;
							delay(1250);
							if(payment(*current_user))
								{
								delay(1000);
								}
							else 
								{
								printf("\n\t\t\t\t\t\t\tPayment error\n");
								free(data); //Deallocating memory for the array of train data 
								fclose(original);
								fclose(new);
								remove("train_details_temp.txt");
								waiting_list_writer(); 
								//Since payment failed, waiting list changes have to be removed
								return payment_error; //Showing payment error
								}
							}
						else
							{
							printf("\n\t\t\t\tError with accessing waiting list.\n");
							free(data); // Deallocating memory for the array of train data 
							fclose(original);
							fclose(new);
							remove("train_details_temp.txt");
							return waiting_list_error; //Showing waiting list error
							}
						}
					else {printf("\n\t\tInvalid input.\n"); delay(1000); goto s;}
					}
			
				else //Confirmed tickets
					{
					printf("\n\t\tBlocked seats, now redirecting to payment gateway...\n");
					(*current_user).charge = -data[i].price * seat_change;
					delay(1250);
					
					if(payment(*current_user)) (*current_user).status = 0;
						
					else 
						{
						printf("\n\t\t\t\tPayment error\n");
						free(data); // Deallocating memory for the array of train data 
						fclose(original);
						fclose(new);
						remove("train_details_temp.txt");
						return payment_error; //Showing payment error
						}
					}						
				}
			else //While cancelling seats
				{
				(*current_user).charge = -0.75*data[i].price*seat_change; //75%refund
				if(data[i].seats<0) //Meaning seats were in waiting before
					{
					int differ;
					if((differ = ((*current_user).status-seat_change))<0) 
					//Implies confirmed seats are also being cancelled
						{
						for(int i = 0; (i<number_of_elements_in_list)&&(-differ); i++)
							{
							if(user_compare(list[i], (*current_user))) continue;
							else if ((list[i].status>0)
								&&(((*current_user).train_number)==(list[i].train_number))
								&&(~(compare((*current_user).departure, list[i].departure)))) 
								//Same train and same date
								{
								for(differ; differ<=0; differ++)
									{
									while(list[i].status>0) 
										{
										list[i].status -= 1;
										data[i].seats -= 1;
										}																		
									} 
								}
							} 	
						}
		//If differ is greater than 0 it means only waiting list seats are removed so no difference				
					}
				//If originally seats were +ve for that train then no difference	
				}	
			
			data[i].seats += seat_change; //Changing number of seats
			}
		
		fprintf(new, "%d %s %s %s %d %.2f %d\n", data[i].number, data[i].name, data[i].origin, data[i].destination,
	  			data[i].price, data[i].time, data[i].seats);
		}
	fscanf(original, "%d", &count);	
	fprintf(new, "%d\n", count);
	
	while(count != -1)
		{
		fscanf(original, "%d/%d/%d\n", &d1->d, &d1->m, &d1->y);
		fprintf(new, "%d/%d/%d\n", d1->d, d1->m, d1->y);
		
		for (int i = 0; i<count; i++)
			{
			data = (train*)realloc(data, count*sizeof(train));			
			fscanf(original, "%d", &data[i].number);
			fscanf(original, "%50s %50s %10s", data[i].name, data[i].origin, data[i].destination);
			fscanf(original, "%d", &data[i].price);
			fscanf(original, "%f", &data[i].time);
			fscanf(original, "%d\n", &data[i].seats);
			
			fprintf(new, "%d %s %s %s %d %.2f %d\n", data[i].number, data[i].name, data[i].origin, data[i].destination,
		  			data[i].price, data[i].time, data[i].seats);
		  	}	
		fscanf(original, "%d", &count);	
		fprintf(new, "%d\n", count);
		}	
	free(d1);
	free(data); //Deallocating memory for the array of train data 
	fclose(original);
	fclose(new);
	
	remove("train_details.txt"); //Deleting original file
	rename("train_details_temp.txt", "train_details.txt"); //Renaming the temporary file	
	
	return 1; //Implies successful
	}

//To display available trains for a date entered by the user
int viewdetails(int a[], date *depart)
{	
	#define no_train -1
	#define trains_present 1 
	
	int count;
	char line[200];
	FILE* fp;
	fp = fopen("train_details.txt","r");
	fscanf(fp, "%d", &count);
	
	date *d1 = (date*)malloc(sizeof(date));
	fscanf(fp, "%d/%d/%d\n", &d1->d, &d1->m, &d1->y);
	while(compare(depart, d1) != 0) //Skipping past portions of the database that aren't required by the user
		{
		for(count; count>0; count--) fgets(line, sizeof(line), fp);
			 
		fscanf(fp, "%d", &count);
		if(count == -1)
			{
			printf("\n\n\t\tNo trains available for that date\n");
			free(d1);
			fclose(fp);
			return no_train;
			}
		
		else fscanf(fp, "%d/%d/%d\n", &d1->d, &d1->m, &d1->y);
		
		}
	//The loop is exitted when the date the user wants is reached 
	train* data = malloc(sizeof(train)*count);
	printf("\n\tAvailable trains on "); 
	display_date(d1);
	printf("\n\n");
	printf("%-10s| %-18s | %-15s | %-15s | %s | %s | %s\n",
	"\tTrain number", "Name", "From", "Destination", "Price(in INR)", "Departure timing", "Available seats");
	for(int i =0;i<count;i++)
		{
			fscanf(fp, "%d", &data[i].number);
			fscanf(fp, "%50s %50s %10s", data[i].name, data[i].origin, data[i].destination);
			fscanf(fp, "%d", &data[i].price);
			fscanf(fp, "%f", &data[i].time);
			fscanf(fp, "%d\n", &data[i].seats);
			
			printf("\t%-12d| %-18s | %-15s | %-15s | %-13d | %-16.2f | %d\n",
			   data[i].number, data[i].name, data[i].origin, data[i].destination, data[i].price, data[i].time, data[i].seats);
			a[i] = data[i].number;
		}
	printf("\nNote: A negative number of seats implies seats are in waiting list.\n");
	fclose(fp);
	return trains_present;	
}

//Reservation function
void reservation()
{
	int n, tckt_count;
	char ch, confirm;
	FILE* fp, fpt;
				
	printf("\n\t\t\tPress 'Enter' to view available trains else press anyother key to go to main menu:\n");
	__fpurge(stdin);
	ch = getchar();
	if (ch == 0x0A)
	{
		int a[10] = {0};
		date *depart = (date*)malloc(sizeof(date));
		
		passdetails.departure = (date*)malloc(sizeof(date));
	
		f: printf("\e[1;1H\e[2J"); //This is used to clear the screen in gcc 
		printf("\n\t\t\t\tEnter the date you wish to travel on in dd/mm/yyyy format: ");
		
		if(read_date(depart)) (*passdetails.departure) = *depart;
		else 
			{
			delay(1250); 
			goto f;
			};
						
		ss: 
		if(viewdetails(a, depart)==-1) {delay(1250); goto f;}
						
		printf("\n\t\t\t\tPress 'Enter' to book tickets, 'c' to change date, or anyother key to go back to menu: ");
		__fpurge(stdin); ch = getchar();
		
		if (ch == 0x0A)
		{ 
			int fleg = 0; //To know if train details have to be printed again or not
			start:
			if(fleg) if(viewdetails(a, depart)==-1) {delay(1250); goto f;}
					
			printf("\n\n\t\tEnter the train number: ");
			__fpurge(stdin);
			scanf("%d", &passdetails.train_number);		
			
			int flag2 = 0; //To check if the user entered the right train number
			for(int i = 0; i<10; i++) if (a[i] == passdetails.train_number) flag2 = 1;
			
			if(flag2)
			{
				seat_change:
				printf("\n\t\tEnter the number of seats needed: ");
				__fpurge(stdin); scanf("%d", &passdetails.seats);
				
				if(passdetails.seats<=0) 
					{
					printf("\n\t\tInvalid input\n");
					delay(1250);
					goto seat_change;
					}
				
				start1:
				printf("\n\tConfirm Ticket?\n\tPress 'y' to continue, 'c' to change seats and 'n' change train: "); 
				__fpurge(stdin); confirm = getchar();
								
				if (confirm == 'y')
				{	
					FILE *fpt;			
					fpt = fopen("ticket_count.txt","r");
					fscanf(fpt, "%d", &tckt_count);
					passdetails.ticket_number = tckt_count;
					fclose(fpt);

					*(passdetails.departure) = *depart;
					
					user *simply = (user*)malloc(sizeof(user)*2);				
					
					int r = train_seats_change(&passdetails,-passdetails.seats,simply,1);
					
					free(simply);
					
					if(r == 1)
					{	
						
						fp = fopen("user_details.txt", "r");
						fscanf(fp, "%d", &n);
						
						user *temp = (user*)malloc(sizeof(user)*n);
						
						date* tpd = (date*)malloc(sizeof(date));
						
						for(int m = 0; m<n; m++)
						{					
							fscanf(fp, "%50s %50s %50s", temp[m].id, temp[m].name, temp[m].train);
							fscanf(fp, "%d %d %d/%d/%d %f %d %d %f",
							&temp[m].train_number, &temp[m].ticket_number, 
							&(tpd->d), &(tpd->m), &(tpd->y),
							&temp[m].time, &temp[m].seats, &temp[m].status, &temp[m].charge);	
							(temp[m].departure) = tpd;
						}
						fclose(fp);
						free(tpd);
						
						ticket_counter();
						n++;
						FILE *fp;
						
						fp = fopen("user_details_tmp.txt", "w");
						fprintf(fp, "%d\n", n);
						
						for(int k = 0; k<n-1; k++)
						{
						fprintf(fp, "%s %s %s %d %d %d/%d/%d %.2f %d %d %.2f\n", 
							temp[k].id, temp[k].name, temp[k].train,
							temp[k].train_number, temp[k].ticket_number, 
							(temp[k].departure)->d, (temp[k].departure)->m,	(temp[k].departure)->y,
							temp[k].time, temp[k].seats, temp[k].status, temp[k].charge
							);
						}
						
						fprintf(fp, "%s %s %s %d %d %d/%d/%d %.2f %d %d %.2f\n", 
							passdetails.id, passdetails.name, passdetails.train,
							passdetails.train_number, passdetails.ticket_number, 
							(passdetails.departure)->d,(passdetails.departure)->m,(passdetails.departure)->y,
							passdetails.time, passdetails.seats, passdetails.status, passdetails.charge
							);
						
						fclose(fp);
						free(temp);
						remove("user_details.txt");
						rename("user_details_tmp.txt", "user_details.txt"); 
						
						printf("\n\n\t\t\tReservation Done\n");
															
						printf("\n\n\t\tPress any key to go back to Main menu");
						__fpurge(stdin);
						getchar();
						return;
					}
				
					else if(r < 0)
					{
						printf("\n\t\t\t\tSomething went wrong please try again later.\n");
						delay(1250);
						return;
					}
					
					else 
					{
						printf("\n\t\tSeats not booked.\n");
						delay(1250);
						return;
					}
				
				}
				else if(confirm == 'n'|| confirm == 'N') 
				{		
					fleg = 1;
					printf("\e[1;1H\e[2J");
					goto start;
				}
				else if(confirm == 'c' || confirm == 'C')
				{
					printf("\e[1;1H\e[2J");
					goto seat_change;
				}
				else
				{
					printf("\n\t\tInvalid input\n");
					delay(1250);
					goto start1;
				}

			}
			else
			{
				printf("\n\t\tInvalid choice entered!\n");
				fleg = 1;
				delay(1250);
				printf("\e[1;1H\e[2J");
				goto start;
			}
		}
		
		else if((ch == 'c') || (ch == 'C')) goto f; //To change date
		
		else return;
		
	}
	else return;	
}

//Cancellation function
int cancel(int train_num, int ticket_num)
	{
	#define database_error -1
	#define no_tickets_booked -2
	
	int cancel_output = 0, count, delta;
		
	FILE *user_file;
	FILE *temp_file;
	
	user_file = fopen("user_details.txt", "r");
	
	
	printf("\n\t\tOpening database....\n\n"); 
	delay(500);
	
	if (user_file == NULL)
		{
		printf("\t\t\tsNO RECORD OF USERS ADDED.\n");
		fclose(user_file);
		return database_error; //Showing failure to check database 
		}
	else
		{
		temp_file = fopen("user_details_temp.txt", "w");		
		
		fscanf(user_file, "%d\n", &count);
		fprintf(temp_file, "%d\n", count);
						
		user *data = (user*)malloc(sizeof(user) * count);
		user current_user;// = passdetails; 
		
		date* tpd = (date*)malloc(sizeof(date));
		int flag = 0; //To check if the user had booked a ticket previously
		int pos; //To locate the data so that it can be altered while writing back
		for (int i = 0; i<count; i++)
			{
			fscanf(user_file, "%50s %50s %50s", data[i].id, data[i].name, data[i].train);
			fscanf(user_file, "%d %d %d/%d/%d %f %d %d %f", &data[i].train_number, &data[i].ticket_number, 
			&(tpd->d), &(tpd->m), &(tpd->y),
			&data[i].time, &data[i].seats, &data[i].status, &data[i].charge);	
			(data[i].departure) = tpd;	
			
			if ((data[i].ticket_number==ticket_num) && (data[i].train_number==train_num)) 
				{
				current_user = data[i];
				flag = 1;
				pos = i;
				} 		
			}
		
		fclose(user_file);
		
		if ((flag == 0)||(current_user.seats <= 0)) 
			{
			if(current_user.seats == -1) printf("\t\t\tBooking has been cancelled\n");
			else printf("\t\t\t\tNo tickets booked yet\n");
			fclose(temp_file);
			remove("user_details_temp.txt");
			free(data);
			return no_tickets_booked; //Show no tickets booked yet
			}
			
		else
			{
			if (current_user.status==0 && current_user.seats)  //Meaning all seats are confirmed
				{
				start: printf("\e[1;1H\e[2J");
				printf("\tYou had booked %d seats all of which were confirmed.\n", current_user.seats);
				
				if(current_user.seats==1) 
					{ 
					printf("\tWould you like to cancel this seat?\n");
					printf("\t(Enter 1 to cancel this seat or 0 to not cancel anything)\n");
					printf("\n\t\t\tYour choice: ");
					__fpurge(stdin); scanf("%d", &delta);
					switch(delta)
						{
						case 1: {
							if(train_seats_change(&current_user, delta, data, count)) 
								{
								current_user.status = -1;
								printf("\tYour seat has been cancelled successfully\n");
								} 
							else cancel_output = -1; //To show that the cancellation is unsuccessful
							break;
							}
						case 0: 
							{
							printf("\n\tYour ticket will remain.\n\tHope you arrive at the station on time on ");
							display_date(current_user.departure);
							printf("\n\n\tWish you a safe and happy journey!!\n");
							break;
							}
						default: {printf("\n\t\tPlease enter a valid input\n"); delay(1250); goto start;}
						}
					}
				else
					{
					printf("\tHow many seats would you like to cancel? (Enter 0 to not cancel anything)\n");
					printf("\n\t\t\tYour choice: ");
					__fpurge(stdin); scanf("%d", &delta);
					if ((delta>current_user.seats)||(delta<0))
						{
						printf("\n\t\tPlease enter a valid input\n");
						delay(1250);
						goto start;
						}
					
					else if(delta == 0)
						{
						printf("\n\tYour ticket will remain.\n\tHope you arrive at the station on time on ");
							display_date(current_user.departure);
						printf("\n\n\tWish you a safe and happy journey!!\n");
						}
					else 
						{
						char check;
						s1: printf("\n\tAre you sure you want to cancel %d seats of the %d seats confirmed?\n",
							delta, current_user.seats);
						printf("\n\n\t\tPress 'y' to confirm and 'n' to go back: ");
						__fpurge(stdin);
						
						check = getchar();
						
						if(check == 'n') goto start;						
						else if(check == 'y')
							{
							if(train_seats_change(&current_user, delta, data, count)) 
								{
								if(current_user.seats == delta) 
									{
									current_user.status = -1;
									printf("\n\tYour ticket has been cancelled sucessfully.\n");
									}
								else 
									{
									current_user.seats -= delta;
									printf("\n\t%d seats of your ticket has been cancelled successfully\n", delta);
									}
								} 
							else cancel_output = -1; //To show that the cancellation is unsuccessful
							}
						else 
						{
						printf("\n\t\tPlease enter a valid input\n");
						delay(1250);
						goto s1;
						}
						}
					}
					
				}
			
			else if (current_user.status == -1) 
				{
				printf("\t\tYou have already cancelled that ticket."); 
				delay(1250); 
				return 0;
				} 
			else //This means that some seats are in waiting list 
				{
				start2: printf("\e[1;1H\e[2J"); 
				printf("\t\tYou had booked %d seats out of which %d tickets are in waiting.\n", 
						current_user.seats, current_user.status);
				printf("\n\t\tHow many seats would you like to cancel from this ticket?\n(The waiting list seats will be cancelled before going to the ones that are confirmed.)\n");
				printf("\t\t\t{Enter 0 to not cancel any seats.}\n");
				printf("\n\t\t\tYour choice: ");
				__fpurge(stdin); scanf("%d", &delta);
				if((delta>0)&&(delta<=current_user.seats))
					{
					if(current_user.seats == delta) 
						{
						char check;
						s2: printf("\t\tAre you sure you want to cancel all the seats?\n");
						printf("\n\n\t\tPress 'y' to confirm and 'n' to go back: ");
						__fpurge(stdin);
						check = getchar();
						
						if(check == 'n') goto start2;
						else if(check == 'y')  
							{
							if(train_seats_change(&current_user, delta, data, count))
								{
								current_user.status = -1;
								printf("\t\tYour ticket has been sucessfully cancelled.\n");
								}
							else cancel_output = -1; //To show that the cancellation is unsuccessful
							}
						else {printf("\n\t\tInvalid input.\n"); delay(1250); goto s2;}
						}
	
					else
						{
						if(current_user.status == delta) //Wanting to cancel all waiting list seats
							{
							char check;
							s3: printf("\n\t\tAre you sure you want to cancel all the seats in waiting list?\n");
							printf("\n\n\t\tPress 'y' to confirm and 'n' to go back: ");
							__fpurge(stdin);
							check = getchar();
							
							if(check == 'n') goto start2;
							else if(check == 'y')  
								{ 
								if(train_seats_change(&current_user, delta, data, count))
									{
									current_user.seats -= delta;
									current_user.status = 0;
									printf("\t\tYour ticket has been sucessfully cancelled.\n");
									}
								else cancel_output = -1; //To show that the cancellation is unsuccessful
								}
							else {printf("\n\t\tInvalid input.\n"); delay(1250); goto s3;}
							}
						else
							{
							char check;
							s4: printf("\t\tAre you sure you want to cancel %d seats?\n", delta);
							printf("\n\n\t\tPress 'y' to confirm and 'n' to go back: ");
							__fpurge(stdin);  check = getchar();
							
							if(check == 'n') goto start2;
							else if(check == 'y')  
								{ 
								if(train_seats_change(&current_user, delta, data, count))
									{
									current_user.seats -= delta;
									current_user.status -= delta;
									if(current_user.status<-1) current_user.status = 0;
									printf("\t\tYour ticket has been sucessfully cancelled.\n");
									}
								else cancel_output = -1; //To show that the cancellation is unsuccessful
								}
							else {printf("\n\t\tInvalid input.\n"); delay(1250); goto s4;}
							}
						}
					
					}
					
				else if(delta == 0)
					{
					printf("\n\tYour ticket will remain.\n\tHope you arrive at the station on time on ");
					display_date(current_user.departure);
					printf("\n\n\tWish you a safe and happy journey!!\n");
					}
				
				else 
					{
					printf("\n\t\tInvalid input.\n");
					delay(1250);
					goto start2;
					}		
				}
			
			
			if(cancel_output==-1) //Meaning cancellation has failed somewhere
				{
				fclose(temp_file);
				remove("user_details_temp.txt");
				free(data);
				}
			else cancel_output = delta;
			
			if(cancel_output) //Some seats have been cancelled
				{
				printf("\n\t\tRedirecting to payment gateway.....\n");
				delay(1250);
				if(payment(current_user))
					{
					data[pos] = current_user;
					for (int i = 0; i<count; i++)
						{		
						//Writing data to the temp text file 
						fprintf(
							temp_file, "%s %s %s %d %d %d/%d/%d %.2f %d %d %.2f\n", 
							data[i].id, data[i].name, data[i].train,
							data[i].train_number, data[i].ticket_number, 
							(data[i].departure)->d, (data[i].departure)->m,	(data[i].departure)->y,
							data[i].time, data[i].seats, data[i].status, data[i].charge
							);
						}
						
						fclose(temp_file);
						
						remove("user_details.txt"); //Deleting original file
						rename("user_details_temp.txt", "user_details.txt"); //Renaming the temporary file 
						free(data);
						waiting_list_writer();	
										
					}
				else
					{
					fclose(temp_file);
					remove("user_details_temp.txt");
					free(data);
					printf("\n\tPayment gateway error. Please try again after some time\n");
					delay(1000);
					}	
				}
			return cancel_output; //To show success if zero or +ve number and failure if -1	
			}		
		}
	}

//Status check
int check_status()
{ 	
	int count, flag = 0;
	
	FILE *user_file = fopen("user_details.txt", "r");
	
	user dummy;
	date* tpd = (date*)malloc(sizeof(date));
	
	fscanf(user_file, "%d", &count);
	
	user *all = (user*)malloc(sizeof(user) * count);
	
	for(int i =0; i< count; i++)
	{//Reading data from the text file into the array of structures of type user
	//id name trainname trainnumber ticketnumber date time seats status price
		fscanf(user_file, "%50s %50s %50s", dummy.id, dummy.name, dummy.train);
			fscanf(user_file, "%d %d %d/%d/%d %f %d %d %f", &dummy.train_number, &dummy.ticket_number, 
			&(tpd->d), &(tpd->m), &(tpd->y),
			&dummy.time, &dummy.seats, &dummy.status, &dummy.charge);	
			(dummy.departure) = tpd;
				
		if((strcmp(usrname, dummy.id) == 0)&&(strcmp(name, dummy.name)==0)) 
			{
			all[flag] = dummy;
			flag++;
			} 		
	}
	
	if(flag) 
		{
		all = (user*)realloc(all, flag*sizeof(user));
		printf("\n\n\t%d tickets were found:\n", flag);
		printf("\t%-10s| %-16s | %-15s | %-9s | %s | %s\n",
		"Train number", "Name", "Ticket Number", "Date", "Seats", "Status");
		
		for(int i = 0; i<flag; i++)
			{
			printf("\t%-12d| %-16s | %-15d | %d/%d/%d | %-5d | ",
				all[i].train_number, all[i].train, all[i].ticket_number,
				all[i].departure->d, all[i].departure->m, all[i].departure->y,
				all[i].seats);
						
			if(all[i].status == 0) printf("Confirmed\n");
			else if(all[i].status == -1) printf("Cancelled\n");
			else printf("%d seats in waiting\n", all[i].status);
			}
		}
		
	else printf("\n\t\tPlease verify your details as you don't appear to be in our database.\n");	

	free(all);
	fclose(user_file); 
	free(tpd);
	return flag;
}

//Payment							
int payment(user new)
{
	printf("\e[1;1H\e[2J");
	int c=0;
	pay p1;
	if(new.charge>0.0)
		{FILE * fPointer;
		fPointer = fopen("bankacc.txt","r");
		printf("\n");
		c = reade(fPointer,&p1,new.charge);
		printf("\n");	
		fclose(fPointer);
		printf("\n");
		if(c) ticket(p1, new);}
	else
		{
		refund(new);
		c++;}
	
	return c;
}

//Payment gateway					
int reade(FILE *fPointer, pay *p1, float amount)
{	int c = 0, chances = 0;
	char banks[][6]={"SBI","HDFC","Axis","ICICI","Union"};
	bank_select: printf("\e[1;1H\e[2J");
	printf("\n\t\t\t================================ Payment Gateway ================================\n");
	printf("\n\t\t\t\t-:Welcome to the Indian Railways smart payment gateway system:-\n");
	printf("\t\t\t\t\t\tCustomer service is our priority ;)\n");
	printf("\t\t\t\t\t\tRecognised by the Govt of India\n");
	printf("\t\t\tThe total amount to be paid by you is Rs.%.2f.",amount); 

	printf("Please select the desired bank:\n\t1) SBI\n\t2) HDFC\n\t3) Axis\n\t4) ICICI\n\t5) Union\n");//Choice of banks for UPI
	printf("\t\t\tAll your transactions on our smart payment gateway system are encrypted.\n\n\t\tEnter the bank no: ");
	__fpurge(stdin); scanf("%d",&(*p1).temp);
	if((*p1).temp<1 || (*p1).temp>5) {printf("\n\t\t\t\t\tInvalid Input\n"); delay(1000); goto bank_select;}
	
	printf("\n");
	start: if(chances>1) {printf("\e[1;1H\e[2J"); printf("\t\t\t\t\t\tTransaction failed\n"); delay(850); return c;}
	printf("\t\tYou have opted for a payment of %.2f through %s bank \n", amount, banks[(*p1).temp-1]);
	printf("\t\tEnter your mobile number for UPI payment: ");
	__fpurge(stdin); scanf("%ld", &(*p1).mob);
	printf("\t\tEnter your secure 4 digit OTP (Do not share it with anyone): ");
	//on a large scale implementation check the OTP from carrier
	__fpurge(stdin); scanf("%d", &(*p1).pin);
	printf("\n");

	long int b;
    	while(!feof(fPointer))
    	{
       	 	fscanf(fPointer,"%ld",&b);
        	if(b == (*p1).mob)
        		{
        		c++;
        		break;
        		}
  	}
	printf("\t\tLoading.....\n");
	delay(800);											
	if(c) printf("\t\t\t\tHurray!! The transaction is completed. Have a safe Journey!\n"); //Meaning successful payment
	else {delay(1500); printf("\e[1;1H\e[2J"); if(chances<1) printf("\t\t\tSorry, the payment failed. Please enter a valid phone number and OTP\n\n"); chances++; goto start;}		
    	return c; //0 Means failure
}


void ticket(pay p1, user new)//Used during reservation
{
	printf("\t\t\t\t\tThanks for banking with us!!\n");
	delay(1250);
	printf("\e[1;1H\e[2J");	
	printf("\t\t                           -:Here's your ticket:-\n\n");
	printf("\t\t================================== E-Ticket ==================================\n");
	printf("\t\t| Your balance amount will be credited back to you within three working days |\n");                        
	printf("\t\t|                   The Indian Railway E-Ticketing company                   |\n");
	printf("\t\t|                                                                            |\n");
	printf("\t\t|   Name : %-20s                                              |\n", new.name);
	printf("\t\t|   Mobile No: %ld                                                    |\n", p1.mob);	
	printf("\t\t|   Ticket Number: %-5d                                                     |\n", new.ticket_number);
	printf("\t\t|   Train: %-20s                                              |\n", new.train);
	printf("\t\t|   Train Number: %d                                                      |\n", new.train_number);
	printf("\t\t|   Time of departure: %-5.2f                                                 |\n", new.time);
	printf("\t\t|   Date of departure: "); display_date(new.departure);
	printf("                                            |\n");
	printf("\t\t|   Amount: %-9.2f                                                        |\n", new.charge);
	printf("\t\t|   Payment Status: Completed                                                |\n");
	printf("\t\t|                                                                            |\n");
	printf("\t\t|                               Happy Journey!!                              |\n");
	printf("\t\t|            For any further assistance, feel free to contact us!            |\n");
	printf("\t\t|                                                                            |\n");
	printf("\t\t================================== Thank You! ================================");
}

void refund(user new)//Used during cancellation 
{
	printf("\t\t                       -:Here's your refund voucher:-\n\n");
	printf("\t\t=============================== Refund Voucher ===============================\n");
	printf("\t\t| Your balance amount will be credited back to you within three working days |\n");                        
	printf("\t\t|                   The Indian Railway E-Ticketing company                   |\n");
	printf("\t\t|                                                                            |\n");
	printf("\t\t|   Username: %-20s                                           |\n", new.id);
	printf("\t\t|   Name : %-20s                                              |\n", new.name);
	printf("\t\t|   Train: %-20s                                              |\n", new.train);
	printf("\t\t|   Train Number: %d                                                      |\n", new.train_number);
	printf("\t\t|   Time of departure: %-5.2f                                                 |\n", new.time);
	printf("\t\t|   Date of departure: "); display_date(new.departure);
	printf("                                            |\n");
	printf("\t\t|   Refund Amount: %-9.2f                                                 |\n", -new.charge);
	printf("\t\t|   Payment Status: Processing                                               |\n");
	printf("\t\t|                                                                            |\n");
	printf("\t\t| Sad that you had to cancel bookings with us. Hope to see you travel soon!! |\n");
	printf("\t\t|            For any further assistance, feel free to contact us!            |\n");

}

int waiting_list_adder(user current_user)//To append to waiting list
{
	FILE* fp;
	
	fp = fopen("waiting_list.txt","a");	
	fprintf(fp, "%s %s %s %d %d %d/%d/%d %.2f %d %d %.2f\n", 
		current_user.id, current_user.name, current_user.train,
		current_user.train_number, current_user.ticket_number, 
		(current_user.departure)->d, (current_user.departure)->m, (current_user.departure)->y,
		current_user.time, current_user.seats, current_user.status, current_user.charge
		);
	fclose(fp);
	
	return 1;//Successfull
}

	
void waiting_list_writer() //To rewrite the waiting list 
{
	FILE *user_file, *waiting_file;
	
	user_file = fopen("user_details.txt", "r");
		
	int count; //Number of data entries
	fscanf(user_file, "%d", &count);
	
	user *data = (user*)malloc(sizeof(user) * count);
	user dummy; 
	
	date* tpd = (date*)malloc(sizeof(date));
	
	int limit = 0;
	for (int i = 0; i<count; i++)
		{
		fscanf(user_file, "%50s %50s %50s", dummy.id, dummy.name, dummy.train);
		fscanf(user_file, "%d %d %d/%d/%d %f %d %d %f\n", &dummy.train_number, &dummy.ticket_number, 
		&(tpd->d), &(tpd->m), &(tpd->y),
		&dummy.time, &dummy.seats, &dummy.status, &dummy.charge);	
		(dummy.departure) = tpd;
		
		if(dummy.status>0) 
			{
			data[limit] = dummy;
			limit++;
			}			
				
		}
	
	fclose(user_file);
	
	//Now writing waiting list
	waiting_file = fopen("waiting_list.txt", "w");		
	for(int i = 0; i<limit; i++)
	{
	fprintf
		(
		waiting_file, "%s %s %s %d %d %d/%d/%d %.2f %d %d %.2f\n", 
		data[i].id, data[i].name, data[i].train,
		data[i].train_number, data[i].ticket_number, 
		(data[i].departure)->d, (data[i].departure)->m,	(data[i].departure)->y,
		data[i].time, data[i].seats, data[i].status, data[i].charge
		);
	}		
	fclose(waiting_file);
}
