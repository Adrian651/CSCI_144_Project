#include <iostream>
#include <stdlib.h>
#include <mutex> //used for lock variables
#include <queue>
#include <fstream> //used to read in file
#include <vector>
#include <thread>
#include <chrono>//For seconds
#include <ctime> //using to generate rand number

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

vector<Stock>stocks;

double balance =0, yeild= 0, profit = 0, cost = 0, num_transact = 0;
bool isBuy = true;

void initalize_stock(){
    
    string value; //used to read
    

    while( ifs >> value){//while not eof
   
        Stock newStock;//Creating a new object
        queue<double>stock_prices; //used to temp hold stock prices
   
        newStock.SYMBOL = value;
        
        ifs >> value;
        while(value != "\n"){//loops until new line
            
            stock_prices.push(stod(value));//converting it to an integer
        
        }
        newStock.pricelist = stock_prices;
        stocks.push_back(newStock);//pushing stocks onto vector
    }
}

void server(){
    while(true){
        
        mx.lock();//aquire a lock
        
        cout << "Total Balance: " << balance << endl;
        //Calculate Yeild
        yeild = profit/cost;
        cout << "Yeild is:" << yeild*100 <<'%' << endl;
        

        sleep_for(seconds(10));
        
        mx.unlock();//realease lock
        
    }
}

int main (int argc, const char * argv[]){

    srand(time_t(NULL)); //Randomize seed initialization
    ifs.open("stocks.cvs");//reading lable
    
     initalize_stock();
    
    thread serverThread(server); // creates server thread;
    
    while(num_transact < NUM_THREADS){
        
        if(isBuy){
            
            int r = rand() % 2;
            
        }
        
    }
    
    cout << "fuuuu" <<endl;
    
    return EXIT_SUCCESS;
}
