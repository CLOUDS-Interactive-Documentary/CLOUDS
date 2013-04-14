#ifndef _BUS_H_ 
#define _BUS_H_ 1

/* MAXBUS is one greater than the total number of useable buses. */
#define MAXBUS 65

#ifdef __cplusplus

enum ErrCode {
   NO_ERR = 0,
   INVAL_BUS_ERR,
   INVAL_BUS_CHAN_ERR,
   LOOP_ERR,
   UNKNOWN_ERR
};

enum BusType {
   BUS_IN,
   BUS_OUT,
   BUS_AUX_IN,
   BUS_AUX_OUT,
   BUS_NONE_IN,
   BUS_NONE_OUT
};

enum IBusClass {
  TO_AUX,
  AUX_TO_AUX,
  TO_OUT,
  TO_AUX_AND_OUT,
  UNKNOWN
};

class BusSlot;

extern "C" {
/* exported functions */
BusSlot *get_bus_config(const char *inst_name);
ErrCode parse_bus_name(char*, BusType*, int*, int*);
} /* extern "C" */
#endif	/* __cplusplus */

#endif /* _BUS_H_ */
