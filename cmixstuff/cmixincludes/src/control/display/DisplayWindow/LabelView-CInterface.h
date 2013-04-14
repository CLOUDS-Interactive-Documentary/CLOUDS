/* RTcmix - Copyright (C) 2011  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#ifndef __LABELVIEW_CINTERFACE_H__
#define __LABELVIEW_CINTERFACE_H__ 1

// in comm.mm
void setLabelViewInstance(void *instance);

// in LabelView.mm
void LabelViewConfigureLabelPrefix(void *labelViewInstance, int idx, char *thePrefix);
void LabelViewConfigureLabelUnits(void *labelViewInstance, int idx, char *theUnits);
void LabelViewConfigureLabelPrecision(void *labelViewInstance, int idx, int thePrecision);
void LabelViewUpdateLabelValue(void *labelViewInstance, int idx, double theValue);
void LabelViewDoUpdate(void *labelViewInstance);

#endif

