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
double cost = 0; 
double num_transact = 0;
int active_thread = 0;
bool isBuy = true;
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
            cout << value << endl;
        
        }
       
        newStock.pricelist = stock_prices;
        avaliable_stocks.push_back(newStock);//pushing stocks onto vector
    }
}

void server(){
    while(true){
        
        mx.lock();//aquire a lock
        
        cout << "Total Balance: " << balance << endl;
        yeild = profit/cost;
        cout << "Yeild is:" << yeild*100 <<'%' << endl;
        

        sleep_for(seconds(10));
        
        mx.unlock();//realease lock
        
    }
}

void buy(){

    //Randomly pick one stock
    
    string transaction;
    
    transaction += "buy "; // Buy or sell
    transaction += "stockSelected.SYMBOL "; // Pass the symbol of the selected stock
    //transaction += to_string(randomNumber) + " "; //Pass the number of stocks that will be bought
    //transaction += to_string(priceOfStock); // Pass the price of the stock
    
    
    
    
}
void sell(){
    
}
/*
void ProcessTransact(string transaction){
    vector<string> transVec = transaction.split(" ");
    
    if(transVec[0] == 'buy'){
        
    }
}

*/
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
            }
            num_transact++;
            isBuy = false;
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
                
                thread sellthread(sell);
            }
            
        }
            
        
        
    }
    
    cout << "fuuuu" <<endl;
    
    return EXIT_SUCCESS;
}
