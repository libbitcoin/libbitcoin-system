/* Define a new structure to use instead of unsigned long long * */
%inline %{
typedef struct {
    unsigned long long value;
} ULongLong;
%}


%{
/* Returns a status value and two values in out1 and out2 */
/*int spam(double a, double b, double *out1, double *out2) {
  int status = 1;
  *out1 = a*10.0;
  *out2 = b*100.0;
  return status;
};
*/
%}

/* 
This typemap will make any unsigned long long * function parameters with name OUTVALUE take an
argument of ULongLong instead of unsigned long long *. This will 
allow the calling function to read the unsigned long long * value after returning from the function.
*/
%typemap(in) unsigned long long *OUTVALUE {
    jclass clazz = jenv->FindClass("ULongLong");
    jfieldID fid = jenv->GetFieldID(clazz, "swigCPtr", "J");
    jlong cPtr = jenv->GetLongField($input, fid);
    ULongLong *pULongLong = NULL;
    *(ULongLong **)&pULongLong = *(ULongLong **)&cPtr;
    $1 = &pULongLong->value;
}

%typemap(jtype) unsigned long long *OUTVALUE "ULongLong"
%typemap(jstype) unsigned long long *OUTVALUE "ULongLong"
%typemap(jni) unsigned long long *OUTVALUE "jobject"

%typemap(javain) unsigned long long *OUTVALUE "$javainput"

/* Now we apply the typemap to the named variables */
//%apply unsigned long long *OUTVALUE { double *out1, double *out2 };
int spam(double a, double b, double *out1, double *out2);