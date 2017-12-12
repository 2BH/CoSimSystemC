#include "CoSimManager.h"

void CoSimManager::syncPolyVR(){
		waitUntilNoOtherActivity();
		sc_core::sc_time nextSimTime(sc_core::SC_ZERO_TIME);
		while(1){
				waitUntilNoOtherActivity();
				//std::cout << "start co-simulation" << std::endl;
				if(m_ptrConnection->recvData(m_clientToServer) < 0){
						return;
				}
				else{

						//std::cout << "current request is:" << m_clientToServer.request << std::endl;
						this->simcontext()->next_time(nextSimTime);
						//std::cout << "current time stamp is " << sc_core::sc_time_stamp().to_seconds() << std::endl;
						//std::cout << "next time stamp is " << float(nextSimTime.to_seconds()) << std::endl;
						//std::cout << "will wait " << (nextSimTime.to_seconds() - sc_core::sc_time_stamp().to_seconds()) << std::endl;

						switch(m_clientToServer.request){
						case POLYVR_ERROR:  break;
						case POLYVR_START_TIME: sendStartTime();  break;
						case POLYVR_CURRENT_TIME: sendCurrentTime(); break;
						case POLYVR_TIME_OVERDATE: sendOverdateTime(); break;
						default: return;
						}
				}

		}
}

void CoSimManager::ackTime(){
		if(m_ptrConnection->recvData(m_clientToServer) < 0){
				std::cout << "recv ack from client failed" << std::endl;
		}
		//std::cout << "ack infomation from client" << m_clientToServer.t << std::endl;
}
void CoSimManager::sendStartTime(){
		m_serverToClient.request = POLYVR_START_TIME;
		m_serverToClient.t = sc_core::sc_time_stamp().to_seconds();


		//while(m_ptrConnection->sendData(m_serverToClient) != 0){}



		if(m_ptrConnection->sendData(m_serverToClient) < 0){
				std::cout << "send the data from server error" << std::endl;
				return;
		}
		std::cout << "send start time:" << m_serverToClient.t << std::endl;
		std::cout << "send start time sucessfully" << std::endl;

		//ackTime();


		/*
		std::cout <<"For Debugging" << std::endl;

		if(m_ptrConnection->recvData(m_clientToServer) < 0){
				std::cout << "recv request failed" << std::endl;
				return;
		}
		else{
				std::cout << "recv request sucessfully" << std::endl;
		}

		if(m_clientToServer.request == POLYVR_START_TIME){
				std::cout << "ack the data sucessfully" << std::endl;
				return;
		}
		else{
				std::cout << "ack the data from client error" << std::endl;
		}
		*/
}


void CoSimManager::sendCurrentTime(){
		m_serverToClient.request = POLYVR_CURRENT_TIME;
		sc_core::sc_time nextSimTime(sc_core::SC_ZERO_TIME);
		this->simcontext()->next_time(nextSimTime);
		m_serverToClient.t = nextSimTime.to_seconds();
		std::cout << "send m_serverToClient.t:" << m_serverToClient.t << std::endl;
		if(m_ptrConnection->sendData(m_serverToClient) < 0){
				std::cout << "send the data from server error" << std::endl;
				return;
		}


		if(m_ptrConnection->recvData(m_clientToServer) < 0){
				std::cout << "recv data from server error!" << std::endl;
				return;
		}

		std::cout << "the time stamp from client is:" << m_clientToServer.t << std::endl;
		std::cout<< "current time stamp is:" << sc_core::sc_time_stamp().to_seconds() << std::endl;

		if(m_clientToServer.request == POLYVR_CURRENT_TIME){
				//ackTime();
				std::cout << "return current time" << std::endl;
				std::cout << "next sim time" << nextSimTime.to_seconds() << std::endl;
				std::cout<< "--------------------------" << std::endl;
				sc_core::wait(nextSimTime - sc_core::sc_time_stamp());
				return;
		}
		else{
				polyvr_time nextSimTime_val = m_clientToServer.t - sc_core::sc_time_stamp().to_seconds();
				//std::cout << "(over date)the time stamp from client is:" << m_clientToServer.t << std::endl;
				//std::cout<< "(over date)current time stamp is:" << sc_core::sc_time_stamp().to_seconds() << std::endl;
				if(nextSimTime_val < 0){
						//ackTime();

						//std::cout << "time over date, will wait zero time" << std::endl;
						//sc_core::wait(sc_core::SC_ZERO_TIME);
				}
				else{
						//ackTime();
						std::cout << "(over data)return out date time" << std::endl;
						sc_core::sc_time nextSimTime(nextSimTime_val, sc_core::SC_SEC);
						//std::cout << "time over date, will wait to:" << nextSimTime.to_seconds() << std::endl;
						sc_core::wait(nextSimTime);
						//std::cout<< "(over date)current time is:" << sc_core::sc_time_stamp().to_seconds() << std::endl;
				}
				std::cout<< "--------------------------" << std::endl;

				return;
		}

}
void CoSimManager::sendOverdateTime(){
		//wait until all other tasks have been proceeded
		//waitUntilNoOtherActivity();

		//send next time to PolyVR
		m_serverToClient.request = POLYVR_TIME_OVERDATE;
		sc_core::sc_time nextSimTime(m_clientToServer.t + sc_core::sc_time_stamp().to_seconds(), sc_core::SC_SEC);
		m_serverToClient.t = nextSimTime.to_seconds();

		if(m_ptrConnection->sendData(m_serverToClient) < 0){
				std::cout << "send the data from server error" << std::endl;
				return;
		}

		if(m_ptrConnection->recvData(m_clientToServer) < 0){
				std::cout << "recv the data from client error" << std::endl;
				return;
		}

		if(m_clientToServer.request == POLYVR_TIME_OVERDATE){
			 	ackTime();
				return;
		}
		else{
				std::cout << "ack the data from client error" << std::endl;
		}

		//wait until end of current animation of PolyVR
		sc_core::wait(nextSimTime);
}


void CoSimManager::waitUntilNoOtherActivity(){
		// wait until there is no activity pending for the current time
	  while( this->simcontext()->pending_activity_at_current_time() )
	  {
	      wait(sc_core::SC_ZERO_TIME);
	  }
}

void CoSimManager::hello_world(){
		sc_core::sc_time t_Wait(200, sc_core::SC_MS);
		while(1){
			  std::cout << "current systemC time:" << sc_core::sc_time_stamp() << std::endl;

			  time_t t = time(0);   // get time now
			      struct tm * now = localtime( & t );
			      std::cout << now->tm_min << ":" << now->tm_sec
			           << std::endl;
				//std::cout << "hello, world" << std::endl;
				sleep(1);
				sc_core::wait(t_Wait);
		}
}
void CoSimManager::hello_fzi(){
	sc_core::sc_time t_Wait1(200, sc_core::SC_MS);
	sc_core::sc_time t_Wait2(1, sc_core::SC_MS);
	for(int i = 0; i < 10000; i++){
			std::cout << "hello, world(short wait)" << std::endl;
			sc_core::wait(t_Wait2);
	}
	std::cout << "hello, world(long wait)" << std::endl;
	sc_core::wait(t_Wait1);
	std::cout << "hello, world(long wait)" << std::endl;
	sc_core::wait(t_Wait1);
}
