/** GROUP_3_FINAL_PROJECT_BLACK_JACK.cpp
 *	@author		Thom MacDonald
 *	@MODIFIED BY: Amar Al-Adil, Tyler Nankishore, Yuan Gao (Peter)
 *	@version	2014.02
 *	@date:		December 9, 2016
 *  @description:	Create a black jack game for the Final Project
*/

#include <iostream>		
#include <stdexcept>
#include "GROUP_3_FINAL_PROJECT_DECK.h"
#include <ctime>
#include <math.h> 
#include <fstream>
#include <iostream>

using namespace std;

// Container class
class StandardDeck
{
	private:
		StandardPlayingCard * myArray;  // pointer to a dynamically-allocated c-array 
		int mySize;		// the size of the c-array
		
	public:
		StandardDeck(int size); // constructor
		virtual void Initialize(int size); 	// Can be called on construction or any time after

		~StandardDeck() {  delete [] myArray;} // destructor
		StandardDeck(const StandardDeck &source); // copy constructor
		StandardDeck& operator=(const StandardDeck &source); // assignment operator	

		int Size() const {return mySize;} // mySize accessor
		
		
		string ShowCards(bool firstCard) const; // an accessor to show info about the state of the object		
		
		void AddCard(StandardPlayingCard newCard); // for adding an element to the back/end
		StandardPlayingCard RemoveCard();				 // for removing an element from the back/end

		void Resize(int newSize);	// re-sizing mutator

		StandardPlayingCard DrawNextCard();	 // for removing an element from the front/start
		StandardPlayingCard PlayerSum(); //Player's hand value
		int ArtificialTotalSum(); // Calculate the AI's hand value
		
		string CardsRemaining() const;	//Shows how much cards are remaining

		StandardPlayingCard DrawRandomCard(); //Removes a random card in the object
		
		StandardPlayingCard Shuffle();	//Reorganizes the deck
		
		StandardDeck operator=(StandardDeck &hand); //Operator that copies the card amount
		
		StandardPlayingCard hitUntilStand(StandardDeck Hand, StandardDeck Deck, int count); //Lets the AI hit until they go over th 17 soft cap
		
		int PlayerHitUntilStand(StandardDeck Hand, StandardDeck Deck, int count, int number); // The AI player's can hit until they stand
}; 

//Different methods that will be used in the program
void CheckBust(char Play, StandardDeck deck); //Check if the user or dealer gets bust, over 21.
void checkAiWins(StandardDeck deck); //Checks AI
void blackJack( char Play, StandardDeck deck); //Checks if user or dealer gets a total of 21, which is black jack
void ShowRules();// Shows the rules
bool WantToPlayAgain(char&); // checks if user wants to play again
bool UserWantsToDraw(char&); //Asks if user wants to hit
void whoWins(int player, int dealer); //Check if the player or dealer wins
void OthersWhoWins(int player, int dealer, int playerNumber); //Check AI Wins / Lose
void TheGame(); //Main Game function
void scoreboard(int playerScore, int dealerScore); //Shows a scoreboard


/*********************************************************
 *          GLOBAL VARIABLES                             *
 *********************************************************/
int const CARD_DECK_SIZE = 52; //The total deck size
int PlayerCardsValue = 0; // Player's hand value
int DealerCardsValue = 0; // Dealer Hand value
int playerChip = 50; // Number of chips given to user
int playerBet = 0; // Gets the number amount of
int numberOfDeck = 0;

//Number of AI allowed
int arrayPlayers[4];
//For user to input to know how many AI players will be present
int otherPlayers = 0;

//AI player hands that they would hold
StandardDeck player1(0);
StandardDeck player2(0);
StandardDeck player3(0);
StandardDeck player4(0);

//Game logs
ofstream gameplayLog;
// current date and time
time_t currentTime = time(0);

int main()
{	

	gameplayLog.open("gameplay_log.txt", ios::app); //Appends the file
	
	currentTime = time(0); // Gets current time
	// converts it to a string form
   	char* datetime = ctime(&currentTime);
	
	gameplayLog << "Progam has started " << datetime << endl; 
	
	gameplayLog.close(); // Closes the file for later
	
	//shows the rules for the user
	ShowRules();
	
	bool inputIsRight = false;
	int input;
	//Loops for AI players
	do
	{
		//Asks the user for the amount of chips being wagered 
		cout << "How many AI players do you want to play with you? (Between 0 - 4)" << endl;
		cin >> input;
		
		//Input was not an integer, therefore it loops until an integer is entered
		while(cin.fail()) 
		{
	        cout << "Must be a number: " << endl;
	        cin.clear();
	        cin.ignore(256,'\n');
	        cin >> input;
	    }
	    //Checks if user enters within the amount they currently have
	    if (input > 4) 
		{ 
			cout << "Must be below 4 extra players." << endl;
		}
		//Checks if the user enters no chip or a negative 
		else if (input < 0)
		{
			cout << "Must have a minimum of zero AI players." << endl;
		}
		else //User enters not out of range of current chip amount
		{
			inputIsRight = true;
			otherPlayers = input;
		} 
	    
	}while (inputIsRight == false); //end AI players
	
	
	bool inputIsRightDeck = false;
	int inputNumberOfDeck;
	//Loops for number of deck
	do
	{
		//Asks the user for the number of decks used in the blackjack
		cout << "How many decks do you want to use? (Either 1, 2, 4, 8 decks can be used)" << endl;
		cin >> inputNumberOfDeck;
		
		//Input was not an integer, therefore it loops until an integer is entered
		while(cin.fail()) 
		{
	        cout << "Must be a number: " << endl;
	        cin.clear();
	        cin.ignore(256,'\n');
	        cin >> inputNumberOfDeck;
	    }
	    
	    //Checks if user enters within the amount they currently have
	    if (inputNumberOfDeck > 8) 
		{ 
			cout << "Must be either 1, 2, 4, 8 decks being used in the game." << endl;
		}
		//Checks if the user enters no chip or a negative 
		else if (inputNumberOfDeck <= 0)
		{
			cout << "Must have a minimum of one deck." << endl;
		}
		else //User enters not out of range of current chip amount
		{
			//Has to be either one of these options to choose from
			if (inputNumberOfDeck == 1 || inputNumberOfDeck == 2 || inputNumberOfDeck == 4 || inputNumberOfDeck == 6 || inputNumberOfDeck == 8) 
			{
				inputIsRightDeck = true;
				numberOfDeck = inputNumberOfDeck;
			}
			else
			{
				cout << "Must be either 1, 2, 4, 8 decks being used in the game." << endl;
			}
			
		} 
	    
	}while (inputIsRightDeck == false); //end Number of deck 
	
	cout << "Starting chips is " << playerChip << " ($5 chips)"<< endl;
	
	//The game starts
	TheGame();
	
	// END-OF-PROGRAM
	cout << endl << endl;
	return 0;
} // end of main()

//The Main games starts here
void TheGame()
{
	try
	{
		//Removes any cards saved inside the AI players
		while(player1.Size() > 0)
		{
			// Remove elements, shrinking the container
			player1.RemoveCard();
		}
		while(player2.Size() > 0)
		{
			// Remove elements, shrinking the container
			player2.RemoveCard();
		}
		while(player3.Size() > 0)
		{
			// Remove elements, shrinking the container
			player3.RemoveCard();
		}
		while(player4.Size() > 0)
		{
			// Remove elements, shrinking the container
			player4.RemoveCard();
		}
		
		char Play = 'N';
		
		int size = CARD_DECK_SIZE * numberOfDeck ;// // Is the max number of cards in a deck
		int betGiven = 0;
		srand(time(NULL)); // seed the random number generator
	    								
		StandardDeck player(0); // Sets the player's hand
		StandardDeck dealer(0); // Sets the dealer's hand
		
		do
		{
			char Draw = 'H';
			
			//Sets the cards again
			StandardDeck cards(size);

			//Shuffles the deck
			cards.Shuffle();
			
			bool betIsRight = false;
			//Loops for the betting input
			do
			{
				//Asks the user for the amount of chips being wagered 
				cout << "How many chips do you want to bet?" << endl;
				cin >> betGiven;
				
				//Input was not an integer, therefore it loops until an integer is entered
				while(cin.fail()) 
				{
			        cout << "Must be a number below the amount of chips you have: " << playerChip << endl;
			        cin.clear();
			        cin.ignore(256,'\n');
			        cin >> betGiven;
			    }
			    //Checks if user enters within the amount they currently have
			    if (betGiven > playerChip) 
				{ 
					cout << "Insufficient Chip: Please be below the ammount you currently have." << endl;
				}
				//Checks if the user enters no chip or a negative 
				else if (betGiven <= 0)
				{
					cout << "Must have a minimum of one chip to bet." << endl;
				}
				else //User enters not out of range of current chip amount
				{
					betIsRight = true;
					playerBet = betGiven;	
				} 
			    
			}while (betIsRight == false); //end betting
			
			// Cards are handed out twice to each player
			for(int i = 0; i < 2; i++)
			{
				player.AddCard(cards.DrawNextCard()) ;
				dealer.AddCard(cards.DrawNextCard()) ;
				//If the user chooses more than zero for the other AI players
				if (otherPlayers >= 1) {
					player1.AddCard(cards.DrawNextCard() );
				}
				if (otherPlayers >= 2) {
					player2.AddCard(cards.DrawNextCard() );
				}
				if (otherPlayers >= 3) {
					player3.AddCard(cards.DrawNextCard() );
				}
				if (otherPlayers >= 4) {
					player4.AddCard(cards.DrawNextCard() );
				}				
			}
			
			//Shows the Player's hand for the user
			cout << "Player Hand: " << endl;
			cout << player.ShowCards(false) << endl;
			
			//Show the Dealer's hand for the user but without showing the first card
			cout << "Dealer has: " << endl;
			cout << dealer.ShowCards(true) << endl; //Show a list of the cards
			
			gameplayLog.open("gameplay_log.txt", ios::app); //Appends the file
	
			currentTime = time(0); // Gets current time
			// converts it to a string form
		   	char* datetime = ctime(&currentTime);
			
			gameplayLog << "Player's hand: " << player.ShowCards(false) << datetime << endl; 
			
			gameplayLog << "Dealer's hand: " << dealer.ShowCards(false) << datetime << endl; 
			
			//Logs all the other players, if they're playing
			if (otherPlayers >= 1) {
				currentTime = time(0); // Gets current time
				// converts it to a string form
		   		char* datetime = ctime(&currentTime);
				gameplayLog << "Player[1] hand: " << player1.ShowCards(false) << datetime << endl; 
			}
			if (otherPlayers >= 2) {
				currentTime = time(0); // Gets current time
				// converts it to a string form
		   		char* datetime = ctime(&currentTime);
				gameplayLog << "Player[2] hand: " << player2.ShowCards(false) << datetime << endl; 
			}
			if (otherPlayers >= 3) {
				currentTime = time(0); // Gets current time
				// converts it to a string form
		   		char* datetime = ctime(&currentTime);
				gameplayLog << "Player[3] hand: " << player3.ShowCards(false) << datetime << endl; 
			}
			if (otherPlayers >= 4) {
				currentTime = time(0); // Gets current time
				// converts it to a string form
		   		char* datetime = ctime(&currentTime);
				gameplayLog << "Player[4] hand: " << player4.ShowCards(false) << datetime << endl; 
			}	
			
			gameplayLog.close(); // Closes the file for later
			
			
			player.PlayerSum() 	;//Calculate the user's total card value
			DealerCardsValue = dealer.ArtificialTotalSum() ;//Calculate the dealer's total value
			
			blackJack(Play, cards);//Checks if either user has gotten black jack
			
			// Asks the user if they want to hit for another card
			while (UserWantsToDraw(Draw))
			{
				player.AddCard(cards.DrawNextCard()) ;//Adds new card
				
				cout << "Player Hand: " << endl;
				cout << player.ShowCards(false) << endl;
				
				gameplayLog.open("gameplay_log.txt", ios::app); //Appends the file
	
				currentTime = time(0); // Gets current time
				// converts it to a string form
			   	char* datetime = ctime(&currentTime);
				
				gameplayLog << "Player Hits for and now has: " << player.ShowCards(false) << datetime << endl; 
				
				gameplayLog.close(); // Closes the file for later
				
				player.PlayerSum() 	;// Calculate the user's hand value
				DealerCardsValue = dealer.ArtificialTotalSum() ;// Calculate the dealer's
				
				//Check to see if anyone lost or won
				CheckBust(Play, cards);
				blackJack(Play, cards);
			}
			dealer.hitUntilStand(dealer,cards, 0) ;
			//Check to see if anyone lost
			CheckBust(Play, cards); //Check for any over 21
			blackJack(Play, cards); // check if any got 21
			
			whoWins(PlayerCardsValue, DealerCardsValue); //Check if player or dealer wins
			checkAiWins(cards); // checks if AI wins or not
		}
		while (WantToPlayAgain(Play));//Asks if user wants to play
		
	}
	catch(const exception& ex)
	{
		cerr << endl << ex.what() << endl;
	}
}

// constructor
StandardDeck::StandardDeck(int size)
{
	// Set the pointer to zero
	myArray = 0;
	// Call the initialization method
	Initialize(size);
}

// to initialize or re-initialize the object
void StandardDeck::Initialize(int size)
{
	// if the pointer is not 0, memory has been allocated
	if(myArray != 0)
		// delete the existing memory
		delete [] myArray;
		
	// Allocate a new array dynamically on construction
	myArray = new StandardPlayingCard [size]; // may throw bad_alloc exception
	mySize = size; // set the size
	
	int i=0;
	for(int cardNumber = 0; cardNumber < mySize; cardNumber++)
	{
		// give the playing card a random suit and rank
		myArray[cardNumber].setSuit(i / (StandardPlayingCard::MAX_RANK)); //{ "Spades", "Hearts", "Diamonds", "Clubs" }
		myArray[cardNumber].setRank(1 + i % (StandardPlayingCard::MAX_RANK)); //{ "Not Used", "Ace", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King"}
		
		//Resets the number for the cards iteration
		if (i == 51 )
		{
			i = 0;
		}
		else
		{
			i++;
		}			
	}
				
}

// copy constructor
StandardDeck::StandardDeck(const StandardDeck &source)
{
	// Allocate a new array dynamically on construction
	mySize = source.mySize; // set the size
	myArray = new StandardPlayingCard [mySize]; // may throw bad_alloc exception
	// Initialize each element to the corresponding playing card in the source object
	for(int i = 0; i < mySize; i++)
		myArray[i] = source.myArray[i];
}

// assignment operator
StandardDeck& StandardDeck::operator=(const StandardDeck &source)
{
	delete [] myArray; // free up the old memory
	// Allocate a new array dynamically on construction
	mySize = source.mySize; // set the size
	myArray = new StandardPlayingCard [mySize]; // may throw bad_alloc exception
	// Initialize each element to the corresponding playing card in the source object
	for(int i = 0; i < mySize; i++)
		myArray[i] = source.myArray[i];
	
	return *this;
} 

// Sets card
StandardDeck StandardDeck::operator=(StandardDeck &source)
{
	//delete [] myArray; // free up the old memory
	// Allocate a new array dynamically on construction
	mySize = 2; // set the size
	myArray = new StandardPlayingCard [mySize]; // may throw bad_alloc exception
	// Initialize each element to the corresponding playing card in the source object
	for(int i = 0; i < 2; i++)
		myArray[i] = source.myArray[i];
	
	return *this;
} 

// an accessor to show a list of the cards in the deck
string StandardDeck::ShowCards(bool firstCard) const
{
	stringstream show;
	show << "\n==================== Show Current Cards ====================" << endl;
	for(int i = 0; i < mySize; i++)
	{
		//Flips first card down if given that it is true
		if (firstCard == true && i == 0)
		{
			show << "[" << i << "]: ************************" << endl;
		}
		else
		{
			show << "[" << i << "]: " << (string) myArray[i] << endl;
		}
		
	}
	show << "\n============================================================" << endl;
	return show.str();
}

// re-sizing mutator
void StandardDeck::Resize(int newSize)
{
	StandardPlayingCard * newArray = new StandardPlayingCard[newSize]; // throws bad_alloc on failure
	int elementsToCopy = (mySize < newSize)? mySize : newSize; // whichever is smaller
	
	// Copy each each existing playing card to the new array
	for(int i = 0; i < elementsToCopy; i++)
		newArray[i] = myArray[i];

	// Set the new size
	mySize = newSize;
	
	// Delete the old array
	delete [] myArray;
	
	// Set the myArray pointer to the new array
	myArray = newArray;
}

// for removing an element from the start/front
StandardPlayingCard StandardDeck::DrawNextCard()
{
	// If the container is empty, throw an error
	if(mySize <= 0)
		throw out_of_range("Cannot .DrawNextCard() an empty container. ");
	// preserve the first value
	StandardPlayingCard card = myArray[0];
	
	// Shift all the existing element value forward by 1 spot:
	for(int moveTo = 0; moveTo < mySize - 1; moveTo++)
	{
		myArray[moveTo] = myArray[moveTo + 1];
	}

	// Make the container smaller by 1
	Resize(mySize - 1);
	// return the former last value
	return card;
}

// Finds the total of player's hand value
StandardPlayingCard StandardDeck::PlayerSum()
{
	PlayerCardsValue = 0;
	
	for(int moveTo = 0; moveTo < mySize; moveTo++)
	{
		StandardPlayingCard card = myArray[moveTo];
		
		if (card.getRank() == 1) // checks if user gets an Ace
		{
			int answer;
			bool isItRight = true;
			do
			{
				
				cout << "Do you want Ace to be 1 or 11" << endl;
				cin >> answer;
				if (cin.fail()) {
					cin.clear();
					cin.ignore(256,'\n');
				    answer = 0;
				}
				if (answer == 1 || answer == 11) 
				{
					PlayerCardsValue += answer;
				 	isItRight = false;	
				}
			}while(isItRight);
			
		}
		else if (card.getRank() >= 10)
		{
			PlayerCardsValue += 10;
		}
		else
		{
			PlayerCardsValue += card.getRank();
		}
		
	}
	cout << "Player Hand Value: " << PlayerCardsValue << endl;
}

// Finds the total artifical sum total for both the dealer and the other AI players
int StandardDeck::ArtificialTotalSum()
{
	int handValue = 0;

	bool isThereAce = false;
	for(int moveTo = 0; moveTo < mySize; moveTo++)
	{
		StandardPlayingCard card = myArray[moveTo];
		
		if (card.getRank() == 1) //Checks if there is an Ace in the persons hands
		{
			isThereAce = true;
			handValue += 1;			
		}
		else if (card.getRank() >= 10) //Any user with a rank either 10 or higher
		{
			handValue += 10;
		}
		else
		{
			handValue += card.getRank();
		}
		
	}
	if (isThereAce == true)
	{
		//checks if the AI should be able to be closer to 21 but not pass it
		if (handValue + 10 <= 21)
		{
			handValue += 10;
		}
		
	}
	return handValue;
}

// Randomizes the deck of card
StandardPlayingCard StandardDeck::Shuffle()
{
	int randomNumber;
	StandardPlayingCard temp;
	for (int i = 0; i < mySize; i++)
		{
			randomNumber=rand() % mySize;
			temp = myArray[i];
			myArray[i] = myArray[randomNumber];
			myArray[randomNumber] = temp;
		}
}

// for adding an element to the back/end
void StandardDeck::AddCard(StandardPlayingCard newCard)
{
	// Make the container larger by 1
	Resize(mySize + 1);
	// Add the new value to the end
	myArray[mySize - 1] = newCard;
}


// for removing an element from the back/end
StandardPlayingCard StandardDeck::RemoveCard()
{
	// If the container is empty, throw an error
	if(mySize <= 0)
		throw out_of_range("Cannot .PopBack() an empty container. ");
	// preserve the last value
	StandardPlayingCard card = myArray[mySize - 1];
	// Make the container smaller by 1
	Resize(mySize - 1);
	// return the former last value
	return card;
}

//This is for the dealer only, where it checks if the dealer can hit more cards before standing
StandardPlayingCard StandardDeck::hitUntilStand(StandardDeck hand, StandardDeck deck, int count )
{
	bool isThereAce = false;	
	
	DealerCardsValue = 0;
	for(int moveTo = 0; moveTo < mySize; moveTo++)
	{
		StandardPlayingCard card = myArray[moveTo];
		
		if (card.getRank() == 1) // Ace
		{
			isThereAce = true;
			DealerCardsValue += 1;			
		}
		else if (card.getRank() >= 10) //ten or higher
		{
			DealerCardsValue += 10;
		}
		else
		{
			DealerCardsValue += card.getRank();
		}
		
	}
	if (isThereAce == true)
	{
		//checks if the AI should be able to be closer to 21 but not pass it
		if (DealerCardsValue+10 <= 21)
		{
			DealerCardsValue += 10;
		}
	}
	if (DealerCardsValue < 17)
	{
		hand.AddCard(deck.DrawNextCard()) ;
		count++;
		hand.hitUntilStand(hand,deck, count); 
	}
	else
	{
		gameplayLog.open("gameplay_log.txt", ios::app); //Appends the file
	
		currentTime = time(0); // Gets current time
		// converts it to a string form
	   	char* datetime = ctime(&currentTime);
			
		if (count == 0)
		{
			cout << "Dealer stands with: " << DealerCardsValue << endl;
			
			gameplayLog << "Dealer stands with: " << DealerCardsValue << datetime << endl; 
		}
		else if(count == 1)
		{
			cout << "Dealer hits once and stands with : " << DealerCardsValue << endl;
			
			gameplayLog << "Dealer hits once and stands with : " << DealerCardsValue << datetime << endl; 
		}
		else
		{
			cout << "Dealer hits " << count << " times before standing with: " << DealerCardsValue << endl;
			
			gameplayLog << "Dealer hits " << count << " times before standing with: " << DealerCardsValue << datetime << endl; 
		}
		cout << hand.ShowCards(false) << endl;
		
		gameplayLog.close(); // Closes the file for later
	}
	//cout << "Dealer: " << DealerCardsValue << endl;

}

//AI players can hit until they have to stand
int StandardDeck::PlayerHitUntilStand(StandardDeck hand, StandardDeck deck, int count, int number )
{
	bool isThereAce = false;
	int handValue = 0;
	for(int moveTo = 0; moveTo < mySize; moveTo++)
	{
		StandardPlayingCard card = myArray[moveTo];
		
		if (card.getRank() == 1)// check if ace
		{
			isThereAce = true;
			handValue += 1;			
		}
		else if (card.getRank() >= 10)//check if higher than 10 or equals to
		{
			handValue += 10;
		}
		else
		{
			handValue += card.getRank();
		}
		
	}
	if (isThereAce == true)//checks if ace can be converted to 11 without busting
	{
		//checks if the AI should be able to be closer to 21 but not pass it
		if (handValue+10 <= 21)
		{
			handValue += 10;
		}
	}
	if (handValue < 17) //Reloops the calculations for card value
	{
		hand.AddCard(deck.DrawNextCard()) ;
		count++;
		handValue = hand.PlayerHitUntilStand(hand,deck, count, number); 
	}
	else //Displays the number of hits the AI has done
	{
		gameplayLog.open("gameplay_log.txt", ios::app); //Appends the file
	
		currentTime = time(0); // Gets current time
		// converts it to a string form
	   	char* datetime = ctime(&currentTime);		
		
		if (count == 0)
		{
			cout << "Player " << number <<" stands with: " << handValue << endl;
			
			gameplayLog << "Player " << number <<" stands with: " << handValue << datetime << endl; 
		}
		else if(count == 1)
		{
			cout << "Player " << number <<" hits once and stands with : " << handValue << endl;
			
			gameplayLog << "Player " << number <<" hits once and stands with : " << handValue << datetime << endl; 
		}
		else
		{
			cout << "Player " << number <<" hits " << count << " times before standing with: " << handValue << endl;
			
			gameplayLog << "Player " << number <<" hits " << count << " times before standing with: " << handValue << datetime << endl; 
		}
		cout << hand.ShowCards(false) << endl;
		gameplayLog << hand.ShowCards(false) << endl; 
		
		gameplayLog.close(); // Closes the file for later
	}
	
	return handValue;
}

//Shows the rules for the user
void ShowRules()
{
	//Display rules and information in a visually pleasing graphic.
	cout << "//////////////////////////////////////////////////////////" << endl;
	cout << "////        Welcome to the game of Twenty One!       /////" << endl;
	cout << "////  _____________________________________________  /////" << endl;
	cout << "//// | Rules:                                      | /////" << endl;
	cout << "//// | o Aces can be either 1 or 11 points.        | /////" << endl;
	cout << "//// | o Dealer wins ties unless player has a      | /////" << endl;
	cout << "//// |   blackjack.                                | /////" << endl;
	cout << "//// |                                             | /////" << endl;
	cout << "//// | Winnings -                                  | /////" << endl;
	cout << "//// | Winnings are based on the weight of a given | /////" << endl;
	cout << "//// | win or loss:                                | /////" << endl;
	cout << "//// | o Your score beats Dealer's score -         | /////" << endl;
	cout << "//// |   1:1 payout ratio.                         | /////" << endl;
	cout << "//// |   (100% increase of your initial bet.)      | /////" << endl;
	cout << "//// | o Dealer's score beats your score -         | /////" << endl;
	cout << "//// |   1:1 loss ratio.                           | /////" << endl;
	cout << "//// |   (100% decrease of your initial bet.)      | /////" << endl;
	cout << "//// | o Blackjack -                               | /////" << endl; 
	cout << "//// |   3:2 payout ratio.                         | /////" << endl;
	cout << "//// |   (150% increase of your initial bet.)      | /////" << endl;
	cout << "//// | o Push (Tie) -                              | /////" << endl;
	cout << "//// |   i.  1:1 payout if in your favor.          | /////" << endl;
	cout << "//// |       (100% increase of your initial bet.)  | /////" << endl;
	cout << "//// |   ii. 0:1 payout if in Dealer's favor.      | /////" << endl;
	cout << "//// |       (No loss, no gain.)                   | /////" << endl;
	cout << "//// |_____________________________________________| /////" << endl;
	cout << "//////////////////////////////////////////////////////////" << endl;
}


// Asks the user if they want to play again
bool WantToPlayAgain(char& Play)
{
	//Checks if the user has any chip left to bet inside the game, else they have to leave
	if (playerChip > 0)
	{
		//Prompt user to see if they would like to play another hand.
		cout << endl << "\nWould you like to play another hand? (Y/N) ";
		cin >> Play;
		cout << "\n" << endl;
		
		//Go back to the main game logic function to restart if they do. 
		if(Play == 'y' || Play == 'Y') 
		{
			cout << "Current chips is " << playerChip << " ($5 chips)"<< endl;
			TheGame();
			return(true);
		}
		//If they don't, exit the program.
		else if (Play == 'n' || Play == 'N') 
		{
			cout << "Leave the table with " << playerChip << " $5 chips" << endl;
			
			gameplayLog.open("gameplay_log.txt", ios::app); //Appends the file
	
			currentTime = time(0); // Gets current time
			// converts it to a string form
		   	char* datetime = ctime(&currentTime);
			
			gameplayLog << "Leave the table with " << playerChip << " $5 chips" << datetime << endl; 
			
			gameplayLog.close(); // Closes the file for later
			
			
			exit(0);
			return(false);
		}
		else
		{
			cout << "Input not found. Please enter either Y/N";
			WantToPlayAgain(Play);
		}
	}
	else
	{
		cout << "You are out of chips, have to leave the table" << endl;
		exit(0);
	}
}


//Asks if the user wants to hit another card or not
bool UserWantsToDraw(char& Draw)
{
	//Prompt user to see if they would like to enter a new time.
	cout << endl << "\nWould you like to hit or stand? (H/S) ";
	cin >> Draw;
	cout << "\n";
	
	//If they do, return true and start the loop to draw another card.
	if(Draw == 'h' || Draw == 'H') 
	{
		return(true);
	}
	//If they don't, skip the loop. 
	else
	{
		
		return(false);
	}
}


//Checks if either dealer or the user busts anyways
void CheckBust(char Play, StandardDeck deck)
{
	//Define local variables.
	int playerScore = PlayerCardsValue;
	int dealerScore = DealerCardsValue;

	//Check if Player busts. 
	//Display message, compute new winnings multiplier, ask to play another hand.
	if(playerScore > 21)
	{
		cout << "You bust with " << playerScore << " points." << endl;
		cout << "\n" << endl;
		cout << "Lose " << playerBet <<  " Chips" << endl;
		playerChip -= playerBet;//lost chips
		
		gameplayLog.open("gameplay_log.txt", ios::app); //Appends the file
	
		currentTime = time(0); // Gets current time
		// converts it to a string form
	   	char* datetime = ctime(&currentTime);
		
		gameplayLog << "Player bust with " << playerScore << " points."<< datetime << endl; 
		
		gameplayLog.close(); // Closes the file for later
		
		
		
		//Starts to check how other players have done
		checkAiWins( deck);
		WantToPlayAgain(Play);//Asks if they want to play again
	}

	//Check if Dealer busts.
	//Display message, compute new winnings multiplier, ask to play another hand.
	else if(dealerScore > 21)
	{
		cout << "\n" << endl;
		cout << "The Dealer went bust. You Win!" << endl;
		cout << "\n" << endl;
		cout << "Win: "  << playerBet << " Chips" << endl;
		playerChip += playerBet;//Gain chips
		
		gameplayLog.open("gameplay_log.txt", ios::app); //Appends the file
	
		currentTime = time(0); // Gets current time
		// converts it to a string form
	   	char* datetime = ctime(&currentTime);
		
		gameplayLog << "The Dealer went bust. Player Wins!" << datetime << endl; 
		
		gameplayLog.close(); // Closes the file for later
		
		//Starts to check how other players have done
		checkAiWins(deck);
		WantToPlayAgain(Play); // Asks if they want to play again
	}
}

//Check if either user or dealer gets a blackjack
void blackJack( char Play, StandardDeck deck)
{
	//Define local Variables
	int playerScore = PlayerCardsValue;
	int dealerScore = DealerCardsValue;

	//If Player has blackjack but Dealer doesn't. 
	if((playerScore == 21) && (dealerScore != 21))
	{
		//Display message, compute new winnings multiplier, ask to play another hand.
		cout << "\n\n";
		cout << "\n";
		cout << "Blackjack! You win a 3:2 payout." << endl;
		cout << "\n";
		//Adds 50% more to the bet for the blackjack
		playerBet += ceil(playerBet * 0.5);
		cout << "Win " << playerBet << " chips" <<  endl;
		playerChip += playerBet;
		
		gameplayLog.open("gameplay_log.txt", ios::app); //Appends the file
	
		currentTime = time(0); // Gets current time
		// converts it to a string form
	   	char* datetime = ctime(&currentTime);
		
		gameplayLog << "Blackjack! Player wins a 3:2 payout." << datetime << endl; 
		
		gameplayLog.close(); // Closes the file for later
		
		
		//Starts to check how other players have done
		checkAiWins( deck);
		WantToPlayAgain(Play);//Asks if they want to play again
	}
	//If both Player and Dealer have blackjack. 
	else if((playerScore == 21) && (dealerScore == 21))
	{
		//Display message, compute new winnings multiplier, ask to play another hand.
		cout << "\n";
		cout << "The Dealer and you both got Blackjack. Push in your favor at 1:1 payout!" << endl;
		cout << "\n";
		cout << "Win " << playerBet << " chips" <<  endl;
		playerChip += playerBet;
		
		
		gameplayLog.open("gameplay_log.txt", ios::app); //Appends the file
	
		currentTime = time(0); // Gets current time
		// converts it to a string form
	   	char* datetime = ctime(&currentTime);
		
		gameplayLog <<  "The Dealer and you both got Blackjack. Push in your favor at 1:1 payout!" << datetime << endl; 
		
		gameplayLog.close(); // Closes the file for later
		
		//Starts to check how other players have done
		checkAiWins( deck);
		WantToPlayAgain(Play);//Asks if they want to play again
	}
}

//Checks if the player wins or the dealer wins
void whoWins(int player, int dealer)
{
	//Declare local variables
	int playerScore = player;
	int dealerScore = dealer;

	//Display the scoreboard
	scoreboard(playerScore, dealerScore);
		
	//win tree
	//If Player scores less than 22
	if( (playerScore < 22) 
		//If Player's score is better than Dealer's score
		&& (playerScore > dealerScore) 
		//If Player's score is under 22 but Dealer's score is over 21
		|| ( (dealerScore > 21) 
			&& (playerScore < 22) ) )
	{
		//Display message and compute new winnings multiplier
		cout << "\n";
		cout << "You win! Won " << playerBet << " chips" << endl;
		playerChip += playerBet;
		//win++;
	}
	//lose tree
	//If you don't win, you lose or tie. 
	else
	{
		//Tie
		//Display message
		if(playerScore == dealerScore)
		{
			cout << "\n";
			cout << "Push in the Dealer's favor. 0:1 payout." << endl;
		}

		//lose
		//Display message and compute new winnings multiplier
		else
		{
			cout << "\n";
			cout << "You lose: " << playerBet << " chips" << endl;
			playerChip -= playerBet;
		}
	}
}

//Checks if the AI player wins or the dealer themselves
void checkAiWins(StandardDeck deck)
{
	//If user enters that they want other players, then it would display what occurred during their time
	if (otherPlayers >= 1)
	{
		cin.clear();
		cin.ignore();		
		
		cout << endl << "Other Players Choices and Ending: " << endl;
		arrayPlayers[0] = player1.PlayerHitUntilStand(player1, deck, 0, 1 ); //Player's hand, the deck currently, number of hits, and the player's number
		OthersWhoWins(arrayPlayers[0], DealerCardsValue, 1);
		
		if (otherPlayers >= 2)
		{
			cin.clear();
			cin.ignore() ;
			arrayPlayers[1] = player2.PlayerHitUntilStand(player2, deck, 0, 2 );
			OthersWhoWins(arrayPlayers[1], DealerCardsValue, 2);
		}
		if (otherPlayers >= 3)
		{
			cin.clear();
			cin.ignore() ;
			arrayPlayers[2] = player3.PlayerHitUntilStand(player3, deck, 0, 3 );
			OthersWhoWins(arrayPlayers[2], DealerCardsValue, 3);
		}
		if (otherPlayers >= 4)
		{
			cin.clear();
			cin.ignore() ;
			arrayPlayers[3] = player4.PlayerHitUntilStand(player4, deck, 0, 4 );
			OthersWhoWins(arrayPlayers[3], DealerCardsValue, 4);
		}	
	}
	
}

void OthersWhoWins(int player, int dealer, int playerNumber)
{
	//Declare local variables
	int playerScore = player;
	int dealerScore = dealer;

	//Display the scoreboard
	scoreboard(playerScore, dealerScore);
		
		
	gameplayLog.open("gameplay_log.txt", ios::app); //Appends the file
	
	currentTime = time(0); // Gets current time
	// converts it to a string form
   char* datetime = ctime(&currentTime);

	//win tree
	//If Player scores less than 22
	if( (playerScore < 22))
	{
		//If Player's score is better than Dealer's score
		if (playerScore > dealerScore) 
		{
			//Display message and compute new winnings multiplier
			cout << "\n";
			cout << "Player [" << playerNumber << "] win!" << endl;
			
			gameplayLog <<  "Player [" << playerNumber << "] win!" << datetime << endl; 
			
		}
		//lose tree
		//If you don't win, you lose or tie. 
		else
		{
			//Tie
			//Display message
			if(playerScore == dealerScore)
			{
				cout << "\n";
				cout << "Player [" << playerNumber << "] ties with dealer, therefore no gain or loss" << endl;
				
				gameplayLog <<  "Player [" << playerNumber << "] ties with dealer, therefore no gain or loss" << datetime << endl; 
			}
	
			//lose
			//Display message and compute new winnings multiplier
			else
			{
				cout << "\n";
				cout << "Player [" << playerNumber << "] loses" << endl;
				
				gameplayLog <<  "Player [" << playerNumber << "] loses" << datetime << endl; 
			}
		}
	}
	else // The player busts
	{
		cout << "Player [" << playerNumber << "] busts. Loses" << endl;
	}
	
	gameplayLog.close(); // Closes the file for later
		
}

//Shows a score between the dealer and the player's total card values
void scoreboard(int playerScore, int dealerScore)
{
	cout << "Player's Value: " << playerScore << " vs. Dealer's Value: " << dealerScore <<endl;
}
