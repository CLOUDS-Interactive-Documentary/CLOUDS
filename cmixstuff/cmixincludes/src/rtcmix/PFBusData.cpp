#include <PFBusData.h>

int PFBusData::drawflag[NPFBUSSES];
const PField *PFBusData::thepfield[NPFBUSSES];
double PFBusData::val[NPFBUSSES];
double PFBusData::percent[NPFBUSSES];
double PFBusData::theincr[NPFBUSSES];
int PFBusData::dqflag[NPFBUSSES];
int PFBusData::pfbus_is_connected[NPFBUSSES];
int PFBusData::dq_now[NPFBUSSES];
int PFBusData::connect_val = -1;

