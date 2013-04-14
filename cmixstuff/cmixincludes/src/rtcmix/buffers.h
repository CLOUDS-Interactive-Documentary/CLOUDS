/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#ifndef _BUFFERS_H_
#define _BUFFERS_H_ 1

#include <rt_types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void copy_interleaved_buf_to_one_buf(BufPtr dest, const BufPtr src,
                               int src_chans, int src_chan, int dest_frames);
void copy_one_buf_to_interleaved_buf(BufPtr dest, const BufPtr src,
                              int dest_chans, int dest_chan, int dest_frames);
void copy_interleaved_buf_to_buf(BufPtr dest, const BufPtr src, int dest_chans,
                 int dest_chan, int dest_frames, int src_chans, int src_chan);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* _BUFFERS_H_ */
