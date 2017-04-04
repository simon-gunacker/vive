//------------------------------------------------------------------------------
// Vive.h
//
// Authors: Simon Gunacker
//
//------------------------------------------------------------------------------

#pragma once

#include "_MLHTCViveSystem.h"
#include <mlModuleIncludes.h>
#include <WEMBase/WEMModuleBase/WEMProcessor.h>
#include <thread>
#include "Vive/Model.h"
#include "Vive/Vive.h"

ML_START_NAMESPACE

class _MLHTCVIVE_EXPORT HTCVive : public WEMProcessor
{
public:

  //! Constructor.
  HTCVive(std::string type="HTCVive");
  virtual ~HTCVive();

  //! Initializes module after loading.
  virtual void activateAttachments();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

  //! Processes input WEM -> modify nodes.
  virtual void _process();

  //------------------------------------------------------------------------------
  // Initializes the vive and renders the current model to it. This method is 
  // passed to a thread. The rendering method runs until _run is set to false.
  //
  void vive();

  //------------------------------------------------------------------------------
  // whenever the user wants to stop a thread, this method is called. It sets 
  // _run to false (-> the render function terminates) and joins the thread (-> 
  // the thread gets shut down)
  //
  void HTCVive::killThread();

  //------------------------------------------------------------------------------
  // whenever the user starts the vr inside MeVisLab, this method is called 
  // (with _run set to true)
  //
  void HTCVive::startThread();
  
private:
	// button for starting vr
	NotifyField		*_notifyStartVR;

	// click this whenever the model has changed in MeVisLab
	NotifyField     *_notifyUpdateVR;

	// button for stopping vr
	NotifyField     *_notifyStopVR;

	// members for managing vr thread
	std::thread _viveThread;
	bool _run;
	
  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(HTCVive)
};

ML_END_NAMESPACE
