#pragma once

class Observer {
	public:
		Observer();
		~Observer();
		virtual void Update() = 0;
};