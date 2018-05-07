#include "EventHandler.h"
#include "EventLooper.h"

void EventHandler::Update()
{
	m_pLooper->UpdateHandler(*this);
}