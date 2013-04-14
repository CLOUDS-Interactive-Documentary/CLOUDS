/* This file is no longer auto-generated -- do not erase or remove!! */

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "../../../rtcmix/rtcmix_types.h"

// Defined in src/rtcmix/dispatch.cpp

extern int perl_dispatch(const char *str, const Arg args[], int n_args, Arg *retarg);

MODULE = RT     PACKAGE = RT

SV *
handle_minc_function(...)

   CODE:
      {
         char     *function_name;
         int      i;
         int      iretval;
         SV       *stack_item;
		 Arg	  *args, retArg;

         /* function name passed in as first argument */
         stack_item = ST(0);
         function_name = SvPV_nolen(stack_item);
		 
		 args = (Arg *) malloc(sizeof(Arg) * items);
		 if (args == NULL)
			 croak("Memory failure in handle_minc_function!");

         for (i = 1; i < items; i++) {
			Arg *arg = &args[i - 1];
            stack_item = ST(i);

			if (SvROK(stack_item)) {
//				printf("DEBUG: handle_minc_function: argument[%d] was a reference\n", i-1);
				arg->_type = HandleType;
				arg->_val.handle = (RtcHandle) SvIV(SvRV(stack_item));
//				printf("DEBUG: retrieved handle at 0x%x\n", arg->_val.handle);
			}
            /* NOTE: Check for double/integer first; otherwise you might
               get the string version of a number, which we'll then cast
               as a string pointer to RTcmix.
            */
			else if (SvNIOK(stack_item)) {
//			   printf("handle_minc_function: argument[%d] was number\n", i-1);
			   arg->_type = DoubleType;
			   arg->_val.number = SvNV(stack_item);
            }
            else if (SvPOK(stack_item)) {
               char *str = SvPV_nolen(stack_item);
			   arg->_type = StringType;
			   arg->_val.string = str;
//			   printf("handle_minc_function: argument[%d] was string\n", i-1);
            }
			else {
				printf("handle_minc_function: \"%s()\": argument[%d] not number, string or reference!\n", function_name, i-1);
				// Default to double 0.0
			    arg->_type = DoubleType;
			    arg->_val.number = 0;
			}
         }

         iretval = perl_dispatch(function_name, args, items - 1, &retArg);
		 if (iretval == 0) {
			 switch (retArg._type) {
			 case DoubleType:
				 RETVAL = newSVnv(retArg._val.number);
				 break;
			 case StringType:
				 RETVAL = newSVpv(retArg._val.string, 0);
				 break;
			 case HandleType:
				 // Create reference to IV scalar containing address
				 RETVAL = newRV(newSViv((IV) retArg._val.handle));
				 break;
			 case ArrayType:
			 default:
				 croak("handle_minc_function: Cannot handle this return type!");
			 }
		 }
		 else RETVAL = newSViv(iretval);
		 free(args);
      }
   OUTPUT:
      RETVAL

