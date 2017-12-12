
#ifndef COSIMDATASTRUCT_H_
#define COSIMDATASTRUCT_H_

typedef double polyvr_time;

enum PolyVRRequestType{
	POLYVR_ERROR,  //receive error time
	POLYVR_START_TIME,  //need timestamp as start time
	POLYVR_CURRENT_TIME,  //need timestamp as current time
	POLYVR_TIME_OVERDATE  //received timestamp is greater than duration
};

struct CoSimSocketData{
	PolyVRRequestType request = POLYVR_ERROR;
	polyvr_time t = 0;
};


#endif /* COSIMDATASTRUCT_H_ */
