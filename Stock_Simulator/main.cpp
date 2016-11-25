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
    bool dont_use = true;
    
    Stock(){
        shares = 0;
        cost_per_share = 0;
    }
};

struct sell_info{
public:
    string SYMBOLL;
    double price;
    
};

//Initalizing Contional Variables && Lock && number of Threads

mutex mx;

vector<Stock>avaliable_stocks;//Free Stocks
vector<string>current_stocks;//Owned stocks

vector<sell_info> sellInfoVec; //To pass in information for sell thread and keep track of what i passed in
int numOfSellThreads = 0; //Keeps trakcs of my position of my sell treads in my vector

double balance = 100000000;
double yeild= 0.0;
double profit = 0.0;
double Tcost = 0.0;
int num_transact = 0;
int active_thread = 0;
bool isBuy = true;
int should_buy = 30;  //(Z) Buy if random Number is under 30%
double sell_if_over = 5; //(X)
double sell_if_under = 5;//(Y)

int check_it;

void initalize_stock(){
    
    string value; //used to read
    
    ifs >> value;
    
    while(!ifs.eof()){//while not eof
 
        Stock newStock;//Creating a new object
        queue<double>stock_prices; //used to temp hold stock prices
   
        newStock.SYMBOL = value;
        //cout << "symbol is: " << value << endl;

        while(ifs >> value){//loops until new line
            
            if(isalpha(value[1])){ break; }
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
        
        yeild = profit - Tcost;

        cout << "Yeild is:" << yeild <<'%' << endl;
       
        sleep_for(seconds(4));
       
    }
}
void ProcessTransact(string);
bool check();
void buy(){

    //srand(time_t(NULL)); //Randomize seed initialization
    
    //Creating a String
    string transaction;
    
    //creating new object
    Stock *stockSelected;
  
   
    //randomly picking number of shares you want to buy
    int rand_num = rand() % 100;
    
    while (true){
 
    //Randomly pick one stock from Avalible Stocks
        mx.lock();
    int random_stock = rand() % avaliable_stocks.size();
        mx.unlock();
    
    stockSelected = &avaliable_stocks[random_stock];
    bool temp = true;
        
         if(!check()){ cout << "NOT_ENOUGH_STOCKS_139" << endl; exit(1);}
         if(stockSelected->dont_use != true){
            temp= false;
            continue;
        }

         if (temp) break;
         else continue;
    }

    mx.lock();
    
    // getting th price of the stock
    double price = stockSelected->pricelist.front();
    //cout << "Price from selected stock to buy:" << price << endl;
    stockSelected->pricelist.pop();
    
    mx.unlock();
    
    //cout <<"random stock 136: " << random_stock << endl;
    //cout << "After popping: ";
    //cout << stockSelected->pricelist.front() << endl;
    //stockSelected->pricelist.pop();

    
    //Adding Transaction string
    transaction += "buy "; // Buy or sell
    transaction += stockSelected->SYMBOL + " "; // Pass the symbol of the selected stock
    transaction += to_string(rand_num) + " "; //Pass the number of stocks that will be bought
    transaction += to_string(price); // Pass the price of the stock
    
    //mx.lock();
    //cout << "Transaction from buy is: "<<transaction << endl;
    //mx.unlock();
    
    ProcessTransact(transaction);
    
}
void sell(sell_info info){//Need to pass in Stock name and Double Price
    
    string transaction;
    
    Stock *name = nullptr;
    
   
    for(int i = 0; i < avaliable_stocks.size(); i++){
        if(info.SYMBOLL == avaliable_stocks[i].SYMBOL){
            name = &avaliable_stocks[i];
            //cout <<"166" << avaliable_stocks[i].SYMBOL << endl;
            //cout <<"167" << name->SYMBOL << endl;
            //if(avaliable_stocks[i].pricelist.front() == 0) cout << "Not enough stocks bruh" << endl; exit(1);
        }
    }
    

    //cout << "Stock.symbol to sell: " << name ->SYMBOL << endl;
    //cout << "Stock prices in sell: " << name ->shares << endl;
    
   
    // int temp = name->shares;
    transaction += "sell ";
    transaction += info.SYMBOLL+ " ";
    transaction += to_string(name->shares) + " ";
    transaction += to_string(info.price);
  

    //cout << "Transaction is in sell: " << transaction << endl;
  

    
    ProcessTransact(transaction);
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
        mx.lock();
        int numOfShares = stoi(transVec[2]);
        double cost = stod(transVec[3]);
        mx.unlock();
        
        
        
        //cout << "got here" << endl;
        //sleep_for(seconds(2));
        
        
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
        
        balance = balance -(numOfShares * cost);
        
        //update Tcost
        
        Tcost = Tcost + (numOfShares * cost);
        
        mx.unlock();
       
        
    }
    else {

       
        for(int i = 0; i < avaliable_stocks.size(); i++){
            if(transVec[1] == avaliable_stocks[i].SYMBOL){
                thisStock = &avaliable_stocks[i];
                break;
            }
        }
        
        mx.lock();
        
      /*
        for(int i; i < transVec.size(); i++){
            cout << endl << transVec[i] << " ";
            cout << endl;
        }*/
       
        double sellPrice = stod(transVec[3]);
        int numOfShares = stoi(transVec[2]);
        mx.unlock();
        
        mx.lock();
        profit += (sellPrice * (double)numOfShares);
        
        //update the cost to 0
        
        thisStock->cost_per_share = 0;
        
        //update the number of shares to 0
        thisStock->shares = 0;
        
        mx.unlock();
    }
}

void print_avaliable_stocks(){
    /*
    for(int i = 0; i < avaliable_stocks.size(); i ++){
        
        cout << avaliable_stocks[i].SYMBOL << endl;
        cout << "Shares Owned: " << avaliable_stocks[i].shares << endl;
        
        for(int j = 0; j < avaliable_stocks[i].pricelist.size(); ){
            cout << avaliable_stocks[j].pricelist.front() << endl;
            avaliable_stocks[j].pricelist.pop();
        }
    }*/
    for(int i = 0; i < avaliable_stocks.size(); i++){
    
    cout << avaliable_stocks[i].SYMBOL << endl;
    
        while(true){
        
            if(avaliable_stocks[i].pricelist.front() == 0) break;
            
            cout << avaliable_stocks[i].pricelist.front() << endl;
            avaliable_stocks[i].pricelist.pop();
            
        }
    }
    
    
}

bool check(){
    
    for(int i = 0; i< avaliable_stocks.size(); i+=1){
        
        if(avaliable_stocks[i].pricelist.empty()){
            
            avaliable_stocks[i].dont_use = false;
            cout << "Stock that ran out"<< avaliable_stocks[i].SYMBOL << endl;
            check_it++;
        }
        
        if(check_it == avaliable_stocks.size()) return false;
        else continue;
    
    }
    return true;
}

int main (int argc, const char * argv[]){

    srand(time_t(10)); //Randomize seed initialization
    ifs.open("stocks2.csv");//reading lable
    
    initalize_stock();
    
    //Print out values from avalible stocks
    //print_avaliable_stocks();
    
    
    thread serverThread(server); // creates server thread;
 
    
    while(num_transact < NUM_THREADS){

        if (check() == false) { cout << "Not enough stocks 353"<< endl; exit(1); }
       
        if(isBuy){
            
            
            int r = rand() % 101; // Generates a Number between 1-100

            //cout << "random Number: " << r << endl;
           
            if(r < should_buy){
     
                sleep_for(milliseconds(100));
               // cout << "got here: 1" << endl;
                thread buythread(buy); // Creates a Buy thread
                buythread.detach();//it finihes the thread
                
                //sleep_for(seconds(1));
                //cout << "got here " << endl;
                
                num_transact++;
                //cout << "Number of Transaction: " <<num_transact << endl;
            }

            isBuy = false;
        }
        //Sell
        else {
            
            if(current_stocks.size() == 0){
                isBuy = true;
                continue;
            }

              Stock *selectedStock = nullptr;//creating new object
            
            
            int random_stock = rand() % current_stocks.size();  //randomly pick a number
            string nameOfStock = current_stocks[random_stock];// picked random stock
        
       
            
            if (check() == false) { cout << "Not enough stocks 416 " << endl; exit(1); }
            
            
            bool temp = true;
            
            //Looping and checking if we have that stock to sell
            for(int i = 0; i < avaliable_stocks.size(); i++){
           
                //if avaliable stock is = to the random stock
                //set it to our new object 'selectedstock'
                if(avaliable_stocks[i].SYMBOL == nameOfStock){
                    selectedStock = &avaliable_stocks[i];
                    if(!selectedStock->dont_use){temp = false; break;}
                    else break;
                }
            }
            
            if(!temp) continue;
            if (!check()) { cout << "Not enough stocks 429"<< endl; exit(1); }

           
            
            mx.lock();
            
            double cost = selectedStock->cost_per_share;
            double price = selectedStock->pricelist.front();
           // cout << "Price from random_selected stock: "<< price << endl;
            mx.unlock();
    
            
            double checkifgreater = (1 + ( ((double)sell_if_over)/ 100.0)) * cost;
            double checkifsmaler = (1 -  ( ((double)sell_if_under)/ 100.0) ) * cost;
          
            if(price > checkifgreater or price < checkifsmaler){
                
                
                
                current_stocks.erase(current_stocks.begin() + random_stock);
                
                //create sell thread
                //passing in the sysmbol, and price
                
                //setting vairbes to struct sell_info
                
                sell_info sell_selected_stock;
                sell_selected_stock.price = price;
                sell_selected_stock.SYMBOLL = selectedStock->SYMBOL;
                
                //Pushing information onto the sell_information vector
                sellInfoVec.push_back(sell_selected_stock);
                //cout <<"Slected stock's symbol: " << sell_selected_stock.SYMBOLL << endl;
                //cout <<"Selected stock Price: " <<sell_selected_stock.price << endl;
    
                //Create a sellthread
                
                
                //cout << sell_selected_stock.SYMBOLL << endl;
                //cout << sell_selected_stock.price << endl;
                
                
                sleep_for(milliseconds(100));
                thread sellThread (sell, sellInfoVec[numOfSellThreads]);
                sellThread.detach();
                
                
                numOfSellThreads++;
                ++num_transact;
                //cout << "Number of Transaction: " <<num_transact << endl;
                //cout << "got here: 2" << endl;
            }
            isBuy = true;
        }
    }
    return EXIT_SUCCESS;
}
