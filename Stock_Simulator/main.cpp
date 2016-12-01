
#include <iostream>
#include <cstdlib>
#include <mutex> //used for lock variables
#include <queue>
#include <fstream> //used to read in file
#include <vector>
#include <thread>
#include <chrono>//For seconds
#include <ctime> //using to generate rand number
#include <string>
#include <sstream>

#define TRANSACTIONS 10000//defining number of threads

using namespace std;
using namespace chrono;
using namespace this_thread;
fstream ifs;

class Stock{
public:
    string SYMBOL;
    int shares;
    double cost_per_share;
    queue<double> pricelist; //Price list

    //Contructer
    Stock(){
        shares = 0;
        cost_per_share = 0;
    }
};

//CREATED A STRUCT TO PASS IN SELL INFO
struct sell_info{
public:
    string SYMBOLL;
    double price;
    
};

//Initalizing Lock

mutex mx;

vector<Stock>avaliable_stocks;//Free Stocks
vector<string>current_stocks;//Owned stocks

vector<sell_info> sellInfoVec; //To pass in information for sell thread and keep track of what i passed in
int numOfSellThreads = 0; //Keeps trakcs of my position of my sell treads in my vector


/********GLOBAL VAIRABLES********/
 
double balance = 1000000;
double yeild= 0.0;
double profit = 0.0;
double Tcost = 0.0;
int num_transact = 0;
int active_thread = 0;
bool isBuy = true;

int bought = 0;
int sold = 0;

int activeThreads = 0;

void initalize_stock(){
    
    string value; //used to read
    
    ifs >> value;
    
    while(!ifs.eof()){//while not eof
        
        Stock newStock;//Creating a new object
        queue<double>stock_prices; //used to temp hold stock prices
        
        newStock.SYMBOL = value;
        //cout << "symbol is: " << value << endl;
        
        while(ifs >> value){//loops until new line
            
            if(isalpha(value[0])){ break; }
            stock_prices.push(stod(value));//converting it to a double
            // cout << value <<endl;
            
        }
        
        newStock.pricelist = stock_prices;
        avaliable_stocks.push_back(newStock);//pushing stocks onto vector
        
    }
}
void server(){
    while(true){
        
        cout << "Total Balance: " << balance << endl;
        
        //yeild = (profit/Tcost) - 1;
        
        cout << "Yeild is:" << ((profit/Tcost) - 1)*100<< '%' << endl;
        
        cout << "active Threads are: " << active_thread << endl;
        cout << endl;
        
       sleep_for(seconds(2));
        
    }
}
void ProcessTransact(string);
void buy(){
    mx.lock();
    active_thread++;
    mx.unlock();
    
    //Creating a String
    string transaction;
    
    //creating new object
    Stock *stockSelected;
    
    
    //randomly picking number of shares you want to buy
    int rand_num = rand() % 100;
    
    
    
    //Randomly pick one stock from Avalible Stocks
    int random_stock = rand() % avaliable_stocks.size();

    stockSelected = &avaliable_stocks[random_stock];
    
    
    
    mx.lock();
    
    // getting th price of the stock
    double price = stockSelected->pricelist.front();
    stockSelected->pricelist.pop();
    
    mx.unlock();
    
    
    //Adding Transaction string
    transaction += "buy "; // Buy or sell
    transaction += stockSelected->SYMBOL + " "; // Pass the symbol of the selected stock
    transaction += to_string(rand_num) + " "; //Pass the number of stocks that will be bought
    transaction += to_string(price); // Pass the price of the stock
    
    ProcessTransact(transaction);
    
    mx.lock();
    active_thread--;
    mx.unlock();
    
    
}
void sell(sell_info info){//Need to pass in Stock name and Double Price
    
    mx.lock();
    active_thread++;
    mx.unlock();
    
    string transaction;
    
    Stock *stock_sold = nullptr;
    
    
    //Looping to get the share information
    for(int i = 0; i < avaliable_stocks.size(); i++){
        if(info.SYMBOLL == avaliable_stocks[i].SYMBOL){
            
            stock_sold = &avaliable_stocks[i];
            
            if(stock_sold->shares == 0) return;//If there are no shares to sell then return
            break;
        }
    }
    
    //Creating a string transaction
    transaction += "sell ";
    transaction += info.SYMBOLL+ " ";
    transaction += to_string(stock_sold->shares) + " ";
    transaction += to_string(info.price);
    
    ProcessTransact(transaction);
    
    mx.lock();
    active_thread--;
    mx.unlock();
    
}
void ProcessTransact(string transaction){
    
    vector<string> transVec;
    istringstream iss(transaction);
    
    //reads string into vector
    for(string s; iss >> s; ){
        transVec.push_back(s);
    }
    
    Stock *thisStock;
    
    if(transVec[0] == "buy"){
        
       
        //Loop through avalibles stocks
        for(int i = 0; i < avaliable_stocks.size(); i++){
            if(transVec[1] == avaliable_stocks[i].SYMBOL){
                thisStock = &avaliable_stocks[i];
                break;
            }
        }//loop

        int numOfShares = stoi(transVec[2]);
        double cost = stod(transVec[3]);
        
        //Checking if currecnt stock is in current stocks
        bool ownStock = false;
        for(int i = 0; i < current_stocks.size(); i++){
            if(transVec[1] == current_stocks[i]){
                ownStock = true;
            }
        }
        //if is it then we dont add it
        //if it is not then we add it to out current stocks
        if(!ownStock) {
            current_stocks.push_back(transVec[1]);
        }
        
        mx.lock();
        
        //cout << "got here1" << endl;
        //update the number of shares
        thisStock->shares += numOfShares;
        
        //update cost per share
        thisStock->cost_per_share = cost;
        
        //update Balance
        
        balance -= (numOfShares * cost);
        
        //update Tcost
        
        Tcost +=(numOfShares * cost);
        
        mx.unlock();
        
    }
    else {
        
        
        for(int i = 0; i < avaliable_stocks.size(); i++){
            if(transVec[1] == avaliable_stocks[i].SYMBOL){
                thisStock = &avaliable_stocks[i];
                break;
            }
        }
        
        double sellPrice = stod(transVec[3]);
        int numOfShares = stoi(transVec[2]);
    
        mx.lock();
        
        //Updating Profit
        double local_profit = (sellPrice * numOfShares);
        
        profit += local_profit;
    
        //update the cost to 0
        
        thisStock->cost_per_share = 0;
        
        //update the number of shares to 0
        thisStock->shares = 0;
        
        //Updating balance
        balance += local_profit;
        
        mx.unlock();
    }
    
    sleep_for(seconds(2));
}
void print_avaliable_stocks(){
  
    
    for(int i = 0; i < avaliable_stocks.size(); i++){
        
        cout << avaliable_stocks[i].SYMBOL << endl;
        
        while(true){
            
            if(avaliable_stocks[i].pricelist.front() == 0) break;
            
            cout << avaliable_stocks[i].pricelist.front() << endl;
            avaliable_stocks[i].pricelist.pop();
            
        }
    }
}
int main (int argc, const char * argv[]){
    
    srand(time_t(10)); //Randomize seed initialization
    ifs.open("stocks2.csv");//reading lable
    
    int should_buy = 30;  //(Z) Buy if random Number is under 30%
    double sell_if_over = 5; //(X)
    double sell_if_under = 5;//(Y)
    
    initalize_stock();
    
    //Print out values from avalible stocks
    //print_avaliable_stocks();
    
    //User Input
    cout << "Z: ";
    cin >> should_buy;
    cout << "X: ";
    cin >> sell_if_over;
    cout << "Y: ";
    cin >> sell_if_under;

    
    thread serverThread(server); // creates server thread;
    
    while(num_transact < TRANSACTIONS){
        
        if(isBuy){
            
            int r = rand() % 101; // Generates a Number between 1-100
            
            if(r < should_buy){
                
                thread buythread(buy); // Creates a Buy thread
                buythread.join();//it finihes the thread
                bought++;
                num_transact++;
            }
            isBuy = false;
        }
        //Sell
        else {
            
            //If no buy threads are finished then I countinue
            if(current_stocks.size() == 0){
                isBuy = true;
                continue;
            }
            
            Stock *selectedStock = nullptr;//creating new object
            
            int random_stock = rand() % current_stocks.size();  //randomly pick a number
            string nameOfStock = current_stocks[random_stock];// picked random stock
            
            //Looping and checking if we have that stock to sell
            for(int i = 0; i < avaliable_stocks.size(); i++){
                
                //if avaliable stock is = to the random stock
                //set it to our new object 'selectedstock'
                if(avaliable_stocks[i].SYMBOL == nameOfStock){
                    selectedStock = &avaliable_stocks[i];
                    break;
                }
            }
        
            //Selected stocks Cost & Price
            double cost = selectedStock->cost_per_share;
            double price = selectedStock->pricelist.front();
            
            double checkifgreater = (1 + ( ((double)sell_if_over)/ 100.0)) * cost;
            double checkifsmaler = (1 -  ( ((double)sell_if_under)/ 100.0) ) * cost;
            
            if(price > checkifgreater or price < checkifsmaler){
                
                //Earsed the Current stock from the list of Current stocks
                current_stocks.erase(current_stocks.begin() + random_stock);
          
                //Created a stuct fot sell_information and setting the the preset values
                sell_info sell_selected_stock;
                sell_selected_stock.price = price;
                sell_selected_stock.SYMBOLL = selectedStock->SYMBOL;
                
                //Pushing information onto the sell_information vector
                sellInfoVec.push_back(sell_selected_stock);
               
               //creating sellthread
                thread sellThread (sell, sellInfoVec[numOfSellThreads]);
                sellThread.join();

                numOfSellThreads++;
                ++num_transact;
                sold++;
            }
            isBuy = true;
        }
    }
    
    serverThread.join();//joining server thread
    ifs.close();//closing the file
    
    //Showing final balances
    cout << "Final is: "<< endl;
    cout << "\n\nBalance is " << balance << endl;
    cout << "Yeild is: " << ((profit/Tcost) - 1) * 100 << '%' << endl;
    cout << "Number of Bought: " << bought << endl;
    cout << "Number of sold: " << sold << endl;

    return EXIT_SUCCESS;
}
