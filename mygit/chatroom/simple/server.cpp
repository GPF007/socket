#include <iostream>
#include <list>
#include <queue>
using namespace std;


class Server{
public:

	void init(){

	}

	void broadcast(){
		for(auto it=clients.begin();it!=clinets.end();it++){
			send(message)
		}
	}

private:
	list<int> clients;


};


int main(int argc, char const *argv[])
{
	cout<<"hello world"<<endl;
	return 0;
}