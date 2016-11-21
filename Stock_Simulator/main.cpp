#include <iostream>
#include <stdlib.h>
#include <mutex> //used for lock variables
#include <queue>
#include <fstream> //used to read in file
#include <vector>
#include <thread>
#include <chrono>//For seconds
#include <ctime> //using to generate rand number
#include <string>
#include <sstream>

#define NUM_THREADS 10000//defining number of threads

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
    
    Stock(){
        shares = 0;
        cost_per_share = 0;
    }
};

//Initalizing Contional Variables && Lock && number of Threads
condition_variable cv;
mutex mx;

vector<Stock>avaliable_stocks;
vector<string>current_stocks;


double balance = 0;
double yeild= 0; 
double profit = 0; 
double Tcost = 0;
int num_transact = 0;
int active_thread = 0;
bool isBuy;
double should_buy = 30;  //(Z) Buy if random Number is under 30%
double sell_if_over = 5; //(X)
double sell_if_under = 5;//(Y)


void initalize_stock(){
    
    string value; //used to read
    
    ifs >> value;
    
    while(!ifs.eof()){//while not eof
 
        Stock newStock;//Creating a new object
        queue<double>stock_prices; //used to temp hold stock prices
   
        newStock.SYMBOL = value;

        while(ifs >> value){//loops until new line
            stock_prices.push(stod(value));//converting it to an integer
        }
       
        newStock.pricelist = stock_prices;
        avaliable_stocks.push_back(newStock);//pushing stocks onto vector
    }
}

void server(){
    while(true){
        
       // mx.lock();//aquire a lock
        
        cout << "Total Balance: " << balance << endl;
        yeild = profit/Tcost;
        cout << "Yeild is:" << yeild*100 <<'%' << endl;
        

        sleep_for(seconds(10));
        
       // mx.unlock();//realease lock
        
    }
}
void ProcessTransact(string);
void buy(){

    //Creating a String
    string transaction;
    
    //creating new object
    Stock stockSelected;
  
    //randomly picking number of shares you want to buy
    int rand_num = rand() % 100;
    
    //Randomly pick one stock from Avalible Stocks
    int random_stock = rand() % avaliable_stocks.size()  - 1;
    stockSelected = avaliable_stocks[random_stock];
    
    // getting th price of the stock
    double price = stockSelected.pricelist.front();
    stockSelected.pricelist.pop();
    
    //Adding Transaction string
    transaction += "buy "; // Buy or sell
    transaction += stockSelected.SYMBOL + " "; // Pass the symbol of the selected stock
    transaction += to_string(rand_num) + " "; //Pass the number of stocks that will be bought
    transaction += to_string(price); // Pass the price of the stock
    
    ProcessTransact(transaction);
    
}
void sell(Stock name, double price){
    
    string transaction;
    
    transaction += "sell ";
    transaction += name.SYMBOL + " ";
    transaction += to_string(price);
    transaction += to_string(name.shares);
    
    ProcessTransact(transaction);
}

void ProcessTransact(string transaction){
    
    vector<string> transVec;
    istringstream iss(transaction);
    
    //reads string into vector
    for(string s; iss >> s; ){
        transVec.insert(transVec.begin(), s);
    }
    
    if(transVec[0] == "buy"){
        
        while(true){
         
            
        }
    }
}


int main (int argc, const char * argv[]){

    srand(time_t(NULL)); //Randomize seed initialization
    ifs.open(argv[1]);//reading lable
    
    initalize_stock();
    
    thread serverThread(server); // creates server thread;
    
    while(num_transact < NUM_THREADS){
        
        if(isBuy){
     
            int r = rand() % 100 + 1; // Generates a Number between 1-100
            
            if(r < should_buy){
     
                thread buythread(buy); // Creates a Buy thread
                num_transact++;
            }

            isBuy = true;
        }
        //Sell
        else {
          
              Stock selectedStock;//creating new object
            
            int random_stock = rand() % current_stocks.size() - 1;  //randomly pick a number
            string nameOfStock = current_stocks[random_stock];// picked random stock
        
            //Looping and checking if we have that stock to sell
            for(int i = 0; i < avaliable_stocks.size(); i++){
           
                //if avaliable stock is = to the random stock
                //set it to our new object 'selectedstock'
                if(avaliable_stocks[i].SYMBOL == nameOfStock){
                    selectedStock = avaliable_stocks[i];
                    break;
                }
            }
            //Set Cost from the selected stocks's cost
            double cost = selectedStock.cost_per_share;
            double price = selectedStock.pricelist.front();
            selectedStock.pricelist.pop();
            
            
            double checkifgreater = (100 + sell_if_over) * cost;
            double checkifsmaler = (100 - sell_if_under) * cost;
          
            if(price > checkifgreater || price < checkifsmaler){
                
                //create sell thread
                //passing in the sysmbol, and price
                thread sellthread(sell,selectedStock.SYMBOL, price);
                ++num_transact;
            }
            isBuy = false;
        }
    
    }
    return EXIT_SUCCESS;
}
