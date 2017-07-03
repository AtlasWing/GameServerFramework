#include "IPCreditAPI.h"
#include <sys/time.h>
#include <pthread.h>
#include <fstream>
#include <algorithm>


#include "servant/Communicator.h"
#include "IPCredit.h"

using namespace std;
using namespace newIPPro;

string keyToStr(key_t key_value) {
	char buf[32];
	snprintf(buf, 32, "0x%08x", key_value);
	string s(buf);
	return s;
}

Communicator g_comm;
int main(int argc,char** argv)
{

	IPCreditAPI api;
	struct IPCreditAPIOptions options;

	try 
	{
		g_comm.setProperty("locator", "taf.tafregistry.QueryObj@tcp -h 172.27.194.147 -p 17890");
//		g_comm.setProperty("locator", "taf.tafregistry.QueryObj@tcp -h 10.130.68.135 -p 17890:tcp -h 10.198.131.34 -p 17890:tcp -h 172.23.31.46 -p 17890");
		g_comm.setProperty("stat","taf.tafstat.StatObj");

		string Obj1 = "newIPPro.IPCreditServer.IPCreditObj";
		string Obj2 = "newIPPro.IPCreditServer.FIPCreditObj";

		options.comm = &g_comm;
		options.ip_servant = Obj1;
		options.fast_ip_servant = Obj2;
		options.log_path = "./log/";
		string server_path = "."; //ServerConfig::BasePath
		options.shm_key_1 = ftok(server_path.c_str(), 0);
		cout << "shm_key_1:" << keyToStr(options.shm_key_1) << endl;
		options.shm_key_2 = ftok(server_path.c_str(), 1);
		cout << "shm_key_2:" << keyToStr(options.shm_key_2) << endl;

		bool succ = api.initialize(options);
		if(succ) 
		{
			string test_ip1 = "171.8.162.250";
			//string test_ip2 = "113.104.214.83";
			string test_ip2 = "1.56.188.8";
			int try_times = 0;
			for(int i = 0 ;i < 5; i++)
			{
				newIPPro::IPInfo strInfo;
				//struct IPInfostr strInfo;
				sleep(1);
				string test_ip = (try_times % 2) ? test_ip1 : test_ip2;
                IPHumanMessage message;
				//if(api.getIPInfoFromStr(test_ip,&strInfo))
                if (api.getIPInfoFromStr(test_ip, &message))
				{
					
                 
                    cout<<"[" << try_times << "]ip = " <<test_ip	
                        << message.ToString()<<endl;		
						
					sleep(1);
					//break;
				}
				else
				{
					cout << "[" << try_times << "]cannot find. ip = " << test_ip<<endl;
					sleep(1);

				}
			    ++try_times;
		}

	} 
	else
	{
			cout<<"init api failed!"<<endl;
	}
}
catch(exception &ex) 
{
		cerr << ex.what() << endl;
}
	return 0; 
}

