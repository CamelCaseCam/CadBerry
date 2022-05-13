/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.12
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package ur_rna.RNAstructure.backend;

public class CIntVector {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected CIntVector(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(CIntVector obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        RNABackendJNI.delete_CIntVector(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public CIntVector() {
    this(RNABackendJNI.new_CIntVector__SWIG_0(), true);
  }

  public CIntVector(long n) {
    this(RNABackendJNI.new_CIntVector__SWIG_1(n), true);
  }

  public long size() {
    return RNABackendJNI.CIntVector_size(swigCPtr, this);
  }

  public long capacity() {
    return RNABackendJNI.CIntVector_capacity(swigCPtr, this);
  }

  public void reserve(long n) {
    RNABackendJNI.CIntVector_reserve(swigCPtr, this, n);
  }

  public boolean isEmpty() {
    return RNABackendJNI.CIntVector_isEmpty(swigCPtr, this);
  }

  public void clear() {
    RNABackendJNI.CIntVector_clear(swigCPtr, this);
  }

  public void add(int x) {
    RNABackendJNI.CIntVector_add(swigCPtr, this, x);
  }

  public int get(int i) {
    return RNABackendJNI.CIntVector_get(swigCPtr, this, i);
  }

  public void set(int i, int val) {
    RNABackendJNI.CIntVector_set(swigCPtr, this, i, val);
  }

}
