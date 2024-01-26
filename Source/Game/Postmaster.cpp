#include "pch.h"
#include "Postmaster.h"
#include "Observer.h"
#include <assert.h>

Postmaster* Postmaster::ourInstance = nullptr;

Postmaster::Postmaster()
{
}

Postmaster::~Postmaster()
{

}

Postmaster* Postmaster::GetInstance()
{
	if (ourInstance == nullptr)
	{
		ourInstance = new Postmaster();
	}
	return ourInstance;
}

void Postmaster::SendMail(eMessage aMsg)
{
	for (auto observer : mySubscribers[aMsg])
	{
		observer->RecieveMessage(aMsg);
	}
}

void Postmaster::Subscribe(Observer* anObserver, const eMessage aMsg)
{
	auto it = std::find(mySubscribers[aMsg].begin(), mySubscribers[aMsg].end(), anObserver);
	assert(it == mySubscribers[aMsg].end() && "Observer already added!");
	mySubscribers[aMsg].push_back(anObserver);
}

void Postmaster::UnSubscribe(Observer* anObserver, const eMessage aMsg)
{
	auto it = std::find(mySubscribers[aMsg].begin(), mySubscribers[aMsg].end(), anObserver);
	//assert(it == mySubscribers[aMsg].end() && "Observer already added!");
	mySubscribers[aMsg].erase(it);
}
