/*
  Copyright (C) 2009 Paul Brossier <piem@aubio.org>

  This file is part of aubio.

  aubio is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  aubio is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with aubio.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef _FMAT_H
#define _FMAT_H

#ifdef __cplusplus
extern "C" {
#endif

/** \file

  Matrix of real valued data

  This file specifies the fmat_t type, which is used in aubio to store arrays
  of floating point values.

  \example test-fmat.c

*/

/** Buffer for real data */
typedef struct {
  uint_t length; /**< length of matrix */
  uint_t height; /**< height of matrix */
  smpl_t **data; /**< data array of size [length] * [height] */
} fmat_t;

/** fmat_t buffer creation function

  \param length the length of the matrix to create
  \param height the height of the matrix to create

*/
fmat_t * new_fmat(uint_t length, uint_t height);
/** fmat_t buffer deletion function

  \param s buffer to delete as returned by new_fmat()

*/
void del_fmat(fmat_t *s);
/** read sample value in a buffer

  Note that this function is not used in the aubio library, since the same
  result can be obtained using vec->data[channel][position]. Its purpose is to
  access these values from wrappers, as created by swig.

  \param s vector to read from
  \param channel channel to read from
  \param position sample position to read from 

*/
smpl_t fmat_read_sample(fmat_t *s, uint_t channel, uint_t position);
/** write sample value in a buffer

  Note that this function is not used in the aubio library, since the same
  result can be obtained by assigning vec->data[channel][position]. Its purpose
  is to access these values from wrappers, as created by swig.

  \param s vector to write to 
  \param data value to write in s->data[channel][position]
  \param channel channel to write to 
  \param position sample position to write to 

*/
void  fmat_write_sample(fmat_t *s, smpl_t data, uint_t channel, uint_t position);
/** read channel vector from a buffer

  Note that this function is not used in the aubio library, since the same
  result can be obtained with vec->data[channel]. Its purpose is to access
  these values from wrappers, as created by swig.

  \param s vector to read from
  \param channel channel to read from

*/
smpl_t * fmat_get_channel(fmat_t *s, uint_t channel);
/** write channel vector into a buffer

  Note that this function is not used in the aubio library, since the same
  result can be obtained by assigning vec->data[channel]. Its purpose is to
  access these values from wrappers, as created by swig.

  \param s vector to write to 
  \param data vector of [length] values to write
  \param channel channel to write to 

*/
void fmat_put_channel(fmat_t *s, smpl_t * data, uint_t channel);
/** read data from a buffer

  Note that this function is not used in the aubio library, since the same
  result can be obtained with vec->data. Its purpose is to access these values
  from wrappers, as created by swig.

  \param s vector to read from

*/
smpl_t ** fmat_get_data(fmat_t *s);

/** print out fmat data 

  \param s vector to print out 

*/
void fmat_print(fmat_t *s);

/** set all elements to a given value

  \param s vector to modify
  \param val value to set elements to

*/
void fmat_set(fmat_t *s, smpl_t val);

/** set all elements to zero 

  \param s vector to modify

*/
void fmat_zeros(fmat_t *s);

/** set all elements to ones 

  \param s vector to modify

*/
void fmat_ones(fmat_t *s);

/** revert order of vector elements

  \param s vector to revert

*/
void fmat_rev(fmat_t *s);

/** apply weight to vector

  If the weight vector is longer than s, only the first elements are used. If
  the weight vector is shorter than s, the last elements of s are not weighted.

  \param s vector to weight
  \param weight weighting coefficients

*/
void fmat_weight(fmat_t *s, fmat_t *weight);

/** make a copy of a matrix 

  \param s source vector
  \param t vector to copy to

*/
void fmat_copy(fmat_t *s, fmat_t *t);

#ifdef __cplusplus
}
#endif

#endif /* _FMAT_H */
