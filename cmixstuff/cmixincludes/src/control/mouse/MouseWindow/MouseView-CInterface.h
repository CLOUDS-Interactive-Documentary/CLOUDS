#ifndef __MOUSEVIEW_CINTERFACE_H__
#define __MOUSEVIEW_CINTERFACE_H__ 1

// in comm.mm
void setMouseViewInstance(void *instance);
void sendCoordinates(const double x, const double y);

// in MouseView.mm
void MouseViewConfigureXLabelPrefix(void *mouseViewInstance, int idx, char *thePrefix);
void MouseViewConfigureYLabelPrefix(void *mouseViewInstance, int idx, char *thePrefix);
void MouseViewConfigureXLabelUnits(void *mouseViewInstance, int idx, char *theUnits);
void MouseViewConfigureYLabelUnits(void *mouseViewInstance, int idx, char *theUnits);
void MouseViewConfigureXLabelPrecision(void *mouseViewInstance, int idx, int thePrecision);
void MouseViewConfigureYLabelPrecision(void *mouseViewInstance, int idx, int thePrecision);
void MouseViewUpdateXLabelValue(void *mouseViewInstance, int idx, double theValue);
void MouseViewUpdateYLabelValue(void *mouseViewInstance, int idx, double theValue);
void MouseViewDoUpdate(void *mouseViewInstance);

#endif

