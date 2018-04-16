/* Define a new structure to use instead of bool * */
%inline %{
typedef struct {
    bool value;
} Bool;
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
This typemap will make any bool * function parameters with name OUTVALUE take an
argument of Bool instead of bool *. This will 
allow the calling function to read the bool * value after returning from the function.
*/
%typemap(in) bool *OUTVALUE {
    jclass clazz = jenv->FindClass("Bool");
    jfieldID fid = jenv->GetFieldID(clazz, "swigCPtr", "J");
    jbool cPtr = jenv->GetLongField($input, fid);
    Bool *pBool = NULL;
    *(Bool **)&pBool = *(Bool **)&cPtr;
    $1 = &pBool->value;
}

%typemap(jtype) bool *OUTVALUE "Bool"
%typemap(jstype) bool *OUTVALUE "Bool"
%typemap(jni) bool *OUTVALUE "jobject"

%typemap(javain) bool *OUTVALUE "$javainput"

/* Now we apply the typemap to the named variables */
//%apply bool *OUTVALUE { double *out1, double *out2 };
int spam(double a, double b, double *out1, double *out2);