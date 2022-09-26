// needed to allow Visual Studio to work with scanf()
#define _CRT_SECURE_NO_WARNINGS

// required to enable use of scanf() and printf()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// this is a variable that will be automatically given a value
//		that represents the most recent error's number
extern int errno;

/* Constants that can be used inside any Function */
#define DISCOUNT_PERCENTAGE 0.20f // this means 20%
#define MENU_OPTION_BUY_CARS 'a'
#define MENU_OPTION_VIEW_SALES 'b'
#define MENU_OPTION_VIEW_CARS 'c'
#define MENU_OPTION_EXIT 'x' 
#define TRUE 1 
#define FALSE 0 
#define MAX_SALES 10
#define CSV_FILE "D:\\carData.csv"

// create a new data type called "bool" that can accept unsigned char type values
// min value: 0     max value: 255
#define bool unsigned char

/* Variables */
// unsigned is to eliminate any negative value
unsigned short correctInput = FALSE, carType;
unsigned short carsRemaininInEachModel[3] = { 25, 25, 25 };
unsigned int carsSold = 0;
int* carsSoldPtr;
float totalPrice = 0, totalSalesValue = 0, carPrices[] = { 20500.5f, 30500.5f, 40500.5f };
char companyEmployee[2];
char userChoice[2];
int numberOfCapturedValues;
int correctCarsInput;

// this array will hold the value of each sale transaction, including any discount given at the time
float totalSales[MAX_SALES];
// this will track how many car sales took place; it will be used as an array index as a data counter
int numberOfSales = 0;
// this array will hold the name of each customer, a maximum of 200 characters per name and an other array holding the car types
char customerNames[MAX_SALES][201], carTypes[][10] = { "Town car", "SUV", "sports car", "   " };

// this array will hold the number of cars sold each sale
unsigned short carAmountPerSale[MAX_SALES];
// this array will hold the type of the cars sold each sale; this relates to the car price
unsigned short typeOfCarPerSale[MAX_SALES];
// this array will hold discount information about each sale
bool discountGivenPerSale[MAX_SALES];

//struct
struct PersonVerification {
	char motherName[30];
	char fatherName[30];
};

// Files Variables & Constants
#define FILE_OPENED 0
#define FILE_CLOSED 1
#define FILE_ERROR 2

// will hold the file stream once opened
FILE* file;
unsigned char fileStatus = FILE_CLOSED;

/* Functions the program will be using */

void clearScreen() {
	// this tells the Command Prompt (i.e. the System's output mechanisn) 
	// to do something to clear the screen/console
	// this is purely for the user's benefit, to keep the screen clear of clutter
	system("cls");
}

void pauseProgram(char userChoice) {
	// give the user a chance to read the previous output, allow them to continue when ready
	// customise the output depending on the user's choice
	if (userChoice == MENU_OPTION_EXIT) {
		printf("\n\nPress Enter to Exit...");
	}
	else {
		printf("\n\nPress Enter to return to the Menu...");
	}

	// that's likely floating around the console window
	// however, if the userChoice value is an underscore character,
	// then this is only used when pausing after a file error occurred
	// so only one getchar() is needed
	if (userChoice != '_') {
		getchar();
	}

	getchar();
}

char getCharFromConsole(char message[201]) {
	char userInput;
	// prompt user using the value of the given message
	printf(message);
	// capture the character
	scanf("\n%c", &userInput);
	// return the captured value
	return userInput;
}

unsigned short getUnsignedShortFromConsole(char message[201]) {
	// will be used to store the captured number
	unsigned short userInput;

	// prompt user using the value of the given message
	printf(message);
	scanf("%hd", &userInput);

	// finally, return/export the value so it can be used by whatever statement called this Function
	return userInput;
}

// *str will allow this function to establish a connection between this str variable
// and the one that will exist at the time/place where the function is executed, thus any direct
// modification of *str in here will actually also modify the existing string variable, 
// as they will both be pointing to the same address in memory

void getStringFromConsole(char message[201], char* str) {
	// prompt user
	printf(message);
	// as we're now working with the pointer str, we don't need & here
	scanf("\n%[^\n]s", str);
}

void swapUnsignedShort(unsigned short* a, unsigned short* b) {
	unsigned short temp;
	// copy the value found at the pointer a's address into the newly created temp variable
	temp = *a;
	// copy the value found at the pointer a's address into the address represented by the pointer b
	*a = *b;
	// copy the value of the temp variable to the pointer b's address
	*b = temp;
}

void swapBool(bool* a, bool* b) {
	bool temp;
	// copy the value found at the pointer a's address into the newly created temp variable
	temp = *a;
	// copy the value found at the pointer a's address into the address represented by the pointer b
	*a = *b;
	// copy the value of the temp variable to the pointer b's address
	*b = temp;
}


/* START FILES FUNCTIONS */

// returns something that is not NULL if successful
FILE* createFile(char fileName[201]) {
	// open the file for writing
	file = fopen(fileName, "w");
	// then close it immediately if successful
	if (file != NULL) {
		fclose(file);
	}
	// return the pointer so that the success may be checked
	return file;
}

// mode is one of: "w" for writing, "r" for reading, "a" for appending 
// appending means writing starting from the end of the file, will not touch existing contents
// there are also other modes available, I encourage you to look these up
void openFile(char fileName[201], char mode[4]) {
	// attempt to open the file
	file = fopen(fileName, mode);

	// if file's value is NULL, then something happened...
	if (file == NULL) {

		// attempt to create it first and then check if it's still NULL, meaning
		//		that something still failed
		if (createFile(fileName) == NULL) {
			fileStatus = FILE_ERROR;

			// will produce the output: Error opening D:\data.csv: No such file of directory
			printf("Error opening %s: %s", fileName, strerror(errno));
		}
		else {
			// we managed to create the file ... so now ...
			// execute this function again with the same parameters
			//		because it should now succeed
			openFile(fileName, mode);
		}

	}
	// otherwise, all OK
	else {
		fileStatus = FILE_OPENED;
	}
}

// should be called once the operations on 
//		the currently opened file have concluded
void closeFile() {
	// only attempt to close the file if it's already open
	if (fileStatus == FILE_OPENED) {
		fclose(file);
		fileStatus = FILE_CLOSED;
	}
}

// will return an array of strings representing the 
//		customer names stored in the file
void readDataFromFile() {
	// will keep track of how many lines were read from the file
	int lineCounter = 0;

	// this is an infinite loop, we'll manually stop it once we reach the end of the file 
	while (1) {

		unsigned short carAmountPerSaleValue = 0, typeOfCarPerSaleValue = 0, discountGivenPerSaleValue = 0;
		char customerNameValue[201] = "";

		int scanResult = fscanf(
			file, // the file stream
			"%hd,%hd,%hd,%[^\n]s%hd", // the format of the line
			&carAmountPerSaleValue, // the variables, one for each placeholder in the format above
			&typeOfCarPerSaleValue,
			&discountGivenPerSaleValue,
			&customerNameValue
		);

		// if we reached the end of the file
		if (scanResult == EOF) {
			// then, stop the loop
			break;
		}

		// add the bits of data that were read above into the correct arrays
		carAmountPerSale[lineCounter] = carAmountPerSaleValue;
		typeOfCarPerSale[lineCounter] = typeOfCarPerSaleValue;
		// also cast (convert) the discountGivenPerSaleValue from unsigned short to a bool type
		//		before putting it in the discountGivenPerSale array
		discountGivenPerSale[lineCounter] = (bool)discountGivenPerSaleValue;
		// need to use strcpy here because we're working with strings
		strcpy(customerNames[lineCounter], customerNameValue);

		// increment the lineCounter, ready for next line that might be read
		lineCounter++;
	}

	// make sure the numberOfSales variable is also aware of how many sales are available after the above operation
	numberOfSales = lineCounter;
}

void getDataFromFile() {
	openFile(CSV_FILE, "r");

	if (fileStatus == FILE_OPENED) {
		readDataFromFile();
	}
	else if (fileStatus == FILE_ERROR) {
		printf("There was an error trying to read from the file %s.", CSV_FILE);
		// this function requires a char value, so we give it one 
		//		that'll tell it we're using it because of a file error
		//			see the function body, it's been updated to check for
		//			this underscore character
		pauseProgram('_');
	}

	closeFile();
}

void writeDataToFile() {
	// loop through every sale
	for (int i = 0; i < numberOfSales; i++) {

		// this string will be built up bit by bit before being written to the opened file
		char line[201];
		// this string will contain the converted int value
		char data[50];

		// convert the unsigned short value into a string, put it in the data string variable; 
		//		the "10" refers to base 10, which is what regular numeric values are written in
		//		e.g. saying you're 22 years old means that 22, in this case, is written in base 10
		//			because that's how we people use numbers by default
		// also cast the unsigned short value into an int type before converting
		_itoa((int)carAmountPerSale[i], data, 10);
		// add the amount of cars to the line; first time we use strcpy, 
		//		then strcat to add more to the string
		strcpy(line, data);
		// add a comma to separate this value from the next on this line
		strcat(line, ",");

		// convert the value into a string
		_itoa((int)typeOfCarPerSale[i], data, 10);
		// add the type of car to the line
		strcat(line, data);
		// add a comma to separate this value from the next on this line
		strcat(line, ",");

		// convert the value into a string
		_itoa((int)discountGivenPerSale[i], data, 10);
		// add the discount given to the line
		strcat(line, data);
		// add a comma to separate this value from the next on this line
		strcat(line, ",");
		
		// add the customer name to the line
		strcat(line, customerNames[i]);

		// write line to file
		fprintf(file, line);

		// only add a newline character if we're not yet writing the very last
		//		line in the file
		if (i < numberOfSales - 1) {
			fprintf(file, "\n");
		}
	}
}

void saveDataToFile() {
	openFile(CSV_FILE, "w");

	if (fileStatus == FILE_OPENED) {
		writeDataToFile();
	}
	else if (fileStatus == FILE_ERROR) {
		printf("There was an error trying to write to the file %s.", CSV_FILE);
		// this function requires a char value, so we give it one 
		//		that'll tell it we're using it because of a file error
		//			see the function body, it's been updated to check for
		//			this underscore character
		pauseProgram('_');
	}

	closeFile();
}

/* END FILES FUNCTIONS */

void menu_greetCustomer() {
	// greet the user
	printf("Welcome to the Car Sales office!\n\n");
}

void menu_showMenu() {
	// present the various actions the user can choose from
	printf("Menu:\n");
	printf("%c. Buy Car\n", MENU_OPTION_BUY_CARS);
	printf("%c. View Sales Stats\n", MENU_OPTION_VIEW_SALES);
	printf("%c. View Cars\n", MENU_OPTION_VIEW_CARS);
	printf("%c. Exit\n\n", MENU_OPTION_EXIT);
}

void menu_showCarTypes() {
	// calculate the number of car types by asking the carPrices
	// array about how many bytes it holds in memory and dividing that
	// result by the number of bytes a float data type holds in memory
	// the division result will be the number of car types (3 in this case)
	int numberOfCars = sizeof(carPrices) / sizeof(float);

	// show the user the types of cars
	printf("\nCar Types:\n");

	for (int i = 0; i < numberOfCars; i++) {
		printf("%d - %s\n", i, carTypes[i]);
	}
}

float menu_applyDiscount(float currentPrice) {
	// multiply the price value by the discount percentage
	// this is: 1 - DISCOUNT_PERCENTAGE (0.20) => 0.80 (or, 80%)
	// so, we calculate the discounted value by reducing it to 80% of its value
	return currentPrice * (1 - DISCOUNT_PERCENTAGE);
}

bool menu_checkIfDiscountIsNeeded(char companyEmploee[2]) {
	// check if we need to give a discount
	companyEmployee[0] = getCharFromConsole("Are you an eployee in this company? (press 'y' for yes or 'n' for no) ");
	if (companyEmployee[0] == 'Y' || companyEmployee[0] == 'y') {
		// return the TRUE value to indicate that discount is to be given
		return TRUE;
	}

	// return the FALSE value to indicate that no discount is to be given
	return FALSE;
}

void menu_printDiscountOutcome(bool giveDiscount) {
	// inform the user about the discount outcome
	// always a good idea to separate the calculation from the output messages, makes it easier to work with the code
	switch (giveDiscount) {
		// a discount was applied
		// please note that no curly brackets { } are needed
	case TRUE:
		printf("\nYou get a discount!");
		break;
		// no discount was applied
	case FALSE:
		printf("\nNo discount given.");
		break;
	}
}

void menu_buyCars() {
	printf("Buy cars:\n\n");

	// updating cars stock
	unsigned short carsAvailable = carsRemaininInEachModel[0] + carsRemaininInEachModel[1] + carsRemaininInEachModel[2];

	// check if we have at least 1 car available
	if (carsAvailable > 0) {
		printf("There are %hd cars available.\n\n", carsAvailable);
	}
	else {
		printf("Sorry, there are no more cars available.");
		// this allows us to terminate/stop the Function early
		return;
	}

	/* Variables - these are only used inside menu_buyCars() Function and nowhere else */
	// don't need negative values; these are generally related to one sale, used when purchasing cars
	unsigned short carsNeeded = 0,companyEmployee;
	bool giveDiscount = FALSE;
	float totalPrice = 0;

	// get the "getStringFromConsole()" Function to directly store the customer's name 
	// into the "customerNames" array at position "numberOfSales"
	//		=> we need the & operator to get the address of that array's position sent to the Function
	//			so that it can directly put the value at that address in memory
	getStringFromConsole("What is your name? Name: ", &customerNames[numberOfSales]);

	// now, we'll determine what type of car the user wants; this will influence the price
	menu_showCarTypes();

	//validating input
	correctInput = FALSE;
	do
	{
		// prompting the user
		printf("Please choose an option ");
		numberOfCapturedValues = scanf("%hd", &carType);

		if (carType > 2 || carType < 0) {
			printf("\nPlease enter numbers between 0 and 2\n\n");
		}
		else {
			// All good, set the value of correctInput to 1 to indicate everything is ok
			correctInput = TRUE;
		}

		// discard any characters in the buffer 
		while (getchar() != '\n');

		// repeat as long as we have no valid input i.e. until correctInput becomes 1
	} while (correctInput == FALSE);

	// Printing how many cars are remaining from the spesific model
	printf("%d cars of this model are remaining\n\n", carsRemaininInEachModel[carType]);

	// check if we have enough cars and validating input
	correctCarsInput = FALSE;
	do
	{
		// prompt the user
		printf("How many cars do you need ");
		numberOfCapturedValues = scanf("%hd", &carsNeeded);

		if (numberOfCapturedValues == 0)
		{
			printf("\nInvalid input please try again entering only whole numbers.\n");
		}
		else {
			// this is to show if the input is correct
			correctCarsInput = TRUE;
		}


		// check if we have enough cars
		if (carsRemaininInEachModel[carType] < carsNeeded) {
			printf("Sorry, there are fewer cars remaining than you require.");
			carsNeeded = getUnsignedShortFromConsole("\nHow many cars do you need ");
		}


		// this is reseting the buffer
		while (getchar() != '\n');




	} while (correctCarsInput == FALSE);

	// Updating carsRemaining-in-each-model 
	carsRemaininInEachModel[carType] = carsRemaininInEachModel[carType] - carsNeeded;

	// also record the type of these cars in the typeOfCarPerSale array
	typeOfCarPerSale[numberOfSales] = carType;

	// calculate total price for this sale by using the correct car type's price
	totalPrice = carsNeeded * carPrices[carType];

	// also record the number of cars needed in the carAmountPerSale array
	carAmountPerSale[numberOfSales] = carsNeeded;

	// update number of cars available by subtracting from it the amount of cars needed
	carsAvailable -= carsNeeded;

	// ask for the user's age
	companyEmployee = 1;

	// calculate the value of giveDiscount by making use of this Function and the user's age
	giveDiscount = menu_checkIfDiscountIsNeeded(companyEmployee);


	// actually apply the discount if needed
	if (giveDiscount == TRUE) {
		totalPrice = menu_applyDiscount(totalPrice);
	}

	// also record the outcome in the discountGivenPerSale array
	discountGivenPerSale[numberOfSales] = giveDiscount;

	// inform the user about the discount outcome
	menu_printDiscountOutcome(giveDiscount);

	// Asking for some verification info //* I tried to use structures for capturing my sales stats but after many days i reached to a point where i wasn't able to make this project work as much as without them *//
	printf("\n\nThere is also one last step!\nYou need to enter your mother's and father's name for verification reasons\nPlease note that these info will be directly sent for verification and will be not stored to any of our servers\n");
	struct PersonVerification verification;
	printf("Please enter your Father's name: ");
	scanf("\n%s", &verification.fatherName);
	printf("VALID NAME(%s)\n\n", verification.fatherName);
	printf("Please enter your Mother's name: ");
	scanf("\n%s", &verification.motherName);
	printf("VALID NAME(%s)\n\n", verification.motherName);

	// present the outcome
	printf("\n\nThank you.\n");
	printf("You have bought %hd cars.\n", carsNeeded);
	printf("Total cost is %f GBP.\n", totalPrice);
	printf("\nThere are %hd cars remaining.", carsAvailable);

	// finally, add 1 to the numberOfSales counter
	numberOfSales++;
}

void sortArraysByNumberOfCarsSoldPerSale() {

	// set up a loop that gives us an index "i" for accessing 
	//		between the (first) and (second to last) positions that contain values
	for (int i = 0; i < numberOfSales - 1; i++) {

		// set up a loop the gives us an index "j" for accessing 
		//		between the (first immediately after i) and (last) positions that contain values
		// this loop will execute fully, from start to finish, 
		//		every time the above for loop begins a new iteration
		for (int j = i + 1; j < numberOfSales; j++) {

			// check if the value at position i is greater than the value at position j
			if (carAmountPerSale[i] < carAmountPerSale[j]) {

				// if so, swap those two values in the carAmountPerSale array
				swapUnsignedShort(&carAmountPerSale[i], &carAmountPerSale[j]);

				// also swap the two values at those same positions in the typeOfCarPerSale array
				swapUnsignedShort(&typeOfCarPerSale[i], &typeOfCarPerSale[j]);

				// and in the discountGivenPerSale array
				swapBool(&discountGivenPerSale[i], &discountGivenPerSale[j]);

				// and lastly, do the same in the customerNames array

				// using a function to perform this swap would complicate this program a bit too much, but we 6
				//		can do the swap directly here quite easily
				char temp[201];
				// copy string from position i to the newly created temp variable
				strcpy(temp, customerNames[i]);
				// copy string from position j into position i
				strcpy(customerNames[i], customerNames[j]);
				// copy string from temp into position j
				strcpy(customerNames[j], temp);

			} // end of "if" statement

		} // end of second "for" loop

	} // end of first "for" loop

}

// specialised function that will display a row representing the sales data at a given array position
void printSalesDataAtPosition(int position) {

	// here, we're using the value found at position i in the typeOfCarPerSale array
	//	as a position for the carPrices array to find out the price of the type of car
	//	the customer purchased this sale
	int typeOfCar = typeOfCarPerSale[position];
	// calculate the current sale's price
	float price = carAmountPerSale[position] * carPrices[typeOfCar];

	// calculate the display text for the discount; this is for the user's benefit
	char discountGivenText[4];
	// if a discount was given, then...
	if (discountGivenPerSale[position] == TRUE) {
		// set the display text to Yes
		strcpy(discountGivenText, "Yes");
		// and also modify the price
		price *= (1 - DISCOUNT_PERCENTAGE);
	}
	else {
		// set the display text to No
		strcpy(discountGivenText, "No");
	}

	// show the user the information related to each sale
	// I've broken down the "printf()" statement into several lines, so we
	//	can read it more easily, but it's still just one statement
	printf("Sale Index: %d | Sale Amount: %f | Type of Car: %s | "
		"Car Price: %f | Number of Cars: %hd | "
		"Discount Given: %s | Customer Name: %s\n",
		// please note that the above are three separate strings that will be glued together by the program
		position, price, carTypes[typeOfCar],
		carPrices[typeOfCar], carAmountPerSale[position],
		discountGivenText, customerNames[position]);

}

void menu_viewSales() {

	unsigned short carsAvailable = carsRemaininInEachModel[0] + carsRemaininInEachModel[1] + carsRemaininInEachModel[2];

	// sort the arrays before showing the sales data
	sortArraysByNumberOfCarsSoldPerSale();

	/* Variables - these are only used inside menu_viewSales() Function and nowhere else */
	// these two will contain the sum the total sales price and total number of cars sold for all sales
	float totalSalesValue = 0;

	

	printf("All Sales Data:\n\n");

	// set up a for loop that will execute the block of code as many times as
	// indicated by the numberOfSales variable's value 
	for (int i = 0; i < numberOfSales; i++) {

		// here, we're using the value found at position i in the typeOfCarPerSale array
		//	as a position for the carPrices array to find out the price of the type of car
		//	the customer purchased this sale
		int typeOfCar = typeOfCarPerSale[i];
		// calculate the current sale's price
		float price = carAmountPerSale[i] * carPrices[typeOfCar];

		// use this function to print the row of sales data at position i
		printSalesDataAtPosition(i);

		// also add up the total sales value and total number of cars sold
		totalSalesValue += price;
		carsSold += carAmountPerSale[i];
		carsSoldPtr = &carsSold;
		

	}

	// also display some overall statistics
	printf("\n%hd cars have been sold with a total value of %f GBP. There are %hd cars unsold.\n",
		*carsSoldPtr, totalSalesValue, carsAvailable- *carsSoldPtr);
}


void menu_viewCars() {
	unsigned short carsAvailable = carsRemaininInEachModel[0] + carsRemaininInEachModel[1] + carsRemaininInEachModel[2];
	printf("\n%d Town cars remaining , %d SUV cars remaining , %d sports cars remaining ", carsRemaininInEachModel[0], carsRemaininInEachModel[1], carsRemaininInEachModel[2]);
	//* Here at first i created a pointer called carsSoldPtr and replaiced it with carsSold but for some reason the pointer was returning NULL.(and now carsSold does not work because it is outside the function)
	printf("\n\nThere is a total of %hd cars remaining.", carsAvailable - carsSold);
}

void menu_exit() {
	printf("Thank you for using this Car Sales program. Bye-bye!");

	// ensure no data is lost, save it to the file
	saveDataToFile();
}

/* Starting Point of our Program */
void main() {

	// see if there's any data in the file
	getDataFromFile();

	/* Variables - these are only used inside main() Function and nowhere else */
	// this will hold the user's choice when presented with the menu
	char userChoice;

	/* The rest of our Program */

	// loop/repeat the code between the immediately following curly brackets
	// for as long as the CONDITION found between the while()'s brackets 
	// (see towards the end of void main() { ... }) evaluates to TRUE; in our case, 
	// we'll keep repeating for as long as the user does not choose to Exit 
	// i.e. pushes 'x' when asked to choose their next action
	do {
		clearScreen();

		menu_greetCustomer();
		menu_showMenu();

		// the return value supplied by the Function once it finishes will be stored in userChoice
		userChoice = getCharFromConsole("Please choose one: ");

		// for the user's benefit...
		clearScreen();

		// next, we check the user's choice and make a decision based on that
		switch (userChoice) {
		case MENU_OPTION_BUY_CARS:
			menu_buyCars();
			break;

		case MENU_OPTION_VIEW_SALES:
			menu_viewSales();
			break;

		case MENU_OPTION_VIEW_CARS:
			menu_viewCars();
			break;

		case MENU_OPTION_EXIT:
			menu_exit();
			break;
		}

		// give the user a chance to read the previous output, allow them to continue when ready
		// this does not naturally exist in C
		pauseProgram(userChoice);

	} while (userChoice != MENU_OPTION_EXIT);

	// again, keep the screen clear
	clearScreen();

	// a good sentence almost never fails to warm the heart...
	printf("\n\nHave a good day!\n\n");
}










