/* Define a new structure to use instead of int32_t */
%import <stdint.i>
%inline %{
typedef struct {
    int32_t value;
} Int32;
%}


//%{
/* Returns a status value and two values in out1 and out2 */
/*int spam(double a, double b, double *out1, double *out2) {
  int status = 1;
  *out1 = a*10.0;
  *out2 = b*100.0;
  return status;
};
*/
//%}

/* 
This typemap will make any int32_t& function parameters with name OUTVALUE take an
argument of ULongLong instead of int32_t *. This will 
allow the calling function to read the int32_t& value after returning from the function.
*/
%typemap(in) int32_t *OUTVALUE {
    jclass clazz = jenv->FindClass("Int32");
    jfieldID fid = jenv->GetFieldID(clazz, "swigCPtr", "J");
    jlong cPtr = jenv->GetLongField($input, fid);
    Int32 *p_int32 = NULL;
    *(Int32 **)&pInt32 = *(Int32 **)&cPtr;
    $1 = &pInt32->value;
}

%typemap(jtype) int32_t *OUTVALUE "Int32"
%typemap(jstype) int32_t *OUTVALUE "Int32"
%typemap(jni) int32_t *OUTVALUE "jobject"

%typemap(javain) int32_t *OUTVALUE "$javainput"

/* Now we apply the typemap to the named variables */
//%apply unsigned long long *OUTVALUE { double *out1, double *out2 };
//int spam(double a, double b, double *out1, double *out2);