/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.12
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package ur_rna.RNAstructure.backend;

public class PartialProgress extends ProgressHandler {
  private transient long swigCPtr;

  protected PartialProgress(long cPtr, boolean cMemoryOwn) {
    super(RNABackendJNI.PartialProgress_SWIGUpcast(cPtr), cMemoryOwn);
    swigCPtr = cPtr;
  }

  protected static long getCPtr(PartialProgress obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        RNABackendJNI.delete_PartialProgress(swigCPtr);
      }
      swigCPtr = 0;
    }
    super.delete();
  }

  public PartialProgress(ProgressHandler clientProgress) {
    this(RNABackendJNI.new_PartialProgress(ProgressHandler.getCPtr(clientProgress), clientProgress), true);
  }

  public void setWorkInNextStep(int value) {
    RNABackendJNI.PartialProgress_workInNextStep_set(swigCPtr, this, value);
  }

  public int getWorkInNextStep() {
    return RNABackendJNI.PartialProgress_workInNextStep_get(swigCPtr, this);
  }

  public void update(int percent) {
    RNABackendJNI.PartialProgress_update(swigCPtr, this, percent);
  }

  public boolean canceled() {
    return RNABackendJNI.PartialProgress_canceled(swigCPtr, this);
  }

  public void cancel() {
    RNABackendJNI.PartialProgress_cancel(swigCPtr, this);
  }

  public void setNextStep(int percentWorkInNextStep) {
    RNABackendJNI.PartialProgress_setNextStep(swigCPtr, this, percentWorkInNextStep);
  }

  public void setWorkComplete(int percentWorkCompleted) {
    RNABackendJNI.PartialProgress_setWorkComplete(swigCPtr, this, percentWorkCompleted);
  }

  public void stepComplete() {
    RNABackendJNI.PartialProgress_stepComplete(swigCPtr, this);
  }

  public void advanceToNextStep(int percentWorkInNextStep) {
    RNABackendJNI.PartialProgress_advanceToNextStep(swigCPtr, this, percentWorkInNextStep);
  }

  public void setClient(ProgressHandler client) {
    RNABackendJNI.PartialProgress_setClient(swigCPtr, this, ProgressHandler.getCPtr(client), client);
  }

}
