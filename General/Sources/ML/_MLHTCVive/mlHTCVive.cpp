#pragma once
#include <stdexcept>
#include <windows.h>
#include "mlHTCVive.h"

ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(HTCVive, WEMProcessor);

HTCVive::HTCVive(std::string type) : WEMProcessor(type)
, _run(false)
{
	// Suppress calls of handleNotification on field changes to
	// avoid side effects during initialization phase.
	handleNotificationOff();

	FieldContainer *fields = getFieldContainer();

	_notifyStartVR = fields->addNotify("Start Virtual Reality");
	_notifyUpdateVR = fields->addNotify("Update Model in Virtual Reality");
	_notifyStopVR = fields->addNotify("Stop Virtual Reality");

	// Reactivate calls of handleNotification on field changes.
	handleNotificationOn();
}

HTCVive::~HTCVive()
{
	killThread();
}

void HTCVive::vive() 
{
	Model model = Model(_outWEM);
	Vive vive = Vive(model);
	vive.render(&_run);
}

void HTCVive::killThread() 
{
	if (_run)
	{
		_run = false;
		_viveThread.join();
	}
}

void HTCVive::startThread()
{
	if (!_run)
	{
		_run = true;
		this->_viveThread = std::thread(&HTCVive::vive, this);
	}
}

// this is where the vive thread actually gets 
// started / stopped.
void HTCVive::handleNotification(Field* field)
{
	if (field == _notifyStartVR)
	{
		startThread();
	}
	else if (field == _notifyUpdateVR)
	{
		killThread();
		startThread();
	}
	else if (field == _notifyStopVR)
	{
		killThread();
	}

	WEMProcessor::handleNotification(field);
}

void HTCVive::activateAttachments()
{
	WEMProcessor::activateAttachments();
}

void HTCVive::_process()
{
	_startProcessing();
	WEMProcessor::_process();
	_finishProcessing();
	_notifyObservers();
}

ML_END_NAMESPACE
