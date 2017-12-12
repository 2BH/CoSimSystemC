/*
 * CoSimManager.h
 *
 *  Created on: 7 Dec 2017
 *      Author: bhzhang
 */

#ifndef COSIMMANAGER_H_
#define COSIMMANAGER_H_

#include <systemc>
#include <string>
#include "socketConnection.h"
#include "CoSimDataStruct.h"
#include <ctime>
const std::string HOST = "127.0.0.1";
const unsigned short PORT = 52156;
/*
 	enum PolyVRRequestType{
	POLYVR_ERROR,  //receive error time
	POLYVR_START_TIME,  //need timestamp as start time
	POLYVR_CURRENT_TIME,  //need timestamp as current time
	POLYVR_TIME_OVERDATE  //received timestamp is greater than duration
};
 *
 */



SC_MODULE(CoSimManager){
		socketConnection_ptr m_ptrConnection;
		polyvr_time m_current_time;
		CoSimSocketData m_clientToServer;
		CoSimSocketData m_serverToClient;
		void syncPolyVR();
		void hello_world();
		void hello_fzi();
		void waitUntilNoOtherActivity();
		void sendStartTime();
		void sendCurrentTime();
		void sendOverdateTime();
		void ackTime();
		SC_CTOR(CoSimManager) : m_ptrConnection(new socketConnection()){
				m_current_time = 0;
				//m_ptrConnection(new socketConnection());
				m_ptrConnection->acceptSerial(PORT);
				SC_THREAD(syncPolyVR);
				SC_THREAD(hello_world);
				//SC_THREAD(hello_fzi);
		}
		/*
		void hello_world(){
				std::cout << "hello world!" << std::endl;
		}

		void hello_fzi(){
				std::cout << "hello fzi!"  << std::endl;
		}
		*/

};
/*
M_CoSimManager::M_CoSimManager(sc_core::sc_module_name name)
    : sc_module(name),
      m_prtConnection(new syncConnection())
{
    // connect with Simulink
    // blocks until connection is established
    connectWithSimulink();

    SC_THREAD(coSimThread);
}

*/

#endif /* COSIMMANAGER_H_ */
