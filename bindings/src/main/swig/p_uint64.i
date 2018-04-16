// used to return result of argument using array
// C++ call func(uint64_t& out) => func(BigInteger[] out) 

%typemap(jni) uint64_t *INOUT, uint64_t &INOUT %{jobjectArray%}
%typemap(jtype) uint64_t *INOUT, uint64_t &INOUT "java.lang.BigInteger[]"
%typemap(jstype) uint64_t *INOUT, uint64_t &INOUT "java.lang.BigInteger[]"
%typemap(javain) uint64_t *INOUT, uint64_t &INOUT "$javainput"

%typemap(in) uint64_t *INOUT (uint64_t strTemp ), uint64_t &INOUT (uint64_t strTemp ) {
  if (!$input) {
    SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "array null");
    return $null;
  }
  if (JCALL1(GetArrayLength, jenv, $input) == 0) {
    SWIG_JavaThrowException(jenv, SWIG_JavaIndexOutOfBoundsException, "Array must contain at least 1 element");
    return $null;
  }

  jobject oInput = JCALL2(GetObjectArrayElement, jenv, $input, 0); 
  if ( NULL != oInput ) {
    jstring sInput = static_cast<jstring>( oInput );

    const char * $1_pstr = (const char *)jenv->GetStringUTFChars(sInput, 0); 
    if (!$1_pstr) return $null;
    strTemp.assign( $1_pstr );
    jenv->ReleaseStringUTFChars( sInput, $1_pstr);  
  }

  $1 = &strTemp;
}

%typemap(freearg) uint64_t *INOUT, uint64_t &INOUT ""

%typemap(argout) uint64_t *INOUT, uint64_t &INOUT
{ 
  jstring jStrTemp = jenv->NewStringUTF( strTemp$argnum.c_str() );
  JCALL3(SetObjectArrayElement, jenv, $input, 0, jStrTemp ); 
}